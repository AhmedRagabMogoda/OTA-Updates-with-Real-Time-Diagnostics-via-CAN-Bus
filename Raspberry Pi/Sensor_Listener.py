import threading
import queue
from CAN_Bus import CanBus
from Utils import SENSOR_TEMP_ID, SENSOR_DIST_ID, SPEED_ID

# Queue to publish sensor messages for other components
xSensorQueue = queue.Queue(maxsize=100)

class SensorListener:
    """
    Listens on CAN bus for sensor data (temperature, distance, speed)
    and enqueues them into xSensorQueue.
    """
    def __init__(self, can_channel='can0', bitrate=500000):
        # Initialize CAN bus interface
        self.can = CanBus(channel=can_channel, bitrate=bitrate)
        self._running = False
        self._thread = threading.Thread(target=self._run, daemon=True)

    def start(self):
        """Start the background listener thread."""
        self._running = True
        self._thread.start()

    def stop(self):
        """Stop the listener thread and shutdown CAN."""
        self._running = False
        self._thread.join()
        self.can.shutdown()

    def _run(self):
        while self._running:
            msg = self.can.recv(timeout=1.0)
            if msg is None:
                continue
            arb_id, data = msg
            # Filter sensor IDs
            if arb_id in (SENSOR_TEMP_ID, SENSOR_DIST_ID, SPEED_ID):
                try:
                    # Enqueue (id, data) tuple
                    xSensorQueue.put((arb_id, data), block=False)
                except queue.Full:
                    # Drop if queue is full
                    pass

# Example usage:
# listener = SensorListener()
# listener.start()
# ...
# listener.stop()
