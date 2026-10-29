BloodLink – Smart Blood Bank Monitoring and Management System

BloodLink is an IoT-based blood bank monitoring and management system developed as a college mini project. It combines RFID-based blood bag identification, environmental monitoring, ESP8266-based connectivity, and a cloud-backed web application to demonstrate centralized blood inventory management.

Project Overview

The system is designed to help track individual blood bags, monitor storage-environment conditions, and maintain centralized records through a web interface.

Key Features

- RFID-based identification: Associates individual blood bags with unique RFID identifiers.
- Environmental monitoring: Measures temperature and humidity using a connected sensor.
- IoT connectivity: Uses ESP8266 microcontroller boards to support hardware monitoring and cloud communication.
- IR sensing: Detects object presence as a supplementary monitoring input.
- OLED display: Provides a local interface for relevant system information.
- Cloud database: Uses Supabase to manage blood bag records and device information.
- Web-based monitoring: Provides centralized access to inventory and device records.
- Four-slot demonstration: Represents the four blood groups while supporting multiple individual blood bag records per slot.

Hardware Components

- ESP8266 microcontroller boards
- RFID reader modules and RFID tags
- Temperature and humidity sensor
- IR sensors
- OLED display
- Four-slot storage demonstration arrangement
- Power supply, connecting wires, and supporting components

Software and Technologies

- ESP8266 firmware
- Supabase database and backend services
- Supabase Edge Functions
- Web application
- Wi-Fi-based IoT communication

«Update this section with the exact programming languages, frameworks, libraries, and tools used in the final implementation.»

System Architecture

RFID Reader ─────────┐
Temperature/Humidity ┤
IR Sensors ──────────┤
                     ▼
                   ESP8266
                     │
                    Wi-Fi
                     │
                     ▼
             Supabase Backend
             ├── blood_bags
             ├── devices
             └── Edge Functions
                     │
                     ▼
               Web Application

OLED Display ◄── Local Hardware Information

Database Overview

- "blood_bags": Stores individual blood bag records and associated timestamps. The "created_at" field is used as the entry-time source.
- "devices": Stores device identifiers, names, status, last-seen timestamps, and creation timestamps.

Access control and authentication should be configured appropriately before exposing the application or database to external users.

Development and Testing

During development, sensor readings and IR sensor detection were tested using serial output. Temperature and humidity readings were obtained, and the IR sensors correctly reported clear and detected conditions.

The project also covers RFID identification, OLED integration, database design, backend communication, and web-based monitoring. Document the exact end-to-end tests completed on the final integrated system separately.

Project Objective

The objective is to demonstrate how IoT, RFID, environmental sensors, and cloud services can be combined to improve blood bag tracking and storage-condition monitoring.

Limitations

BloodLink is an academic prototype. It is not a certified medical storage, blood safety, or clinical decision-making system. Deployment in a real blood bank would require appropriate sensor calibration, validation, security reviews, regulatory compliance, and clinical workflow assessment.

Future Improvements

- Configurable environmental alerts
- Improved device health monitoring
- Enhanced access control and audit logging
- Inventory reports and analytics
- Validation under realistic operating conditions

Project Status

College mini project – completed prototype.

Contributing

Suggestions and educational contributions are welcome. Ensure that no real patient data, credentials, API keys, or other sensitive information are committed to the repository.