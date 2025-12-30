// Simplified Waste Sorting Robot - Mr. Trash Wheel
// Pin Definitions
const int MOTOR_PIN = 3, COLOR_S0 = 4, COLOR_S1 = 5, COLOR_S2 = 6, COLOR_S3 = 7;
const int COLOR_OUT = 8, WEIGHT_PIN = A0, STEP1 = 9, STEP2 = 10, STEP3 = 11, STEP4 = 12;
const int PROX_PIN = 2, ORG_LED = A3, INORG_LED = A4, BUZZER = 7, EMERGENCY = 13;

// Constants
const int CONVEYOR_SPEED = 150, THRESHOLD = 500, SEPARATOR_DELAY = 1000;
const int ORGANIC = 0, INORGANIC = 1;

// Variables
bool systemActive = true, emergencyStop = false, wasteDetected = false;
int wasteType = -1, storageLevel = 0, stepperPos = 0;
unsigned long lastStatusTime = 0, separationStart = 0;
bool separationActive = false, ledActive = false;
unsigned long ledStartTime = 0;

// Simplified stepper sequence
const int steps[8][4] = {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,1,1},{0,0,0,1},{1,0,0,1}};

void setup() {
  Serial.begin(9600);
  Serial.println("Mr. Trash Wheel Initializing...");
  
  // Initialize all pins
  int pins[] = {MOTOR_PIN, COLOR_S0, COLOR_S1, COLOR_S2, COLOR_S3, STEP1, STEP2, STEP3, STEP4, ORG_LED, INORG_LED, BUZZER};
  for(int pin : pins) pinMode(pin, OUTPUT);
  
  pinMode(COLOR_OUT, INPUT);
  pinMode(WEIGHT_PIN, INPUT);
  pinMode(PROX_PIN, INPUT);
  pinMode(EMERGENCY, INPUT);
  
  // Initialize color sensor
  digitalWrite(COLOR_S0, HIGH);
  digitalWrite(COLOR_S1, LOW);
  
  // Initialize LEDs and stop motors
  digitalWrite(ORG_LED, LOW);
  digitalWrite(INORG_LED, LOW);
  analogWrite(MOTOR_PIN, 0);
  
  Serial.println("System Ready!");
}

void loop() {
  if (emergencyStop) {
    emergencyStopProcedure();
    return;
  }
  
  // Update storage levels
  storageLevel = checkStorage();
  
  // Check for waste and control conveyor
  wasteDetected = digitalRead(PROX_PIN) == LOW;
  
  if (wasteDetected && systemActive) {
    Serial.println("Waste detected!");
    
    // Stop conveyor briefly
    analogWrite(MOTOR_PIN, 0);
    
    // Classify and separate
    wasteType = classifyWaste();
    
    if (wasteType == ORGANIC) {
      Serial.println("Organic waste");
      separateWaste(ORGANIC);
    } else {
      Serial.println("Inorganic waste");
      separateWaste(INORGANIC);
    }
    
    // Resume conveyor
    analogWrite(MOTOR_PIN, CONVEYOR_SPEED);
    
    // Non-blocking delay
    static unsigned long lastProcess = 0;
    if (lastProcess == 0) lastProcess = millis();
    if (millis() - lastProcess >= 2000) {
      lastProcess = 0;
      separationActive = false;
    }
    
  } else if (systemActive) {
    analogWrite(MOTOR_PIN, CONVEYOR_SPEED);
  }
  
  // Handle separation timing
  if (separationActive && millis() - separationStart >= SEPARATOR_DELAY) {
    returnToNeutral();
  }
  
  // Handle LED timing
  if (ledActive && millis() - ledStartTime >= 500) {
    digitalWrite(wasteType == ORGANIC ? ORG_LED : INORG_LED, LOW);
    ledActive = false;
  }
  
  // Storage alerts
  if (storageLevel >= 90) {
    alertStorage();
  }
  
  // Display status every 10 seconds
  if (millis() - lastStatusTime > 10000) {
    displayStatus();
    lastStatusTime = millis();
  }
  
  delay(50); // Small delay for stability
}

// Simplified classification
int classifyWaste() {
  // Read color values
  int red = readColor(COLOR_S2, COLOR_S3, LOW, LOW);
  int green = readColor(COLOR_S2, COLOR_S3, HIGH, HIGH);
  int blue = readColor(COLOR_S2, COLOR_S3, LOW, HIGH);
  int weight = analogRead(WEIGHT_PIN);
  
  Serial.print("R:"); Serial.print(red);
  Serial.print(" G:"); Serial.print(green);
  Serial.print(" B:"); Serial.print(blue);
  Serial.print(" W:"); Serial.println(weight);
  
  // Simple classification logic
  bool isOrganic = (red > 400 && green > 300 && blue < 300) || 
                   (green > red && green > blue && green > 350) ||
                   (weight < THRESHOLD);
  
  if (weight > 800) isOrganic = false;
  
  Serial.println(isOrganic ? "ORGANIC" : "INORGANIC");
  return isOrganic ? ORGANIC : INORGANIC;
}

// Read color sensor
int readColor(int s2, int s3, int val2, int val3) {
  digitalWrite(s2, val2);
  digitalWrite(s3, val3);
  return pulseIn(COLOR_OUT, LOW);
}

// Simplified separation
void separateWaste(int type) {
  separationActive = true;
  separationStart = millis();
  
  // Move stepper to position
  int targetPos = (type == ORGANIC) ? 128 : 384;
  moveStepper(targetPos);
  
  // Turn on LED
  digitalWrite(type == ORGANIC ? ORG_LED : INORG_LED, HIGH);
  ledActive = true;
  ledStartTime = millis();
}

// Move stepper motor
void moveStepper(int targetPos) {
  int stepsToMove = targetPos - stepperPos;
  
  // Normalize steps
  if (stepsToMove > 256) stepsToMove -= 512;
  else if (stepsToMove < -256) stepsToMove += 512;
  
  // Move stepper
  for (int i = 0; i < abs(stepsToMove); i++) {
    if (stepsToMove > 0) stepForward();
    else stepBackward();
    delay(2);
  }
  
  stepperPos = targetPos % 512;
}

// Stepper control
void stepForward() {
  static int currentStep = 0;
  currentStep = (currentStep + 1) % 8;
  
  for (int i = 0; i < 4; i++) {
    digitalWrite(STEP1 + i, steps[currentStep][i]);
  }
}

void stepBackward() {
  static int currentStep = 0;
  currentStep = (currentStep + 7) % 8;
  
  for (int i = 0; i < 4; i++) {
    digitalWrite(STEP1 + i, steps[currentStep][i]);
  }
}

// Return to neutral
void returnToNeutral() {
  moveStepper(0);
  separationActive = false;
}

// Check storage levels
int checkStorage() {
  // Simple ultrasonic measurement
  digitalWrite(A1, LOW);
  delayMicroseconds(2);
  digitalWrite(A1, HIGH);
  delayMicroseconds(10);
  digitalWrite(A1, LOW);
  
  long duration = pulseIn(A2, HIGH);
  int distance = duration * 0.034 / 2;
  
  return constrain(map(distance, 0, 50, 100, 0), 0, 100);
}

// Emergency stop
void emergencyStopProcedure() {
  static bool activated = false;
  
  if (!activated) {
    Serial.println("EMERGENCY STOP!");
    analogWrite(MOTOR_PIN, 0);
    moveStepper(0);
    digitalWrite(ORG_LED, HIGH);
    digitalWrite(INORG_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    systemActive = false;
    activated = true;
  }
  
  // Wait for button release
  if (digitalRead(EMERGENCY) == HIGH) {
    digitalWrite(BUZZER, LOW);
    digitalWrite(ORG_LED, LOW);
    digitalWrite(INORG_LED, LOW);
    emergencyStop = false;
    activated = false;
    systemActive = true;
    Serial.println("Emergency cleared");
  }
}

// Storage alert
void alertStorage() {
  static unsigned long alertStart = 0;
  static int alertStep = 0;
  
  if (alertStart == 0) {
    alertStart = millis();
    alertStep = 0;
  }
  
  unsigned long now = millis();
  
  switch (alertStep) {
    case 0: // Beep
      digitalWrite(BUZZER, HIGH);
      if (now - alertStart >= 200) {
        digitalWrite(BUZZER, LOW);
        alertStep = 1;
        alertStart = now;
      }
      break;
    case 1: // Pause
      if (now - alertStart >= 100) {
        alertStep = 2;
        alertStart = now;
      }
      break;
    case 2: // Beep
      digitalWrite(BUZZER, HIGH);
      if (now - alertStart >= 200) {
        digitalWrite(BUZZER, LOW);
        alertStep = 0;
        alertStart = 0;
      }
      break;
  }
}

// Display status
void displayStatus() {
  Serial.println("=== Status ===");
  Serial.print("Active: "); Serial.println(systemActive ? "YES" : "NO");
  Serial.print("Waste: "); Serial.println(wasteDetected ? "DETECTED" : "CLEAR");
  Serial.print("Type: ");
  if (wasteType == ORGANIC) Serial.println("Organic");
  else if (wasteType == INORGANIC) Serial.println("Inorganic");
  else Serial.println("None");
  Serial.print("Storage: "); Serial.print(storageLevel); Serial.println("%");
  Serial.println("=============");
}
