import threading
import queue
import can
import time

class CanBus:
    """Class for managing CAN bus communication"""
    def __init__(self, channel='can0', bitrate=500000, rx_queue_size=100, can_filters=None):
        self.channel = channel
        self.bitrate = bitrate
        self.rx_queue = queue.Queue(maxsize=rx_queue_size)
        self._running = True
        self._lock = threading.Lock()
        
        # Initialize CAN connection
        self._init_bus(can_filters)
        
        # Start receive thread
        self._thread = threading.Thread(target=self._recv_loop, daemon=True)
        self._thread.start()
    
    def _init_bus(self, can_filters=None):
        try:
            self.bus = can.interface.Bus(
                channel=self.channel,
                bustype='socketcan',
                bitrate=self.bitrate
            )
            if can_filters:
                self.bus.set_filters(can_filters)
        except Exception as e:
            print(f"CAN init error: {e}, retrying in 1s")
            time.sleep(1)
            self._init_bus(can_filters)
    
    def _recv_loop(self):
        """CAN message receive loop"""
        while self._running:
            try:
                msg = self.bus.recv(timeout=1.0)
                if msg:
                    frame = (msg.arbitration_id, bytes(msg.data))
                    self.rx_queue.put(frame)
            except Exception as e:
                print(f"CAN recv error: {e}, restarting bus")
                self._init_bus(None)
    
    def send(self, arbitration_id, data_bytes):
        """Send data over CAN"""
        msg = can.Message(
            arbitration_id=arbitration_id,
            data=data_bytes,
            is_extended_id=False
        )
        with self._lock:
            try:
                self.bus.send(msg)
                return True
            except Exception as e:
                print(f"CAN send error: {e}, restarting bus and retry")
                self._init_bus(None)
                try:
                    self.bus.send(msg)
                    return True
                except Exception as e2:
                    print(f"CAN send retry failed: {e2}")
                    return False
    
    def recv(self, timeout=1.0):
        """Receive data from CAN"""
        try:
            return self.rx_queue.get(block=True, timeout=timeout)
        except queue.Empty:
            return None
    
    def shutdown(self):
        """Shutdown service"""
        self._running = False
        self._thread.join()
        try:
            self.bus.shutdown()
        except:
            pass