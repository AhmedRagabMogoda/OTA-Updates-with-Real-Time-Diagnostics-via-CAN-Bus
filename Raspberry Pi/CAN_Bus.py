import can
import threading
import queue

from Utils import DEFAULT_TIMEOUT

class CanBus:
    """
    Wrapper for SocketCAN interface using python-can.
    Manages send and receive via internal thread and queue.
    """
    def __init__(self, channel='can0', bitrate=500000, rx_queue_size=100):
        # Initialize SocketCAN bus
        self.bus = can.interface.Bus(channel=channel, bustype='socketcan', bitrate=bitrate)
        # Queue for incoming CAN messages: (arbitration_id, data_bytes)
        self.rx_queue = queue.Queue(maxsize=rx_queue_size)
        # Control flag for background thread
        self._running = True
        # Start receiver thread
        self._thread = threading.Thread(target=self._recv_loop, daemon=True)
        self._thread.start()

    def _recv_loop(self):
        """
        Background loop: receive messages from CAN bus and enqueue.
        """
        while self._running:
            msg = self.bus.recv(timeout=1.0)
            if msg is None:
                continue
            try:
                self.rx_queue.put((msg.arbitration_id, msg.data), block=False)
            except queue.Full:
                # Drop messages if queue is full
                pass

    def send(self, arbitration_id, data_bytes):
        """
        Send a CAN frame.

        :param arbitration_id: CAN standard identifier (int)
        :param data_bytes: bytes or bytearray up to 8 bytes
        """
        msg = can.Message(arbitration_id=arbitration_id,
                          data=data_bytes,
                          is_extended_id=False)
        self.bus.send(msg)

    def recv(self, timeout=DEFAULT_TIMEOUT):
        """
        Retrieve a message from receive queue.

        :param timeout: seconds to wait for a message
        :return: (arbitration_id, data_bytes) or None if timed out
        """
        try:
            return self.rx_queue.get(block=True, timeout=timeout)
        except queue.Empty:
            return None

    def shutdown(self):
        """
        Stop background thread and close CAN interface.
        """
        self._running = False
        self._thread.join()
        self.bus.shutdown()

