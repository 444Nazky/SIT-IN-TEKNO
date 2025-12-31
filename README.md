# Mr. Trash Wheel Robot

## Requirements
- Water resistant
- Can float

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
- `pin.md` - Pin configuration

