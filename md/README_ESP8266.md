# 🤖 Waste Sorting Robot - ESP8266 Arduino Project

## 📋 Daftar Isi
1. [Ringkasan Project](#ringkasan-project)
2. [Apa yang Baru untuk ESP8266](#apa-yang-baru-untuk-esp8266)
3. [File-File Tersedia](#file-file-tersedia)
4. [Cara Memilih Versi](#cara-memilih-versi)
5. [Quick Start](#quick-start)
6. [Hardware Spesifikasi](#hardware-spesifikasi)
7. [Fitur Sistem](#fitur-sistem)
8. [Dokumentasi Lengkap](#dokumentasi-lengkap)

---

## 📱 Ringkasan Project

**Mr. Trash Wheel Robot** adalah sistem otomatis untuk:
- ♻️ Mendeteksi sampah menggunakan sensor proximity
- 🎨 Mengklasifikasi sampah (organik/anorganik) dengan color sensor
- ⚙️ Memisahkan sampah menggunakan servo/stepper motor
- 📦 Menyimpan di dua kategori berbeda
- 🚨 Monitoring kapasitas penyimpanan

**Teknologi**: Arduino-compatible dengan:
- Motor kontrol (konveyor & separator)
- Multiple sensors (color, weight, proximity, ultrasonic)
- LED indicators & buzzer alerts
- Optional WiFi & web dashboard

---

## ✨ Apa yang Baru untuk ESP8266

### 🎯 Keunggulan ESP8266 vs Arduino Uno:

| Fitur | Arduino Uno | ESP8266 |
|-------|------------|---------|
| **Processor** | 8-bit 16MHz | 32-bit 80/160MHz |
| **RAM** | 2 KB | 160 KB |
| **Flash** | 32 KB | 4 MB |
| **WiFi** | ❌ | ✅ |
| **Clock accuracy** | Sedang | Tinggi (RTC) |
| **ADC pins** | 6x | 1x |
| **PWM pins** | 6x | 4x |
| **Kosong lebih murah** | ❌ | ✅ |

### 🔧 Optimisasi untuk ESP8266:

✅ **Non-blocking architecture** - Semua timing menggunakan `millis()` bukan `delay()`  
✅ **Memory efficient** - String handling yang lebih baik  
✅ **WiFi integration** - Built-in WiFi dengan web server  
✅ **Better power management** - Sleep modes support  
✅ **Async I/O** - Tidak freeze saat sensor reading  

---

## 📂 File-File Tersedia

### 1️⃣ **sketch_esp8266.ino** ⭐ RECOMMENDED UNTUK PEMULA
```
Ukuran: ~10 KB
Dependencies: Built-in only
Mode: Standalone (tanpa WiFi)
Fitur:
  ✓ Motor control (conveyor & stepper)
  ✓ Sensor reading (color, weight, proximity)
  ✓ Waste classification (organic/inorganic)
  ✓ Storage monitoring
  ✓ Emergency stop
  ✓ Serial monitoring
  ✗ WiFi
  ✗ Web dashboard
```

**Kapan digunakan:**
- Testing & development
- Deployment tanpa internet
- Minimalisir memory footprint
- Serial monitoring cukup

---

### 2️⃣ **sketch_esp8266_wifi.ino** 🌐 ADVANCED
```
Ukuran: ~20 KB
Dependencies: ESP8266WiFi.h, ESP8266WebServer.h
Mode: WiFi enabled
Fitur:
  ✓ Semua fitur di sketch_esp8266.ino +
  ✓ WiFi connectivity
  ✓ Web dashboard (real-time)
  ✓ REST API endpoints
  ✓ Data logging (circular buffer)
  ✓ JSON responses
  ✓ Statistics tracking
```

**Kapan digunakan:**
- Remote monitoring
- Mobile app integration
- Cloud logging
- Dashboard visualization
- Multi-robot management

---

### 3️⃣ **sketch_wokwi.ino** 💻 UNTUK SIMULASI
```
Ukuran: ~12 KB
Dependencies: Built-in only
Mode: Arduino Uno compatible (untuk simulator)
Fitur:
  ✓ Interactive simulation
  ✓ Sensor value injection
  ✓ Serial command interface
  ✓ Detailed logging
  ✗ WiFi (tidak di simulator)
```

**Kapan digunakan:**
- Testing tanpa hardware
- Development sebelum upload
- Educational purposes
- Debugging logic
- Share dengan team via Wokwi link

---

### 📚 Dokumentasi

| File | Fungsi | Untuk |
|------|--------|-------|
| **QUICK_START.md** | 5-menit setup guide | Semua user |
| **ESP8266_GUIDE.md** | Dokumentasi lengkap | Hardware setup, troubleshooting |
| **README.md** (ini) | Overview project | Planning & understanding |
| **flowchart.md** | System logic diagram | Architecture understanding |
| **wokwi.toml** | Simulator config | Running di Wokwi |

---

## 🎯 Cara Memilih Versi

```
┌─ Apakah perlu WiFi?
│  ├─ TIDAK  → sketch_esp8266.ino ⭐
│  └─ YA    → sketch_esp8266_wifi.ino 🌐
│
└─ Apakah ada hardware?
   ├─ TIDAK  → sketch_wokwi.ino 💻
   └─ YA    → Pilih berdasarkan WiFi requirement
```

### Contoh Use Cases:

**🏭 Industri kecil**
→ `sketch_esp8266.ino` (cost-effective, reliable)

**🏢 Pabrik besar dengan multiple units**
→ `sketch_esp8266_wifi.ino` (centralized monitoring)

**🎓 Lab/Educational**
→ `sketch_wokwi.ino` (testing, no hardware)

**🏠 Smart home integration**
→ `sketch_esp8266_wifi.ino` (remote access)

---

## 🚀 Quick Start

### ⚡ 5 Minutes (No WiFi)

1. **Install Board Support**
   ```
   Arduino IDE → Tools → Board Manager
   Search: esp8266
   Install: esp8266 by ESP8266 Community
   ```

2. **Configure Board**
   ```
   Tools → Board → NodeMCU 1.0 (ESP-12E Module)
   Tools → Upload Speed → 115200
   ```

3. **Open & Upload**
   ```
   File → Open → sketch_esp8266.ino
   Sketch → Upload (Ctrl+U)
   ```

4. **Monitor**
   ```
   Tools → Serial Monitor
   Baud Rate: 115200
   ```

**Expected output:**
```
Mr. Trash Wheel Robot - ESP8266
Initializing...
✓ Pins initialized
✓ Color sensor initialized
▶ Running startup sequence...
  [1/5] Testing conveyor... ✓
  ...
✓ All systems initialized successfully!
```

### 🌐 Add WiFi (10 Minutes Extra)

1. Edit WiFi credentials:
   ```cpp
   const char* SSID = "MyWiFi";
   const char* PASSWORD = "MyPassword";
   ```

2. Upload `sketch_esp8266_wifi.ino`

3. Find IP in Serial Monitor output

4. Open browser: `http://<IP_ADDRESS>`

---

## ⚙️ Hardware Spesifikasi

### Board
- **NodeMCU v3** (dengan USB, micro-SD optional)
- **5V USB Power supply** (min 2A)

### Motors & Actuators
| Component | Type | Quantity | Pin |
|-----------|------|----------|-----|
| Conveyor Motor | DC 5V | 1 | D1 (PWM) |
| Separator | Stepper NEMA17 | 1 | D7,D8,D0,D1 |

### Sensors
| Component | Type | Quantity | Pin | Notes |
|-----------|------|----------|-----|-------|
| Color Sensor | TCS34725 | 1 | D2-D6 | Frequency-based |
| Weight Sensor | Load Cell + HX711 | 1 | A0 | Single ADC |
| Proximity | IR Infrared | 1 | D4 | Digital IN |
| Ultrasonic | HC-SR04 | 1 | D3,D2 | Distance |

### Indicators
| Component | Type | Quantity | Pin |
|-----------|------|----------|-----|
| LED Green | 5mm | 1 | D5 |
| LED Blue | 5mm | 1 | D6 |
| Buzzer | 5V active | 1 | D7 |
| Button | Push button | 1 | D8 |

### Power Budget
```
Idle:           ~150 mA (ESP8266 + sensors)
Conveyor ON:    ~250 mA
Stepper moving: ~400 mA
Peak (all on):  ~500 mA
```

**Requirement**: 5V/2A power supply

---

## 🔄 Fitur Sistem

### 🚀 Startup Sequence
1. Serial init (115200 baud)
2. Pin configuration
3. Color sensor calibration
4. Hardware self-test
5. Ready for operation

### 🎯 Main Loop
```
Every 100ms:
  ├─ Check emergency stop
  ├─ Check waste detection
  ├─ Classify if detected
  ├─ Route to storage
  └─ Resume conveyor

Every 10 seconds:
  └─ Display status
```

### 🧠 Classification Algorithm

**Input**: Color (RGB), Weight

**Color-based**:
- Brown (R>400, G>300, B<300) → Organic
- Green (G>R, G>B, G>350) → Organic

**Weight-based**:
- Light (<500) → Organic
- Heavy (>800) → Inorganic

**Output**: ORGANIC or INORGANIC

### 🎯 Separation Mechanism

1. Detect waste on conveyor
2. Stop conveyor (500ms)
3. Classify waste type
4. Move stepper to position
   - Organic: 128 steps
   - Inorganic: 384 steps
5. Hold for 800ms
6. Return to home (0 steps)
7. Resume conveyor

### 📊 Storage Monitoring

- **Ultrasonic sensor** measures distance
- Maps distance (50cm=empty, 5cm=full)
- LED indicators:
  - Off: <30% full
  - Blinking: 30-70% full
  - Solid: >70% full
- Buzzer alert at >90%

### 🚨 Emergency Stop

**Activation**: Press button on D8
- Stops all motors immediately
- Flashing warning LEDs
- Continuous buzzer alert
- Requires button release to resume

---

## 📚 Dokumentasi Lengkap

### Baca ini untuk:

**QUICK_START.md**
- ⏱️ 5-minute setup
- 🔌 Pin configuration
- 🐛 Basic troubleshooting
- 💡 Quick tips

**ESP8266_GUIDE.md**
- 🔧 Detailed pin mapping
- 🌐 WiFi setup
- 🔌 Wiring diagrams
- 🛠️ Advanced troubleshooting
- 📈 Performance optimization

**flowchart.md**
- 📊 System logic diagram
- 🔄 Process flow
- 🎯 Decision trees

**README.md** (original)
- 🏗️ Hardware components
- ⚡ Power specifications
- 🎯 Project objectives

---

## 📊 Comparison Table

### sketch_esp8266.ino vs sketch_esp8266_wifi.ino

| Aspek | Standard | WiFi |
|-------|----------|------|
| **Flash usage** | 60 KB | 120 KB |
| **RAM usage** | 20 KB | 35 KB |
| **Boot time** | 2-3s | 4-5s |
| **Power (idle)** | 100 mA | 150 mA |
| **Monitoring** | Serial | Serial + Web |
| **Scalability** | Single unit | Multiple units |
| **Cost** | Same | Same |
| **Complexity** | Low | Medium |
| **Learning curve** | Easy | Medium |

---

## 🆘 Troubleshooting

### ❌ "Board not found"
- Install CH340 drivers
- Use proper USB data cable
- Try different USB port

### ❌ "Serial shows garbage"
- Set baud rate to **115200**
- Press RST button
- Check cable quality

### ❌ "WiFi won't connect"
- Double-check SSID & password (case-sensitive)
- 2.4GHz band only (ESP8266)
- Check router not blocking

### ❌ "Sensor reading wrong"
- Verify pin connections
- Check sensor voltage (3.3V or 5V)
- Use voltage divider if needed
- Check pullup/pulldown resistors

Lihat **ESP8266_GUIDE.md** untuk troubleshooting lengkap.

---

## ✅ Testing Checklist

- [ ] Board terdeteksi di IDE
- [ ] Kode compile tanpa error
- [ ] Serial output normal (115200 baud)
- [ ] Startup sequence tampil
- [ ] Conveyor motor berputar
- [ ] Separator stepper bergerak
- [ ] LED indicators menyala
- [ ] Emergency stop berfungsi
- [ ] Sensor readings reasonable
- [ ] (WiFi) Connected to network
- [ ] (WiFi) Dashboard accessible

---

## 📚 Resources

- **Arduino ESP8266 Core**: https://arduino-esp8266.readthedocs.io/
- **ESP8266 Pinout**: https://nodemcu.readthedocs.io/
- **Wokwi Simulator**: https://wokwi.com
- **Arduino IDE**: https://www.arduino.cc/en/software

---

## 📝 License & Notes

**Project**: Waste Sorting Robot  
**Platform**: ESP8266 (NodeMCU v3)  
**Compatibility**: Arduino IDE 1.8.x / 2.0.x  
**Last Updated**: 2025

---

**Happy coding! 🚀♻️**

Untuk bantuan lebih lanjut:
1. Baca QUICK_START.md
2. Baca ESP8266_GUIDE.md
3. Check flowchart.md untuk logic
4. Test dengan sketch_wokwi.ino di Wokwi
5. Deploy dengan sketch_esp8266.ino atau sketch_esp8266_wifi.ino

