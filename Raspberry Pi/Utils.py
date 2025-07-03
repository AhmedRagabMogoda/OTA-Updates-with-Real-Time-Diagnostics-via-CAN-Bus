"""
Utils file
"""

# ------------------------------
# CAN identifiers (IDs on the bus)
# ------------------------------
SENSOR_TEMP_ID          = 0x100  # Temperature DID (via diagnostics)
SENSOR_DIST_ID          = 0x101  # Distance DID (via diagnostics)
SPEED_ID                = 0x102  # Speed frames (real-time)

# UDS-over-CAN frame IDs
DIAG_REQUEST_ID         = 0x7E0  # CAN ID for sending UDS requests to ECU
DIAG_RESPONSE_ID        = 0x7E8  # CAN ID for receiving UDS responses from ECU

# UDS Diagnostic Service Identifiers (SIDs)
SID_DIAG_SESSION_CTRL   = 0x10   # Diagnostic Session Control
SID_SECURITY_ACCESS     = 0x27   # Security Access
SID_READ_DATA_BY_ID     = 0x22   # Read Data By Identifier
SID_READ_DTC            = 0x19   # Read DTC Information
SID_CLEAR_DTC           = 0x14   # Clear DTC Information
SID_REQUEST_DOWNLOAD    = 0x34   # Request Download (OTA)

# Bootloader OTA CAN message IDs
UPDATE_FRAME_ID         = 0x200
UPDATE_ACK_ID           = 0x201

# OTA ACK/NACK payload codes
CODE_NEXT               = 0xA5   # Next chunk
CODE_DONE               = 0x5A   # Update complete
CODE_ERROR              = 0xFF   # Error, retry
START_UPDATE            =0XAB
# Data identifiers for storing new DTCs (original UDS DIDs)
DTC_ID_TEMP             = 0xF190  # DID for over-temperature
DTC_ID_DIST             = 0xF191  # DID for obstacle distance

# -------------------------------------------------------------------
# Custom DTC codes for your sensor-fault messages
# -------------------------------------------------------------------
# Ultrasonic sensor faults
DTC_ULTRASONIC_FAILURE   = 0xF250  # Ultrasonic sensor hardware failure
DTC_ULTRASONIC_BLOCKED   = 0xF251  # Ultrasonic sensor blocked or reading zero

# Temperature sensor faults
DTC_TEMP_SENSOR_FAILURE  = 0xF260  # Temperature sensor hardware failure
DTC_OVER_TEMPERATURE     = 0xF261  # Over-temperature detected

# Motor control ID and thresholds
MOTOR_CMD_ID            = 0x300
MOTOR_BLOCK_DISTANCE_CM = 5       # cm threshold for auto-stop

# Firmware server settings
VERSION_JSON_URL        = "https://my-vehicle-firmware.s3.eu-north-1.amazonaws.com/firmware/version.json"
FIRMWARE_DIR            = "/home/ahmed-mogoda/OTA_firmware"

# Timeouts (seconds)
DEFAULT_TIMEOUT         = 1.0     # timeout for CAN recv
OTA_DONE_TIMEOUT        = 5.0     # timeout for final DONE ACK

"""
Human-friendly message mappings for UDS services and DTC codes.
"""

# Positive/Negative UDS service messages
POS_NEG_MESSAGES = {
    0x10: {True:  "Session control successful",
           False: "Session control failed"},
    0x27: {True:  "Security unlocked successfully",
           False: "Security unlock failed"},
    0x22: {True:  "Read data successful",
           False: "Read data failed"},
    0x19: {True:  "DTC read successful",
           False: "DTC read failed"},
    0x14: {True:  "DTC clear successful",
           False: "DTC clear failed"},
    0x34: {True:  "Download mode entered",
           False: "Download mode request failed"},
}

# DTC code → human description
DTC_DESCRIPTIONS = {
    0xF250: "Ultrasonic sensor hardware failure",
    0xF251: "Ultrasonic sensor blocked or reading zero",
    0xF260: "Temperature sensor hardware failure",
    0xF261: "Over-temperature detected"
}
# Mapping of negative UDS responses (NRC) per SID
UDS_NEG_MESSAGES = {
    0x10: {  # SID_DIAG_SESSION_CTRL
        0x31: "Security access required",
        0x11: "Service not supported"
    },
    0x27: {  # SID_SECURITY_ACCESS
        0x35: "Security denied",
        0x11: "Service not supported"
    },
    0x22: {  # SID_READ_DATA_BY_ID
        0x11: "Service not supported"
    },
    0x19: {  # SID_READ_DTC
        0x11: "Service not supported"
    },
    0x14: {  # SID_CLEAR_DTC
        0x11: "Service not supported"
    },
    0x34: {  # SID_REQUEST_DOWNLOAD
        0x11: "Service not supported"
    }
}

# Mapping of positive UDS responses per SID
# Sub-function as key, or None for generic
UDS_POS_MESSAGES = {
    0x10: {  # SID_DIAG_SESSION_CTRL
        0x00: "Session set to DEFAULT",
        0x01: "Session set to SENSOR",
        0x02: "Session set to CONTROL",
        0x03: "Session set to PROGRAMMING"
    },
    0x27: {  # SID_SECURITY_ACCESS
        None: "Security access granted"
    },
    0x22: {  # SID_READ_DATA_BY_ID
        None: "Data read successfully"
    },
    0x19: {  # SID_READ_DTC
        None: "DTC list received"
    },
    0x14: {  # SID_CLEAR_DTC
        None: "DTC cleared"
    },
    0x34: {  # SID_REQUEST_DOWNLOAD
        None: "Download mode entered"
    }
}