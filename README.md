# IoT_Environment_Monitor
ESP32 + DHT11 | Firmware Devlopment | Cloud Integration (HiveMQ Cloud) | Edge Integration | FreeRTOS | IoT prototype


**Author:** Mahidul Islam Nihad  
**Email:** mahidulislamnihad28@gmail.com  

ESP32 + DHT11 IoT prototype demonstrating:
- Local data acquisition
- Secure MQTT (HiveMQ Cloud) with JSON payloads
- Real-time multitasking using FreeRTOS

---

## 🌱 Project stages
| Stage | Folder | Description |  Status |
|--------|---------|-------------| -------------|
| 1 | firmware_local | Reads DHT11 & prints temperature/humidity | Completed |
| 2 | firmware_mqtt_hivemq | Sends JSON data to HiveMQ Cloud | Completed |
| 3 | firmware_freertos | FreeRTOS tasks: sensor + publisher | Completed |
| 4 | On Device ML | TinyML Tensorflow Lite Based Environmental Classification | Created Different Repo [On_Device_ML](https://github.com/mahidulislamnihad/On_Device_ML) |
---

## 🚀 Quick Start
1. Copy `credentials.example.h` → rename to `credentials.h` → fill your Wi-Fi & MQTT info.  
2. Open Arduino IDE → Board: *ESP32 Dev Module* → upload the `.ino`.  
3. Use MQTTx to subscribe to topic `esp32/env_data` and view JSON payloads.  

---

## 📸 Screenshots
See `/docs/screenshots` for Serial output and MQTT dashboard images.

---

## 🔒 Notes
AWS IoT Core integration not tested due to account limits; HiveMQ Cloud used as industrial-grade MQTT broker.
