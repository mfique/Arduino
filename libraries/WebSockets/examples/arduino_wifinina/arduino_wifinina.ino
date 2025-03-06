#include <ESP8266WiFi.h> // Use <WiFi.h> for ESP32
#include <PubSubClient.h>
// Wi-Fi credentials
const char* ssid = "Dieudonne";
const char* password = "HelloWorld";
// MQTT Broker settings
const char* mqtt_server = "157.173.101.159";
const int mqtt_port = 1883;
const char* mqtt_client_id = "group_HDP";
// Ultrasonic sensor pins
const int trigPin = 2;  // GPIO 2 (D4 on NodeMCU)
const int echoPin = 14; // GPIO 14 (D5 on NodeMCU)
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
  Serial.begin(9600);
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // Connect to Wi-Fi
  setupWiFi();
  // Connect to MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  reconnect();
}
// [Rest of the code remains unchanged: setupWiFi(), reconnect(), getDistance(), loop()]
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.0343 / 2;
  return distance;
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  float distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  char distanceStr[8];
  dtostrf(distance, 6, 2, distanceStr);
  client.publish("/honore_dieudonne_pacifique/room_distance", distanceStr);
  delay(1000);
}