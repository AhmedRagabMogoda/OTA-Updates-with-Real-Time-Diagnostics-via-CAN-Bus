# ota_client.py

import time
from can_bus import CanBus
from Utils import UPDATE_FRAME_ID, UPDATE_ACK_ID, CODE_NEXT, CODE_DONE, CODE_ERROR

class OTAClient:
    """
    Handles OTA firmware upload over CAN to the STM32 bootloader.
    """
    def __init__(self, can_bus: CanBus, chunk_size=1024, frame_size=8, timeout=1.0):
        self.can = can_bus
        self.chunk_size = chunk_size
        self.frame_size = frame_size
        self.timeout = timeout

    def upload(self, firmware_path):
        # Load firmware
        with open(firmware_path, 'rb') as f:
            firmware = f.read()

        total_size = len(firmware)
        total_chunks = (total_size + self.chunk_size - 1) // self.chunk_size
        # 1) Send size frame
        size_payload = bytes([total_chunks]) + b'\x00' * (self.frame_size - 1)
        self.can.send(UPDATE_FRAME_ID, size_payload)
        if not self._wait_ack(CODE_NEXT):
            print("Size frame ACK failed")
            return False

        # 2) Send each chunk
        for idx in range(total_chunks):
            start = idx * self.chunk_size
            chunk = firmware[start:start + self.chunk_size]
            if len(chunk) < self.chunk_size:
                chunk = chunk + b'\xFF' * (self.chunk_size - len(chunk))
            # send frames of frame_size bytes
            for offset in range(0, self.chunk_size, self.frame_size):
                frame = chunk[offset:offset + self.frame_size]
                self.can.send(UPDATE_FRAME_ID, frame)
                time.sleep(0.001)
            # wait for next ACK
            code = self._wait_ack()
            if code == CODE_NEXT:
                continue
            elif code == CODE_ERROR:
                print(f"Chunk {idx} error, retrying")
                return False
            else:
                print(f"Unexpected ACK code: {code}")
                return False

        # 3) wait for DONE
        if self._wait_ack(CODE_DONE, timeout=5.0):
            print("OTA upload completed")
            return True
        print("OTA upload did not complete")
        return False

    def _wait_ack(self, expected=None, timeout=None):
        t0 = time.time()
        to = timeout if timeout is not None else self.timeout
        while time.time() - t0 < to:
            msg = self.can.recv(timeout=0.1)
            if msg is None:
                continue
            arb_id, data = msg
            if arb_id != UPDATE_ACK_ID or len(data) == 0:
                continue
            code = data[0]
            if expected is None or code == expected:
                return code
        return None
