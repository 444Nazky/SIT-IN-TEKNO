# Simplified Waste Sorting Robot - Flowchart

## Main Process Flow

```
┌─────────────────┐
│   START/SETUP   │
└─────────┬───────┘
          │
┌─────────▼───────┐
│  INITIALIZE     │
│  ALL SYSTEMS    │
└─────────┬───────┘
          │
┌─────────▼───────┐
│   MAIN LOOP     │
└─────────┬───────┘
          │
    ┌─────┴─────┐
    │ Emergency │
    │   Stop?   │
    └─────┬─────┘
          │
    ┌─────┴─────┐
    │           │
   YES          NO
    │           │
┌───▼───┐  ┌────▼─────────┐
│ STOP  │  │ CHECK STORAGE│
│ ALL   │  │ STATUS       │
└───┬───┘  └────┬─────────┘
    │           │
    └───────────┼─────────┐
                │         │
          ┌─────▼─────────▼─────┐
          │    WASTE DETECTED?  │
          └─────┬───────────────┘
                │
          ┌─────┴─────┐
          │           │
         YES          NO
          │           │
    ┌─────▼─────┐  ┌──▼─────────┐
    │ STOP      │  │ RUN        │
    │ CONVEYOR  │  │ CONVEYOR   │
    └─────┬─────┘  └─────┬──────┘
          │               │
    ┌─────▼─────┐         │
    │ CLASSIFY  │         │
    │ WASTE     │         │
    │           │         │
    │ • Color   │         │
    │ • Weight  │         │
    └─────┬─────┘         │
          │               │
    ┌─────┴─────┐         │
    │ ORGANIC?  │         │
    └─────┬─────┘         │
          │               │
    ┌─────┴─────┐         │
    │           │         │
   YES          NO        │
    │           │         │
┌───▼───┐  ┌────▼─────┐   │
│ SEPARATE│  │ SEPARATE │   │
│ ORGANIC │  │INORGANIC │   │
└───┬───┘  └────┬─────┘   │
    │           │         │
    └─────┬─────┼─────────┘
          │     │
    ┌─────▼─────▼─────┐
    │ RETURN TO       │
    │ NEUTRAL         │
    └─────┬───────────┘
          │
    ┌─────▼─────┐
    │ RESUME    │
    │ CONVEYOR  │
    └─────┬─────┘
          │
          └───────────┐
                      │
          ┌───────────▼───────────┐
          │      LOOP BACK        │
          └───────────────────────┘
```

## System Components & Equipment Details

### Sensors & Input Devices
| Component | Model/Type | Pin Connection | Function | Specifications |
|-----------|------------|----------------|----------|----------------|
| **Proximity Sensor** | IR Infrared | Digital Pin 2 | Detects waste presence | Active LOW, 5V |
| **Color Sensor** | TCS34725 | Pins 4,5,6,7,8 | RGB color analysis | I2C, 3.3V-5V |
| **Weight Sensor** | Load Cell + HX711 | Analog Pin A0 | Measures waste weight | 0-5V, 10-bit ADC |
| **Ultrasonic Sensor** | HC-SR04 | Trig A1, Echo A2 | Storage level detection | 2-400cm range |
| **Emergency Stop** | Push Button | Digital Pin 13 | System safety halt | Pull-up resistor |

### Actuators & Output Devices  
| Component | Model/Type | Pin Connection | Function | Specifications |
|-----------|------------|----------------|----------|----------------|
| **Conveyor Motor** | DC Motor + Driver | PWM Pin 3 | Moves conveyor belt | 12V, PWM 0-255 |
| **Stepper Motor** | 28BYJ-48 | Pins 9,10,11,12 | Waste separation mechanism | 5V, 512 steps/rev |
| **LED Organic** | Green LED | Pin A3 | Organic storage indicator | 2V, 20mA |
| **LED Inorganic** | Blue LED | Pin A4 | Inorganic storage indicator | 2V, 20mA |
| **Buzzer** | Active Buzzer | Digital Pin 7 | Audio alerts | 5V, Active High |

### Power & Control
| Component | Connection | Function | Rating |
|-----------|------------|----------|--------|
| **Arduino Board** | USB 5V | Main controller | ATmega328P |
| **Power Supply** | 12V adapter | Motor power | 2A max |
| **Breadboard** | Distribution | Circuit connections | 400 points |
| **Jumper Wires** | M-M, M-F | Signal connections | 22 AWG |

### Pin Configuration Summary
```
Digital Pins:
├── Pin 2  → Proximity Sensor
├── Pin 3  → Conveyor Motor (PWM)
├── Pins 4-7 → Color Sensor (S0-S3)
├── Pin 8  → Color Sensor Output
├── Pins 9-12 → Stepper Motor (IN1-IN4)
└── Pin 13 → Emergency Stop

Analog Pins:
├── A0 → Weight Sensor
├── A1 → Ultrasonic Trig
├── A2 → Ultrasonic Echo
├── A3 → Organic LED
└── A4 → Inorganic LED
```

## Key Decisions

### Classification Logic
```
WASTE CLASSIFICATION:
├── Brown/Green Color → ORGANIC
├── Light Weight (<500) → ORGANIC  
└── Heavy Weight (>800) → INORGANIC
```

### Separation Positions
```
STEPPER MOTOR:
├── Organic → 128 steps (1/4 turn)
├── Inorganic → 384 steps (3/4 turn)
└── Neutral → 0 steps (home)
```

### Storage Alerts
```
STORAGE LEVEL:
├── < 70% → Normal (LED off)
├── 70-90% → Warning (LED blink)
└── > 90% → Alert (Buzzer + LED on)
```

## Operation States

1. **READY** - System initialized, waiting for waste
2. **RUNNING** - Conveyor moving, normal operation  
3. **DETECTED** - Waste found, stopping for analysis
4. **CLASSIFYING** - Analyzing color and weight data
5. **SEPARATING** - Moving stepper to correct position
6. **ALERT** - Storage full or emergency condition

## Essential Timings

- **Loop Cycle**: 100ms
- **Classification**: 2 seconds
- **Separation**: 1 second  
- **Status Display**: Every 10 seconds
- **Emergency Response**: Instant

This simplified flowchart shows the core waste sorting process from detection to separation, focusing on the main decision points and system responses.
