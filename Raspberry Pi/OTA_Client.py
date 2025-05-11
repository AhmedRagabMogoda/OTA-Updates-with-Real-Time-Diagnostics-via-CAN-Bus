# ota_client.py

import time
from CAN_Bus import CanBus
from Utils import (
    UPDATE_FRAME_ID,
    UPDATE_ACK_ID,
    CODE_NEXT,
    CODE_DONE,
    CODE_ERROR,
    DEFAULT_TIMEOUT,
    OTA_DONE_TIMEOUT
)

class OTAClient:
    """
    OTA firmware upload over CAN to STM32 Bootloader.
    Requires Bootloader to be in programming session (via DiagnosticsClient.request_download()).
    Yields integer progress percentages after each 1 KB page is acknowledged.
    """
    def __init__(self, can_bus: CanBus, chunk_size=1024, frame_size=8,
                 timeout=DEFAULT_TIMEOUT, done_timeout=OTA_DONE_TIMEOUT):
        self.can = can_bus
        self.chunk_size = chunk_size
        self.frame_size = frame_size
        self.timeout = timeout
        self.done_timeout = done_timeout

    def upload(self, firmware_path):
        """
        Upload firmware file in pages of chunk_size.
        Yields progress percentage (0-100) after each page ACK.
        """
        # Load entire firmware binary
        with open(firmware_path, 'rb') as f:
            firmware = f.read()

        total_size = len(firmware)
        total_chunks = (total_size + self.chunk_size - 1) // self.chunk_size

        # 1) Send size frame and wait for CODE_NEXT
        size_payload = bytes([total_chunks]) + b'\x00' * (self.frame_size - 1)
        self.can.send(UPDATE_FRAME_ID, size_payload)
        if self._wait_ack(CODE_NEXT, timeout=self.timeout) != CODE_NEXT:
            raise RuntimeError('Bootloader did not ACK size frame')

        # 2) Send each 1 KB chunk
        for idx in range(total_chunks):
            start = idx * self.chunk_size
            chunk = firmware[start:start + self.chunk_size]
            # pad last chunk if smaller
            if len(chunk) < self.chunk_size:
                chunk += b'\xFF' * (self.chunk_size - len(chunk))

            # send chunk in frame_size-byte CAN frames
            for offset in range(0, self.chunk_size, self.frame_size):
                frame = chunk[offset:offset + self.frame_size]
                self.can.send(UPDATE_FRAME_ID, frame)
                time.sleep(0.001)

            # wait for ACK for this chunk
            code = self._wait_ack(timeout=self.timeout)
            if code == CODE_NEXT:
                # calculate and yield progress
                progress = int((idx + 1) * 100 / total_chunks)
                yield progress
            elif code == CODE_ERROR:
                # retry same chunk
                idx -= 1
                continue
            else:
                raise RuntimeError(f'Unexpected ACK code: {code}')

        # 3) Wait for final DONE ACK
        final = self._wait_ack(CODE_DONE, timeout=self.done_timeout)
        if final != CODE_DONE:
            raise RuntimeError('Did not receive final DONE ACK')

        # ensure 100% at end
        yield 100

    def _wait_ack(self, expected=None, timeout=None):
        """
        Wait for an ACK frame from Bootloader.
        :param expected: specific code to match or None
        :param timeout: seconds to wait
        :return: received code or None
        """
        deadline = time.time() + (timeout if timeout is not None else self.timeout)
        while time.time() < deadline:
            msg = self.can.recv(timeout=0.1)
            if not msg:
                continue
            arb_id, data = msg
            if arb_id != UPDATE_ACK_ID or not data:
                continue
            code = data[0]
            if expected is None or code == expected:
                return code
        return None
