import time
import threading

from CAN_Bus import CanBus
from Utils import (
    UPDATE_FRAME_ID,
    UPDATE_ACK_ID,
    START_UPDATE,
    CODE_NEXT,
    CODE_DONE,
    CODE_ERROR,
)

class OTAClient:
    """
    OTA firmware upload over CAN to STM32 Bootloader.
    Listens for START_UPDATE interrupt and then streams the .hex/text file
    in configurable chunks over CAN, converting hex strings to raw bytes,
    using a single initial CODE_NEXT ACK, and then sends all data frames with
    a delay between each frame to prevent network drops.
    """
    def __init__(self, can_bus: CanBus, chunk_size=1024, frame_size=8, inter_frame_delay=0.05):
        self.can = can_bus
        self.chunk_size = chunk_size
        self.frame_size = frame_size
        self.delay = inter_frame_delay

    def upload(self, firmware_path):
        """
        Upload the firmware file in chunks.
        - Reads a hex-formatted file and converts to raw bytes
        - Waits one CODE_NEXT ACK after the size frame
        - Streams all data frames with a delay after each frame
        Yields progress percentage after each chunk sent.
        Raises RuntimeError on CODE_ERROR or unexpected final code.
        """
        # Read hex representation, strip whitespace, convert to bytes
        with open(firmware_path, 'r') as f:
            hex_str = f.read().strip().replace('\n', '').replace(' ', '')
        firmware = bytes.fromhex(hex_str)

        total_chunks = (len(firmware) + self.chunk_size - 1) // self.chunk_size

        # Send size frame and wait for initial ACK
        size_frame = bytes([total_chunks]) + b'\x00' * (self.frame_size - 1)
        self.can.send(UPDATE_FRAME_ID, size_frame)
        time.sleep(self.delay)

        code = self._wait_ack_blocking()
        if code == CODE_ERROR:
            raise RuntimeError('Bootloader returned CODE_ERROR on size frame')
        if code != CODE_NEXT:
            raise RuntimeError(f'Unexpected ACK after size frame: {code}')

        # Stream all chunks, frame by frame, with delay after each frame
        for idx in range(total_chunks):
            start = idx * self.chunk_size
            chunk = firmware[start:start + self.chunk_size]
            if len(chunk) < self.chunk_size:
                chunk += b'\xFF' * (self.chunk_size - len(chunk))

            for offset in range(0, self.chunk_size, self.frame_size):
                frame = chunk[offset:offset + self.frame_size]
                self.can.send(UPDATE_FRAME_ID, frame)
                time.sleep(self.delay)

            progress = int((idx + 1) * 100 / total_chunks)
            yield progress

        # Wait for final DONE code from bootloader
        final = self._wait_ack_blocking()
        if final == CODE_ERROR:
            raise RuntimeError('CODE_ERROR at final stage')
        if final != CODE_DONE:
            raise RuntimeError(f'Expected CODE_DONE, got {final}')

        yield 100

    def _wait_ack_blocking(self):
        """
        Wait indefinitely for a valid ACK message on UPDATE_ACK_ID.
        Returns the ACK code as an integer.
        """
        while True:
            msg = self.can.recv(timeout=None)
            if not msg:
                continue
            arb_id, data = msg
            if arb_id == UPDATE_ACK_ID and data:
                return data[0]

    def watch_for_start(self, firmware_path):
        """
        Run a background thread that waits for START_UPDATE,
        then begins the firmware upload process.
        """
        def _runner():
            while True:
                msg = self.can.recv(timeout=None)
                if not msg:
                    continue
                arb_id, data = msg
                if arb_id == UPDATE_ACK_ID and data and data[0] == START_UPDATE:
                    try:
                        for pct in self.upload(firmware_path):
                            print(f"Upload progress: {pct}%")
                    except Exception as e:
                        print(f"OTA failed: {e}")

        threading.Thread(target=_runner, daemon=False).start()
