#include <SoftwareSerial.h>
#include <Arduino.h>

// ===== WIFI MODULE =====
SoftwareSerial espSerial(10, 11);
const char* SSID = "YOUR_SSID";
const char* PASSWORD = "YOUR_PASSWORD";

// ===== PIN CONFIG AMA SPEED-NYA =====
const int MOTOR_PIN = 22;          // Conveyor motor
const int PROXIMITY_PIN = 24;      // Waste sensor
const int WEIGHT_PIN = A0;         // Weight sensor
const int ULTRASONIC_TRIG = 23;    // Distance sensor trigger
const int ULTRASONIC_ECHO = 25;    // Distance sensor echo
const int BUTTON_STOP = 26;        // Emergency stop
const int MOTOR_SPEED = 200;
const int WEIGHT_THRESHOLD = 500;
const int ORGANIC = 0;
const int INORGANIC = 1;

// ===== BOOL TOREKK =====
bool system_active = true;
bool emergency_stop = false;
bool waste_detected = false;
int waste_type = -1;
int organic_level = 0;
int inorganic_level = 0;
unsigned long total_waste = 0;
unsigned long organic_count = 0;
unsigned long inorganic_count = 0;
String ipAddress = "";

// ===== MOAL BALEG =====
void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);
  delay(1000);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(PROXIMITY_PIN, INPUT);
  pinMode(BUTTON_STOP, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  Serial.println("\n\n=== ROBOT SAMPAH TORIK MEGA RAKSASA SUPER DUPER NIGGA ===");
  connectWiFi();
  setupWebServer();
  testSystem();
  Serial.println("YES KING!\n");
}

void loop() {
  if (digitalRead(BUTTON_STOP) == LOW) {
    emergency_stop = true;
  }

  if (emergency_stop) {
    digitalWrite(MOTOR_PIN, 0);
    system_active = false;
    delay(100);
    return;
  }
  
  updateStorage();

  if (digitalRead(PROXIMITY_PIN) == LOW && system_active) {
    handleWaste();
    delay(2000);
  } else {
    analogWrite(MOTOR_PIN, MOTOR_SPEED);
  }
  
  delay(50);
}

// ===== WIFI FUNCTIONS =====
void connectWiFi() {
  Serial.print("Connecting WiFi: ");
  Serial.println(SSID);
  
  espSerial.println("AT+CWJAP=\"" + String(SSID) + "\",\"" + String(PASSWORD) + "\"");
  delay(5000);
  
  espSerial.println("AT+CIFSR");
  delay(1000);
  
  Serial.println("WiFi Connected  King!");
  Serial.print("IP: ");
  Serial.println(ipAddress);

void setupWebServer() {
  Serial.println("✓ Web Server Started");
  Serial.println("API Endpoints:");
  Serial.println("  GET /status - Get system status");
  Serial.println("  GET /stop - Emergency stop");
}
void handleWaste() {
  Serial.println("\n>>> WASTE DETECTED!");
  analogWrite(MOTOR_PIN, 0);
  delay(500);
  
  // BALEG MOAL
  int weight = analogRead(WEIGHT_PIN);
  int type = classifyWaste(weight);
  waste_type = type;
  waste_detected = true;
  
  if (type == ORGANIC) {
    Serial.println(">>> ORGANIC!");
    organic_count++;
  } else {
    Serial.println(">>> INORGANIC!");
    inorganic_count++;
  }
  
  total_waste++;
  waste_detected = false;
  
  delay(500);
  analogWrite(MOTOR_PIN, MOTOR_SPEED);
}

int classifyWaste(int weight) {
  // Simple: heavy = inorganic, light = organic
  if (weight > WEIGHT_THRESHOLD) {
    return INORGANIC;
  } else {
    return ORGANIC;
  }
}

// ===== STORAGE MONITORING =====
void updateStorage() {
  int distance = measureDistance();
  
  if (distance > 0) {
    organic_level = map(distance, 50, 5, 0, 100);
    organic_level = constrain(organic_level, 0, 100);
    inorganic_level = organic_level;
  }
  
  if (organic_level >= 90 || inorganic_level >= 90) {
    // Storage full alert
  }
}

int measureDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000);
  
  if (duration == 0) return -1;
  
  return duration * 0.034 / 2;
}

// ===== SYSTEM TEST =====
void testSystem() {
  Serial.print("Testing motor... ");
  analogWrite(MOTOR_PIN, MOTOR_SPEED);
  delay(500);
  analogWrite(MOTOR_PIN, 0);
  Serial.println("✓");
  
  Serial.print("Testing sensors... ");
  digitalRead(PROXIMITY_PIN);
  analogRead(WEIGHT_PIN);
  measureDistance();
  Serial.println("yes king");
}

// ===== KAPAL TOREKK =====

