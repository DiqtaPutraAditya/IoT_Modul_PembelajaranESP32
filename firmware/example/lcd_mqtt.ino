#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ====== Konfigurasi WiFi & MQTT ======
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

// ====== LCD ======
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C bisa 0x27 atau 0x3F

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_LCD")) {
      client.subscribe("esp32/lcd");
    } else {
      delay(2000);
    }
  }
}

// ====== Callback pesan MQTT ======
void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  if (String(topic) == "esp32/lcd") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
