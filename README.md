# Aqua Monitor System — IoT Environmental Intelligence Node

An IoT environmental monitoring system built with an Arduino Mega 2560 and ESP32, 
integrating multiple sensors with real-time local display and cloud data logging.

## Features
- Temperature & humidity monitoring (DHT11)
- Air quality monitoring (MQ135)
- pH level monitoring
- Real-time readings on a local LCD display
- Serial communication link between Mega and ESP32
- WiFi-based cloud data logging via ThingSpeak
- Custom PCB design (EasyEDA) for the full system

## Hardware
- Arduino Mega 2560
- ESP32 Dev Board
- DHT11 Temperature/Humidity Sensor
- MQ135 Gas Sensor
- pH Sensor
- I2C LCD Display

## How It Works
The Arduino Mega reads all sensor data and displays it locally on the LCD. 
Sensor readings are sent over a serial link to the ESP32, which connects to 
WiFi and uploads the data to ThingSpeak for remote, real-time monitoring.

## Files
- `ESP_code_IOT.ino` — ESP32 firmware (WiFi + cloud upload)
- `mega_code_IOT.ino` — Arduino Mega firmware (sensor reading + display)
- PCB design files — custom circuit board layout (EasyEDA)

## Author
Mohamad Abdelmuniem — Computer Engineering Graduate
