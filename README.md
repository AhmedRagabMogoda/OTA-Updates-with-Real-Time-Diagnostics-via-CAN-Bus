# OTA Updates with Real-Time Diagnostics via CAN Bus 


## 1. Introduction

In this graduation project, we designed and implemented an intelligent vehicle control system featuring Over-The-Air (OTA) updates and real-time diagnostics via CAN Bus. The system enables wireless firmware updates, secure validation of update packages, and immediate fault detection and reporting on a web interface.
#### Implements a three-tier architecture
1. **Vehicle Layer**: STM32-based ECUs for real-time control
2. **Gateway Layer**: Raspberry Pi CAN interface
3. **Cloud Layer**: Firmware distribution via AWS

## 2. Hardware Components

- **Raspberry Pi 4 (2GB)**: Acts as the main server to download firmware updates from the cloud, host the web interface, and coordinate communication over CAN Bus.
- **STM32F103C8T6 Microcontrollers (×2)**:
  - **Main ECU**: Manages OTA update commands and diagnostic requests.
  - **Application ECU**: Receives validated firmware and programs it into flash memory.
- **CAN Transceiver (TJA1050)**: Acts as the physical layer interface between the CAN controller (MCP2515) and the CAN bus lines..
- **CAN Hat Module (MCP2515 + TJA1050)**: Facilitates communication between the Raspberry Pi and the STM32 MCUs.
- **L298N Motor Driver**: Controls dual DC motors driving the robot chassis..
- **Ultrasonic Distance Sensor**: Measures obstacles and distance for safety diagnostics.
- **Temperature Sensor**: Monitors ambient and component temperatures for diagnostic data.
- **Bluetooth Module**: Provides remote control capabilities via a mobile device.
- **HDMI Touchscreen Display**: Serves as the local user interface for update status and diagnostics.

## 3. System Overview

The system consists of three main nodes communicating over a shared CAN Bus (500 kbps): the Main ECU, the Application ECU, and the Raspberry Pi.

### 3.1 Main ECU (STM32F103)

- **Primary Role**: Coordinates OTA updates and handles high-level diagnostic services.
- **Software Stack**: STM32F103 running FreeRTOS.
- **Key Tasks**:
  - **CANReceiveTask()**: Listens for OTA triggers, control commands, and diagnostic requests.
  - **CANTransmitTask()**: Sends acknowledgments, status updates, and diagnostic responses.
  - **MonitorControlTask()**: Monitors system health and enforces safety fallback logic.
  - **DiagnosticsTask()**: Implements UDS services for session control, security access, and DTC read/clear.
  - **OTAManagerTask()**: Validates firmware metadata, triggers bootloader entry, and resets to initiate update.

### 3.2 Application ECU (STM32F103)

- **Primary Role**: Executes real-time sensor acquisition, motor control, and local command processing.
- **Software Stack**: STM32F103 under FreeRTOS.
- **Key Tasks**:
  - **UltrasonicTask()**: Captures distance measurements and queues data for CAN transmission.
  - **TemperatureSensorTask()**: Reads LM35 temperature values and enqueues for telemetry.
  - **MotorControlTask()**: Computes PWM signals based on sensor inputs and safety parameters.
  - **BluetoothTask()**: Receives remote drive/stop commands and forwards to control logic.
  - **CAN Rx/Tx Tasks**: Manage incoming control frames and outgoing telemetry or diagnostics data.

### 3.3 Raspberry Pi 4 (2GB)

- **Primary Role**: User interface hub, OTA client, and CAN gateway.
- **Operating System**: Raspberry Pi OS.
- **Software Components**:
  - **SocketCAN Driver**: Configured at 500 kbps for CAN communication via MCP2515.
  - **Flask Web Server**: Hosts an HTML/CSS/JS dashboard for live telemetry and update control.
  - **CAN Listener Service**: Reads live CAN frames (speed, temperature, distance) and pushes to dashboard via Server-Sent Events (SSE).
  - **OTA Client**: Polls cloud REST API (e.g., AWS S3) for new firmware, downloads signed chunks, and sends them over CAN.
  - **Diagnostics Client**: Sends UDS requests over CAN, retrieves DTCs, and displays results in the web interface.

## 4. OTA Updates

The Over-The-Air update mechanism is a core feature of our system, enabling seamless firmware upgrades without physical connections.

### 4.1 Bootloader Design

The Application ECU (STM32F103) contains a custom bootloader that facilitates secure and reliable firmware updates via CAN bus. The bootloader is a minimal program residing at the beginning of the flash memory that is responsible for receiving, verifying, and flashing the new firmware image.

- **Startup Behavior**:
  - On reset, the bootloader checks for a specific flag in flash memory indicating whether a firmware update is pending.
  - If no update is required, it jumps to the main application.

- **Firmware Reception**:
  - The bootloader listens on the CAN bus for incoming firmware chunks transmitted by the Raspberry Pi via the Main ECU.
  - Each chunk includes a header, payload, and CRC.

- **Fail-Safe Mechanism**:
  - If any chunk fails verification or the update is interrupted, the bootloader retains the last valid application.
  - Flash write operations are protected to avoid bricking the device.

- **Completion**:
  - Once all chunks are received and validated, the bootloader sets a completion flag and jumps to the new application.

- **Benefits**:
  - Enables robust field updates without external tools.
  - Reduces maintenance costs and physical intervention.
  - Supports update rollback if integrity checks fail.

### 4.2 OTA Workflow

The Over-The-Air update mechanism is a core feature of our system, enabling seamless firmware upgrades without physical connections.

- **Workflow**:
  1. **Version Check**: Raspberry Pi polls the cloud server API for the latest firmware version.
  2. **Download**: If a newer version is available, the OTA Client downloads the firmware package in encrypted chunks.
  3. **Integrity Verification**: Each chunk is verified using an RSA digital signature before transmission.
  4. **Transmission**: The Raspberry Pi sends chunks over CAN to the Main ECU, which forwards them to the Application ECU.
  5. **Bootloader Activation**: Upon receiving a complete, validated image, the Main ECU triggers its bootloader to switch to the new firmware.
  6. **Confirmation**: After reboot, the Main ECU confirms the successful update back to the Raspberry Pi.

- **Fail-Safe Mechanism**:
  -  If an update fails or the signature check does not pass, the system rolls back to the previous stable version.

- **Key Benefits**:
  - Eliminates manual wiring and physical access for firmware deployments.
  - Provides quick patching capability for critical bug fixes.
  - Enhances flexibility for field maintenance and continuous improvement.


## 5. Diagnostics (UDS Services)

Our diagnostics implementation uses the Unified Diagnostic Services (UDS) protocol over CAN to manage sessions, security, and fault codes.

### 5.1 Diagnostic Sessions

- **Default Session (0x01)**: Entry-level session for basic communication without advanced functions.
- **Programming Session (0x02)**: Grants access to firmware programming operations and extended memory services.
- **Extended Diagnostic Session (0x03)**: Enables enhanced diagnostics, such as advanced data capture and specialized services.

To switch sessions, the tester tool sends a **Diagnostic Session Control** request (Service 0x10) with the desired session parameter, and waits for a positive response (0x50).

### 5.2 Security Access (0x27)

1. **Request Seed**: Tester sends **SecurityAccess Request (0x27, SubFunction 0x01)** to obtain a seed.
2. **Send Key**: Tester calculates the key (seed encrypted with a predefined key algorithm) and sends **SecurityAccess (0x27, SubFunction 0x02)** with the key.
3. **Positive Response**: ECU replies with **0x67** indicating access granted.

### 5.3 Read Diagnostic Trouble Codes (DTCs)

- **Service 0x19 (ReadDTCInformation)** with SubFunction 0x02 returns the number of stored DTCs.
- **Service 0x19, SubFunction 0x0A** retrieves detailed DTC records (DTC code, status byte, timestamp).

### 5.4 Clear Diagnostic Trouble Codes

- **Service 0x14 (ClearDiagnosticInformation)**: Tester sends a request with a DTC group parameter (e.g., 0xFF to clear all). ECU responds with **0x54** on success.

### 5.5 I/O Control (0x2F)

- Allows activation/deactivation of specific actuators for testing (e.g., toggle motors or LEDs).
- **SubFunction 0x01**: Enable control; **SubFunction 0x02**: Disable control.

### 5.6 Routine Control (0x31)

- Used for tasks like firmware download initiation and end-of-routine checks.
- **SubFunction 0x01 (StartRoutine)**: Begins a predefined routine (e.g., flash erase).
- **SubFunction 0x02 (StopRoutine)**: Terminates the routine.
- **SubFunction 0x03 (RequestRoutineResults)**: Retrieves results or progress status.

## 6. Cloud Integration

To support OTA and data storage, the project integrates with cloud services as follows:

- **Cloud Storage (AWS S3 / Firebase Storage)**:
  - Firmware binaries are uploaded to a secure S3 bucket or Firebase storage.
  - Version metadata (version number, release notes, signatures) is maintained in a JSON file or Firestore document.

- **REST API (Flask Backend)**:
  - **GET /api/firmware/latest**: Returns metadata for the latest firmware version.
  - **GET /api/firmware/download/{version}**: Streams the encrypted firmware package in chunks.
  - **GET /api/diagnostics/logs**: Retrieves historical diagnostic logs and telemetry data.

- **Authentication & Security**:
  - API requests require an API key or JWT token to prevent unauthorized access.
  - HTTPS is enforced for all endpoints.

- **Data Logging & Monitoring**:
  - Telemetry (temperature, distance, speed) and diagnostic events are sent to a cloud database (e.g., Firebase Realtime Database).
  - Cloud Functions trigger alerts when critical faults are detected (e.g., DTC severity > threshold).

- **Benefits**:
  - Centralized management of firmware versions and logs.
  - Scalability for multiple deployed units.
  - Remote monitoring and alerting for proactive maintenance.
 
    
## 7. Web Interface

The system includes a user-friendly web interface hosted on the Raspberry Pi, providing an interactive way for users to control and monitor the vehicle.

- **Technologies Used**:
  - Flask (Python backend)
  - HTML, CSS, JavaScript (frontend)
  - Server-Sent Events (SSE) for real-time data updates

- **Features**:
  - **OTA Management**: Check for updates, download, and initiate installation.
  - **Live Telemetry**: View real-time sensor data (e.g., temperature, distance, speed).
  - **Diagnostics Panel**: Send diagnostic commands, view fault codes, and clear DTCs.
  - **Status Monitoring**: Display connection status with each ECU and update/diagnostic results.

- **User Experience**:
  - Touchscreen-compatible UI.
  - Accessible via tablet or external screen connected via HDMI.
  - Clear visual feedback for all operations.

## 8. Conclusion

This project demonstrates the integration of embedded systems, IoT, and automotive diagnostics into a cohesive platform that supports secure wireless updates, real-time monitoring, and remote diagnostics. Through careful hardware-software co-design, we achieved a reliable system suitable for intelligent vehicle control and future expansion.
