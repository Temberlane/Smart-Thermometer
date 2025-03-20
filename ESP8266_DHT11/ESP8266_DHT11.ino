#include <ESP8266WiFi.h>
#include <dht_nonblocking.h>

// WiFi credentials
#define WIFI_SSID "Thompson_Wi-Fi"
#define WIFI_PASS "RogersThompson"

// DHT sensor setup
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 12;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// Measurement interval (3 seconds)
const unsigned long MEASURE_INTERVAL = 30;
unsigned long lastMeasurement = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("ESP8266 DHT11 Test");
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temperature;
  float humidity;
  
  // Check if it's time for a new measurement
  if (millis() - lastMeasurement >= MEASURE_INTERVAL) {
    if (dht_sensor.measure(&temperature, &humidity)) {
      lastMeasurement = millis();
      
      // Print JSON formatted data
      Serial.print("{\"temp\":");
      Serial.print(temperature, 1);
      Serial.print(",\"humidity\":");
      Serial.print(humidity, 1);
      Serial.println("}");
    }
  }
}