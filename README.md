# 🚗 AI-Based Driver Drowsiness Detection and Automatic Speed Control System with Alcohol Monitoring

<p align="center">
  <strong>An IoT-enabled driver safety system for drowsiness detection, alcohol monitoring, and automatic vehicle speed control.</strong>
</p>

---

## 🌐 Local Demo

The project provides a local web interface for monitoring the system.

**Local Project URL:**

`http://10.51.227.152/`

> **Note:** This is a private/local network address and may only be accessible when the device hosting the project and the user device are connected to the same network.

---

## 📌 Project Overview

The **AI-Based Driver Drowsiness Detection and Automatic Speed Control System with Alcohol Monitoring** is an embedded and IoT-based safety system designed to improve driver safety by monitoring driver alertness, detecting alcohol presence, and controlling vehicle speed based on safety conditions.

The system uses an **IR Eye Blink Sensor** to detect prolonged eye closure, an **MQ-3 Alcohol Sensor** to monitor alcohol presence, and an **HC-SR04 Ultrasonic Sensor** for distance detection.

A **NodeMCU ESP8266** acts as the main controller and processes the sensor inputs. Based on the detected conditions, the system controls the vehicle motor through an **L298N Motor Driver Module** and displays relevant information on a **16×2 LCD Display**.

---

## 🎯 Objectives

* Detect driver drowsiness using an IR Eye Blink Sensor.
* Monitor alcohol presence using an MQ-3 Alcohol Sensor.
* Detect obstacles or nearby objects using an HC-SR04 Ultrasonic Sensor.
* Automatically control vehicle speed according to safety conditions.
* Display system status on a 16×2 LCD.
* Develop a compact prototype for intelligent vehicle safety applications.

---

## ✨ Key Features

* 👁️ **Drowsiness Detection**
* 🍷 **Alcohol Monitoring**
* 🚗 **Automatic Speed Control**
* 📏 **Distance Detection**
* 📺 **LCD Status Display**
* ⚡ **ESP8266-Based Control**
* 🔧 **Embedded Hardware Prototype**
* 🛡️ **Driver Safety Monitoring**

---

## 🧠 System Working

The system continuously monitors the driver and surrounding conditions through multiple sensors.

### Working Flow

```text
IR Eye Blink Sensor
        │
        ▼
Drowsiness Detection
        │
        ▼
   NodeMCU ESP8266
        │
        ├──────────────► L298N Motor Driver
        │                       │
        │                       ▼
        │                DC Geared Motor
        │
        ├──────────────► 16×2 LCD Display
        │
        ├──────────────► MQ-3 Alcohol Sensor
        │
        └──────────────► HC-SR04 Ultrasonic Sensor
```

---

## 🔧 Hardware Components

| Component                          | Purpose                                |
| ---------------------------------- | -------------------------------------- |
| **NodeMCU ESP8266**                | Main microcontroller                   |
| **L298N Motor Driver Module**      | Controls the DC motor                  |
| **DC Geared Motor with Wheel**     | Vehicle movement                       |
| **18650 Lithium-Ion Battery Pack** | Power supply                           |
| **16×2 LCD Display**               | Displays system status                 |
| **HC-SR04 Ultrasonic Sensor**      | Distance detection                     |
| **MQ-3 Alcohol Sensor**            | Alcohol monitoring                     |
| **IR Eye Blink Sensor**            | Drowsiness detection                   |
| **Breadboard**                     | Circuit prototyping                    |
| **Jumper Wires**                   | Electrical connections                 |
| **USB Power Cable**                | Power/programming connection           |
| **Safety Goggles**                 | Mounting platform for eye blink sensor |

---

## 💻 Technologies Used

* **Embedded Systems**
* **IoT**
* **NodeMCU ESP8266**
* **Embedded C / Arduino Programming**
* **Sensor Interfacing**
* **Motor Control**
* **LCD Interfacing**

---

## 🏗️ System Architecture

The system follows a sensor-based decision-making architecture:

```text
          SENSOR INPUTS
               │
     ┌─────────┼─────────┐
     │         │         │
     ▼         ▼         ▼
   IR Eye     MQ-3     HC-SR04
   Sensor     Sensor    Sensor
     │         │         │
     └─────────┼─────────┘
               │
               ▼
       NodeMCU ESP8266
               │
       ┌───────┴───────┐
       │               │
       ▼               ▼
  LCD Display    L298N Motor Driver
                       │
                       ▼
                DC Geared Motor
```

---

## 📊 Project Prototype

The project prototype integrates the sensors, NodeMCU ESP8266, motor driver, LCD display, battery supply, and DC geared motor into a compact vehicle safety model.

### Prototype

![Project Prototype](images/prototype.jpg)

---

## 📁 Project Structure

```text
AI-Driver-Drowsiness-Speed-Control/
│
├── README.md
│
├── src/
│   └── driver_drowsiness_speed_control.ino
│
├── docs/
│
├── images/
│   └── prototype.jpg
│
└── report/
```

---

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/kittyyash/AI-Driver-Drowsiness-Speed-Control.git
```

### 2. Open the Project

Open the project folder in **Arduino IDE** or **Visual Studio Code**.

### 3. Connect the Hardware

Connect the NodeMCU ESP8266, sensors, LCD, motor driver, and motor according to the circuit design provided in the `docs` folder.

### 4. Upload the Code

Open:

```text
src/driver_drowsiness_speed_control.ino
```

Select the appropriate ESP8266 board and upload the program.

### 5. Access the Local Interface

After the system is running and connected to the same network, open:

```text
http://10.51.227.152/
```

in a web browser.

---

## 🔄 System Workflow

```text
START
  │
  ▼
Initialize NodeMCU and Sensors
  │
  ▼
Read Eye Blink Sensor
  │
  ▼
Drowsiness Detected?
  │
 ┌┴───────────────┐
Yes               No
 │                 │
 ▼                 ▼
Control Speed    Continue Monitoring
 │
 └──────────┐
            ▼
      Read MQ-3 Sensor
            │
            ▼
     Alcohol Detected?
            │
       ┌────┴────┐
      Yes        No
       │          │
       ▼          ▼
 Safety Response Continue
       │
       └──────┬──────┘
              ▼
       Read Distance
              │
              ▼
       Control Motor
              │
              ▼
       Update LCD
              │
              ▼
      Continue Monitoring
```

---

## 📈 Expected Outcome

The prototype is designed to:

* Identify signs of driver drowsiness.
* Monitor alcohol presence.
* Detect nearby obstacles.
* Control the motor according to predefined safety conditions.
* Provide system information through the LCD display.
* Provide a local interface for monitoring the project.

---

## 🔮 Future Enhancements

Future versions of the system can include:

* 📱 Mobile application integration
* ☁️ Cloud-based monitoring
* 📊 Real-time IoT dashboard
* 📍 Advanced vehicle tracking
* 🤖 Machine-learning-based drowsiness analysis
* 📷 Camera-based driver monitoring
* 📈 Data logging and analytics
* 🔔 Advanced safety notification mechanisms

---

## 🎓 Academic Project

**Project Title:**
**AI-Based Driver Drowsiness Detection and Automatic Speed Control System with Alcohol Monitoring**

**Domain:** Electrical and Electronics Engineering / Embedded Systems / IoT

---

## 👩‍💻 Author

**Dharunya**

🔗 GitHub: [@kittyyash](https://github.com/kittyyash)

---

## 📜 License

This project is intended for **academic, educational, and research purposes**.

---

<p align="center">
  ⭐ If you find this project useful, consider giving the repository a star!
</p>
