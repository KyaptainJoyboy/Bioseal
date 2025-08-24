# 🧪 Bioseal System

A smart fermentation monitoring system designed for the **Arduino Nano** platform.  
It uses the **SCD40/41** sensor to measure **CO₂ concentration**, **temperature**, and **humidity**, displaying the results in real-time on an **SSD1306 OLED** via **U8g2** and **Adafruit SSD1306** libraries.

---

## 📜 Project Information

**Proponents:**
- Zachiebelle Avril Canapi  
- Princess Joy Garcia  
- Clare Elisha Antonio  

**Programmed & Embedded by:**
- Kian Clarel R. Bunagan  

---

## 📖 Description

Bioseal continuously monitors fermentation environments, offering instant readings for:

- **CO₂ concentration** (ppm)
- **Temperature** (°C)
- **Humidity** (%RH)
- **Fermentation Status** (calculated from CO₂ thresholds)

The system is built to be compact, memory-efficient, and optimized for the Arduino Nano, ensuring fast startup and reliable data updates.

---

## ⚙️ Features

- Real-time CO₂, temperature, and humidity monitoring  
- OLED-based data display with **U8g2** for optimized memory usage  
- Minimal boot delay for near-instant readings  
- Threshold-based fermentation status indicator  
- Designed specifically for **Arduino Nano** hardware limitations  

---

## 🛠️ Hardware Requirements

- **Arduino Nano** (ATmega328P-based)
- **SCD40 or SCD41** CO₂ sensor
- **SSD1306 OLED Display** (128x32 or 128x64, I²C)
- Breadboard & jumper wires
- USB power supply

---

## 💻 Software Requirements

- **Arduino IDE** (v1.8.19 or Arduino IDE 2.x)
- Required Libraries:
  - `Wire.h` – I²C communication
  - `U8g2lib.h` – Efficient graphics for OLED
  - `Adafruit_SSD1306.h` – SSD1306 OLED driver
  - `SparkFun_SCD4x_Arduino_Library.h` – SCD40/41 sensor control

---

## 🚀 Installation & Setup

### 1. Open the Project
- Launch **Arduino IDE**  
- Open the `.ino` file from the **Bioseal** directory  

### 2. Install Required Libraries
- Go to **Sketch > Include Library > Manage Libraries**  
- Install the four libraries listed above  

### 3. Connect the Hardware
Wire **SCD40/41** and **SSD1306** to Arduino Nano’s **I²C pins**:

| Pin  | Arduino Connection |
|------|---------------------|
| SDA  | A4                  |
| SCL  | A5                  |
| VCC  | 5V                  |
| GND  | GND                 | 


### 4. Upload the Code
- Select **Arduino Nano** and the correct processor in **Tools**  
- Click **Upload**
