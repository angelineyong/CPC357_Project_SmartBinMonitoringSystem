# CPC357_Project_SmartBinMonitoringSystem

# Smart Bin Monitoring System (ESP32)

## Overview

The **Smart Bin Monitoring System** is an Internet of Things (IoT) solution designed to improve waste management efficiency by monitoring trash levels, environmental conditions, and user interaction in real time. The system is built using an **ESP32 microcontroller** integrated with multiple sensors and actuators to collect data, provide visual feedback, and automatically control the bin lid.

This project demonstrates how embedded systems, sensors, actuators, and cloud-based data storage can be combined to build a practical smart city application. The system is suitable for deployment in environments such as campuses, offices, public areas, and residential buildings.

Collected sensor data is transmitted to **InfluxDB Cloud** for data storage and future analysis. We uses **Grafana** as the visualization tool to monitor the sensor values

---

## System Features

### 1. Trash Level Monitoring
- Uses an **ultrasonic distance sensor** mounted at the top of the bin
- Measures the distance to the trash surface
- Calculates the **trash fill percentage**
- Provides real-time bin status

---

### 2. Visual Trash Level Indication
Three LEDs indicate the current trash level:
- **Green LED** – Bin is empty or at a low fill level (0–33%)
- **Yellow LED** – Bin is partially filled (34–66%)
- **Red LED** – Bin is almost full or full (67–100%)

This allows users and maintenance staff to quickly identify bin status without accessing a digital dashboard.

---

### 3. Automatic Lid Control (PIR + Servo)
- A **PIR motion sensor** detects user presence
- When motion is detected:
  - A **servo motor rotates 90°** to open the bin lid
  - The lid remains open for a predefined duration
  - The lid closes automatically afterward
- Enables **touchless operation**, improving hygiene and usability

---

### 4. Environmental Monitoring
- **MQ135 gas sensor** monitors changes in air quality around the bin
- **DHT11 temperature sensor** measures ambient temperature
- Provides environmental data that can help identify:
  - Gas buildup
  - Poor ventilation
  - Abnormal conditions around waste areas

---

### 5. Cloud Data Logging (InfluxDB)
- Sensor readings are sent securely to **InfluxDB Cloud**
- Logged parameters include:
  - Trash fill percentage
  - Gas sensor values
  - Temperature
  - Motion detection state
- Enables:
  - Real-time dashboards
  - Historical data analysis
  - Future system optimization

---

### 6. WiFi-Enabled IoT Architecture
- ESP32 connects to a WiFi network for internet access
- Uses HTTPS-based API communication
- Designed to support scalability and future cloud integration

---

## Applications
- Smart campus waste management
- Smart city infrastructure
- Office and building automation
- Public hygiene monitoring
- Educational IoT demonstrations

---

## Summary

The Smart Bin Monitoring System integrates sensing, automation, and cloud connectivity into a single embedded platform. By combining trash level detection, visual indicators, touchless interaction, environmental monitoring, and cloud-based data storage, the system demonstrates a practical and scalable approach to smart waste management.