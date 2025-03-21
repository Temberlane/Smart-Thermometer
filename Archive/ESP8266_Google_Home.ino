#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "YOUR_ADAFRUIT_USERNAME"
#define AIO_KEY         "YOUR_ADAFRUIT_IO_KEY"

// Initialize WiFi client
WiFiClient client;

// Setup the MQTT client class
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Feeds for publishing
Adafruit_MQTT_Publish tempFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidityFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");

// Variables to store sensor values
float temperature = 0;
float humidity = 0;

void setup() {
  // Start serial connection
  Serial.begin(9600);
  delay(10);
  
  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT
  connect();
}

void loop() {
  // Ensure MQTT connection is active
  if (!mqtt.connected()) {
    connect();
  }

  // Check if data is available from Arduino
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    
    // Parse JSON data
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, data);
    
    if (!error) {
      temperature = doc["temp"];
      humidity = doc["humidity"];
      
      // Publish to Adafruit IO
      if (tempFeed.publish(temperature) && humidityFeed.publish(humidity)) {
        Serial.println("Data sent to Adafruit IO!");
      } else {
        Serial.println("Failed to send data to Adafruit IO");
      }
    }
  }
  
  // Process MQTT events
  mqtt.processPackets(500);
  
  // Ping to keep connection alive
  if ((millis() - lastPing) > 60000) {
    if (mqtt.ping()) {
      lastPing = millis();
    }
  }
}

// Connect/reconnect to Adafruit IO
void connect() {
  Serial.print("Connecting to Adafruit IO... ");
  
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println("Wrong protocol"); break;
      case 2: Serial.println("ID rejected"); break;
      case 3: Serial.println("Server unavailable"); break;
      case 4: Serial.println("Bad user/password"); break;
      case 5: Serial.println("Not authorized"); break;
      case 6: Serial.println("Failed to subscribe"); break;
      default: Serial.println("Connection failed"); break;
    }
    
    if(ret >= 0)
      mqtt.disconnect();
      
    Serial.println("Retrying connection...");
    delay(5000);
  }
  
  Serial.println("Connected to Adafruit IO!");
  lastPing = millis();
}

unsigned long lastPing = 0;