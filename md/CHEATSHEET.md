# 🚀 TRASH ROBOT ESP8266 - CHEAT SHEET

## ⚡ 30-SECOND SETUP

```bash
1. Open: sketch_esp8266.ino
2. Select: Tools → Board → NodeMCU 1.0
3. Upload: Ctrl+U
4. Monitor: Tools → Serial Monitor (115200 baud)
```

✅ Done! System ready to test.

---

## 📱 3 VERSIONS AT A GLANCE

```
sketch_esp8266.ino
├─ Size: 60 KB
├─ Speed: 2-3s boot
├─ Features: Basic
└─ Use: Prototype & Testing ⭐

sketch_esp8266_wifi.ino
├─ Size: 120 KB
├─ Speed: 4-5s boot
├─ Features: WiFi + Dashboard
└─ Use: Production 🌐

sketch_wokwi.ino
├─ Size: 12 KB
├─ Speed: Instant
├─ Features: Simulation
└─ Use: Development 💻
```

---

## 🔌 PIN QUICK MAP

```
D0 → Stepper 3
D1 → Conveyor Motor
D2 → Color S0
D3 → Color S1
D4 → Proximity / Color S2
D5 → Inorganic LED / Color S3
D6 → Color OUT
D7 → Buzzer
D8 → Emergency Stop
A0 → Weight
```

⚠️ Multiple pins have conflicts! See ESP8266_GUIDE.md

---

## 📋 SERIAL COMMANDS

```
s → Status
e → Emergency
r → Toggle
c → Color read
w → Weight read
h → Help
```

Type in Serial Monitor while connected.

---

## 🎛️ WEB API (WiFi Version)

```
GET  http://<IP>/              → Dashboard
GET  http://<IP>/api/status    → JSON status
GET  http://<IP>/api/logs      → Waste history
GET  http://<IP>/api/control?action=toggle
GET  http://<IP>/api/control?action=emergency
```

---

## 🐛 QUICK FIXES

| Problem | Fix |
|---------|-----|
| Garbage text | Set baud to 115200 |
| Board not found | Install CH340 |
| Upload fails | Hold FLASH button |
| No WiFi | Check SSID/pass |
| Motor won't move | Supply 5V power |

---

## ⚙️ SENSOR VALUES

**Color Classification:**
- Brown: R>400, G>300, B<300 → Organic
- Green: G>R & G>B & G>350 → Organic
- Light: Weight<500 → Organic
- Heavy: Weight>800 → Inorganic

**Storage Levels:**
- 0-30%: LED off
- 30-70%: LED blink
- 70-100%: LED solid

---

## 📚 DOCS ROADMAP

```
START HERE
    ↓
FILE_GUIDE.md (this overview)
    ↓
QUICK_START.md (5 min setup)
    ↓
    ├→ Working? → Deploy & Enjoy ✅
    └→ Problems? → ESP8266_GUIDE.md
    
Need more?
    ↓
README_ESP8266.md (full reference)
```

---

## 🎯 DEPLOYMENT FLOW

```
1. Code Development
   └→ Test with sketch_wokwi.ino on Wokwi

2. Hardware Integration
   └→ Upload sketch_esp8266.ino
   └→ Test sensors & motors
   └→ Verify via Serial Monitor

3. Production (Optional)
   └→ Upload sketch_esp8266_wifi.ino
   └→ Setup WiFi credentials
   └→ Access web dashboard
   └→ Setup logging/alerts

4. Monitor & Maintain
   └→ Check status via Serial or Web
   └→ Review logs
   └→ Adjust thresholds as needed
```

---

## ✅ VERIFICATION

```
✓ Board detected in IDE
✓ Code compiles (no errors)
✓ Upload succeeds
✓ Serial output visible (115200 baud)
✓ Startup sequence shows
✓ Motors respond to power
✓ Sensors give readings
✓ LED lights up
✓ Buttons work
```

---

## 🔧 COMMON CONFIGURATIONS

### Increase Motor Speed
```cpp
const int CONVEYOR_SPEED = 255;  // was 200
```

### Adjust Classification Threshold
```cpp
const int CLASSIFICATION_THRESHOLD = 600;  // was 500
```

### Change Separation Timing
```cpp
const int SEPARATOR_DELAY = 1200;  // was 800 (ms)
```

### Set WiFi Credentials
```cpp
const char* SSID = "YourNetwork";
const char* PASSWORD = "YourPassword";
```

---

## 📊 STATUS OUTPUT

```
╔════════════════════════════════════════╗
║      SYSTEM STATUS REPORT              ║
╠════════════════════════════════════════╣
║ Status: ACTIVE
║ Waste Detected: YES
║ Current Type: ORGANIC
║ Organic Storage: 45%
║ Inorganic Storage: 30%
╚════════════════════════════════════════╝
```

---

## 🆘 EMERGENCY STOP

**Activate**: Press physical button on D8

**Effects:**
- All motors stop immediately
- LEDs flash warning
- Buzzer sounds continuously
- System halts

**Resume**: Release button → System resumes

---

## 🌐 WIFI DASHBOARD

Access: `http://<ESP_IP_ADDRESS>`

Shows:
- Real-time status
- Storage levels (progress bars)
- Waste statistics
- Control buttons
- System health

Updated every 1 second.

---

## 📈 PERFORMANCE SPECS

```
Processor:       80 MHz 32-bit
RAM:             160 KB
Flash:           4 MB
Max WiFi:        11 Mbps
Boot Time:       2-5 seconds
Response Time:   <200ms
Power (idle):    100-150 mA
Power (active):  200-500 mA
```

---

## 🎓 LEARNING PATH

```
Beginner
  ├→ Read QUICK_START.md
  ├→ Upload sketch_esp8266.ino
  ├→ Test with Serial Monitor
  └→ Done! ✅

Intermediate
  ├→ Read ESP8266_GUIDE.md
  ├→ Understand pin mapping
  ├→ Modify configurations
  └→ Optimize performance

Advanced
  ├→ Read README_ESP8266.md
  ├→ Upload sketch_esp8266_wifi.ino
  ├→ Setup WiFi & dashboard
  ├→ Implement custom logic
  └→ Add cloud integration
```

---

## 🚀 TIPS & TRICKS

**Faster debugging:**
```cpp
// Use Serial.print for quicker output
Serial.print("Value: ");
Serial.println(value);
```

**Save memory:**
```cpp
// Use const for unchanging values
const int SPEED = 200;
```

**Avoid freeze:**
```cpp
// Always use millis() not delay()
if (millis() - lastTime >= 1000) {
  // Do something
}
```

**WiFi power save:**
```cpp
WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
```

---

## 🎯 SUCCESS CHECKLIST

- [ ] Chose version (Standard/WiFi/Wokwi)
- [ ] Downloaded/opened correct file
- [ ] Board configured correctly
- [ ] Code uploaded successfully
- [ ] Serial Monitor showing output
- [ ] Startup sequence visible
- [ ] Commands responding
- [ ] Ready to deploy ✅

---

## 🆚 TROUBLESHOOTING MATRIX

```
Serial garbage?
├─ Check baud rate = 115200
└─ Press RST button

Board not found?
├─ Install CH340 driver
└─ Try different USB port

WiFi won't connect?
├─ Check SSID spelling
├─ Check password spelling
└─ Use 2.4GHz (not 5GHz)

Motor doesn't move?
├─ Supply external 5V power
├─ Check pin connections
└─ Verify motor working

Sensor reading wrong?
├─ Check sensor connected
├─ Verify voltage (3.3V or 5V)
└─ Test with known values
```

---

## 📞 HELP RESOURCES

| Need | Read |
|------|------|
| Quick start | QUICK_START.md |
| Pin mapping | ESP8266_GUIDE.md |
| Full docs | README_ESP8266.md |
| Logic flow | flowchart.md |
| All files | FILE_GUIDE.md |

---

## 🎉 YOU'RE READY!

```
1. Pick a version
2. Follow QUICK_START.md
3. Upload code
4. Monitor output
5. Test system
6. Deploy 🚀
```

**Questions?** Check the docs. It's there. 📚

---

**Version**: 1.0  
**Updated**: 2025  
**Status**: ✅ READY  

