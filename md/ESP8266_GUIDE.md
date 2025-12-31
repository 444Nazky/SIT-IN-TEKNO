# ESP8266 Implementation Guide

## 📋 Pengantar

Dokumen ini menjelaskan cara mengimplementasikan **Waste Sorting Robot** menggunakan **ESP8266** (mikrokontroler dengan WiFi built-in). Versi ini menawarkan keuntungan:

✅ WiFi connectivity untuk remote monitoring  
✅ Memori yang cukup untuk algoritma kompleks  
✅ HTTP server untuk dashboard web (opsional)  
✅ Cloud logging dan data analytics  
✅ Power management yang lebih baik  

---

## 🔧 Spesifikasi Hardware

### Board: NodeMCU ESP8266 v3

```
┌─────────────────────────────────────┐
│  NodeMCU ESP8266 v3 (Lolin)         │
├─────────────────────────────────────┤
│ • 32-bit Processor @ 80/160 MHz     │
│ • 160 KB RAM + 4 MB Flash           │
│ • 1x ADC 10-bit (A0/GPIO17)         │
│ • 11x GPIO pins                     │
│ • WiFi 802.11 b/g/n                │
│ • Micro USB power                   │
└─────────────────────────────────────┘
```

### Pin Mapping ESP8266

| Pin Name | GPIO | Fungsi | Catatan |
|----------|------|--------|---------|
| D0 | GPIO16 | Stepper Pin 3 | Wake up, no PWM |
| D1 | GPIO5 | Conveyor Motor | PWM support |
| D2 | GPIO4 | Color S0 | ADC, I2C |
| D3 | GPIO0 | Color S1 | Boot, Pull-up |
| D4 | GPIO2 | Proximity Sensor | ADC, I2C |
| D5 | GPIO14 | Inorganic LED | PWM, SPI |
| D6 | GPIO12 | Color Out | PWM, SPI |
| D7 | GPIO13 | Buzzer | PWM, SPI |
| D8 | GPIO15 | Emergency Stop | Boot, Pull-down |
| A0 | ADC0 | Weight Sensor | Single ADC pin |

⚠️ **Catatan Penting:**
- ESP8266 hanya memiliki **1 pin ADC** (A0)
- Jika memerlukan multiple analog input, gunakan **ADS1115** I2C converter
- D3 dan D8 digunakan untuk boot sequence, hindari untuk input kritis

---

## 📦 Instalasi Arduino IDE

### 1. Tambah Board Manager
1. Buka **Arduino IDE** → **Preferences**
2. Di bagian **Additional Boards Manager URLs**, tambahkan:
```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
3. Klik OK

### 2. Install Board Support
1. Buka **Tools** → **Board Manager**
2. Cari `esp8266` dan install **esp8266 by ESP8266 Community**
3. Tunggu hingga selesai (~5 menit)

### 3. Pilih Board dan Port
```
Tools → Board → NodeMCU 1.0 (ESP-12E Module)
Tools → Port → /dev/cu.SLAB_USBtoUART  (macOS)
                atau COM3 (Windows)
Tools → Upload Speed → 115200
Tools → CPU Frequency → 80 MHz
```

---

## 🔌 Wiring Diagram ESP8266

```
┌──────────────────────────────────────────────────────────────┐
│ NodeMCU ESP8266                                              │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  USB Power (5V) ────→ Vin / GND                             │
│                                                              │
│  Motors & Sensors:                                           │
│  ├─ D1 (GPIO5)  ──→ Conveyor Motor PWM                      │
│  ├─ D5 (GPIO14) ──→ Inorganic LED (via 220Ω)               │
│  ├─ D6 (GPIO12) ──→ Color Sensor OUT                        │
│  ├─ D7 (GPIO13) ──→ Buzzer (via transistor)                 │
│  ├─ D2 (GPIO4)  ──→ Proximity Sensor IN                     │
│  ├─ D3 (GPIO0)  ──→ Ultrasonic Trig                         │
│  ├─ D4 (GPIO2)  ──→ Ultrasonic Echo                         │
│  ├─ A0          ──→ Weight Sensor (via 3.3V divider)        │
│  └─ D8 (GPIO15) ──→ Emergency Stop (via 10kΩ pull-down)     │
│                                                              │
│  Color Sensor (TCS34725):                                    │
│  ├─ S0 ──→ D2 (GPIO4)                                        │
│  ├─ S1 ──→ D3 (GPIO0)                                        │
│  ├─ S2 ──→ D4 (GPIO2) [conflict with Proximity!]            │
│  ├─ S3 ──→ D5 (GPIO14) [conflict with LED!]                 │
│  └─ OUT ─→ D6 (GPIO12)                                       │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

⚠️ **PIN CONFLICT DETECTED!**
- Color Sensor pins D4 (S2) konflik dengan Proximity Sensor
- Color Sensor pins D5 (S3) konflik dengan Inorganic LED
- Solusi: **Gunakan I2C mode untuk Color Sensor** atau mapping ulang pins

---

## 🔧 Solusi PIN CONFLICT - Mapping Alternatif

### Opsi A: Gunakan I2C Color Sensor (Rekomendasi)

```cpp
#include <Wire.h>
#include <Adafruit_TCS34725.h>

// I2C Pin (sudah built-in di NodeMCU)
// SDA = D4 (GPIO2)
// SCL = D3 (GPIO0)

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_1MS_READ_TIME, TCS34725_GAIN_1X);

void setup() {
  Wire.begin(D4, D3);  // SDA, SCL
  if (!tcs.begin()) {
    Serial.println("Color sensor not found!");
  }
}

void readColorSensor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  // gunakan r, g, b values
}
```

### Opsi B: Remap Pins (Jika pakai frequency-based sensor)

```cpp
// MAPPING BARU:
const int COLOR_SENSOR_S0 = D0;      // GPIO16
const int COLOR_SENSOR_S1 = D1;      // GPIO5
const int COLOR_SENSOR_S2 = D2;      // GPIO4
const int COLOR_SENSOR_S3 = D8;      // GPIO15 (pindah dari D5)
const int COLOR_SENSOR_OUT = D6;     // GPIO12
const int INORGANIC_STORAGE_LED = A0; // ANALOG (tidak ideal, tapi terpaksa)
```

**Rekomendasi: Gunakan Opsi A (I2C)** lebih clean dan supported dengan library resmi.

---

## 💾 Upload Kode

### Langkah-Langkah:

1. **Buka Arduino IDE**
2. **Copy file `sketch_esp8266.ino`** ke dalam IDE
3. **Verify:** `Sketch` → `Verify/Compile` (Ctrl+R)
4. **Upload:** `Sketch` → `Upload` (Ctrl+U)
5. **Monitor:** `Tools` → `Serial Monitor` (Ctrl+Shift+M)
   - Set baud rate ke **115200**

### Expected Output:

```
▼ Mr. Trash Wheel Robot - ESP8266
Initializing...

✓ Pins initialized
✓ Color sensor initialized

▶ Running startup sequence...

  [1/5] Testing conveyor motor... ✓
  [2/5] Testing separator (forward)... ✓
  [3/5] Testing separator (backward)... ✓
  [4/5] Testing indicator LEDs... ✓
  [5/5] Testing sensors... ✓

✓ All systems initialized successfully!

✓ Mr. Trash Wheel Robot Ready!
════════════════════════════════════════
```

---

## 📊 Monitoring via Serial

### Perintah Tersedia:

```
s / S  → Display system status
e / E  → Emergency stop
r / R  → Toggle system on/off
c / C  → Read color sensor
w / W  → Read weight sensor
h / H  → Display help menu
```

### Contoh Interaksi:

```
Type 's' to see status:
╔════════════════════════════════════════╗
║      SYSTEM STATUS REPORT              ║
╠════════════════════════════════════════╣
║ Status: ACTIVE                         ║
║ Waste Detected: YES                    ║
║ Current Type: ORGANIC                  ║
║ Organic Storage: 45%                   ║
║ Inorganic Storage: 30%                 ║
╚════════════════════════════════════════╝
```

---

## 🌐 WiFi Integration (Opsional)

Untuk menambahkan WiFi connectivity, tambahkan library:

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

ESP8266WebServer server(80);

void setup() {
  // ... existing setup ...
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  // ... existing loop ...
  server.handleClient();
}

void handleRoot() {
  String html = "<h1>Trash Robot Status</h1>";
  html += "<p>Organic: " + String(system.organicStorageLevel) + "%</p>";
  html += "<p>Inorganic: " + String(system.inorganicStorageLevel) + "%</p>";
  server.send(200, "text/html", html);
}

void handleStatus() {
  String json = "{";
  json += "\"active\":" + String(system.active ? "true" : "false") + ",";
  json += "\"organic\":" + String(system.organicStorageLevel) + ",";
  json += "\"inorganic\":" + String(system.inorganicStorageLevel);
  json += "}";
  server.send(200, "application/json", json);
}
```

---

## 🐛 Troubleshooting

### Issue: Board tidak terdeteksi

**Solusi:**
1. Install CH340 driver: https://sparks.gogo.co.nz/ch340.html
2. Cek Device Manager (Windows) atau System Report (macOS)
3. Gunakan cable USB yang support data transfer (bukan charging saja)

### Issue: Upload failed "espcomm_sync_cmd: wrong direction"

**Solusi:**
1. Tekan & tahan tombol **FLASH** (GPIO0) saat upload
2. Atau set `Upload Speed` ke **74880** baud
3. Check koneksi USB

### Issue: Serial output garbled/not showing

**Solusi:**
1. Set Serial Monitor baud rate ke **115200**
2. Reset board (tekan tombol RST)
3. Cek cable USB connection

### Issue: ADC membaca nilai aneh

**Solusi:**
1. Gunakan voltage divider untuk analog input:
```
    5V
    │
   100Ω
    │
    ├─→ A0 (ESP8266)
    │
    ├─→ Signal from sensor
    │
   100Ω
    │
   GND
```

2. Atau gunakan external ADC (ADS1115) via I2C

---

## 📈 Performance Tips

### Memory Optimization:
```cpp
// Gunakan PROGMEM untuk strings
const char* status_str PROGMEM = "System Status";

// Hindari string concatenation yang berat
Serial.print("Value: ");
Serial.println(value);  // Better than "Value: " + value
```

### Power Management:
```cpp
// Set to light sleep saat idle (menghemat 10mA)
wifi_set_sleep_type(LIGHT_SLEEP_T);

// Or deep sleep untuk non-real-time applications
ESP.deepSleep(10e6);  // Sleep 10 seconds
```

### Non-blocking Delays:
```cpp
// ❌ JANGAN gunakan delay() yang lama
delay(5000);

// ✅ GUNAKAN millis() based timing
static unsigned long lastTime = 0;
if (millis() - lastTime >= 5000) {
  lastTime = millis();
  // Do something
}
```

---

## 📚 Resources

- **Arduino IDE Documentation**: https://docs.arduino.cc/
- **ESP8266 Core Documentation**: https://arduino-esp8266.readthedocs.io/
- **NodeMCU Pinout**: https://nodemcu.readthedocs.io/en/latest/
- **Adafruit TCS34725 Library**: https://github.com/adafruit/Adafruit_TCS34725

---

## ✅ Checklist Sebelum Deploy

- [ ] Semua pin sudah dimapping dengan benar
- [ ] Baud rate Serial Monitor = 115200
- [ ] Startup sequence berjalan tanpa error
- [ ] Emergency stop button responsif
- [ ] Sensor readings masuk akal
- [ ] Motor response sesuai ekspektasi
- [ ] LED indicators berfungsi
- [ ] Tidak ada memory leaks

---

**Happy coding! 🚀**
