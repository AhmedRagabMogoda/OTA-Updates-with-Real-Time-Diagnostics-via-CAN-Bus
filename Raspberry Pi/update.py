#!/usr/bin/env python3
"""
pi_updater_interrupt.py

Event-driven OTA updater for STM32 bootloader over SocketCAN.

Protocol:
1) Send one CAN frame whose first data byte = number of 1 KB pages to update.
2) Wait for ACK (0xA5) from STM32 before sending any data.
3) For each 1 KB chunk:
   • Split into 8-byte CAN frames.
   • Transmit frames back-to-back.
   • Wait for ACK (0xA5) for that chunk before continuing.
4) After all chunks, wait for DONE ACK (0x5A) to confirm completion.

Usage:
  sudo ip link set can0 type can bitrate 500000
  sudo ip link set can0 up
  python3 pi_updater_interrupt.py firmware.bin
"""

import sys
import time
import threading
import can

# CAN identifiers (must match STM32 code)
UPDATE_FRAME_ID = 0x200
UPDATE_ACK_ID   = 0x201

# ACK codes defined by STM32
CODE_NEXT  = 0xA5  # STM32 asks for next chunk
CODE_DONE  = 0x5A  # STM32 signals update done
CODE_ERROR = 0xFF  # STM32 signals an error (retry chunk)

CHUNK_SIZE = 1024   # 1 KB per flash page

# Threading event and shared code for incoming ACK
ack_event = threading.Event()
ack_code  = None
ack_lock  = threading.Lock()

class AckListener(can.Listener):
    """Listener for ACK frames from STM32."""
    def on_message_received(self, msg):
        global ack_code
        if msg.arbitration_id == UPDATE_ACK_ID and len(msg.data) > 0:
            with ack_lock:
                ack_code = msg.data[0]
                ack_event.set()

def send_frame(bus, can_id, data_bytes):
    """Send a single CAN frame on the bus."""
    msg = can.Message(arbitration_id=can_id,
                      data=data_bytes,
                      is_extended_id=False)
    bus.send(msg)

def wait_for_ack(timeout=1.0):
    """
    Wait for ack_event to be signaled by AckListener.
    Returns the ACK code byte, or None on timeout.
    """
    if ack_event.wait(timeout):
        with ack_lock:
            code = ack_code
            ack_event.clear()
            return code
    return None

def main(firmware_path):
    # Load firmware as raw binary
    with open(firmware_path, 'rb') as f:
        firmware = f.read()

    total_size = len(firmware)
    total_chunks = (total_size + CHUNK_SIZE - 1) // CHUNK_SIZE
    print(f"Firmware size: {total_size} bytes → {total_chunks} chunks")

    # Initialize SocketCAN bus
    bus = can.interface.Bus(channel='can0', bustype='socketcan')

    # Set up listener for ACK frames
    listener = AckListener()
    notifier = can.Notifier(bus, [listener])

    # Wait for manual trigger to start the update
    input("Press ENTER to start firmware update...")


    # 1) Send size frame (first byte = total_chunks)
    size_payload = bytes([total_chunks]) + b'\x00' * 7
    print("Sending size frame...")
    send_frame(bus, UPDATE_FRAME_ID, size_payload)

    code = wait_for_ack()
    if code != CODE_NEXT:
        print(f"Size ACK failed (got {code}), abort.")
        notifier.stop()
        return

    # 2) Transmit each 1KB chunk
    for chunk_index in range(total_chunks):
        offset = chunk_index * CHUNK_SIZE
        chunk = firmware[offset : offset + CHUNK_SIZE]
        # Pad last chunk to full CHUNK_SIZE
        if len(chunk) < CHUNK_SIZE:
            chunk += b'\xFF' * (CHUNK_SIZE - len(chunk))

        print(f"Sending chunk {chunk_index+1}/{total_chunks}...")
        # Send 8-byte frames back-to-back
        for i in range(0, CHUNK_SIZE, 8):
            frame = chunk[i : i+8]
            send_frame(bus, UPDATE_FRAME_ID, frame)
            time.sleep(0.001)  # small inter-frame gap

        # Wait for ACK before proceeding
        code = wait_for_ack()
        if code == CODE_NEXT:
            continue
        elif code == CODE_ERROR:
            print(f"Chunk {chunk_index} error, retrying...")
            chunk_index -= 1  # retry this chunk
            continue
        else:
            print(f"Unexpected ACK code {code}, abort.")
            notifier.stop()
            return

    # 3) Wait for final DONE ACK
    print("Waiting for DONE ACK...")
    code = wait_for_ack(timeout=5.0)
    if code == CODE_DONE:
        print("Update completed successfully!")
    else:
        print(f"Did not receive DONE ACK (got {code}), abort.")

    notifier.stop()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: pi_updater_interrupt.py <firmware.bin>")
        sys.exit(1)
    main(sys.argv[1])
