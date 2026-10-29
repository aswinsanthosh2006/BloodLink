BloodLink — Smart Blood Bank Monitoring and Management System
BloodLink is an IoT-based academic prototype designed to demonstrate how blood-bank inventory can be monitored and managed using RFID identification, environmental sensing, embedded devices, cloud services, and a web application.
The system combines ESP8266 microcontrollers, RFID readers and tags, temperature and humidity monitoring, IR sensors, an OLED display, and Supabase as the cloud database and backend platform. A web application provides a centralized interface for blood-bag and device information.
The physical demonstration is organized around four storage slots representing the four blood groups. Each slot can accommodate multiple blood-bag records; RFID identifies an individual tagged bag rather than treating the slot as the bag itself.
Project status: Completed academic prototype. BloodLink demonstrates inventory-monitoring concepts and is not a certified blood-storage, blood-safety, or clinical decision-making system.
1. Project Overview
Blood banks need reliable ways to identify individual units, maintain organized inventory records, and monitor storage conditions. BloodLink explores how an IoT system can connect physical storage and tagged items with centralized digital records.
RFID provides an identification mechanism for individual blood bags. Environmental sensors measure temperature and humidity, while IR sensors provide supplementary object-presence detection. ESP8266 boards interface with the hardware and provide Wi-Fi connectivity. Supabase supplies cloud database and backend capabilities, and the web application is intended to make inventory and device information accessible through a centralized interface. An OLED display provides local feedback at the device.
The project brings together three major areas:
Embedded systems: Microcontrollers, sensors, RFID readers, and a local display.
Cloud-connected IoT: Wi-Fi communication and Supabase-backed data services.
Inventory management software: Structured blood-bag and device records presented through a web application.
BloodLink is a prototype for demonstrating identification, monitoring, and digital inventory workflows. It does not replace approved blood-bank procedures or clinical verification.
2. Project Objectives
Identify individual tagged blood bags using RFID.
Demonstrate a four-slot arrangement representing the four blood groups.
Monitor environmental temperature and humidity.
Detect object presence using IR sensors.
Provide local information through an OLED display.
Connect embedded hardware to cloud services through Wi-Fi.
Store blood-bag and device information in a structured database.
Provide a web-based interface for centralized inventory monitoring and record management.
3. Main Features and Implementation Status
Feature
Purpose
Status supported by available project information
RFID identification
Read a tag identifier associated with an individual blood bag
RFID-based identification is part of the project workflow; complete RFID-to-database behaviour should be verified against the final source
Four-slot arrangement
Represent the four blood groups in the physical demonstration
Confirmed project design; each slot can represent multiple bags
Temperature monitoring
Observe storage-area temperature
Temperature readings were obtained through serial output during development
Humidity monitoring
Observe environmental humidity
Humidity readings were obtained through serial output during development
IR sensors
Provide supplementary object-presence detection
Two sensors were tested; clear states were observed and detection with a hand in front of a sensor was reported
OLED display
Present selected information locally
OLED integration was part of the project; exact final screen behaviour depends on the completed firmware
Supabase database
Store blood-bag and device records
blood_bags and devices are known table names from the project discussion
Supabase Edge Functions
Support server-side processing and hardware/backend workflows
Included in the architecture; exact deployed function contracts require the source code for verification
Web application
Centralize access to inventory and device information
Part of the project; exact framework and refresh behaviour are not established by the available details
Environmental alerts
Notify users when configured conditions are exceeded
Treat as future work unless confirmed in the final implementation
4. System Architecture
The system is organized into hardware, embedded processing, communication, cloud backend, database, and web application layers.
flowchart TB
    subgraph HW["Hardware and Sensing Layer"]
        RFID["RFID Reader and Tags"]
        ENV["Temperature and Humidity Sensor"]
        IR["IR Sensor 1 and IR Sensor 2"]
        OLED["OLED Display"]
    end

    subgraph MCU["Embedded Layer"]
        ESP["ESP8266 Firmware"]
    end

    subgraph NET["Communication Layer"]
        WIFI["Wi-Fi Network"]
    end

    subgraph CLOUD["Cloud Backend Layer"]
        EDGE["Supabase Edge Functions"]
        DB[("Supabase Database")]
    end

    subgraph APP["Application Layer"]
        WEB["Web Application"]
    end

    RFID --> ESP
    ENV --> ESP
    IR --> ESP
    ESP --> OLED
    ESP -. "Network requests, according to firmware configuration" .-> WIFI
    WIFI -.-> EDGE
    EDGE -. "Backend database operations" .-> DB
    WEB -. "Database access according to application configuration" .-> DB
    WEB -. "Function calls where configured" .-> EDGE
Architecture overview
Hardware and sensing: RFID reads tag identifiers; the environmental sensor measures temperature and humidity; IR sensors provide presence states.
Embedded processing: ESP8266 firmware interfaces with the connected modules and handles the device-side logic.
Local feedback: The OLED is intended to show selected information without requiring the operator to consult the web interface.
Network communication: The ESP8266 provides Wi-Fi connectivity for communication with cloud services.
Cloud backend: Supabase Edge Functions can perform server-side operations for requests from supported clients.
Database: Supabase stores the blood-bag and device records used by the system.
Web application: The application provides a centralized view of the information and management workflows supported by its implementation.
The diagram describes the system's logical architecture. Exact request paths, database operations, and automatic synchronization behaviour depend on the final firmware, Edge Function, and web-application source. Local sensor readings alone do not prove that every measurement is uploaded to the cloud.
5. Hardware Components
Component
Role in BloodLink
Interface details
ESP8266 development board(s)
Embedded processing and Wi-Fi connectivity
GPIO and Wi-Fi; exact board model and pin assignments must be taken from the final firmware
RFID reader module
Reads identifiers from compatible RFID tags
Exact reader model, communication bus, library, and pin mapping need confirmation from the source
RFID tags/cards
Provide identifiers for individual tagged bags
Read wirelessly by the compatible RFID reader
Temperature and humidity sensor
Measures environmental conditions
Exact sensor model, interface, library, and pin assignment need confirmation
IR sensor 1
Detects nearby object presence
Exact module, output polarity, and pin mapping need confirmation
IR sensor 2
Provides a second object-presence input
Exact module, output polarity, and pin mapping need confirmation
OLED display
Shows selected device or inventory information locally
Exact display controller, bus, address, and pin mapping need confirmation
Storage-slot demonstration
Organizes the physical model into four blood-group categories
Each slot may represent multiple individual blood bags
Hardware configuration note
The project discussion confirms the component categories, but the complete final firmware and wiring diagram are not available in the material used to prepare this document. For that reason, exact GPIO numbers, module models, and library names are not guessed here. The tested firmware and physical wiring should be treated as the source of truth for those details.
6. Technology Stack
Layer
Technology
Role
Embedded controller
ESP8266
Reads connected peripherals and provides Wi-Fi connectivity
Identification
RFID reader and tags
Identifies tagged items
Environmental monitoring
Temperature/humidity sensor
Measures environmental conditions
Presence detection
Two IR sensors
Provides supplementary object-presence states
Local interface
OLED display
Provides device-side feedback
Network
Wi-Fi
Connects the embedded device to network services
Cloud platform
Supabase
Provides the database and backend platform
Server-side processing
Supabase Edge Functions
Supports backend operations
Data storage
Supabase database
Stores blood-bag and device records
User interface
Web application
Provides centralized inventory and device information
The exact firmware language, board package, peripheral libraries, web framework, and package manager should be documented from the final source files rather than inferred from common choices for similar projects.
7. Working Principle
7.1 Blood-bag identification with RFID
Each tagged blood bag can be identified by presenting its RFID tag to a compatible reader. The reader obtains the tag identifier, which the firmware can use to identify the corresponding bag record according to the implemented lookup and backend logic.
RFID is useful for distinguishing individual tagged items without requiring each bag to be identified by its physical slot alone. The four slots represent blood-group categories in the demonstration, while multiple individual bag records may belong to a slot.
A tag identifier does not independently verify the bag's blood group, compatibility, expiry, authenticity, or clinical suitability. Those properties require reliable records and approved verification procedures.
7.2 Temperature and humidity monitoring
The environmental sensor provides temperature and humidity readings to the connected microcontroller. During development, readings were observed through the serial monitor. An example recorded output was:
Temperature: 31.80 C
Humidity: 75.00 %
These are example development readings, not calibrated reference measurements and not evidence that suitable blood-storage conditions were maintained. The exact sensor model, sampling interval, calibration, and any cloud-transmission behaviour should be confirmed from the final firmware.
7.3 IR sensor operation
Two IR sensors were used for supplementary object-presence detection. The development serial output included:
IR Sensor 1: CLEAR
IR Sensor 2: CLEAR
The project discussion also records that detection was observed when a hand was placed in front of a sensor. The exact relationship between the electrical output and the displayed state depends on the sensor module and firmware logic.
IR detection is separate from RFID identification: an IR sensor can indicate that an object is present, while RFID can provide an identifier for a compatible tagged item.
7.4 ESP8266 processing and connectivity
The ESP8266 is the embedded connection point for the attached peripherals. It reads the local inputs and can use Wi-Fi to communicate with backend services. The exact events sent, request formats, retry behaviour, and error handling are defined by the final firmware and should not be inferred solely from the architecture.
7.5 OLED feedback
The OLED is intended to present selected information locally, allowing the hardware to provide feedback without relying exclusively on the web application. The project discussion specified blood_bags.created_at as the source for the blood-bag entry-time display requirement. The precise final display layout and refresh logic should be confirmed from the completed OLED firmware.
7.6 Cloud database and web application
Supabase is used as the cloud database/backend platform. The known database tables include blood_bags and devices. Supabase Edge Functions form part of the backend architecture, while the web application provides a centralized interface for inventory and device information.
The precise API contracts, access-control rules, and synchronization behaviour depend on the deployed functions and application code. Real-time updates or automatic uploads should only be claimed where the implementation confirms them.
8. Database Design
The database supports two known parts of the system: blood-bag information and device information. The available project discussion contains only partial schema details, so this section documents the known fields without inventing missing columns, keys, constraints, or relationships.
8.1 blood_bags
Property
Description
Table name
blood_bags
Purpose
Stores blood-bag information and associated timestamps
created_at
Used as the blood-bag entry-time source for the OLED integration requirement discussed during development
Other columns
The complete column list and data types must be confirmed from the final Supabase schema
Keys and constraints
Primary key, foreign keys, uniqueness, and indexes require verification
Access policies
Row Level Security and database policies require verification
Timestamp meaning: created_at is the database record's creation timestamp as used by the discussed integration. It should not automatically be interpreted as the donation time, collection time, expiry time, or exact physical arrival time unless the application explicitly defines it that way.
8.2 devices
The known schema fragment for devices included these columns:
Column
Type
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
Present in the schema fragment; key/default/constraint details require verification
The available schema fragment does not establish all constraints or the complete table definition. The primary key, default values, indexes, foreign keys, and RLS policies must be checked in the actual Supabase schema.
8.3 Data relationships
The intended data model distinguishes individual blood-bag records from devices and from the four physical demonstration slots. The exact database relationship between a bag, its slot or blood group, and a device cannot be confirmed from the available schema fragment.
For an accurate final schema description, inspect the actual database or migration files and verify:
All table columns, data types, nullability, and defaults.
Primary and foreign keys.
Constraints and indexes.
Relationships between bags, slots, and devices.
Row Level Security settings and policies.
Timestamp generation and time-zone assumptions.
9. Backend and Web Application
Supabase backend
Supabase provides the cloud database and backend services used by BloodLink. Supabase Edge Functions are part of the architecture for server-side processing and hardware communication workflows.
The exact deployed function names, request methods, URL paths, payload fields, response formats, authentication configuration, and error handling are not included in the verified project details available here. They should be documented from the actual function source rather than replaced with guessed API examples.
Web application
The web application is intended to provide a centralized interface for blood-bag records and device information. It complements the embedded hardware by presenting information in a form suitable for inventory review and record management.
The exact framework, page structure, authentication flow, database queries, and update mechanism require confirmation from the application source. The existence of a web application does not by itself establish live synchronization, push notifications, or real-time subscriptions.
10. Security Considerations
BloodLink is an academic prototype, but the following principles matter when handling its cloud backend:
Do not commit Wi-Fi passwords, private keys, access tokens, or real service credentials.
Never expose a Supabase service_role key in browser code or distributed ESP8266 firmware.
Use appropriately scoped client credentials and enforce access through authentication and database policies.
Review Row Level Security policies for every table exposed to client access.
Validate backend requests and restrict each operation to the data and permissions it actually needs.
Avoid publishing real blood-bag records or any patient-identifying information.
Treat credentials embedded in firmware as potentially extractable.
Rotate credentials if they are accidentally exposed in source control.
The actual authentication and Row Level Security configuration must be inspected before making claims about the deployed system's security.
11. Testing and Validation
The following table summarizes the observations recorded during development. Local tests are distinguished from full cloud-to-web integration tests.
Component
Test or observation
Result
Temperature monitoring
Read the sensor and inspect serial output
Temperature readings were obtained; an example was 31.80 C
Humidity monitoring
Read the sensor and inspect serial output
Humidity readings were obtained; an example was 75.00 %
IR sensor states
Inspect serial output for both sensors
Both sensors displayed CLEAR in the recorded observation
IR detection
Place a hand in front of a sensor
Correct detection was reported during testing
RFID workflow
Use RFID tags to identify individual items
RFID-based identification was part of the project; exact end-to-end database behaviour requires verification against the final source
OLED
Integrate a display for local information
Integration was discussed; exact final output requires confirmation from the completed firmware
Supabase and web application
Review the cloud/database/application workflow
The components are part of the project architecture; complete end-to-end test evidence is not established by the details available here
These observations demonstrate local sensor testing, but they do not establish calibrated sensor accuracy, long-term reliability, successful operation during network outages, or complete end-to-end validation of every cloud workflow.
12. Limitations
BloodLink is an academic prototype, not a certified blood storage or clinical system.
Environmental readings alone cannot establish blood safety or suitability for transfusion.
Sensor accuracy, calibration, placement, sampling intervals, and fault detection require further validation.
Network failure, power interruption, retry behaviour, local buffering, and data recovery require appropriate testing before operational deployment.
The complete final firmware, database schema, Edge Function contracts, and web-app implementation should be used to verify exact technical details.
Security policies, authentication, device identity, and access permissions require review before use with sensitive data.
Real blood-bank deployment would require approved workflows, traceability, qualified personnel, and compliance with applicable standards and regulations.
13. Future Improvements
Potential extensions to BloodLink include:
Environmental threshold alerts: Configurable limits and notifications when monitored conditions fall outside validated ranges.
Device health monitoring: More complete tracking of connectivity, last-seen time, sensor faults, and device status.
Inventory reporting: Summaries of stock by blood group, storage location, and inventory state.
Audit logs: Traceable records of inventory changes and the users or devices responsible.
Improved access controls: Role-based permissions and thoroughly tested database policies.
Network resilience: Safe retry logic, local buffering, duplicate-request protection, and recovery after connectivity loss.
Automated tests: Repeatable tests for RFID parsing,