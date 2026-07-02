# Wireless-Sensor-Logger-with-Offline-Failover-Buffering
Dual ESP32 wireless sensor logger using ESP-NOW with offline buffering and automatic ThingSpeak cloud synchronization.

# 📡 Wireless Sensor Logger with Offline Failover Buffering

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue)
![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D)
![ESP-NOW](https://img.shields.io/badge/Communication-ESP--NOW-orange)
![ThingSpeak](https://img.shields.io/badge/Cloud-ThingSpeak-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 📖 Overview

The **Wireless Sensor Logger with Offline Failover Buffering** is an ESP32-based IoT project designed to ensure reliable wireless data logging even when internet connectivity is unavailable.

The system consists of two ESP32 development boards:

- **Transmitter ESP32** reads analog values from a **10kΩ potentiometer**.
- **Receiver ESP32** receives the data using **ESP-NOW** and uploads it to **ThingSpeak** over Wi-Fi.

If the Wi-Fi connection is interrupted, the receiver temporarily stores the received data in an offline buffer. Once the internet connection is restored, all buffered readings are automatically uploaded to ThingSpeak, preventing data loss.

This project demonstrates reliable wireless communication, cloud integration, and fault-tolerant IoT system design.

---

# ✨ Features

- 📡 ESP-NOW wireless communication
- 🌐 Real-time cloud monitoring using ThingSpeak
- 💾 Offline data buffering during Wi-Fi outages
- 🔄 Automatic synchronization after reconnection
- 📊 Live sensor data visualization
- ⚡ Reliable and low-latency communication
- 🔋 Low-power ESP32 operation
- ✅ Fault-tolerant wireless data logging

---

# 🏗️ System Architecture

```
10kΩ Potentiometer
        │
        ▼
+-------------------+
| ESP32 Transmitter |
+-------------------+
        │
     ESP-NOW
        │
        ▼
+----------------+
| ESP32 Receiver |
+----------------+
        │
        ▼
   Wi-Fi Network
        │
        ▼
   ThingSpeak Cloud
```

When Wi-Fi is unavailable:

```
ESP32 Receiver
      │
      ▼
Offline Buffer
      │
      ▼
Wi-Fi Reconnected
      │
      ▼
ThingSpeak Cloud
```

---

# 🛠 Hardware Components

| Component | Quantity |
|-----------|----------|
| ESP32 Development Board | 2 |
| 10kΩ Potentiometer | 1 |
| Breadboard | 1 |
| Jumper Wires | As required |
| USB Cable | 2 |

---

# 💻 Software Used

- Arduino IDE
- ESP32 Arduino Board Package
- ESP-NOW Library
- WiFi Library
- ThingSpeak Cloud Platform

---

# ⚙️ Working Principle

1. The transmitter ESP32 reads the analog value from the potentiometer.
2. The sensor value is transmitted wirelessly using ESP-NOW.
3. The receiver ESP32 receives the data.
4. If Wi-Fi is available, the receiver uploads the reading to ThingSpeak.
5. If Wi-Fi is unavailable, the reading is stored in an offline buffer.
6. Once Wi-Fi reconnects, all buffered data is automatically uploaded to ThingSpeak.

---

# 📂 Project Structure

```
Wireless-Sensor-Logger-With-Offline-Failover-Buffering
│
├── Transmitter
│   └── Transmitter.ino
│
├── Receiver
│   └── Receiver.ino
│
├── Get_mac_address
│   └── Get_mac_address.ino
│
├── images
│   └── Block diagram.png
│
├── docs
│   └── Wireless Sensor Logger with Offline Failover Buffering.pdf
│
└── README.md
```

---

# 📸 Project Images

### Block Diagram

> Add your block diagram image here.

```
images/Block diagram.png
```

# 📊 Applications

- Smart Agriculture
- Environmental Monitoring
- Industrial IoT
- Wireless Sensor Networks
- Remote Data Logging
- Academic & Research Projects

---

# 🚀 Future Improvements

- Support multiple analog sensors
- SD Card backup storage
- MQTT cloud integration
- OLED display
- Mobile application
- OTA firmware updates
- Data encryption
- Battery-powered operation

---

# 🎯 Skills Demonstrated

- Embedded Systems
- ESP32 Programming
- Arduino Programming
- ESP-NOW Communication
- Internet of Things (IoT)
- ThingSpeak Cloud Integration
- Wireless Sensor Networks
- Data Logging
- Fault-Tolerant System Design

---

# 👨‍💻 Author

**Ranjith**

Bachelor of Engineering (Electronics and Communication Engineering)

**Areas of Interest**

- Embedded Systems
- Internet of Things (IoT)
- Firmware Development
- PCB Design
- Electronics Hardware

---

# 📄 License

This project is licensed under the **MIT License**.

---

## ⭐ Support

If you found this project helpful, please consider giving it a **⭐ Star** on GitHub.
