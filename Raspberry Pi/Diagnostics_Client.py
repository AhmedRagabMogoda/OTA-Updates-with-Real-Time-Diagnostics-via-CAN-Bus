# Diagnostics_client.py

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
)

class DiagnosticsClient:
    def __init__(self, can_bus: CanBus, timeout=1.0):
        self.can = can_bus
        self.timeout = timeout

    def _send_and_recv(self, data_bytes, expected_sid):
        self.can.send(DIAG_REQUEST_ID, data_bytes)
        start = time.time()
        while time.time() - start < self.timeout:
            msg = self.can.recv(timeout=0.1)
            if msg is None:
                continue
            arb_id, data = msg
            if arb_id != DIAG_RESPONSE_ID:
                continue
            sid = data[0]
            if sid == expected_sid:
                return data
        return None

    def start_session(self, session_type):
        data = bytes([SID_DIAG_SESSION_CTRL, session_type])
        resp = self._send_and_recv(data, SID_DIAG_SESSION_CTRL + 0x40)
        return resp is not None

    def send_password(self, password):
        high = (password >> 8) & 0xFF
        low = password & 0xFF
        data = bytes([SID_SECURITY_ACCESS, 0x00, high, low])
        resp = self._send_and_recv(data, SID_SECURITY_ACCESS + 0x40)
        return resp is not None

    def read_data_by_id(self, identifier):
        hi = (identifier >> 8) & 0xFF
        lo = identifier & 0xFF
        data = bytes([SID_READ_DATA_BY_ID, 0x00, hi, lo])
        resp = self._send_and_recv(data, SID_READ_DATA_BY_ID + 0x40)
        if resp:
            return resp[2:]
        return None

    def read_dtc(self):
        data = bytes([SID_READ_DTC, 0x00])
        resp = self._send_and_recv(data, SID_READ_DTC + 0x40)
        if not resp:
            return None
        count = resp[2]
        dtcs = []
        for _ in range(count):
            frame = self._send_and_recv(b'', SID_READ_DTC)
            if frame:
                code = (frame[0]<<24) | (frame[1]<<16) | (frame[2]<<8) | frame[3]
                dtcs.append(code)
        return dtcs

    def clear_dtc(self):
        data = bytes([SID_CLEAR_DTC, 0x00])
        return self._send_and_recv(data, SID_CLEAR_DTC + 0x40) is not None

