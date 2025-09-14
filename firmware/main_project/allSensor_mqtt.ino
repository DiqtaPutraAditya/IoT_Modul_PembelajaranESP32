/* esp32_all_sensors.ino
   Terbaca: LDR, Ultrasonic, PIR, Potensiometer, Button
   Aktuator: LED, Relay, Buzzer, Motor, LCD
   Publish ke MQTT: iot/module1/data
   Subscribe ke MQTT: iot/module1/cmd
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ======= CONFIG =======
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_topic_pub = "iot/module1/data";
const char* mqtt_topic_sub = "iot/module1/cmd";
const char* device_id = "esp32_01";
// =======================

// Pin mapping (sesuaikan bila perlu)
const int PIN_LDR = 34;     // ADC
const int PIN_POT = 35;     // ADC
const int PIN_PIR = 13;
const int PIN_TRIG = 5;
const int PIN_ECHO = 18;
const int PIN_BUTTON = 14;  // gunakan INPUT_PULLUP
const int PIN_LED = 2;
const int PIN_RELAY = 26;
const int PIN_BUZZER = 27;
const int PIN_MOTOR_IN1 = 32;
const int PIN_MOTOR_IN2 = 33;
const int PIN_MOTOR_EN = 25; // PWM via ledc
// LCD I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi & MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// PWM channel for motor
const int MOTOR_PWM_CHANNEL = 0;
const int MOTOR_PWM_FREQ = 5000;
const int MOTOR_PWM_RES = 8; // 8 bit resolution (0-255)

unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 2000; // 2000 ms

// ======== Helpers ========
void setup_wifi() {
  Serial.print("Connecting to WiFi ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries > 60) break; // timeout ~30s
  }
  Serial.println();
  Serial.print("WiFi connected, IP: ");
  Serial.println(WiFi.localIP());
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // parse payload as JSON
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.println("MQTT payload JSON parse failed");
    return;
  }
  // contoh payload: {"led":"ON","relay":"OFF","buzzer":"ON","motor":"FWD","motor_speed":200,"lcd":"Hello"}
  if (doc.containsKey("led")) {
    String v = doc["led"].as<String>();
    digitalWrite(PIN_LED, (v == "ON") ? HIGH : LOW);
    Serial.print("Set LED: "); Serial.println(v);
  }
  if (doc.containsKey("relay")) {
    String v = doc["relay"].as<String>();
    digitalWrite(PIN_RELAY, (v == "ON") ? HIGH : LOW);
    Serial.print("Set Relay: "); Serial.println(v);
  }
  if (doc.containsKey("buzzer")) {
    String v = doc["buzzer"].as<String>();
    digitalWrite(PIN_BUZZER, (v == "ON") ? HIGH : LOW);
    Serial.print("Set Buzzer: "); Serial.println(v);
  }
  if (doc.containsKey("motor")) {
    String cmd = doc["motor"].as<String>();
    if (cmd == "FWD") {
      digitalWrite(PIN_MOTOR_IN1, HIGH);
      digitalWrite(PIN_MOTOR_IN2, LOW);
    } else if (cmd == "REV") {
      digitalWrite(PIN_MOTOR_IN1, LOW);
      digitalWrite(PIN_MOTOR_IN2, HIGH);
    } else if (cmd == "STOP") {
      digitalWrite(PIN_MOTOR_IN1, LOW);
      digitalWrite(PIN_MOTOR_IN2, LOW);
    }
    Serial.print("Motor cmd: "); Serial.println(cmd);
  }
  if (doc.containsKey("motor_speed")) {
    int spd = doc["motor_speed"];
    if (spd < 0) spd = 0;
    if (spd > 255) spd = 255;
    ledcWrite(MOTOR_PWM_CHANNEL, spd);
    Serial.print("Motor speed: "); Serial.println(spd);
  }
  if (doc.containsKey("lcd")) {
    String text = doc["lcd"].as<String>();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(text.substring(0, 16)); // tampilkan maksimal 16 char baris1
    // kalau mau baris 2, bisa parsing lagi
    Serial.print("LCD text: "); Serial.println(text);
  }
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(device_id)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(", retrying in 2s");
      delay(2000);
    }
  }
}

long readUltrasonicCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  long duration = pulseIn(PIN_ECHO, HIGH, 30000); // timeout 30ms
  if (duration == 0) return -1; // timeout
  long distance = duration * 0.034 / 2; // cm (speed sound 0.034 cm/us)
  return distance;
}

// ======== Setup & Loop ========
void setup() {
  Serial.begin(115200);
  delay(100);

  // Pins
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  // Motor PWM setup
  ledcSetup(MOTOR_PWM_CHANNEL, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  ledcAttachPin(PIN_MOTOR_EN, MOTOR_PWM_CHANNEL);
  ledcWrite(MOTOR_PWM_CHANNEL, 0); // stop motor

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ESP32 IoT Module");

  // WiFi + MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
}

void loop() {
  if (!client.connected()) reconnect_mqtt();
  client.loop();

  unsigned long now = millis();
  if (now - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = now;

    // Read sensors
    int lightVal = analogRead(PIN_LDR); // 0..4095 on ESP32 ADC (depends), map if perlu
    // Normalize ADC to 0..1023 to be simpler:
    int lightMapped = map(lightVal, 0, 4095, 0, 1023);
    long distance = readUltrasonicCM(); // -1 jika timeout
    int pirVal = digitalRead(PIN_PIR); // 0/1
    int potValRaw = analogRead(PIN_POT); // 0..4095
    int potMapped = map(potValRaw, 0, 4095, 0, 1023);
    int btnState = (digitalRead(PIN_BUTTON) == LOW) ? 1 : 0; // 1 = pressed

    // Display summary to LCD (baris 1 & 2)
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("L:");
    lcd.print(lightMapped);
    lcd.print(" D:");
    if (distance < 0) lcd.print("---");
    else lcd.print(distance);
    lcd.setCursor(0,1);
    lcd.print("P:");
    lcd.print(pirVal);
    lcd.print(" Pot:");
    lcd.print(potMapped);

    // Build JSON
    StaticJsonDocument<256> doc;
    doc["device_id"] = device_id;
    // timestamp basic (millis) or implement NTP if perlu
    doc["timestamp"] = millis();
    doc["light"] = lightMapped;
    doc["distance"] = distance;
    doc["pir"] = pirVal;
    doc["potensio"] = potMapped;
    doc["button"] = btnState;

    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    // Publish
    if (client.publish(mqtt_topic_pub, buffer, n)) {
      Serial.print("Published: ");
      Serial.println(buffer);
    } else {
      Serial.println("Publish failed");
    }
  } // end publish block

  // small delay so loop isn't busy (but keeping client.loop running)
  delay(50);
}
