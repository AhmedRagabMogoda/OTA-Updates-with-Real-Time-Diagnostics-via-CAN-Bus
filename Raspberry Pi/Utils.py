
# CAN identifiers
SENSOR_TEMP_ID      = 0x100  # App ECU → Main ECU: temperature
SENSOR_DIST_ID      = 0x101  # App ECU → Main ECU: distance
SPEED_ID            = 0x102  # App ECU → Main ECU: speed

DIAG_REQUEST_ID     = 0x600  # Pi → Main ECU diagnostic request
DIAG_RESPONSE_ID    = 0x601  # Main ECU → Pi diagnostic response

UPDATE_FRAME_ID     = 0x200  # Pi → STM32 OTA frames
UPDATE_ACK_ID       = 0x201  # STM32 → Pi OTA acknowledgments

# UDS Service IDs
SID_DIAG_SESSION_CTRL  = 0x10
SID_SECURITY_ACCESS    = 0x27
SID_READ_DATA_BY_ID    = 0x22
SID_READ_DTC           = 0x19
SID_CLEAR_DTC          = 0x14

# OTA ACK codes
CODE_NEXT           = 0xA5
CODE_DONE           = 0x5A
CODE_ERROR          = 0xFF

# Motor control identifiers
MOTOR_CMD_ID        = 0x300
MOTOR_DIR_STOP      = 0x00
MOTOR_BLOCK_DISTANCE_CM = 5

# DTC codes
DTC_ID_TEMP         = 0xF190
DTC_ID_DIST         = 0xF191

# Timeouts
DEFAULT_TIMEOUT     = 1.0  # seconds for diagnostic/OTA ACK
OTA_DONE_TIMEOUT    = 5.0  # seconds waiting for DONE
