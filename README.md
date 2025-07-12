***OTA Updates with Real-Time Diagnostics via CAN Bus section overview Project Overview***
- ``Integrated automotive solution combining secure over-the-air (OTA) firmware updates, ISO 14229-compliant diagnostics, and real-time telemetry monitoring using CAN bus. Implements a three-tier architecture``
1. **Vehicle Layer**: STM32-based ECUs for real-time control
2. **Gateway Layer**: Raspberry Pi CAN interface
3. **Cloud Layer**: Firmware distribution via AWS


*subsection hardware Hardware Components*
| Component       | Model         | Functionality                     |
|-----------------|--------------|-----------------------------------|
| Main ECU        | STM32F103C8T6 | OTA management & diagnostics      |
| App ECU         | STM32F103C8T6 | Sensor control & motor PWM        |
| Gateway         | Raspberry Pi 4 | Web interface & cloud bridge      |
| CAN Transceiver | MCP2515      | 500kbps CAN communication         |
| Sensors         | HC-SR04/LM35 | Distance & temperature monitoring |

