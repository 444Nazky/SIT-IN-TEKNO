## Design Requirement and Objectives
1). Water resistant
2). Can float

## Function List
1). Dapat menarik sampah menggunakan konveyor
2). Dapat memisahkan sampah organik & anorganik menggunakan servo dan kamera
3). Memiliki area penyimpanan untuk sampah organik & anorganik

## robotnya bisa ngapain aja

---

# 🧪 WOKWI SIMULATOR - STEP BY STEP TESTING GUIDE

## Apa itu Wokwi?

**Wokwi** adalah online simulator untuk Arduino yang memungkinkan Anda:
- ✅ Test kode tanpa hardware fisik
- ✅ Simulate sensor & actuator
- ✅ Debug logic sebelum deploy
- ✅ Share project dengan team
- ✅ Pembelajaran interaktif

**Website**: https://wokwi.com

---

## 📋 STEP 1: Siapkan File Kode

### File yang Digunakan untuk Wokwi:
```
sketch_wokwi.ino
```

**File ini sudah ada di project. Gunakan versi ini khusus untuk Wokwi.**

### Apa Bedanya dengan sketch_esp8266.ino?
- ✅ Dioptimalkan untuk simulator
- ✅ Interactive sensor value injection
- ✅ Serial command interface untuk testing
- ✅ Arduino Uno compatible (Wokwi standard)

---

## 🌐 STEP 2: Akses Wokwi Online

### Langkah 2a: Buka Wokwi
1. Buka browser (Chrome, Firefox, Safari, Edge)
2. Kunjungi: **https://wokwi.com**
3. Akan melihat halaman home Wokwi

### Langkah 2b: Login (Optional)
- Anda bisa login atau test tanpa login
- Login memungkinkan save project

### Langkah 2c: Buat Project Baru
1. Klik: **New Project**
2. Pilih: **Arduino** sebagai board
3. Klik: **Create Project**

---

## 💻 STEP 3: Copy Code ke Wokwi

### Langkah 3a: Buka File sketch_wokwi.ino

Di project folder, buka file:
```
apalahitu/sketch_wokwi.ino
```

### Langkah 3b: Copy Semua Code

1. Select All (Ctrl+A atau Cmd+A)
2. Copy (Ctrl+C atau Cmd+C)

### Langkah 3c: Paste ke Wokwi

1. Di Wokwi editor, select semua code bawaan
2. Delete semua code lama
3. Paste code dari sketch_wokwi.ino
4. Save (Ctrl+S atau Cmd+S)

### Langkah 3d: Verify Code Compiles

1. Klik: **Green Play Button** (Run)
2. Tunggu compilation process (~5 detik)
3. Jika kompilasi sukses → **"Compilation Successful"**
4. Jika error → Debug error message yang muncul

**Expected Output di Serial Monitor:**
```
▼ Mr. Trash Wheel Robot - Wokwi Simulator
Initializing...

✓ Pins initialized
✓ Color sensor initialized

▶ Running startup test sequence...

  [1/5] Testing conveyor... ✓
  [2/5] Testing separator... ✓
  [3/5] Testing LEDs... ✓
  [4/5] Testing buzzer... ✓
  [5/5] Testing sensors... ✓

✓ All systems initialized!

INTERACTIVE SIMULATION MODE
Send values to simulate waste detection:
  w[value] - Set simulated weight (0-1023)
  r[value] - Set red color (0-1023)
  g[value] - Set green color (0-1023)
  b[value] - Set blue color (0-1023)
  d[value] - Set distance (0-100)
  p - Trigger proximity sensor (waste)
  e - Emergency stop
  s - Show status
  h - Help
```

---

## 🎮 STEP 4: Interact dengan Serial Monitor

Wokwi Serial Monitor adalah tempat Anda bisa:
- 📤 Melihat output sistem
- 📥 Mengirim input commands

### Lokasi Serial Monitor:
- Di sebelah kanan/bawah Wokwi editor
- Panel dengan label **"Serial Monitor"**
- Berwarna hitam dengan text putih

### Cara Menggunakan:

1. **Klik di text input area** (kolom atas di serial monitor)
2. **Ketik command** (lihat command list di bawah)
3. **Tekan ENTER** untuk send
4. **Lihat output** di serial monitor

---

## 📊 STEP 5: Test Scenario 1 - Basic Status

### Tujuan:
Lihat status sistem awal

### Langkah-Langkah:

1. **Ketik di Serial Monitor**:
   ```
   s
   ```

2. **Tekan ENTER**

3. **Output yang diharapkan**:
   ```
   ╔════════════════════════════════════════╗
   ║      SYSTEM STATUS REPORT              ║
   ╠════════════════════════════════════════╣
   ║ Status: ACTIVE
   ║ Waste Detected: NO
   ║ Current Type: NONE
   ║ Organic Storage: 0%
   ║ Inorganic Storage: 0%
   ╚════════════════════════════════════════╝
   ```

✅ **Hasil**: Sistem dalam keadaan ACTIVE dan siap.

---

## 🎯 STEP 6: Test Scenario 2 - Simulate Organic Waste

### Tujuan:
Simulate sampah organik dideteksi dan diklasifikasi

### Langkah-Langkah:

#### 6a: Set Sensor Values untuk Organic Waste

**Input 1 - Set Weight (Ringan)**
```
w300
```
*Output: "Weight set to: 300"*

**Input 2 - Set Green Color (Organik)**
```
g600
```
*Output: "Green set to: 600"*

**Input 3 - Set Red Color**
```
r200
```
*Output: "Red set to: 200"*

**Input 4 - Set Blue Color**
```
b150
```
*Output: "Blue set to: 150"*

#### 6b: Trigger Proximity Sensor

**Input 5 - Trigger Waste Detection**
```
p
```
*Output: "Proximity sensor triggered!"*

#### 6c: Lihat Hasil Klasifikasi

**Input 6 - Check Status**
```
s
```

**Output yang diharapkan**:
```
─────────────────────────────────────────
⚠ WASTE DETECTED ON CONVEYOR!
  Classifying waste...
    Color (R:G:B) = 200:600:150
    Weight = 300
    → Green color (likely organic)
✓ Classification: ORGANIC WASTE
  ➔ Activating separator...
  ✓ Separation complete
─────────────────────────────────────────

╔════════════════════════════════════════╗
║      SYSTEM STATUS REPORT              ║
╠════════════════════════════════════════╣
║ Status: ACTIVE
║ Waste Detected: NO
║ Current Type: ORGANIC
║ Organic Storage: 0%
║ Inorganic Storage: 0%
╚════════════════════════════════════════╝
```

✅ **Hasil**: Sistem berhasil mendeteksi dan mengklasifikasi sampah organik!

---

## 🎯 STEP 7: Test Scenario 3 - Simulate Inorganic Waste

### Tujuan:
Simulate sampah inorganik (plastik, logam)

### Langkah-Langkah:

#### 7a: Set Sensor Values untuk Inorganic

**Input 1 - Set Heavy Weight**
```
w900
```

**Input 2 - Set Blue Color (Plastik)**
```
b800
```

**Input 3 - Set Red & Green**
```
r100
g100
```

#### 7b: Trigger Detection

**Input 4 - Trigger Proximity**
```
p
```

#### 7c: Check Status

**Input 5**:
```
s
```

**Output yang diharapkan**:
```
─────────────────────────────────────────
⚠ WASTE DETECTED ON CONVEYOR!
  Classifying waste...
    Color (R:G:B) = 100:100:800
    Weight = 900
    → Heavy weight (override to inorganic)
✓ Classification: INORGANIC WASTE
  ➔ Activating separator...
  ✓ Separation complete
─────────────────────────────────────────
```

✅ **Hasil**: Sistem berhasil mendeteksi sampah inorganik!

---

## 🚨 STEP 8: Test Scenario 4 - Emergency Stop

### Tujuan:
Test safety feature - emergency stop

### Langkah-Langkah:

#### 8a: Trigger Emergency Stop

**Input 1**:
```
e
```

#### 8b: Lihat Output

**Output yang diharapkan**:
```
🛑 EMERGENCY STOP ACTIVE! 🛑
⏸ Conveyor stopped
```

#### 8c: Status Sekarang

**Input 2**:
```
s
```

**Output**:
- System akan menunjukkan **INACTIVE**
- Semua motor berhenti

✅ **Hasil**: Emergency stop berfungsi dengan sempurna!

---

## 🎯 STEP 9: Test Scenario 5 - Help Menu

### Tujuan:
Lihat semua available commands

### Langkah-Langkah:

**Input**:
```
h
```

**Output yang diharapkan**:
```
╔════════════════════════════════════════╗
║      SIMULATION COMMANDS               ║
╠════════════════════════════════════════╣
║ w[0-1023] - Set weight sensor value    ║
║ r[0-1023] - Set red color value        ║
║ g[0-1023] - Set green color value      ║
║ b[0-1023] - Set blue color value       ║
║ d[0-100]  - Set distance value         ║
║ p         - Trigger waste detection    ║
║ e         - Emergency stop             ║
║ s         - Show status                ║
║ h         - Show this help             ║
╚════════════════════════════════════════╝
```

---

## 📋 COMMAND REFERENCE

### Sensor Value Commands

| Command | Range | Fungsi | Contoh |
|---------|-------|--------|--------|
| `w[val]` | 0-1023 | Set weight | `w300` |
| `r[val]` | 0-1023 | Set red color | `r400` |
| `g[val]` | 0-1023 | Set green color | `g600` |
| `b[val]` | 0-1023 | Set blue color | `b150` |
| `d[val]` | 0-100 | Set distance | `d30` |

### Control Commands

| Command | Fungsi |
|---------|--------|
| `p` | Proximity sensor (waste detected) |
| `e` | Emergency stop |
| `s` | Show status |
| `h` | Help menu |

### Sensor Value Guidelines

**Untuk Organic Waste:**
- Weight: 100-400 (light)
- Red: 200-300 (low)
- Green: 500-700 (high)
- Blue: 100-200 (low)

**Untuk Inorganic Waste:**
- Weight: 600-1000 (heavy)
- Red: 100-300 (low)
- Green: 100-300 (low)
- Blue: 500-800 (high)

---

## ✅ CHECKLIST - Wokwi Testing

### Test Coverage

- [ ] **Startup**
  - [ ] Code compile successfully
  - [ ] Startup sequence visible
  - [ ] All systems initialized

- [ ] **Basic Operation**
  - [ ] Status command works (`s`)
  - [ ] Help command works (`h`)
  - [ ] Serial output readable

- [ ] **Organic Waste**
  - [ ] Set sensor values (weight, colors)
  - [ ] Trigger proximity (`p`)
  - [ ] Classification: ORGANIC
  - [ ] Separator activated

- [ ] **Inorganic Waste**
  - [ ] Set sensor values (heavy, blue)
  - [ ] Trigger proximity (`p`)
  - [ ] Classification: INORGANIC
  - [ ] Separator activated

- [ ] **Safety Features**
  - [ ] Emergency stop (`e`)
  - [ ] All motors stop
  - [ ] Status shows INACTIVE

- [ ] **Edge Cases**
  - [ ] Very light weight (<50)
  - [ ] Very heavy weight (>800)
  - [ ] Distance values
  - [ ] Multiple waste detections

---

## 🎓 EXPECTED RESULTS SUMMARY

| Test | Input | Expected Output | Status |
|------|-------|-----------------|--------|
| Startup | (auto) | Initialization sequence | ✅ |
| Status | `s` | System status displayed | ✅ |
| Organic | `w300`, `g600`, `p` | "ORGANIC WASTE" | ✅ |
| Inorganic | `w900`, `b800`, `p` | "INORGANIC WASTE" | ✅ |
| E-Stop | `e` | "EMERGENCY STOP ACTIVE" | ✅ |
| Help | `h` | Command menu | ✅ |

---

## 🐛 TROUBLESHOOTING WOKWI

### Masalah: Code tidak compile

**Solusi:**
1. Check error message di Wokwi console
2. Pastikan copy seluruh file sketch_wokwi.ino
3. Tidak ada karakter aneh atau encoding issue
4. Coba refresh page (F5)

### Masalah: Serial Monitor tidak muncul

**Solusi:**
1. Klik play button untuk run
2. Serial Monitor otomatis terbuka
3. Jika tetap tidak ada, cek window size
4. Scroll down untuk lihat monitor panel

### Masalah: Input command tidak responsif

**Solusi:**
1. Pastikan fokus di serial monitor input box
2. Tekan ENTER setelah command
3. Tunggu beberapa detik untuk respons
4. Cek baud rate (harus 9600)

### Masalah: Output garbled atau blank

**Solusi:**
1. Refresh page (F5)
2. Clear serial monitor (ada X button)
3. Jalankan ulang simulasi
4. Check browser console (F12)

---

## 💡 TIPS & TRICKS

### 1. Rapid Testing
```
Ketik cepat tanpa tunggu:
w500
r300
g400
b100
p
s
```
Semua command akan diproses berurutan.

### 2. Save Test Values
Catat sensor values yang sering digunakan:
```
// Organic
Light Green: w300 r200 g600 b150

// Inorganic
Heavy Blue: w900 r100 g100 b800
```

### 3. Debug Classification
Jika hasil tidak sesuai:
1. Check weight value (`w`)
2. Check color values (`r`, `g`, `b`)
3. Lihat classification logic di kode
4. Adjust threshold di `sketch_wokwi.ino`

### 4. Monitor in Real-time
Sering ketik `s` untuk monitor status:
```
p
s
w400
s
e
s
```

---

## 📸 SCREENSHOT GUIDE

### Screenshot 1: Wokwi Home
```
https://wokwi.com → New Project → Arduino
```

### Screenshot 2: Code Editor
```
Paste sketch_wokwi.ino code
Click Play to compile & run
```

### Screenshot 3: Serial Monitor
```
Lihat output di panel kanan bawah
Ketik command di input box
```

---

## 🎯 NEXT STEPS SETELAH WOKWI TESTING

Setelah sukses test di Wokwi:

### 1. Code Aman untuk Deploy
✅ Logic sudah verified
✅ Klasifikasi sudah tested
✅ Safety features confirmed

### 2. Deploy ke Hardware Fisik
```
Gunakan: sketch_esp8266.ino
Upload ke: NodeMCU ESP8266
Follow: QUICK_START.md
```

### 3. Calibrate dengan Hardware
```
Test dengan sensor & motor asli
Adjust thresholds jika diperlukan
Fine-tune classification
```

### 4. Full System Integration
```
Pasang semua komponen
Test end-to-end
Optimize performance
```

---

## 📞 REFERENCE

Untuk info lebih lanjut:
- **Wokwi Docs**: https://docs.wokwi.com
- **Arduino Basics**: https://www.arduino.cc
- **sketch_wokwi.ino**: Code reference
- **QUICK_START.md**: Hardware deployment

---

## ✨ SUCCESS INDICATORS

Ketika testing Wokwi berhasil, Anda akan melihat:

- ✅ Startup sequence lengkap
- ✅ Status command berfungsi
- ✅ Organic detection bekerja
- ✅ Inorganic detection bekerja
- ✅ Emergency stop responsive
- ✅ Semua output clear & readable

**Jika semua ini tampil: SISTEM READY FOR DEPLOYMENT! 🚀**


