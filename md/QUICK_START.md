# Trash Robot ESP8266 - Quick Start Guide

## 📦 Files yang Tersedia

| File | Deskripsi | Fitur |
|------|-----------|-------|
| `sketch_esp8266.ino` | Versi Standard | ✓ Motor control ✓ Sensor reading ✓ Classification |
| `sketch_esp8266_wifi.ino` | Versi WiFi Advanced | ✓ Semua fitur di atas + WiFi + Web Dashboard + Data logging |
| `ESP8266_GUIDE.md` | Dokumentasi Lengkap | ✓ Pin mapping ✓ Troubleshooting ✓ Setup guide |

---

## 🚀 Quick Start (5 Menit)

### Opsi 1: Tidak Ada WiFi (Recommended untuk Pemula)

1. **Download & Extract**
   ```bash
   # Clone atau download project
   ```

2. **Buka Arduino IDE**
   - File → Open → `sketch_esp8266.ino`

3. **Select Board**
   ```
   Tools → Board → NodeMCU 1.0 (ESP-12E Module)
   Tools → Port → /dev/cu.SLAB_USBtoUART (macOS)
                  atau COM3 (Windows)
   Tools → Upload Speed → 115200
   ```

4. **Upload**
   - Sketch → Upload (Ctrl+U)
   - Tunggu hingga "Done uploading"

5. **Monitor**
   - Tools → Serial Monitor
   - Set Baud Rate: **115200**
   - Press RST button pada board
   - Lihat output initialization

### Opsi 2: Dengan WiFi & Web Dashboard

1. **Edit WiFi Credentials**
   
   Buka `sketch_esp8266_wifi.ino`, cari bagian:
   ```cpp
   // ==================== WiFi Configuration ====================
   const char* SSID = "YOUR_WIFI_SSID";           // ← Ganti ini
   const char* PASSWORD = "YOUR_WIFI_PASSWORD";   // ← Ganti ini
   ```
   
   Contoh:
   ```cpp
   const char* SSID = "MyHomeWiFi";
   const char* PASSWORD = "MyPassword123";
   ```

2. **Upload Code** (sama seperti Opsi 1)

3. **Find IP Address**
   
   Buka Serial Monitor, lihat output:
   ```
   ✓ WiFi connected!
   IP Address: 192.168.1.100     ← Copy IP ini
   ```

4. **Access Dashboard**
   
   Buka browser, ke: `http://192.168.1.100`
   
   Anda akan melihat dashboard interaktif!

---

## 🔌 Pin Configuration Cepat

### Board: NodeMCU ESP8266

```
USB
│
├─ GND → Ground
├─ 5V  → Power Supply
│
├─ A0  → Weight Sensor
├─ D0  → (GPIO16)
├─ D1  → Conveyor Motor
├─ D2  → Color Sensor Pin
├─ D3  → Color Sensor Pin
├─ D4  → Proximity Sensor / Ultrasonic Echo
├─ D5  → Inorganic LED
├─ D6  → Color Sensor OUT
├─ D7  → Buzzer
└─ D8  → Emergency Stop
```

⚠️ **PENTING:** Hanya ada 1 ADC pin (A0) di ESP8266!
   - Untuk multiple analog input, gunakan **ADS1115** I2C module

---

## 📊 Serial Commands

Ketika terhubung ke Serial Monitor, ketik:

| Perintah | Fungsi |
|----------|--------|
| `s` | Tampilkan status sistem |
| `e` | Emergency stop |
| `r` | Toggle on/off |
| `c` | Baca color sensor |
| `w` | Baca weight sensor |
| `h` | Help menu |

---

## 🌐 Web Dashboard Features (WiFi Version)

**URL:** `http://<IP_ADDRESS>`

### Available Endpoints:

```
GET  /              → HTML Dashboard
GET  /api/status    → JSON status
GET  /api/logs      → JSON waste logs
GET  /api/control?action=toggle     → Toggle system
GET  /api/control?action=emergency  → Emergency stop
```

### Example Response:
```json
{
  "active": true,
  "currentTask": "Processing Organic",
  "wasteDetected": true,
  "organicLevel": 45,
  "inorganicLevel": 30,
  "totalProcessed": 128,
  "organicCount": 75,
  "inorganicCount": 53
}
```

---

## 🔧 Troubleshooting

### ❌ Board tidak terdeteksi

**Solusi:**
1. Install CH340 driver:
   - Windows: http://wch.cn/downloads/CH341SER_ZIP.html
   - macOS: https://sparks.gogo.co.nz/ch340.html

2. Gunakan cable USB data (bukan charging only)

3. Cek Device Manager / System Report

### ❌ Upload failed

**Solusi:**
1. Tekan & tahan tombol FLASH saat upload
2. Coba set Upload Speed ke 74880
3. Try different USB port

### ❌ Serial Monitor shows garbage

**Solusi:**
1. **PASTI set Baud Rate ke 115200**
2. Press RST button
3. Check USB cable quality

### ❌ WiFi tidak connect

**Solusi:**
1. Double-check SSID dan password (case-sensitive!)
2. Pastikan 2.4GHz band enabled (5GHz tidak support)
3. Check router tidak blocking ESP8266

---

## 💡 Tips & Tricks

### Memory Optimization
```cpp
// ✅ Baik
Serial.print("Value: ");
Serial.println(value);

// ❌ Buruk (boros memory)
Serial.println("Value: " + String(value));
```

### Non-blocking Delays
```cpp
// ✅ Baik (tidak freeze)
static unsigned long lastTime = 0;
if (millis() - lastTime >= 1000) {
  lastTime = millis();
  // Do something
}

// ❌ Buruk (freezes everything)
delay(1000);
```

### WiFi Power Save
```cpp
// Light sleep mode (saves 10mA)
wifi_set_sleep_type(LIGHT_SLEEP_T);
```

---

## 📈 Performance Metrics

### Standard Version (sketch_esp8266.ino)
- Memory: ~60 KB Flash, ~20 KB RAM
- Power: ~100 mA (idle) / 200 mA (motor on)
- Boot time: ~3 seconds

### WiFi Version (sketch_esp8266_wifi.ino)
- Memory: ~120 KB Flash, ~35 KB RAM
- Power: ~150 mA (WiFi idle) / 250 mA (processing)
- Boot time: ~4-5 seconds
- Web server response: <200 ms

---

## 📚 Next Steps

1. **Test di Wokwi Simulator** (optional)
   - Upload kode ke Wokwi.com
   - Test logic tanpa hardware

2. **Pasang Sensors & Motors**
   - Follow pin configuration
   - Test each component

3. **Calibrate Classification**
   - Adjust color thresholds
   - Test dengan berbagai sampah

4. **Deploy & Monitor**
   - Setup WiFi (untuk wireless monitoring)
   - Monitor melalui dashboard

---

## 🆘 Bantuan Lebih Lanjut

Baca file berikut untuk detail lengkap:
- **ESP8266_GUIDE.md** - Setup & troubleshooting lengkap
- **flowchart.md** - Logic flow sistem
- **README.md** - Spesifikasi hardware

---

## ✅ Checklist Sebelum Deploy

- [ ] Board sudah terdeteksi di Arduino IDE
- [ ] Kode berhasil diupload tanpa error
- [ ] Serial Monitor menunjukkan startup sequence
- [ ] Semua pin dikonfigurasi dengan benar
- [ ] Sensor memberikan reading yang reasonable
- [ ] Motor merespons perintah
- [ ] Emergency stop berfungsi
- [ ] LED indicators menyala
- [ ] (WiFi) Connected to network & dashboard accessible

---

**Happy trash sorting! 🚀♻️**
