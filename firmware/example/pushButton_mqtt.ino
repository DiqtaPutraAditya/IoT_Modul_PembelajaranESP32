#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

#define BUTTON_PIN 14

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Button")) break;
    delay(2000);
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int state = digitalRead(BUTTON_PIN);
  if (state == LOW) {
    client.publish("esp32/button", "Pressed");
  } else {
    client.publish("esp32/button", "Released");
  }
  delay(500);
}
