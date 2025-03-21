//www.elegoo.com
//2018.10.25

#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

// Measurement interval (3 seconds)
const unsigned long MEASURE_INTERVAL = 3000ul;

/*
 * Initialize the serial port.
 */
void setup()
{
  Serial.begin(9600);
}

/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
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

/*
 * Main program loop.
 */
void loop()
{
  float temperature;
  float humidity;

  /* Measure temperature and humidity. If the function returns
     true, then a measurement is available. */
  if(measure_environment(&temperature, &humidity) == true)
  {
    // Send data in JSON format for ESP8266 to parse easily
    Serial.print("{\"temp\":");
    Serial.print(temperature, 1);
    Serial.print(",\"humidity\":");
    Serial.print(humidity, 1);
    Serial.println("}");
  }
}
