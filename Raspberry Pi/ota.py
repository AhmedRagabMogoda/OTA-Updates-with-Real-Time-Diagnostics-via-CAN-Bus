import time
import binascii
from .can_bus import can_bus  # Import CAN bus instance
from .utils import *

def start_firmware_upload(firmware_path, ota_q, ota_ack_queue):
    """Start firmware upload with HEX conversion and single initial ACK"""
    if not os.path.exists(firmware_path):
        ota_q.put("Firmware file not found")
        return False

    try:
        with open(firmware_path, 'r') as f:
            hex_data = f.read().strip().replace('\n', '').replace(' ', '')
            firmware_bytes = binascii.unhexlify(hex_data)

        file_size = len(firmware_bytes)
        total_kb = (file_size + 1023) // 1024

        size_frame = bytes([total_kb]) + b'\x00' * 7
        can_bus.send(UPDATE_FRAME_ID, size_frame)
        time.sleep(0.01)

        ota_q.put(f"Sending file size: {total_kb} KB")

        try:
            ack = ota_ack_queue.get(timeout=5.0)
            if ack != CODE_NEXT:
                ota_q.put(f"Initial ACK not received: 0x{ack:02X}")
                return False
        except queue.Empty:
            ota_q.put("Initial ACK timeout")
            return False

        for chunk_index in range(total_kb):
            start = chunk_index * 1024
            end   = min(start + 1024, file_size)
            chunk = firmware_bytes[start:end]

            if len(chunk) < 1024:
                chunk += b'\xFF' * (1024 - len(chunk))

            for i in range(0, 1024, 8):
                frame_data = chunk[i:i+8]
                can_bus.send(UPDATE_FRAME_ID, frame_data)
                time.sleep(0.01)

            progress = int((chunk_index + 1) / total_kb * 100)
            ota_q.put(progress)
            time.sleep(0.1)

        can_bus.send(UPDATE_ACK_ID, bytes([CODE_DONE]))
        ota_q.put("Firmware transmission completed")
        ota_q.put("Waiting for device confirmation...")

        try:
            final_ack = ota_ack_queue.get(timeout=10.0)
            if final_ack == CODE_DONE:
                ota_q.put(100)
                ota_q.put("Device confirmed successful update")
            else:
                ota_q.put(f"Unexpected final code: 0x{final_ack:02X}")
        except queue.Empty:
            ota_q.put("Device confirmation timeout")
        
        return True
    except Exception as e:
        ota_q.put(f"Upload error: {str(e)}")
        return False