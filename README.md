# Smart Thermometer IoT System

## Overview
This project implements a complete IoT temperature monitoring system using an ESP8266 microcontroller with a DHT11 sensor and a cloud-based backend. The system publishes temperature and humidity data to an MQTT broker, which is then stored in Firebase and made available via HTTP endpoints.

## System Architecture
- **ESP8266 with DHT11**: Collects temperature and humidity data and publishes to MQTT
- **MQTT Broker (HiveMQ Cloud)**: Receives and routes sensor data
- **Firebase Functions**: Bridges MQTT and Firebase, provides REST API endpoints
- **Firebase Realtime Database**: Stores the latest sensor readings

## Why MQTT?
MQTT serves as an ideal communication protocol for this IoT project for several reasons:
1. **Low bandwidth usage**: MQTT is designed for constrained environments, making it perfect for the limited resources of the ESP8266.
2. **Publish/Subscribe pattern**: This decouples the sensor from the backend, allowing multiple subscribers to receive data without the sensor needing to know about them.
3. **Quality of Service options**: MQTT ensures message delivery even in unstable network conditions common in IoT deployments.
4. **Low power consumption**: Critical for battery-powered deployments, MQTT's minimal overhead extends device battery life.
5. **Scalability**: A single MQTT broker can handle thousands of connected devices, making it future-proof as the system scales.
6. **Reliable messaging**: The persistent connection model works better than HTTP for IoT devices that need to frequently send small amounts of data.

While connecting directly to Firebase is possible, the MQTT bridge architecture offers better reliability, lower power consumption, and greater flexibility for expanding the system with additional sensors or data consumers.

## Components

### ESP8266 Temperature Sensor
The ESP8266 microcontroller reads temperature and humidity data from a DHT11 sensor and publishes it to the HiveMQ MQTT broker using a secure connection (MQTTS). The data is formatted as JSON with temperature and humidity fields.

### MQTT-Firebase Bridge
A set of Firebase Cloud Functions that:
1. Connects to the MQTT broker to receive temperature data
2. Stores the data in Firebase Realtime Database
3. Provides HTTP endpoints to access the temperature data
4. Runs on a scheduled interval to ensure data is regularly updated

## API Endpoints
- `/getTemperature`: Returns the latest temperature and humidity readings
- `/mqttBridge`: Manually triggers an MQTT data fetch
- `/testInsertData`: Inserts test data for development purposes

## Deployment
The project uses Firebase for hosting and cloud functions. Deploy with:

## Hardware Requirements
- ESP8266 NodeMCU or similar board
- DHT11 temperature and humidity sensor
- Power supply for the ESP8266

## Future Improvements
- Add authentication to the API endpoints
- Implement historical data storage and visualization
- Create a web or mobile interface for monitoring
- Add support for controlling heating/cooling systems
