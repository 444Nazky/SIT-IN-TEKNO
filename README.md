## Simulation :
https://www.tinkercad.com/things/jpwSAPOeOHG/editel?returnTo=%2Fdashboard&sharecode=fUyAqQrS5y_9E6Z51sX3TLiKuFIDK8nvYsb_JUKOY_c

## Voltage:

| Komponen | Voltage | Arus (perkiraan) |
|----------|---------|------------------|
| Servo SG90 | 5V-6V | 100-250mA |
| Gearbox DC | 6V-12V | 200-500mA per motor |

## Pin Configuration

| Component | Pin |
|-----------|-----|
| conveyor | 22 |
| waste_sensor | 24 |
| weight | A0 |
| ultrasonic_trig | 23 |
| ultrasonic_echo | 25 |
| emergency_stop | 26 |
| esp8266_rx | 10 |
| esp8266_tx | 11 |

---

## Functions
1. Pull waste using conveyor
2. Separate organic & inorganic waste using camera
3. Storage area for both waste types

---

## Quick Start (Wokwi Simulator)

1. **Buka** https://wokwi.com
2. **New Project** → Arduino
3. **Copy** code dari sketch_wokwi.ino
4. **Paste** ke Wokwi editor
5. **Run** (play button)

## Commands (Serial Monitor)

| Command | Description |
|---------|-------------|
| `s` | Show status |
| `h` | Help menu |
| `p` | Trigger waste detection |
| `e` | Emergency stop |
| `w[val]` | Set weight (0-1023) |
| `r[val]` | Set red color (0-1023) |
| `g[val]` | Set green color (0-1023) |
| `b[val]` | Set blue color (0-1023) |

## Test Examples

**Organic Waste:**
```
w300 g600 p → "ORGANIC WASTE"
```

**Inorganic Waste:**
```
w900 b800 p → "INORGANIC WASTE"
```

**Emergency Stop:**
```
e → "EMERGENCY STOP"
```

## Files
- `sketch_esp8266_wifi.ino` - Main code (NodeMCU ESP8266)
- `sketch_esp8266.ino` - Basic version
- `TODO.md` - Pin configuration

