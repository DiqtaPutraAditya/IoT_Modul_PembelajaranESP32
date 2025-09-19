# IoT Modul Pembelajaran ESP32

Proyek ini adalah **modul pembelajaran IoT** berbasis **ESP32** yang dirancang untuk pemula.  
Dengan modul ini, pengguna dapat mencoba berbagai sensor dan aktuator hanya dengan "colok jumper", serta memahami alur dasar sistem **Internet of Things (IoT)** menggunakan **protokol MQTT**.  

## ✨ Fitur Utama
- ESP32 sebagai mikrokontroler utama
- Mendukung beberapa sensor dan aktuator:
  - Sensor cahaya (LDR)
  - Sensor jarak (Ultrasonic HC-SR04)
  - Sensor gerak (PIR)
  - Potensiometer (Analog Input)
  - Push Button
  - LED, Motor, Motor Driver, dan LCD
- Komunikasi data menggunakan **MQTT**
- Format data berbasis **JSON**
- Mudah dikembangkan untuk percobaan tambahan

## 📂 Struktur Project
    IoT_Modul_PembelajaranESP32/
    
    │─── firmware/ # Source code ESP32
    │  ├── example/ # Source code ESP32
    │    ├── blink_mqtt.ino
    │    ├── ultrasonic_mqtt.ino
    │    ├── lcd_mqtt.ino
    │    └── ...
    │  └── main_project
    │    └── allsensor_mqtt.ino
    │── docs/ # Dokumentasi modul
    │ ├── tutorial.md
    │ └── flowchart.png
    │── images/ # Gambar rangkaian & ilustrasi
    │── README.md # Deskripsi project (file ini)


## 🛠️ Perangkat Keras
- ESP32 Devkit
- Breadboard + Kabel Jumper
- Sensor:
  - PIR
  - Ultrasonic HC-SR04
  - Potensiometer
  - LDR
  - Push Button
- Aktuator:
  - LED
  - Motor + Driver
  - LCD 16x2 (I2C)

## 🔗 Alur Sistem
1. ESP32 membaca data sensor (LDR, PIR, Ultrasonic, Potensiometer, Button).
2. Data dikirim ke **Broker MQTT** dalam format JSON.
3. Client (PC/HP/Node-RED/MQTT Explorer) melakukan **subscribe** ke topic.
4. Client dapat mengirim perintah balik (misalnya menyalakan LED, menampilkan teks ke LCD, atau mengontrol motor).
5. ESP32 menerima perintah dan mengeksekusinya pada aktuator.

![Flowchart IoT ESP32](https://github.com/DiqtaPutraAditya/IoT_Modul_PembelajaranESP32/blob/main/docs/iot_esp32_mqtt_flowchart.png)

## 📡 Contoh Format Data (JSON)
```bash
{
  "light": 350,
  "distance": 45,
  "pir": 1,
  "potensio": 512,
  "button": 0
}
```
🚀 Cara Menjalankan
Clone repository ini:

```bash
git clone https://github.com/DiqtaPutraAditya/IoT_Modul_PembelajaranESP32.git
```

Buka project di Arduino IDE atau PlatformIO.

Ubah konfigurasi WiFi dan MQTT broker di kode:
```bash
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "IP_BROKER";
```

Upload ke board ESP32.

Jalankan client MQTT (misalnya MQTT Explorer atau Node-RED) untuk melihat data.

## 📘 Dokumentasi
Lihat folder docs/ untuk tutorial penggunaan step-by-step.

## 📌 Kesimpulan
Modul ini memudahkan pemula untuk belajar IoT dari dasar.
Dengan ESP32 + MQTT, data sensor dapat dimonitor secara real-time dan aktuator dapat dikendalikan dari jarak jauh.
Struktur project modular membuat pengguna bebas mengembangkan lebih lanjut sesuai kebutuhan.

👨‍💻 Author: Diqta Putra Aditya

📅 Tahun: 2025
