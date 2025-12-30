# Mr. Trash Wheel Robot - Arduino Project

## Overview
This Arduino project creates a waste separation robot that:
- Pulls garbage using a conveyor belt
- Automatically separates organic and inorganic waste
- Stores waste in designated areas
- Monitors storage levels and provides alerts

## Essential Components

### Core Electronics
- **Arduino Uno R3** - Main microcontroller
- **DC Motor** - 6-12V with gearbox (conveyor belt)
- **Stepper Motor** - 28BYJ-48 5V (waste separation)
- **Color Sensor** - TCS230/TCS3200 (waste classification)
- **Weight Sensor** - HX711 + load cell (material detection)
- **Ultrasonic Sensor** - HC-SR04 (storage monitoring)
- **IR Sensor** - Proximity detection

### Basic Wiring Kit
- **Jumper Wires** - 40 pieces, male-to-male, 20cm, assorted colors
- **Breadboard** - 830 tie-points
- **Resistors** - 220Ω (LED protection), 10kΩ (pull-up)
- **LEDs** - Green and Blue, 5mm
- **Buzzer** - 5V active buzzer
- **Emergency Stop Button** - Momentary switch
- **Motor Drivers** - L298N (DC motor), ULN2003 (stepper motor)

### Power & Cables
- **9V/2A Power Adapter** - For motors
- **USB Cable** - Arduino programming
- **Wire Strippers** - 20-30 AWG
- **Multimeter** - For testing connections

## Pin Configuration

| Component | Pin | Type |
|-----------|-----|------|
| Conveyor Motor | 3 | PWM Output |
| Color Sensor S0 | 4 | Digital Output |
| Color Sensor S1 | 5 | Digital Output |
| Color Sensor S2 | 6 | Digital Output |
| Color Sensor S3 | 7 | Digital Output |
| Color Sensor OUT | 8 | Digital Input |
| Weight Sensor | A0 | Analog Input |
| Stepper Pin 1 | 9 | Digital Output |
| Stepper Pin 2 | 10 | Digital Output |
| Stepper Pin 3 | 11 | Digital Output |
| Stepper Pin 4 | 12 | Digital Output |
| IR Sensor | 2 | Digital Input |
| Organic LED | 5 | Digital Output |
| Inorganic LED | 6 | Digital Output |
| Buzzer | 7 | Digital Output |
| Emergency Stop | 13 | Digital Input |
| Ultrasonic Trig | A1 | Digital Output |
| Ultrasonic Echo | A2 | Digital Input |

## How It Works

### 1. Waste Detection
- IR sensor detects when waste enters the conveyor system
- System automatically stops conveyor for classification

### 2. Waste Classification
- **Color Sensor**: Analyzes RGB values to determine waste type
  - Brown/Green colors → Organic waste
  - Various other colors → Inorganic waste
- **Weight Sensor**: Additional validation based on weight
  - Light weight → More likely organic
  - Heavy weight → More likely inorganic

### 3. Separation Process
- Stepper motor moves to appropriate position:
  - 128 steps (quarter turn) → Organic storage
  - 384 steps (three-quarter turn) → Inorganic storage
- LED indicators show which storage area is active
- Waste is directed to correct storage bin

### 4. Storage Management
- Ultrasonic sensor monitors storage levels
- Alerts when storage is >90% full
- LEDs blink to indicate storage status

### 5. Safety Features
- Emergency stop button halts all operations
- System automatically stops if sensors fail
- Buzzer alerts for various conditions

## Serial Monitor Commands

The robot responds to these commands via Serial Monitor (9600 baud):

| Command | Function |
|---------|----------|
| `S` or `s` | Display current system status |
| `E` or `e` | Activate emergency stop |
| `R` or `r` | Toggle system on/off |

## Setup Instructions

### 1. Hardware Assembly
1. Connect all components according to the pin configuration table
2. Install motor drivers for conveyor and stepper motors
3. Set up power supplies (ensure adequate current for all motors)
4. Test all connections before powering on

### 2. Arduino Setup
1. Install Arduino IDE if not already installed
2. Upload the `sketch.ino` file to your Arduino
3. Open Serial Monitor to view system status and logs

### 3. Calibration
- Adjust `CLASSIFICATION_THRESHOLD` based on your weight sensor
- Modify color detection values in `classifyWaste()` function
- Tune conveyor speed in `CONVEYOR_SPEED` constant
- Adjust storage level mapping in `checkStorageLevels()`

## Code Structure

### Main Functions
- `setup()` - Initialize all hardware and run test sequence
- `loop()` - Main control cycle, handles waste detection and processing

### Sensor Functions
- `readColorSensor()` - Read RGB values from color sensor
- `readWeightSensor()` - Get weight measurement
- `detectWaste()` - Check for waste presence
- `checkStorageLevels()` - Monitor storage capacity

### Motor Control
- `startConveyor()` / `stopConveyor()` - Control belt movement
- `activateSeparator()` - Move stepper to correct position
- `moveStepperToPosition()` - Precise stepper positioning

### Classification Logic
- `classifyWaste()` - Combine sensor data to determine waste type
- Uses color analysis and weight validation

### Safety & Monitoring
- `emergencyStopProcedure()` - Handle emergency stops
- `displayStatus()` - Show current system state
- `updateStorageStatus()` - Monitor and alert for full storage

## Customization Options

### Waste Classification
Modify the classification logic in `classifyWaste()` to improve accuracy for your specific waste types.

### Motor Speed
Adjust `CONVEYOR_SPEED` (0-255) and stepper timing in stepper functions.

### Storage Thresholds
Change alert levels by modifying the percentage thresholds in `updateStorageStatus()`.

### LED Behavior
Customize LED patterns in `updateLEDs()` function.

## Troubleshooting

### Common Issues
1. **Motors not moving**: Check power supply and driver connections
2. **Poor classification**: Adjust sensor calibration values
3. **Stepper not positioning correctly**: Verify stepper sequence and connections
4. **Storage alerts too frequent**: Adjust distance mapping values

### Testing
- Use the built-in test sequence in `setup()` to verify all components
- Monitor Serial output for detailed system status
- Test emergency stop functionality regularly

## Future Enhancements

- Add more sophisticated classification algorithms
- Implement wireless monitoring via WiFi/Bluetooth
- Add servo motors for more precise separation
- Include data logging for waste analysis
- Add touch screen interface for status display

## Safety Notes

- Always test in a controlled environment
- Ensure emergency stop is easily accessible
- Check all connections before powering on
- Use appropriate safety measures when working with moving parts

## Support

This code provides a solid foundation for a waste separation robot. Modify and extend it based on your specific hardware and requirements.
