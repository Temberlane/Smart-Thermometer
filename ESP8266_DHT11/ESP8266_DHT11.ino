#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

// WiFi and MQTT Settings
const char* ssid = "Thompson_Wi-Fi";
const char* password = "RogersThompson";
const char* mqtt_server = "0698d4f73af846d2a3ddb4811efceb90.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;  // Secure port for MQTT
const char* mqtt_username = "Thermometer";  // Add your HiveMQ username
const char* mqtt_password = "Samoht100";  // Add your HiveMQ password

// Define DHT11 Pin
static const int DHT_SENSOR_PIN = 12;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

const unsigned long MEASURE_INTERVAL = 150ul;

// MQTT Client with secure connection
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect with username and password
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected!");
      // Subscribe to topics here if needed
      // client.subscribe("home/commands");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

static bool measure_environment(float *temperature, float *humidity)
{
  static unsigned long measurement_timestamp = millis();

  if(millis() - measurement_timestamp > MEASURE_INTERVAL)
  {
    if(dht_sensor.measure(temperature, humidity) == true)
    {
      measurement_timestamp = millis();
      return(true);
    }
  }

  return(false);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP8266 Starting...");
  
  setup_wifi();
  
  // Skip certificate validation (for development only)
  espClient.setInsecure();
  
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("MQTT server configured");
  
  randomSeed(micros());
  Serial.println("Setup complete");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature;
  float humidity;

  // Add debug output
  Serial.println("Checking for new measurements...");
  
  if(measure_environment(&temperature, &humidity) == true){
    Serial.println("Measurement successful!");
    String payload = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
    client.publish("home/sensors/temperature", payload.c_str());
    Serial.println("Published: " + payload);
    Serial.println("Temperature: " + String(temperature));
    Serial.println("Humidity: " + String(humidity));
  } else {
    Serial.println("No new measurement available");
  }

  delay(100);
}

