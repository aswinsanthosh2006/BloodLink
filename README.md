
🩸 BloodLink
Smart Blood Bank Monitoring and Management System
� � � � �
An IoT-based prototype for RFID blood-bag identification, environmental monitoring, and cloud-connected inventory management.

📌 What is BloodLink?
BloodLink is a smart blood-bank monitoring and management prototype that connects physical storage hardware with a cloud-backed inventory system. It combines RFID-based identification, temperature and humidity sensing, IR-based object detection, an OLED display, ESP8266 microcontrollers, Supabase services, and a web application.
The demonstration uses four storage slots representing the four blood groups. Each slot can accommodate multiple blood-bag records. RFID is used to identify an individual tagged bag, while the slot represents its category or storage position in the demonstration.
The goal is to demonstrate how embedded hardware and cloud software can work together to make inventory information easier to identify, organize, and monitor.
Project scope: BloodLink is a completed academic prototype. It is not a certified blood-storage, blood-safety, or clinical decision-making system.
Core capabilities
Capability
Role in the system
🏷️ RFID identification
Reads a tag identifier associated with an individual blood bag
🌡️ Environmental monitoring
Reads temperature and humidity through a connected sensor
📡 Object-presence detection
Uses two IR sensors for supplementary presence detection
🖥️ Local feedback
Uses an OLED display for selected device or inventory information
📶 Embedded connectivity
Uses ESP8266 Wi-Fi connectivity for communication with cloud services
☁️ Cloud data management
Uses Supabase tables and Edge Functions as part of the backend architecture
🌐 Web-based management
Provides a centralized interface for inventory and device information
🧊 Four-slot demonstration
Represents four blood-group categories, with multiple bags per slot
🏗️ System Architecture
BloodLink is organized into six logical layers: sensing, embedded processing, communication, backend services, database, and web application.
┌───────────────────────────────────────────────────────────┐
│                 HARDWARE & SENSING LAYER                  │
│                                                           │
│   RFID Reader + Tags   Temperature/Humidity Sensor        │
│   IR Sensor 1          IR Sensor 2          OLED Display  │
└───────────────┬───────────────────┬───────────────────────┘
                │                   │
                ▼                   ▼
┌───────────────────────────────────────────────────────────┐
│                   EMBEDDED LAYER                           │
│                                                           │
│                     ESP8266 Board(s)                       │
│       Peripheral reading • Local processing • Wi-Fi       │
└────────────────────────────┬──────────────────────────────┘
                             │
                             ▼
┌───────────────────────────────────────────────────────────┐
│                  COMMUNICATION LAYER                       │
│                         Wi-Fi                             │
└────────────────────────────┬──────────────────────────────┘
                             │
                             ▼
┌───────────────────────────────────────────────────────────┐
│                   CLOUD / BACKEND                          │
│                                                           │
│              Supabase Edge Function(s)                     │
│           Server-side processing and validation            │
└────────────────────────────┬──────────────────────────────┘
                             │
                             ▼
┌───────────────────────────────────────────────────────────┐
│                     DATABASE                              │
│                                                           │
│                Supabase PostgreSQL                         │
│             blood_bags • devices                           │
└────────────────────────────┬──────────────────────────────┘
                             │
                             ▼
┌───────────────────────────────────────────────────────────┐
│                  APPLICATION LAYER                         │
│                                                           │
│             Web Application / Inventory UI                 │
│       Centralized access to supported records              │
└───────────────────────────────────────────────────────────┘
Architecture responsibilities
Layer
Components
Responsibility
Hardware
RFID reader, environmental sensor, IR sensors, OLED
Reads tags and sensor inputs; presents local feedback
Embedded
ESP8266 firmware
Interfaces with peripherals and handles device-side logic
Communication
Wi-Fi
Provides the network path from the ESP8266 to cloud services
Backend
Supabase Edge Functions
Supports server-side operations used by the application or device
Database
Supabase tables
Stores blood-bag and device information
Application
Web application
Presents centralized inventory and device information
Implementation note: This is the logical architecture of the project. The exact firmware request paths, Edge Function names, and web-app synchronization behaviour must be confirmed from the final source code. Local sensor readings do not, by themselves, prove that every measurement is uploaded to the database.
🔄 How BloodLink Works
RFID Tag / Sensors
        │
        ▼
ESP8266 reads connected peripherals
        │
        ├──► OLED provides local feedback
        │
        ▼
Wi-Fi communication
        │
        ▼
Supabase backend processing
        │
        ▼
Database records
        │
        ▼
Web application
The complete workflow is intended to operate as follows:
Sense: The environmental sensor measures temperature and humidity. The two IR sensors provide object-presence states.
Identify: When a compatible RFID tag is presented to the reader, its identifier can be used to identify an individual blood-bag record.
Process: The ESP8266 firmware reads the attached modules and handles device-side logic.
Display: The OLED provides local feedback according to the display logic implemented in the firmware.
Communicate: The ESP8266 uses Wi-Fi to reach configured backend services.
Process and store: Supabase Edge Functions and database operations support the cloud workflow implemented by the project.
Review: The web application provides centralized access to the records and actions it supports.
The precise request formats, retry behaviour, database updates, and synchronization mechanisms depend on the final implementation. This README does not assume automatic real-time synchronization unless it is confirmed by the source.
🏷️ RFID-Based Blood-Bag Identification
RFID is the mechanism intended to distinguish individual tagged blood bags.
Identification workflow
Present tagged blood bag
          │
          ▼
RFID reader reads tag identifier
          │
          ▼
ESP8266 processes the identifier
          │
          ▼
Identifier is used by the implemented lookup/workflow
          │
          ▼
Corresponding blood-bag record
RFID is important because the bag and the storage slot are different entities:
Blood bag: An individual item identified by its tag.
Storage slot: A location/category in the demonstration.
Blood group: A category represented by one of the four slots.
Database record: The digital representation used by the inventory application.
A slot can represent multiple bags, so the design should not treat one RFID reader or one scan as equivalent to one permanently occupied slot. The exact tag-to-record lookup key, duplicate-scan handling, unknown-tag behaviour, and database transaction rules depend on the final firmware and backend code.
RFID identifies a tag; it does not independently validate blood type, compatibility, expiry, sterility, or suitability for transfusion.
🌡️ Environmental Monitoring
The system includes temperature and humidity sensing connected to the embedded hardware.
During development, sensor readings were observed through serial output. One recorded example was:
Temperature: 31.80 C
Humidity: 75.00 %
IR Sensor 1: CLEAR
IR Sensor 2: CLEAR
These are development observations, not calibrated reference measurements.
Signal
Purpose
Evidence available
Temperature
Observe the surrounding storage environment
Readings observed through serial output
Humidity
Observe environmental humidity
Readings observed through serial output
IR sensor 1
Supplementary object-presence detection
CLEAR state observed; detection with a hand in front of a sensor was reported
IR sensor 2
Second presence input
CLEAR state observed in the recorded output
The exact environmental sensor model, sampling interval, calibration, sensor library, and cloud transmission behaviour must be confirmed from the final firmware. Environmental monitoring alone cannot establish that a blood bag is safe or has remained within approved storage conditions.
🖥️ OLED Display
The OLED is intended to provide local information at the hardware prototype. It can complement the web interface by showing selected information at the device itself.
The project discussion specified blood_bags.created_at as the source for the blood-bag entry-time display requirement.
The exact final screen layout, display library, communication bus, address, refresh behaviour, and complete pin mapping should be taken from the final firmware and wiring. A database record's creation timestamp should not automatically be interpreted as the exact time a physical bag entered storage unless the application explicitly defines it that way.
🗄️ Database Design
Supabase is the cloud database/backend platform. The known table names are blood_bags and devices.
blood_bags
Property
Description
Purpose
Stores blood-bag information and associated timestamps
created_at
Timestamp used as the entry-time source for the OLED requirement discussed during development
Other columns
The complete column list and types require confirmation from the actual schema
Keys and relationships
Primary key, foreign keys, constraints, and indexes require confirmation
Access control
Row Level Security and database policies require inspection
Timestamp semantics: created_at is used as the record-creation/entry-time source in the discussed integration. It should not automatically be treated as a donation date, collection date, expiry date, or verified physical storage-entry event.
devices
The available schema fragment contained the following fields:
Column
Data type
Known meaning
device_code
text NOT NULL
Device identifier/code
device_name
text NULL
Human-readable device name
status
text NULL
Device status
last_seen_at
timestamp without time zone NULL
Last-seen timestamp
created_at
timestamp without time zone NULL
Record creation timestamp
id
uuid
Present in the fragment; key/default details require confirmation
The available fragment does not establish the full table definition, primary-key constraints, defaults, indexes, foreign keys, or Row Level Security policies.
Database responsibilities
Maintain structured blood-bag records.
Store device identifiers and relevant device metadata.
Provide data access for the backend and web application according to configured permissions.
Support inventory and device-management workflows implemented by the application.
The exact relationship between a bag, a slot/blood group, and a device must be confirmed from the complete schema. No additional columns or foreign-key relationships are assumed here.
☁️ Supabase Backend
Supabase forms the cloud side of BloodLink. The project architecture includes:
Supabase database: Stores the known blood_bags and devices tables.
Supabase Edge Functions: Provide a server-side processing layer for configured workflows.
Web application: Uses the backend/database access path implemented in the application.
ESP8266 connectivity: Provides a network-capable embedded endpoint for configured device-to-cloud communication.
The exact deployed Edge Function names, HTTP methods, endpoint paths, JSON payloads, response formats, authentication settings, and error-handling behaviour cannot be reliably stated without the final function source. They should be documented from the deployed code rather than inferred from a generic Supabase setup.
Security principles
Never expose a Supabase service_role key in browser code or distributed ESP8266 firmware.
Keep privileged credentials in appropriately protected server-side configuration.
Apply least-privilege access and review Row Level Security policies.
Validate incoming backend requests and restrict permitted database operations.
Do not publish Wi-Fi passwords, private keys, API secrets, or real blood-bag/patient records.
🌐 Web Application
The web application provides the centralized software interface for the inventory system. Its purpose is to make supported blood-bag and device information accessible without relying only on the local hardware display.
The application's exact framework, page structure, authentication flow, database queries, and refresh strategy should be verified from its source code. Automatic updates, real-time subscriptions, notifications, and specific management actions should only be described as implemented when confirmed by the actual application.
🧪 Testing and Validation
The project discussion contains the following development observations:
Component
Validation performed / observation
Status
Temperature sensor
Temperature readings obtained through serial output
Observed locally
Humidity sensor
Humidity readings obtained through serial output
Observed locally
IR sensor states
Both sensors displayed CLEAR in the recorded output
Observed locally
IR detection
Detection with a hand placed in front of a sensor was reported
Tested at prototype level
RFID
RFID-based item identification was part of the project workflow
Full database-linked behaviour should be verified from the final source/test record
OLED
Local display integration was part of the project
Exact final display behaviour requires confirmation
Cloud/backend
Supabase database and Edge Functions form part of the architecture
Exact end-to-end request and persistence behaviour requires source/test verification
Web application
Centralized inventory/device interface forms part of the project
Exact UI and synchronization behaviour require source verification
These observations should not be interpreted as proof of calibrated sensor accuracy, long-term reliability, full cloud integration, or production readiness.
🧰 Technology Stack
Technology / component
Role
ESP8266
Embedded controller and Wi-Fi connectivity
RFID reader and tags
Individual tag identification
Temperature/humidity sensor
Environmental monitoring
IR sensors
Supplementary object-presence detection
OLED
Local information display
Wi-Fi
Network communication
Supabase
Cloud database and backend platform
Supabase Edge Functions
Server-side processing for configured workflows
Web application
Centralized inventory and device interface
The exact firmware language, peripheral libraries, web framework, and package manager are not listed here because the final source manifests were not available for verification. This avoids adding technologies merely because they are commonly used in similar projects.
⚠️ Limitations
BloodLink is an academic prototype, not a certified blood-storage, blood-safety, or clinical decision-making system.
Temperature and humidity readings alone cannot establish blood safety or transfusion suitability.
Sensor calibration, measurement accuracy, sampling behaviour, and fault detection require validation.
Network and power failure handling, local buffering, retries, and data recovery need testing before any operational deployment.
Exact pin assignments, peripheral models, full schema, API contracts, and web-app behaviour must be confirmed from the final source artifacts.
Database permissions, authentication, device identity, and secret handling require security review before using sensitive data.
Real blood-bank deployment requires approved workflows, traceability, qualified personnel, and compliance with applicable regulations.
🚀 Future Improvements
Improvement
Potential benefit
Configurable environmental threshold alerts
Notify authorized users when readings exceed validated limits
Device health monitoring
Improve visibility into connectivity, last-seen time, and sensor faults
Inventory analytics
Summarize stock by blood group, storage location, and inventory state
Audit logs
Track inventory changes and the actors responsible
Stronger access control
Improve role separation and database permissions
Resilient network handling
Add retries, local buffering, and safe recovery from disconnection
Automated tests
Validate RFID processing, sensor logic, API requests, and database operations
Calibration and reliability studies
Establish documented measurement performance and operating limits
Better traceability
Represent inventory events and timestamps explicitly
Complete deployment documentation
Publish verified wiring, pin mappings, schema, and API contracts
These are potential future improvements, not claims that the features are already implemented.
🎯 Project Summary
BloodLink brings together RFID-based item identification, environmental sensing, IR presence detection, ESP8266 connectivity, OLED feedback, Supabase-backed data management, and a web application in one academic prototype.
Its four-slot demonstration represents the four blood groups while allowing multiple individual blood bags per slot. The project demonstrates the integration of embedded systems, IoT communication, cloud data handling, and inventory-management software.
BloodLink is intended to demonstrate smart inventory monitoring—not to replace certified blood-bank equipment, validated storage procedures, or clinical decision-making.

BloodLink · IoT · RFID · ESP8266 · Supabase · Inventory Monitoring