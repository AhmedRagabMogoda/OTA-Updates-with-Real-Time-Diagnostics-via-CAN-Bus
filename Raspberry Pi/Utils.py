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
VERSION_JSON_URL        = "https://yourserver.com/firmware/version.json"
FIRMWARE_DIR            = "/home/pi/firmware"

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
