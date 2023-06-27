#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define RX2 16
#define TX2 17

#define WIFI_SSID "vivo 1938"
#define WIFI_PASSWORD "123sin@1234"

#define MQTT_SERVER "192.168.43.116"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC "Data"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial2.begin(9600);
  
  if (!WiFi.begin(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Failed to connect to WiFi");
    while (1);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);
  
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (mqttClient.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker!");
      mqttClient.subscribe("rel1");
    }
    else {
      Serial.print("Failed to connect to MQTT broker with state ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message diterima [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Konversi payload menjadi string
  String rel = "";
  for (int i = 0; i < length; i++) {
    rel += (char)payload[i];
  }
  
  Serial.print(rel);
  
  Serial2.print(rel);
  
}

void loop() {
 if (Serial2.available()) {
    String message = Serial2.readStringUntil('\n');
    Serial.print(message);
    Serial.println();

// split the message by line and publish each line separately
    int index = 0;
    while ((index = message.indexOf('\n')) != -1) {
      String line = message.substring(0, index);
      mqttClient.publish(MQTT_TOPIC, line.c_str());
      message = message.substring(index + 1);
  }
    // publish the last line
    mqttClient.publish(MQTT_TOPIC, message.c_str());
}

mqttClient.loop();
}
