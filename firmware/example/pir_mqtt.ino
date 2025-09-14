#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

#define PIR_PIN 13

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_PIR")) break;
    delay(2000);
  }
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int motion = digitalRead(PIR_PIN);
  if (motion) {
    client.publish("esp32/pir", "Motion Detected");
  } else {
    client.publish("esp32/pir", "No Motion");
  }
  delay(2000);
}
