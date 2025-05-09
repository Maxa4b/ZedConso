# FPGA Consumption Monitor

This project is a real-time electrical consumption monitoring system built with an ESP32 and a ZedBoard FPGA.

## Features

- Measurement of voltage, current, power, and energy using an INA219 sensor
- Real-time display of current on an OLED screen
- Communication between FPGA and ESP32 via UART
- Data transmission over MQTT to a local broker (PC)
- Automatic CSV export with timestamped filenames
- Compatible with standalone FPGA operation (no USB needed)

## Components

- ESP32 DevKit
- ZedBoard FPGA (UART TX on Pmod JA1)
- INA219 (I2C sensor)
- SSD1306 OLED display
- MQTT broker on PC (e.g. Mosquitto)
- Python script for CSV logging and analysis

## Use Case

Designed for monitoring and analyzing energy consumption in embedded systems or academic FPGA projects. Ideal for learning UART communication, MQTT, and real-world sensor integration.

---

📡 Built with hardware precision and wireless freedom.
