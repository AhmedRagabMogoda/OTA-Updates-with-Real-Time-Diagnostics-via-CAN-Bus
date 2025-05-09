import can
import threading
import queue

class CanBus:
    """
    Wrapper for SocketCAN interface using python-can.
    Provides send and receive methods with an internal Rx queue.
    """
    def __init__(self, channel='can0', bitrate=500000, rx_queue_size=100):
        # Initialize SocketCAN bus
        self.bus = can.interface.Bus(channel=channel, bustype='socketcan', bitrate=bitrate)
        # Internal queue for received messages
        self.rx_queue = queue.Queue(maxsize=rx_queue_size)
        # Start background thread for receiving
        self._running = True
        self._thread = threading.Thread(target=self._recv_loop, daemon=True)
        self._thread.start()

    def _recv_loop(self):
        while self._running:
            msg = self.bus.recv(timeout=1.0)
            if msg is not None:
                try:
                    # Put (id, data) tuple in queue
                    self.rx_queue.put((msg.arbitration_id, msg.data), block=False)
                except queue.Full:
                    # Drop message if queue is full
                    pass

    def send(self, arbitration_id, data_bytes):
        """
        Send a single CAN frame.

        :param arbitration_id: Standard identifier (int)
        :param data_bytes: bytes or bytearray up to 8 bytes
        """
        msg = can.Message(arbitration_id=arbitration_id,
                          data=data_bytes,
                          is_extended_id=False)
        self.bus.send(msg)

    def recv(self, timeout=None):
        """
        Receive a (arbitration_id, data) tuple from the internal queue.

        :param timeout: seconds to wait (None means block indefinitely)
        :return: (arbitration_id, data_bytes) or None if timed out
        """
        try:
            return self.rx_queue.get(block=(timeout is None), timeout=timeout)
        except queue.Empty:
            return None

    def shutdown(self):
        """
        Stop background thread and cleanup.
        """
        self._running = False
        self._thread.join()
        self.bus.shutdown()
