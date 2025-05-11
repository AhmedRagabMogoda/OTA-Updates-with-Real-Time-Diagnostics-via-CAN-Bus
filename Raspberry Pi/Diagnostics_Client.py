# diagnostics_client.py

import time
from CAN_Bus import CanBus
from Utils import (
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
        # session_type: 0 (default), 1 (sensor), 2 (control), 3 (programming)
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
        hi = (identifier >> 8) & 0xFF
        lo = identifier & 0xFF
        data = bytes([SID_READ_DATA_BY_ID, 0x00, hi, lo])
        resp = self._send_and_wait(data, SID_READ_DATA_BY_ID + 0x40)
        if resp:
            # return data bytes after sid and sub
            return resp[2:]
        return None

    def read_dtc(self) -> list[int] | None:
        data = bytes([SID_READ_DTC, 0x00])
        resp = self._send_and_wait(data, SID_READ_DTC + 0x40)
        if not resp or len(resp) < 3:
            return None
        count = resp[2]
        dtcs = []
        # read each DTC code frame (6 bytes)
        for i in range(count):
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
        # enter programming session first
        # assume session set externally
        data = bytes([SID_REQUEST_DOWNLOAD, 0x00])
        resp = self._send_and_wait(data, SID_REQUEST_DOWNLOAD + 0x40)
        return resp is not None
