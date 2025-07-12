DTC_DESCRIPTIONS = {
    0xF250: "Ultrasonic sensor hardware failure",
    0xF251: "Ultrasonic sensor blocked or reading zero",
    0xF260: "Temperature sensor hardware failure",
    0xF261: "Over-temperature detected"
}

DTC_LOG_FILE = "/home/ahmed-mogoda/dtc_log.json"

# UDS message mappings
UDS_NEG_MESSAGES = {
    0x10: {  # SID_DIAG_SESSION_CTRL
        0x31: "Security access required",  # NRC_SECURITY_REQUIRED
        0x11: "Service not supported"     # NRC_SERVICE_NOT_SUPP
    },
    0x27: {  # SID_SECURITY_ACCESS
        0x35: "Security denied",          # NRC_SECURITY_DENIED
        0x11: "Service not supported"     # NRC_SERVICE_NOT_SUPP
    },
    0x22: {  # SID_READ_DATA_BY_ID
        0x11: "Service not supported"     # NRC_SERVICE_NOT_SUPP
    },
    0x19: {  # SID_READ_DTC
        0x11: "Service not supported"     # NRC_SERVICE_NOT_SUPP
    },
    0x14: {  # SID_CLEAR_DTC
        0x11: "Service not supported"     # NRC_SERVICE_NOT_SUPP
    },
    0x34: {  # SID_REQUEST_DOWNLOAD
        0x11: "Service not supported"     # NRC_SERVICE_NOT_SUPP
    }
}