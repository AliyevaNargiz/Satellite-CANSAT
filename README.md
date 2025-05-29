# CANSAT Azerbaijan 2023 & CANSAT US 2024 - CanStellar Team Project

## Project Overview
This repository contains the complete codebase for the CanStellar team's winning entry in the CANSAT Azerbaijan 2023 model satellite competition by AzerCosmos and CANSAT Global in United Stated 2024. The project consists of two main components: the Satellite System and the Ground Station (YIS).

## Project Structure
```
Satellite-CANSAT-main/
├── TelemetriyaArduino/    # Satellite onboard software
│   └── telemetriya/       # Arduino code for satellite operations
├── YIS/                   # Ground Station software
│   ├── UI/                # Frontend interface
│   └── CanStellarBack/    # Backend server (.NET)
└── pom.xml                # Maven project configuration
```

## Components

### 1. Satellite System (TelemetriyaArduino)
- **Hardware Requirements:**
  - Arduino-compatible board
  - MPL3115A2 altitude sensor
  - GPS module
  - XBee radio module
  - SD card module
  
- **Features:**
  - Real-time altitude measurement
  - GPS location tracking
  - Data transmission via XBee
  - Local data logging to SD card
  - Automated status monitoring
  - Time synchronization

### 2. Ground Station (YIS)
- **Backend (CanStellarBack):**
  - Built with .NET
  - Uses SignalR for real-time data updates
  - Processes telemetry data from CSV format
  - Provides REST API endpoints

- **Frontend (UI):**
  - Web-based interface
  - Real-time telemetry display
  - Data visualization
  - Mission status monitoring

## Data Flow
1. Satellite collects sensor data (altitude, GPS, etc.)
2. Data is transmitted via XBee radio and stored on SD card
3. Ground station receives data and stores in CSV format
4. Backend processes data and sends to frontend via SignalR
5. Frontend displays real-time updates

## Setup Requirements

### Satellite Setup
1. Install required Arduino libraries:
   - SD
   - SPI
   - Wire
   - EEPROM
   - TimeLib
   - DS1302
   - Adafruit_Sensor
   - Adafruit_MPL3115A2

2. Configure hardware pins according to the Arduino code
3. Upload code to Arduino board

### Ground Station Setup
1. Backend (.NET):
   ```bash
   cd YIS/CanStellarBack
   dotnet restore
   dotnet run
   ```

2. Frontend (UI):
   - Serve the UI files using any web server
   - Configure to point to backend server

## Data Format
The system expects telemetry data in the following format:
```csv
TeamID,WorkingTime,PackageCount,Voltage,Altitude,Speed,Latitude,Longitude,SeparationTime,VideoDuration
```

## Notes
- The Ground Station (YIS) depends on data from the satellite system
- Data is stored in CSV format at `./data/data.csv`
- Real-time updates occur approximately every 1 second
- XBee communication uses 9600 baud rate

## Awards
- 1st Place in CANSAT Azerbaijan 2023
- Prize: 6000 AZN
- First team from Azerbaijan in Global CANSAT US 2024

## Contributors
- Team CanStellar

For more information or questions, please reach naraliyeva1722@outlook.com
