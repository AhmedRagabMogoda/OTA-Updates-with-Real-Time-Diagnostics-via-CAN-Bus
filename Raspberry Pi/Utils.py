
"""
Shared constants and configurations for the Vehicle Control OTA system.
"""

# CAN identifiers
SENSOR_TEMP_ID       = 0x100  # Temperature DID (via diagnostics)
SENSOR_DIST_ID       = 0x101  # Distance DID (via diagnostics)
SPEED_ID             = 0x102  # Speed frames (real-time)

# UDS Diagnostic Service Identifiers
SID_DIAG_SESSION_CTRL = 0x10  # Diagnostic Session Control
SID_SECURITY_ACCESS   = 0x27  # Security Access
SID_READ_DATA_BY_ID   = 0x22  # Read Data By Identifier
SID_READ_DTC          = 0x19  # Read DTC Information
SID_CLEAR_DTC         = 0x14  # Clear DTC Information
SID_REQUEST_DOWNLOAD  = 0x34  # Request Download (OTA)

# Bootloader OTA CAN message IDs
UPDATE_FRAME_ID = 0x200
UPDATE_ACK_ID   = 0x201

# OTA ACK/NACK payload codes\CODE_NEXT    = 0xA5  # Next chunk
CODE_DONE    = 0x5A  # Update complete
CODE_ERROR   = 0xFF  # Error, retry

# Data identifiers for storing new DTCs
DTC_ID_TEMP = 0xF190  # DID for over-temperature
DTC_ID_DIST = 0xF191  # DID for obstacle distance

# Motor control ID and thresholds
MOTOR_CMD_ID            = 0x300
MOTOR_BLOCK_DISTANCE_CM = 5    # cm threshold for auto-stop

# Firmware server settings
VERSION_JSON_URL = "https://yourserver.com/firmware/version.json"
FIRMWARE_DIR     = "/home/pi/firmware"

# Timeouts (seconds)
DEFAULT_TIMEOUT  = 1.0  # timeout for CAN recv
OTA_DONE_TIMEOUT = 5.0  # timeout for final DONE ACK
