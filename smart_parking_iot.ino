#include <Ultrasonic.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Define the pins for the ultrasonic sensor
#define TRIGGER_PIN 5
#define ECHO_PIN 18

// Create an instance of the Ultrasonic sensor
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);

// WiFi credentials
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// MQTT broker details
const char* mqttServer = "192.168.0.10";  // Replace with your Mosquitto broker IP address
const int mqttPort = 1883;  // MQTT default port
const char* clientId = "arduino_client";
const char* topic = "distance";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    if (client.connect(clientId)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  // Read the distance from the ultrasonic sensor
  float distance = ultrasonic.read();

  // Publish the distance to the MQTT topic
  char payload[10];
  sprintf(payload, "%.2f", distance);
  client.publish(topic, payload);

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000);  // Wait for 1 second before taking the next reading
}
