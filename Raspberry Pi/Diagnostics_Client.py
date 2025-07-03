import time
import threading

from CAN_Bus import CanBus
from Utils import (
    UDS_NEG_MESSAGES,
    UDS_POS_MESSAGES,
    DIAG_REQUEST_ID,
    DIAG_RESPONSE_ID,
    SID_DIAG_SESSION_CTRL,
    SID_SECURITY_ACCESS,
    SID_READ_DATA_BY_ID,
    SID_READ_DTC,
    SID_CLEAR_DTC,
    SID_REQUEST_DOWNLOAD,
    DEFAULT_TIMEOUT
)

class DiagnosticsClient:
    """
    Client for UDS diagnostic services over CAN.
    Provides methods to send service requests and receive responses.
    """
    def __init__(self, can_bus: CanBus, timeout=DEFAULT_TIMEOUT):
        self.can = can_bus
        self.timeout = timeout

    def _send_and_wait(self, data: bytes, expected_sid: int):
        # send request frame
        self.can.send(DIAG_REQUEST_ID, data)
        # wait for response
        deadline = time.time() + self.timeout
        while time.time() < deadline:
            msg = self.can.recv(timeout=0.1)
            if msg is None:
                continue
            arb_id, payload = msg
            if arb_id != DIAG_RESPONSE_ID or len(payload) < 1:
                continue
            sid = payload[0]
            if sid == expected_sid:
                return payload
        return None

    def start_session(self, session_type: int) -> bool:
        data = bytes([SID_DIAG_SESSION_CTRL, session_type])
        resp = self._send_and_wait(data, SID_DIAG_SESSION_CTRL + 0x40)
        return resp is not None

    def send_password(self, password: int) -> bool:
        high = (password >> 8) & 0xFF
        low = password & 0xFF
        data = bytes([SID_SECURITY_ACCESS, 0x00, high, low])
        resp = self._send_and_wait(data, SID_SECURITY_ACCESS + 0x40)
        return resp is not None

    def read_data_by_id(self, identifier: int) -> bytes | None:
        hi, lo = (identifier >> 8) & 0xFF, identifier & 0xFF
        data = bytes([SID_READ_DATA_BY_ID, 0x00, hi, lo])
        resp = self._send_and_wait(data, SID_READ_DATA_BY_ID + 0x40)
        if resp:
            return resp[2:]
        return None

    def read_dtc(self) -> list[int] | None:
        data = bytes([SID_READ_DTC, 0x00])
        resp = self._send_and_wait(data, SID_READ_DTC + 0x40)
        if not resp or len(resp) < 3:
            return None
        count = resp[2]
        dtcs = []
        for _ in range(count):
            # each DTC code comes back in a single 4‐byte payload here
            resp2 = self._send_and_wait(b'', SID_READ_DTC + 0x40)
            if resp2 and len(resp2) >= 4:
                code = (resp2[0] << 24) | (resp2[1] << 16) | (resp2[2] << 8) | resp2[3]
                dtcs.append(code)
        return dtcs

    def clear_dtc(self) -> bool:
        data = bytes([SID_CLEAR_DTC, 0x00])
        resp = self._send_and_wait(data, SID_CLEAR_DTC + 0x40)
        return resp is not None

    def request_download(self) -> bool:
        data = bytes([SID_REQUEST_DOWNLOAD, 0x00])
        resp = self._send_and_wait(data, SID_REQUEST_DOWNLOAD + 0x40)
        return resp is not None
    
    
    def start_response_monitor(self, diag_queue):
        """
        Start a background thread that listens for any UDS response frames
        and pushes an English text into diag_queue.
        """
        def _runner():
            while True:
                msg = self.can.recv(timeout=1.0)
                if not msg:
                    continue
                arb_id, data = msg
                if arb_id != DIAG_RESPONSE_ID or len(data) < 2:
                    continue
                self._handle_uds_frame(data, diag_queue)

        threading.Thread(target=_runner, daemon=True).start()

    def _handle_uds_frame(self, data: bytes, diag_queue):
        """
        Decode one UDS response frame (positive or negative)
        and enqueue a human-readable English message.
        """
        first = data[0]
        # Negative response
        if first == 0x7F and len(data) >= 3:
            sid, nrc = data[1], data[2]
            text = UDS_NEG_MESSAGES.get(sid, {}).get(
                nrc,
                f"Unknown negative response: SID=0x{sid:02X}, NRC=0x{nrc:02X}"
            )
            diag_queue.put(f"[UDS NEG] SID=0x{sid:02X}, NRC=0x{nrc:02X} ? {text}")
            return

        # Positive response: first byte = SID + 0x40
        sid = first - 0x40
        sub = data[1]
        pos_map = UDS_POS_MESSAGES.get(sid, {})
        text = pos_map.get(sub) or pos_map.get(
            None,
            f"Unknown positive response: SID=0x{sid:02X}, Sub=0x{sub:02X}"
        )
        diag_queue.put(f"[UDS POS] SID=0x{sid:02X} ? {text}")
        