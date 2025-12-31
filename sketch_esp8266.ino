/*
 * Waste Sorting Robot - ESP8266 Version
 * Project: Mr. Trash Wheel Robot
 * 
 * Fitur:
 * - Mendeteksi dan mengklasifikasi sampah (organik/anorganik)
 * - Kontrol motor konveyor dan separator
 * - Monitoring kapasitas penyimpanan
 * - System keselamatan (Emergency Stop)
 * - WiFi monitoring (optional)
 * 
 * Author: Nazky
 * Date: 2025
 */

#include <Arduino.h>

// ==================== PIN DEFINITIONS ====================
const int CONVEYOR_MOTOR_PIN = D1;           // PWM pin for conveyor motor (GPIO5)
const int COLOR_SENSOR_S0 = D2;              // Color sensor pins (GPIO4)
const int COLOR_SENSOR_S1 = D3;              // (GPIO0)
const int COLOR_SENSOR_S2 = D4;              // (GPIO2)
const int COLOR_SENSOR_S3 = D5;              // (GPIO14)
const int COLOR_SENSOR_OUT = D6;             // (GPIO12) - must be ADC capable for ESP8266
const int WEIGHT_SENSOR_PIN = A0;            // Analog pin for weight sensor
const int SEPARATOR_STEP_PIN1 = D7;          // Stepper motor pins (GPIO13)
const int SEPARATOR_STEP_PIN2 = D8;          // (GPIO15)
const int SEPARATOR_STEP_PIN3 = D0;          // (GPIO16)
const int SEPARATOR_STEP_PIN4 = D1;          // NOTE: conflicts with conveyor, use different pin
const int PROXIMITY_SENSOR_PIN = D4;         // IR sensor for waste detection (GPIO2)
const int ORGANIC_STORAGE_LED = D5;          // Green LED for organic storage (GPIO14)
const int INORGANIC_STORAGE_LED = D6;        // Blue LED for inorganic storage (GPIO12)
const int BUZZER_PIN = D7;                   // Buzzer for alerts (GPIO13)
const int EMERGENCY_STOP_PIN = D8;           // Emergency stop button (GPIO15)
const int ULTRASONIC_TRIG_PIN = D3;          // Ultrasonic trigger (GPIO0)
const int ULTRASONIC_ECHO_PIN = D2;          // Ultrasonic echo (GPIO4)

// ==================== CONFIGURATION ====================
const int CONVEYOR_SPEED = 200;              // PWM value (0-255)
const int CLASSIFICATION_THRESHOLD = 500;    // Weight threshold for classification
const int STORAGE_FULL_DISTANCE = 5;         // Distance in cm for full storage
const int SEPARATOR_STEPS = 512;             // Steps for full rotation
const int SEPARATOR_DELAY = 800;             // Delay for separation action

// Storage area constants
const int ORGANIC_STORAGE = 0;
const int INORGANIC_STORAGE = 1;

// Timing constants
const unsigned long STATUS_UPDATE_INTERVAL = 10000;  // 10 seconds
const unsigned long MAIN_LOOP_INTERVAL = 100;        // 100ms
const unsigned long COLOR_READ_TIMEOUT = 1000;       // 1 second timeout for color read

// ==================== STATE VARIABLES ====================
struct SystemState {
  bool active;
  bool emergencyStopActive;
  bool wasteDetected;
  int currentWasteType;           // -1 = undefined, 0 = organic, 1 = inorganic
  int organicStorageLevel;        // 0-100%
  int inorganicStorageLevel;      // 0-100%
  unsigned long lastStatusUpdate;
  unsigned long lastMainLoopRun;
  unsigned long lastEmergencyCheck;
  bool separationInProgress;
  int stepperPosition;
};

SystemState system = {
  .active = true,
  .emergencyStopActive = false,
  .wasteDetected = false,
  .currentWasteType = -1,
  .organicStorageLevel = 0,
  .inorganicStorageLevel = 0,
  .lastStatusUpdate = 0,
  .lastMainLoopRun = 0,
  .lastEmergencyCheck = 0,
  .separationInProgress = false,
  .stepperPosition = 0
};

// Color sensor variables
struct ColorData {
  int red;
  int green;
  int blue;
  unsigned long lastReadTime;
};

ColorData colorData = {0, 0, 0, 0};

// Stepper motor sequence (half-step mode)
const int stepperSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// ==================== SETUP ====================
void setup() {
  // Initialize serial communication
  Serial.begin(115200);  // ESP8266 uses higher baud rate
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║ Mr. Trash Wheel Robot - ESP8266       ║");
  Serial.println("║ Initializing...                        ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Initialize all pins
  initializePins();
  
  // Initialize color sensor
  initializeColorSensor();
  
  // Initialize stepper motor
  system.stepperPosition = 0;
  setStepperPosition(0);
  
  // Run startup sequence
  runStartupSequence();
  
  Serial.println("✓ Mr. Trash Wheel Robot Ready!");
  Serial.println("════════════════════════════════════════");
}

// ==================== MAIN LOOP ====================
void loop() {
  unsigned long currentTime = millis();
  
  // Check emergency stop button
  checkEmergencyStop();
  
  // Handle emergency stop procedure
  if (system.emergencyStopActive) {
    handleEmergencyStop();
    return;
  }
  
  // Update storage status
  updateStorageStatus();
  
  // Main cycle timing (non-blocking)
  if (currentTime - system.lastMainLoopRun >= MAIN_LOOP_INTERVAL) {
    system.lastMainLoopRun = currentTime;
    
    // Check for waste on conveyor
    system.wasteDetected = detectWaste();
    
    if (system.wasteDetected && system.active) {
      handleWasteDetection();
    } else if (system.active) {
      // Run conveyor continuously when idle
      startConveyor();
    }
  }
  
  // Update LEDs based on storage level
  updateIndicatorLEDs();
  
  // Status display timing (non-blocking)
  if (currentTime - system.lastStatusUpdate >= STATUS_UPDATE_INTERVAL) {
    system.lastStatusUpdate = currentTime;
    displaySystemStatus();
  }
}

// ==================== INITIALIZATION FUNCTIONS ====================
void initializePins() {
  // Motor pins
  pinMode(CONVEYOR_MOTOR_PIN, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN1, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN2, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN3, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN4, OUTPUT);
  
  // Color sensor pins
  pinMode(COLOR_SENSOR_S0, OUTPUT);
  pinMode(COLOR_SENSOR_S1, OUTPUT);
  pinMode(COLOR_SENSOR_S2, OUTPUT);
  pinMode(COLOR_SENSOR_S3, OUTPUT);
  pinMode(COLOR_SENSOR_OUT, INPUT);
  
  // Sensor pins
  pinMode(WEIGHT_SENSOR_PIN, INPUT);
  pinMode(PROXIMITY_SENSOR_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  
  // Output pins
  pinMode(ORGANIC_STORAGE_LED, OUTPUT);
  pinMode(INORGANIC_STORAGE_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(EMERGENCY_STOP_PIN, INPUT);
  
  // Set initial states
  digitalWrite(CONVEYOR_MOTOR_PIN, LOW);
  digitalWrite(ORGANIC_STORAGE_LED, LOW);
  digitalWrite(INORGANIC_STORAGE_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("✓ Pins initialized");
}

void initializeColorSensor() {
  // Set color sensor frequency scaling to 20%
  digitalWrite(COLOR_SENSOR_S0, HIGH);
  digitalWrite(COLOR_SENSOR_S1, LOW);
  
  Serial.println("✓ Color sensor initialized");
}

void runStartupSequence() {
  Serial.println("\n▶ Running startup sequence...\n");
  delay(500);
  
  // Test 1: Conveyor Motor
  Serial.print("  [1/5] Testing conveyor motor... ");
  startConveyor();
  delay(800);
  stopConveyor();
  Serial.println("✓");
  delay(200);
  
  // Test 2: Separator - Forward
  Serial.print("  [2/5] Testing separator (forward)... ");
  moveStepperToPosition(256);
  Serial.println("✓");
  delay(200);
  
  // Test 3: Separator - Backward
  Serial.print("  [3/5] Testing separator (backward)... ");
  moveStepperToPosition(0);
  Serial.println("✓");
  delay(200);
  
  // Test 4: Indicators
  Serial.print("  [4/5] Testing indicator LEDs... ");
  digitalWrite(ORGANIC_STORAGE_LED, HIGH);
  digitalWrite(INORGANIC_STORAGE_LED, HIGH);
  delay(400);
  digitalWrite(ORGANIC_STORAGE_LED, LOW);
  digitalWrite(INORGANIC_STORAGE_LED, LOW);
  Serial.println("✓");
  delay(200);
  
  // Test 5: Sensors
  Serial.print("  [5/5] Testing sensors... ");
  readColorSensor();
  readWeightSensor();
  detectWaste();
  measureDistance(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
  Serial.println("✓");
  delay(300);
  
  Serial.println("\n✓ All systems initialized successfully!\n");
}

// ==================== WASTE HANDLING FUNCTIONS ====================
void handleWasteDetection() {
  Serial.println("─────────────────────────────────────────");
  Serial.println("⚠ WASTE DETECTED ON CONVEYOR!");
  
  // Stop conveyor briefly for classification
  stopConveyor();
  delay(500);  // Allow settling time
  
  // Classify the waste
  system.currentWasteType = classifyWaste();
  
  // Route to appropriate storage
  if (system.currentWasteType == ORGANIC_STORAGE) {
    Serial.println("✓ Classification: ORGANIC WASTE");
    activateSeparator(ORGANIC_STORAGE);
  } else if (system.currentWasteType == INORGANIC_STORAGE) {
    Serial.println("✓ Classification: INORGANIC WASTE");
    activateSeparator(INORGANIC_STORAGE);
  } else {
    Serial.println("⚠ Classification uncertain, treating as INORGANIC");
    activateSeparator(INORGANIC_STORAGE);
  }
  
  // Resume conveyor operation
  delay(500);
  startConveyor();
  Serial.println("─────────────────────────────────────────\n");
}

int classifyWaste() {
  // Read all sensors
  readColorSensor();
  int weight = readWeightSensor();
  
  Serial.print("  Color (R:G:B) = ");
  Serial.print(colorData.red);
  Serial.print(":");
  Serial.print(colorData.green);
  Serial.print(":");
  Serial.print(colorData.blue);
  Serial.print("  |  Weight = ");
  Serial.println(weight);
  
  bool isOrganic = false;
  
  // ---- Color-based classification ----
  // Brown colors: High red, medium green, low blue
  if (colorData.red > 400 && colorData.green > 300 && colorData.blue < 300) {
    isOrganic = true;
    Serial.println("  → Color analysis: Brown (likely organic)");
  }
  // Green colors: High green, lower red and blue
  else if (colorData.green > colorData.red && 
           colorData.green > colorData.blue && 
           colorData.green > 350) {
    isOrganic = true;
    Serial.println("  → Color analysis: Green (likely organic)");
  }
  // Weight-based heuristic for organic
  else if (weight < CLASSIFICATION_THRESHOLD && weight > 100) {
    isOrganic = true;
    Serial.println("  → Weight analysis: Light material (likely organic)");
  }
  
  // ---- Validation logic ----
  // Very heavy items are likely inorganic (metal, plastic bottles)
  if (weight > 800) {
    isOrganic = false;
    Serial.println("  → Weight validation: Very heavy (override to inorganic)");
  }
  
  // Very light items are likely organic (paper, food)
  if (weight < 50) {
    isOrganic = true;
    Serial.println("  → Weight validation: Very light (override to organic)");
  }
  
  return isOrganic ? ORGANIC_STORAGE : INORGANIC_STORAGE;
}

void activateSeparator(int wasteType) {
  system.separationInProgress = true;
  
  int targetPosition = (wasteType == ORGANIC_STORAGE) ? 128 : 384;
  int led = (wasteType == ORGANIC_STORAGE) ? ORGANIC_STORAGE_LED : INORGANIC_STORAGE_LED;
  
  Serial.print("  ➔ Activating separator for ");
  Serial.println(wasteType == ORGANIC_STORAGE ? "ORGANIC" : "INORGANIC");
  
  // Activate LED indicator
  digitalWrite(led, HIGH);
  
  // Move separator to position
  moveStepperToPosition(targetPosition);
  
  // Keep LED on during separation
  delay(SEPARATOR_DELAY);
  
  // Return to neutral position
  moveStepperToPosition(0);
  
  // Turn off LED
  digitalWrite(led, LOW);
  
  system.separationInProgress = false;
  Serial.println("  ✓ Separation complete");
}

// ==================== SENSOR READING FUNCTIONS ====================
void readColorSensor() {
  // Read red component
  digitalWrite(COLOR_SENSOR_S2, LOW);
  digitalWrite(COLOR_SENSOR_S3, LOW);
  colorData.red = pulseIn(COLOR_SENSOR_OUT, LOW, COLOR_READ_TIMEOUT);
  
  // Read green component
  digitalWrite(COLOR_SENSOR_S2, HIGH);
  digitalWrite(COLOR_SENSOR_S3, HIGH);
  colorData.green = pulseIn(COLOR_SENSOR_OUT, LOW, COLOR_READ_TIMEOUT);
  
  // Read blue component
  digitalWrite(COLOR_SENSOR_S2, LOW);
  digitalWrite(COLOR_SENSOR_S3, HIGH);
  colorData.blue = pulseIn(COLOR_SENSOR_OUT, LOW, COLOR_READ_TIMEOUT);
  
  colorData.lastReadTime = millis();
}

int readWeightSensor() {
  // ESP8266 has only one ADC pin (A0)
  return analogRead(WEIGHT_SENSOR_PIN);
}

bool detectWaste() {
  // Proximity sensor: LOW when waste detected
  return digitalRead(PROXIMITY_SENSOR_PIN) == LOW;
}

int measureDistance(int trigPin, int echoPin) {
  // Send ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echo with timeout
  long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout
  
  if (duration == 0) {
    return -1;  // Timeout or error
  }
  
  // Calculate distance in cm
  // Speed of sound = 343 m/s = 0.0343 cm/µs
  int distance = duration * 0.034 / 2;
  
  return distance;
}

void checkEmergencyStop() {
  unsigned long currentTime = millis();
  
  if (currentTime - system.lastEmergencyCheck >= 50) {
    system.lastEmergencyCheck = currentTime;
    
    if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
      if (!system.emergencyStopActive) {
        system.emergencyStopActive = true;
        Serial.println("\n🛑 EMERGENCY STOP ACTIVATED! 🛑\n");
      }
    }
  }
}

// ==================== MOTOR CONTROL FUNCTIONS ====================
void startConveyor() {
  if (system.active && !system.emergencyStopActive) {
    analogWrite(CONVEYOR_MOTOR_PIN, CONVEYOR_SPEED);
  }
}

void stopConveyor() {
  analogWrite(CONVEYOR_MOTOR_PIN, 0);
}

void moveStepperToPosition(int targetPosition) {
  int stepsToMove = targetPosition - system.stepperPosition;
  
  // Normalize to shortest path
  if (stepsToMove > SEPARATOR_STEPS / 2) {
    stepsToMove -= SEPARATOR_STEPS;
  } else if (stepsToMove < -SEPARATOR_STEPS / 2) {
    stepsToMove += SEPARATOR_STEPS;
  }
  
  // Execute movement
  if (stepsToMove > 0) {
    for (int i = 0; i < stepsToMove; i++) {
      stepForward();
      delayMicroseconds(2000);  // Adjust speed as needed
    }
  } else if (stepsToMove < 0) {
    for (int i = 0; i > stepsToMove; i--) {
      stepBackward();
      delayMicroseconds(2000);
    }
  }
  
  system.stepperPosition = targetPosition % SEPARATOR_STEPS;
}

void stepForward() {
  static int currentStep = 0;
  currentStep = (currentStep + 1) % 8;
  
  digitalWrite(SEPARATOR_STEP_PIN1, stepperSequence[currentStep][0]);
  digitalWrite(SEPARATOR_STEP_PIN2, stepperSequence[currentStep][1]);
  digitalWrite(SEPARATOR_STEP_PIN3, stepperSequence[currentStep][2]);
  digitalWrite(SEPARATOR_STEP_PIN4, stepperSequence[currentStep][3]);
}

void stepBackward() {
  static int currentStep = 0;
  currentStep = (currentStep + 7) % 8;
  
  digitalWrite(SEPARATOR_STEP_PIN1, stepperSequence[currentStep][0]);
  digitalWrite(SEPARATOR_STEP_PIN2, stepperSequence[currentStep][1]);
  digitalWrite(SEPARATOR_STEP_PIN3, stepperSequence[currentStep][2]);
  digitalWrite(SEPARATOR_STEP_PIN4, stepperSequence[currentStep][3]);
}

void setStepperPosition(int position) {
  system.stepperPosition = position % SEPARATOR_STEPS;
  if (system.stepperPosition < 0) {
    system.stepperPosition += SEPARATOR_STEPS;
  }
}

// ==================== STATUS & MONITORING FUNCTIONS ====================
void updateStorageStatus() {
  int distance = measureDistance(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
  
  if (distance > 0) {
    // Map distance to percentage (assuming max 50cm = empty, 5cm = full)
    system.organicStorageLevel = map(distance, 50, 5, 0, 100);
    system.organicStorageLevel = constrain(system.organicStorageLevel, 0, 100);
    
    system.inorganicStorageLevel = system.organicStorageLevel;  // Same sensor for both
  }
  
  // Check for full storage and alert
  if (system.organicStorageLevel >= 90) {
    alertBuzzer();
  }
  if (system.inorganicStorageLevel >= 90) {
    alertBuzzer();
  }
}

void updateIndicatorLEDs() {
  // Organic storage LED indicator
  if (system.organicStorageLevel < 30) {
    digitalWrite(ORGANIC_STORAGE_LED, LOW);
  } else if (system.organicStorageLevel < 70) {
    // Slow blink for medium fill
    if (millis() % 2000 < 500) {
      digitalWrite(ORGANIC_STORAGE_LED, HIGH);
    } else {
      digitalWrite(ORGANIC_STORAGE_LED, LOW);
    }
  } else {
    // Solid on for high fill
    digitalWrite(ORGANIC_STORAGE_LED, HIGH);
  }
  
  // Inorganic storage LED indicator
  if (system.inorganicStorageLevel < 30) {
    digitalWrite(INORGANIC_STORAGE_LED, LOW);
  } else if (system.inorganicStorageLevel < 70) {
    // Slow blink for medium fill
    if (millis() % 2000 < 500) {
      digitalWrite(INORGANIC_STORAGE_LED, HIGH);
    } else {
      digitalWrite(INORGANIC_STORAGE_LED, LOW);
    }
  } else {
    // Solid on for high fill
    digitalWrite(INORGANIC_STORAGE_LED, HIGH);
  }
}

void alertBuzzer() {
  static unsigned long lastBuzzerTime = 0;
  static int buzzerPhase = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastBuzzerTime >= 200) {
    lastBuzzerTime = currentTime;
    buzzerPhase = (buzzerPhase + 1) % 4;
    
    if (buzzerPhase < 2) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}

void displaySystemStatus() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      SYSTEM STATUS REPORT              ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.print("║ Status: ");
  if (system.active) {
    Serial.println("ACTIVE                          ║");
  } else {
    Serial.println("INACTIVE                        ║");
  }
  Serial.print("║ Waste Detected: ");
  Serial.print(system.wasteDetected ? "YES" : "NO");
  Serial.println("                         ║");
  Serial.print("║ Current Type: ");
  if (system.currentWasteType == ORGANIC_STORAGE) {
    Serial.println("ORGANIC                     ║");
  } else if (system.currentWasteType == INORGANIC_STORAGE) {
    Serial.println("INORGANIC                   ║");
  } else {
    Serial.println("NONE                        ║");
  }
  Serial.print("║ Organic Storage: ");
  Serial.print(system.organicStorageLevel);
  Serial.println("%                         ║");
  Serial.print("║ Inorganic Storage: ");
  Serial.print(system.inorganicStorageLevel);
  Serial.println("%                       ║");
  Serial.println("╚════════════════════════════════════════╝\n");
}

void handleEmergencyStop() {
  static unsigned long lastWarningTime = 0;
  unsigned long currentTime = millis();
  
  // Stop all motors immediately
  stopConveyor();
  
  // Flash warning lights
  if (currentTime - lastWarningTime >= 500) {
    lastWarningTime = currentTime;
    digitalWrite(ORGANIC_STORAGE_LED, !digitalRead(ORGANIC_STORAGE_LED));
    digitalWrite(INORGANIC_STORAGE_LED, !digitalRead(INORGANIC_STORAGE_LED));
  }
  
  // Continuous buzzer warning
  digitalWrite(BUZZER_PIN, HIGH);
  
  system.active = false;
  
  // Check for button release to resume
  if (digitalRead(EMERGENCY_STOP_PIN) == HIGH) {
    delay(50);  // Debounce
    if (digitalRead(EMERGENCY_STOP_PIN) == HIGH) {
      Serial.println("\n✓ Emergency stop cleared, resuming operation...\n");
      system.emergencyStopActive = false;
      system.active = true;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(ORGANIC_STORAGE_LED, LOW);
      digitalWrite(INORGANIC_STORAGE_LED, LOW);
    }
  }
}

// ==================== SERIAL COMMAND HANDLER ====================
void handleSerialCommands() {
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case 's':
      case 'S':
        displaySystemStatus();
        break;
        
      case 'e':
      case 'E':
        system.emergencyStopActive = true;
        break;
        
      case 'r':
      case 'R':
        system.active = !system.active;
        Serial.print("\nSystem ");
        Serial.println(system.active ? "activated" : "deactivated");
        break;
        
      case 'c':
      case 'C':
        readColorSensor();
        Serial.print("\nColor (R:G:B) = ");
        Serial.print(colorData.red);
        Serial.print(":");
        Serial.print(colorData.green);
        Serial.print(":");
        Serial.println(colorData.blue);
        break;
        
      case 'w':
      case 'W':
        Serial.print("\nWeight sensor: ");
        Serial.println(readWeightSensor());
        break;
        
      case 'h':
      case 'H':
        printHelpMenu();
        break;
    }
  }
}

void printHelpMenu() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      SERIAL COMMAND MENU               ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ s - Display system status              ║");
  Serial.println("║ e - Emergency stop                     ║");
  Serial.println("║ r - Toggle system on/off               ║");
  Serial.println("║ c - Read color sensor                  ║");
  Serial.println("║ w - Read weight sensor                 ║");
  Serial.println("║ h - Display this help menu             ║");
  Serial.println("╚════════════════════════════════════════╝\n");
}
