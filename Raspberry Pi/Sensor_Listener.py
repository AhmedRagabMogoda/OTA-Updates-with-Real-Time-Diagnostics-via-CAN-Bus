#Sensor_Listener file
import threading
import queue
from CAN_Bus import CanBus
from Utils import SPEED_ID

# Queue for publishing speed messages (id, speed)
speed_queue = queue.Queue(maxsize=100)

class SensorListener:
    """
    Listens on CAN bus for real-time speed frames only.
    """
    def __init__(self, channel='can0', bitrate=500000, queue_size=100):
        self.can = CanBus(channel=channel, bitrate=bitrate, rx_queue_size=queue_size)
        self._running = False
        self._thread = threading.Thread(target=self._run, daemon=True)

    def start(self):
        """Start listening thread."""
        if not self._running:
            self._running = True
            self._thread.start()

    def stop(self):
        """Stop listening and shutdown CAN."""
        self._running = False
        self._thread.join()
        self.can.shutdown()

    def _run(self):
        """Internal loop: enqueue only speed data."""
        while self._running:
            msg = self.can.recv()
            if not msg:
                continue
            arb_id, data = msg
            if arb_id == SPEED_ID:
                speed = data[0] | (data[1] << 8)
                try:
                    speed_queue.put(speed, block=False)
                except queue.Full:
                    pass

# Usage:
# listener = SensorListener()
# listener.start()
# # read speed via speed_queue.get()
# listener.stop()
