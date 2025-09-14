#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

#define BUZZER_PIN 27

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Buzzer")) {
      client.subscribe("esp32/buzzer");
    } else {
      delay(2000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)message[i];
  
  if (msg == "ON") digitalWrite(BUZZER_PIN, HIGH);
  else if (msg == "OFF") digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
