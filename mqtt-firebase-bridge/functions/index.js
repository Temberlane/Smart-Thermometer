const functions = require("firebase-functions");
const admin = require("firebase-admin");
const mqtt = require("mqtt");

admin.initializeApp();
const db = admin.database();

// Change to HTTP function instead of schedule
exports.mqttBridge = functions.https.onRequest(async (req, res) => {
  try {
    const mqttServer = "0698d4f73af846d2a3ddb4811efceb90.s1.eu.hivemq.cloud";
    const client = mqtt.connect(`mqtt://${mqttServer}`, {
      username: "Thermometer",
      password: "Samoht100",
      port: 8883,
      protocol: "mqtts",
    });

    let messageReceived = false;

    client.on("connect", () => {
      console.log("Connected to MQTT broker");
      client.subscribe("home/sensors/temperature");
    });

    client.on("message", (topic, message) => {
      console.log(`Received message on ${topic}: ${message.toString()}`);
      try {
        const payload = JSON.parse(message.toString());
        db.ref("sensors").set(payload);
        console.log("Data saved to Firebase");
        messageReceived = true;
      } catch (error) {
        console.error("Error parsing message:", error);
      }
    });

    // Wait for messages for up to 30 seconds
    await new Promise((resolve) => {
      setTimeout(() => {
        client.end();
        resolve();
      }, 30000);
    });

    if (messageReceived) {
      res.status(200).send("Data successfully received and saved");
    } else {
      res.status(200).send("Completed, but no new data received");
    }
  } catch (error) {
    console.error("Error in MQTT Bridge:", error);
    res.status(500).send("Error processing MQTT data");
  }
});

exports.getTemperature = functions.https.onRequest(async (req, res) => {
  try {
    const snapshot = await db.ref("sensors").once("value");
    const data = snapshot.val();

    if (!data) {
      return res.status(404).json({
        speech: "Sorry, temperature data is not available right now.",
      });
    }

    const speech = [
      `The room temperature is ${data.temperature} degrees Celsius`,
      `and humidity is ${data.humidity} percent.`,
    ].join(" ");
    return res.json({speech});
  } catch (error) {
    console.error("Error getting temperature:", error);
    return res.status(500).json({
      speech: "Sorry, there was an error getting the temperature.",
    });
  }
});

// Add a test function to directly write temperature data
exports.testInsertData = functions.https.onRequest(async (req, res) => {
  try {
    // Create test data
    const testData = {
      temperature: 22.5,
      humidity: 45.0,
    };

    // Write directly to the same path getTemperature reads from
    await db.ref("sensors").set(testData);

    res.status(200).send("Test data inserted successfully");
  } catch (error) {
    console.error("Error inserting test data:", error);
    res.status(500).send("Error inserting test data");
  }
});

// Add this function to periodically fetch MQTT data
exports.scheduledMqttBridge = functions.pubsub
    .schedule("every 5 minutes")
    .onRun(async (context) => {
      const mqttServer = "0698d4f73af846d2a3ddb4811efceb90.s1.eu.hivemq.cloud";
      const client = mqtt.connect(`mqtt://${mqttServer}`, {
        username: "Thermometer",
        password: "Samoht100",
        port: 8883,
        protocol: "mqtts",
      });

      let messageReceived = false;

      client.on("connect", () => {
        console.log("Scheduled job: Connected to MQTT broker");
        client.subscribe("home/sensors/temperature");
      });

      client.on("message", (topic, message) => {
        try {
          const payload = JSON.parse(message.toString());
          db.ref("sensors").set(payload);
          console.log("Scheduled job: Data saved to Firebase");
          messageReceived = true;
        } catch (error) {
          console.error("Error parsing message:", error);
        }
      });

      // Wait for messages
      await new Promise((resolve) => {
        setTimeout(() => {
          client.end();
          resolve();
        }, 30000);
      });

      return null;
    });
