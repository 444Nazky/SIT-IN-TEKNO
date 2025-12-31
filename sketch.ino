// Pin Definitions
const int CONVEYOR_MOTOR_PIN = 3;        // PWM pin for conveyor motor
const int COLOR_SENSOR_S0 = 4;           // Color sensor pins
const int COLOR_SENSOR_S1 = 5;           // danis kintil
const int COLOR_SENSOR_S2 = 6;
const int COLOR_SENSOR_S3 = 7;
const int COLOR_SENSOR_OUT = 8;
const int WEIGHT_SENSOR_PIN = A0;        // Analog pin for weight sensor
const int SEPARATOR_STEP_PIN1 = 9;       // Stepper motor pins for separation
const int SEPARATOR_STEP_PIN2 = 10;
const int SEPARATOR_STEP_PIN3 = 11;
const int SEPARATOR_STEP_PIN4 = 12;
const int PROXIMITY_SENSOR_PIN = 2;      // IR sensor for waste detection
const int ORGANIC_STORAGE_LED = 5;       // Green LED for organic storage
const int INORGANIC_STORAGE_LED = 6;     // Blue LED for inorganic storage
const int BUZZER_PIN = 7;                // Buzzer for alerts
const int EMERGENCY_STOP_PIN = 13;       // Emergency stop button
const int ULTRASONIC_TRIG_PIN = A1;      // Ultrasonic sensor trigger
const int ULTRASONIC_ECHO_PIN = A2;      // Ultrasonic sensor echo

// Constants
const int CONVEYOR_SPEED = 150;          // PWM value (0-255)
const int CLASSIFICATION_THRESHOLD = 500; // Weight threshold for classification
const int STORAGE_FULL_DISTANCE = 5;     // Distance in cm for full storage
const int SEPARATOR_STEPS = 512;         // Steps for full rotation
const int SEPARATOR_DELAY = 1000;        // Delay for separation action

// Storage areas
const int ORGANIC_STORAGE = 0;
const int INORGANIC_STORAGE = 1;

// Variables
int organicStorageLevel = 0;
int inorganicStorageLevel = 0;
bool systemActive = true;
bool emergencyStop = false;
bool wasteDetected = false;
int currentWasteType = -1;  // -1 = undefined, 0 = organic, 1 = inorganic
int stepperPosition = 0;

// Stepper motor sequence (half-step mode)
int stepperSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// Color sensor variables
int redValue = 0, greenValue = 0, blueValue = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Mr. Trash Wheel Robot Initializing...");
  
  // Initialize pins
  pinMode(CONVEYOR_MOTOR_PIN, OUTPUT);
  pinMode(COLOR_SENSOR_S0, OUTPUT);
  pinMode(COLOR_SENSOR_S1, OUTPUT);
  pinMode(COLOR_SENSOR_S2, OUTPUT);
  pinMode(COLOR_SENSOR_S3, OUTPUT);
  pinMode(COLOR_SENSOR_OUT, INPUT);
  pinMode(WEIGHT_SENSOR_PIN, INPUT);
  pinMode(SEPARATOR_STEP_PIN1, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN2, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN3, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN4, OUTPUT);
  pinMode(PROXIMITY_SENSOR_PIN, INPUT);
  pinMode(ORGANIC_STORAGE_LED, OUTPUT);
  pinMode(INORGANIC_STORAGE_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(EMERGENCY_STOP_PIN, INPUT);
  
  // Initialize color sensor (set frequency scaling to 20%)
  digitalWrite(COLOR_SENSOR_S0, HIGH);
  digitalWrite(COLOR_SENSOR_S1, LOW);
  
  // Initialize stepper motor
  stepperPosition = 0;
  setStepperPosition(0);
  
  // Set initial LED states
  digitalWrite(ORGANIC_STORAGE_LED, LOW);
  digitalWrite(INORGANIC_STORAGE_LED, LOW);
  
  // Test sequence
  testSequence();
  
  Serial.println("Mr. Trash Wheel Robot Ready!");
  Serial.println("=================================");
}

void loop() {
  if (emergencyStop) {
    emergencyStopProcedure();
    return;
  }
  
  // Check storage levels
  updateStorageStatus();
  
  // Check for waste on conveyor
  wasteDetected = detectWaste();
  
  if (wasteDetected && systemActive) {
    Serial.println("Waste detected on conveyor!");
    
    // Stop conveyor briefly for classification
    stopConveyor();
    
    // Classify the waste
    currentWasteType = classifyWaste();
    
    if (currentWasteType == ORGANIC_STORAGE) {
      Serial.println("Organic waste detected");
      activateSeparator(ORGANIC_STORAGE);
    } else if (currentWasteType == INORGANIC_STORAGE) {
      Serial.println("Inorganic waste detected");
      activateSeparator(INORGANIC_STORAGE);
    } else {
      Serial.println("Classification failed, treating as inorganic");
      activateSeparator(INORGANIC_STORAGE);
    }
    
    // Resume conveyor
    startConveyor();
    
    // Non-blocking delay for 2000ms before next detection
    static unsigned long separationDelayStart = 0;
    if (separationDelayStart == 0) {
      separationDelayStart = millis();
    }
    if (millis() - separationDelayStart >= 2000) {
      separationDelayStart = 0; // Reset for next cycle
    }
  } else {
    // Run conveyor continuously
    startConveyor();
  }
  
  // Display status every 10 seconds
  static unsigned long lastStatusTime = 0;
  if (millis() - lastStatusTime > 10000) {
    displayStatus();
    lastStatusTime = millis();
  }
  
  // Small delay for stability (non-blocking)
  static unsigned long lastLoopTime = 0;
  const unsigned long LOOP_DELAY = 100;
  if (millis() - lastLoopTime >= LOOP_DELAY) {
    lastLoopTime = millis();
    // Main loop operations here
  }
}

// Sensor Functions
void readColorSensor() {
  // Read red component
  digitalWrite(COLOR_SENSOR_S2, LOW);
  digitalWrite(COLOR_SENSOR_S3, LOW);
  redValue = pulseIn(COLOR_SENSOR_OUT, LOW);
  
  // Read green component
  digitalWrite(COLOR_SENSOR_S2, HIGH);
  digitalWrite(COLOR_SENSOR_S3, HIGH);
  greenValue = pulseIn(COLOR_SENSOR_OUT, LOW);
  
  // Read blue component
  digitalWrite(COLOR_SENSOR_S2, LOW);
  digitalWrite(COLOR_SENSOR_S3, HIGH);
  blueValue = pulseIn(COLOR_SENSOR_OUT, LOW);
}

int readWeightSensor() {
  return analogRead(WEIGHT_SENSOR_PIN);
}

bool detectWaste() {
  return digitalRead(PROXIMITY_SENSOR_PIN) == LOW;
}

int checkStorageLevels() {
  // Measure storage level manually with ultrasonic sensor
  int distance = measureDistance(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
  
  // For simplicity, use same distance for both storages
  organicStorageLevel = map(distance, 0, 50, 100, 0);
  organicStorageLevel = constrain(organicStorageLevel, 0, 100);
  
  inorganicStorageLevel = organicStorageLevel; // Same level for demo
  
  return (organicStorageLevel + inorganicStorageLevel) / 2;
}

// Motor Control Functions
void controlConveyor(int speed) {
  analogWrite(CONVEYOR_MOTOR_PIN, speed);
}

void startConveyor() {
  if (systemActive && !emergencyStop) {
    controlConveyor(CONVEYOR_SPEED);
  }
}

void stopConveyor() {
  controlConveyor(0);
}

void activateSeparator(int wasteType) {
  Serial.println("Activating separation mechanism...");
  
  // Non-blocking LED timing for separation
  static unsigned long ledStartTime = 0;
  static bool ledActive = false;
  static bool separationInProgress = false;
  static unsigned long separationStartTime = 0;
  static int targetPosition = 0;
  
  // Move stepper to appropriate position
  if (wasteType == ORGANIC_STORAGE) {
    targetPosition = 128; // Quarter turn for organic
    digitalWrite(ORGANIC_STORAGE_LED, HIGH);
    ledActive = true;
    ledStartTime = millis();
  } else {
    targetPosition = 384; // Three quarter turn for inorganic
    digitalWrite(INORGANIC_STORAGE_LED, HIGH);
    ledActive = true;
    ledStartTime = millis();
  }
  
  // Move stepper (non-blocking)
  if (!separationInProgress) {
    moveStepperToPosition(targetPosition);
    separationInProgress = true;
    separationStartTime = millis();
  }
  
  // Handle LED timing
  if (ledActive && millis() - ledStartTime >= 500) {
    if (wasteType == ORGANIC_STORAGE) {
      digitalWrite(ORGANIC_STORAGE_LED, LOW);
    } else {
      digitalWrite(INORGANIC_STORAGE_LED, LOW);
    }
    ledActive = false;
  }
  
  // Handle separation delay
  if (separationInProgress && millis() - separationStartTime >= SEPARATOR_DELAY) {
    // Return to neutral
    moveStepperToPosition(0);
    separationInProgress = false;
    Serial.println("Separation complete");
  }
}

void stopAllMotors() {
  controlConveyor(0);
  moveStepperToPosition(0);
}

// Classification Logic
int classifyWaste() {
  Serial.println("Classifying waste...");
  
  // Read sensors
  readColorSensor();
  int weight = readWeightSensor();
  
  Serial.print("Color values - R: ");
  Serial.print(redValue);
  Serial.print(" G: ");
  Serial.print(greenValue);
  Serial.print(" B: ");
  Serial.println(blueValue);
  
  Serial.print("Weight: ");
  Serial.println(weight);
  
  // Simple classification logic
  // Organic waste: Typically brown/green colors, lighter weight
  // Inorganic waste: Various colors, often heavier
  
  bool isOrganic = false;
  
  // Color-based classification
  if (redValue > 400 && greenValue > 300 && blueValue < 300) {
    // Brown color - likely organic
    isOrganic = true;
  } else if (greenValue > redValue && greenValue > blueValue && greenValue > 350) {
    // Green color - likely organic
    isOrganic = true;
  } else if (weight < CLASSIFICATION_THRESHOLD) {
    // Light weight - might be organic
    isOrganic = true;
  }
  
  // Validate with additional logic
  if (weight > 800) {
    // Very heavy - likely inorganic
    isOrganic = false;
  }
  
  if (isOrganic) {
    Serial.println("Classified as: ORGANIC");
    return ORGANIC_STORAGE;
  } else {
    Serial.println("Classified as: INORGANIC");
    return INORGANIC_STORAGE;
  }
}

// Storage Management
void routeToStorage(int wasteType) {
  activateSeparator(wasteType);
}

void updateStorageStatus() {
  checkStorageLevels();
  
  // Alert if storage is full
  if (organicStorageLevel >= 90) {
    Serial.println("WARNING: Organic storage is almost full!");
    alertBuzzer();
    blinkLED(ORGANIC_STORAGE_LED, 3);
  }
  
  if (inorganicStorageLevel >= 90) {
    Serial.println("WARNING: Inorganic storage is almost full!");
    alertBuzzer();
    blinkLED(INORGANIC_STORAGE_LED, 3);
  }
}

// Main System Functions
void emergencyStopProcedure() {
  static bool emergencyActivated = false;
  static unsigned long lastCheckTime = 0;
  const unsigned long CHECK_INTERVAL = 100; // Check every 100ms
  
  if (!emergencyActivated) {
    Serial.println("EMERGENCY STOP ACTIVATED!");
    
    // Stop all motors
    stopAllMotors();
    
    // Turn on warning LEDs
    digitalWrite(ORGANIC_STORAGE_LED, HIGH);
    digitalWrite(INORGANIC_STORAGE_LED, HIGH);
    
    // Continuous alert
    digitalWrite(BUZZER_PIN, HIGH);
    
    systemActive = false;
    emergencyActivated = true;
    lastCheckTime = millis();
  }
  
  // Check for emergency stop release (non-blocking)
  if (millis() - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = millis();
    
    if (digitalRead(EMERGENCY_STOP_PIN) == HIGH) { // Button released
      // Resume normal operation
      emergencyStop = false;
      emergencyActivated = false;
      systemActive = true;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(ORGANIC_STORAGE_LED, LOW);
      digitalWrite(INORGANIC_STORAGE_LED, LOW);
      
      Serial.println("Emergency stop cleared, resuming operation");
    }
  }
}

void displayStatus() {
  Serial.println("=== Mr. Trash Wheel Status ===");
  Serial.print("System Active: ");
  Serial.println(systemActive ? "YES" : "NO");
  Serial.print("Waste Detected: ");
  Serial.println(wasteDetected ? "YES" : "NO");
  Serial.print("Current Waste Type: ");
  if (currentWasteType == ORGANIC_STORAGE) {
    Serial.println("Organic");
  } else if (currentWasteType == INORGANIC_STORAGE) {
    Serial.println("Inorganic");
  } else {
    Serial.println("None");
  }
  Serial.print("Organic Storage Level: ");
  Serial.print(organicStorageLevel);
  Serial.println("%");
  Serial.print("Inorganic Storage Level: ");
  Serial.print(inorganicStorageLevel);
  Serial.println("%");
  Serial.println("==============================");
}

// User Interface Functions
void updateLEDs() {
  // Organic storage LED
  if (organicStorageLevel < 30) {
    digitalWrite(ORGANIC_STORAGE_LED, LOW);
  } else if (organicStorageLevel < 70) {
    // Blink slowly
    if (millis() % 2000 < 500) {
      digitalWrite(ORGANIC_STORAGE_LED, HIGH);
    } else {
      digitalWrite(ORGANIC_STORAGE_LED, LOW);
    }
  } else {
    digitalWrite(ORGANIC_STORAGE_LED, HIGH);
  }
  
  // Inorganic storage LED
  if (inorganicStorageLevel < 30) {
    digitalWrite(INORGANIC_STORAGE_LED, LOW);
  } else if (inorganicStorageLevel < 70) {
    // Blink slowly
    if (millis() % 2000 < 500) {
      digitalWrite(INORGANIC_STORAGE_LED, HIGH);
    } else {
      digitalWrite(INORGANIC_STORAGE_LED, LOW);
    }
  } else {
    digitalWrite(INORGANIC_STORAGE_LED, HIGH);
  }
}

void sendAlerts() {
  if (organicStorageLevel >= 90 || inorganicStorageLevel >= 90) {
    alertBuzzer();
  }
}

void serialMonitoring() {
  // Additional monitoring can be added here
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case 's':
      case 'S':
        displayStatus();
        break;
      case 'e':
      case 'E':
        emergencyStop = true;
        break;
      case 'r':
      case 'R':
        systemActive = !systemActive;
        Serial.print("System ");
        Serial.println(systemActive ? "activated" : "deactivated");
        break;
    }
  }
}

// Utility Functions
void testSequence() {
  static int testStep = 0;
  static unsigned long testStartTime = 0;
  static unsigned long stepStartTime = 0;
  const unsigned long TEST_STEP_DURATION = 1000; // 1 second per step
  
  if (testStartTime == 0) {
    testStartTime = millis();
    stepStartTime = millis();
    testStep = 0;
    Serial.println("Running test sequence...");
  }
  
  // Execute each test step with timing
  switch (testStep) {
    case 0:
      if (millis() - stepStartTime == 0) {
        Serial.println("Testing conveyor motor...");
        startConveyor();
      }
      if (millis() - stepStartTime >= TEST_STEP_DURATION) {
        stopConveyor();
        testStep++;
        stepStartTime = millis();
      }
      break;
      
    case 1:
      if (millis() - stepStartTime == 0) {
        Serial.println("Testing separator stepper...");
        moveStepperToPosition(128);
      }
      if (millis() - stepStartTime >= TEST_STEP_DURATION) {
        testStep++;
        stepStartTime = millis();
      }
      break;
      
    case 2:
      if (millis() - stepStartTime == 0) {
        moveStepperToPosition(384);
      }
      if (millis() - stepStartTime >= TEST_STEP_DURATION) {
        testStep++;
        stepStartTime = millis();
      }
      break;
      
    case 3:
      if (millis() - stepStartTime == 0) {
        moveStepperToPosition(0);
      }
      if (millis() - stepStartTime >= TEST_STEP_DURATION) {
        testStep++;
        stepStartTime = millis();
      }
      break;
      
    case 4:
      if (millis() - stepStartTime == 0) {
        Serial.println("Testing sensors...");
        readColorSensor();
        readWeightSensor();
        detectWaste();
        checkStorageLevels();
      }
      if (millis() - stepStartTime >= TEST_STEP_DURATION) {
        testStep++;
        stepStartTime = millis();
      }
      break;
      
    case 5:
      if (millis() - stepStartTime == 0) {
        Serial.println("Testing indicators...");
        digitalWrite(ORGANIC_STORAGE_LED, HIGH);
        digitalWrite(INORGANIC_STORAGE_LED, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
      }
      if (millis() - stepStartTime >= TEST_STEP_DURATION) {
        digitalWrite(ORGANIC_STORAGE_LED, LOW);
        digitalWrite(INORGANIC_STORAGE_LED, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        testStep++;
        stepStartTime = millis();
      }
      break;
      
    default:
      Serial.println("Test sequence complete!");
      testStartTime = 0; // Reset for next test
      break;
  }
}

void alertBuzzer() {
  static unsigned long buzzerStartTime = 0;
  static int buzzerStep = 0;
  static bool buzzerActive = false;
  
  if (!buzzerActive) {
    buzzerActive = true;
    buzzerStartTime = millis();
    buzzerStep = 0;
  }
  
  unsigned long currentTime = millis();
  
  switch (buzzerStep) {
    case 0: // First beep
      digitalWrite(BUZZER_PIN, HIGH);
      if (currentTime - buzzerStartTime >= 200) {
        digitalWrite(BUZZER_PIN, LOW);
        buzzerStep = 1;
        buzzerStartTime = currentTime;
      }
      break;
      
    case 1: // First pause
      if (currentTime - buzzerStartTime >= 100) {
        buzzerStep = 2;
        buzzerStartTime = currentTime;
      }
      break;
      
    case 2: // Second beep
      digitalWrite(BUZZER_PIN, HIGH);
      if (currentTime - buzzerStartTime >= 200) {
        digitalWrite(BUZZER_PIN, LOW);
        buzzerStep = 3;
        buzzerStartTime = currentTime;
        buzzerActive = false; // Complete
      }
      break;
      
    default:
      buzzerActive = false;
      break;
  }
}

void blinkLED(int pin, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(300);
    digitalWrite(pin, LOW);
    delay(300);
  }
}

// Stepper motor control functions
void setStepperPosition(int position) {
  stepperPosition = position % SEPARATOR_STEPS;
  if (stepperPosition < 0) stepperPosition += SEPARATOR_STEPS;
}

void moveStepperToPosition(int targetPosition) {
  int stepsToMove = targetPosition - stepperPosition;
  
  // Normalize steps to move in the shortest direction
  if (stepsToMove > SEPARATOR_STEPS / 2) {
    stepsToMove -= SEPARATOR_STEPS;
  } else if (stepsToMove < -SEPARATOR_STEPS / 2) {
    stepsToMove += SEPARATOR_STEPS;
  }
  
  // Move stepper motor
  if (stepsToMove > 0) {
    for (int i = 0; i < stepsToMove; i++) {
      stepForward();
      delay(2); // Adjust speed as needed
    }
  } else if (stepsToMove < 0) {
    for (int i = 0; i > stepsToMove; i--) {
      stepBackward();
      delay(2); // Adjust speed as needed
    }
  }
  
  stepperPosition = targetPosition % SEPARATOR_STEPS;
}

void stepForward() {
  // Move stepper forward one step
  static int currentStep = 0;
  currentStep = (currentStep + 1) % 8;
  
  digitalWrite(SEPARATOR_STEP_PIN1, stepperSequence[currentStep][0]);
  digitalWrite(SEPARATOR_STEP_PIN2, stepperSequence[currentStep][1]);
  digitalWrite(SEPARATOR_STEP_PIN3, stepperSequence[currentStep][2]);
  digitalWrite(SEPARATOR_STEP_PIN4, stepperSequence[currentStep][3]);
}

void stepBackward() {
  // Move stepper backward one step
  static int currentStep = 0;
  currentStep = (currentStep + 7) % 8; // Move backward
  
  digitalWrite(SEPARATOR_STEP_PIN1, stepperSequence[currentStep][0]);
  digitalWrite(SEPARATOR_STEP_PIN2, stepperSequence[currentStep][1]);
  digitalWrite(SEPARATOR_STEP_PIN3, stepperSequence[currentStep][2]);
  digitalWrite(SEPARATOR_STEP_PIN4, stepperSequence[currentStep][3]);
}

// Ultrasonic distance measurement
int measureDistance(int trigPin, int echoPin) {
  // Send ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echo
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm
  int distance = duration * 0.034 / 2;
  
  return distance;
}

// Check emergency stop button
void checkEmergencyStop() {
  if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
    emergencyStop = true;
  }
}
