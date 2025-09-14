#include <WiFi.h>
#include <PubSubClient.h>

// ====== Konfigurasi WiFi & MQTT ======
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

// ====== Pin Ultrasonic ======
#define TRIG_PIN 5
#define ECHO_PIN 18

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Ultrasonic")) {
      // Bisa subscribe topik di sini kalau perlu
    } else {
      delay(2000);
    }
  }
}

long readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long distance = readUltrasonic();
  String payload = String(distance);
  client.publish("esp32/ultrasonic", payload.c_str());

  Serial.print("Distance: ");
  Serial.println(distance);

  delay(2000);
}
