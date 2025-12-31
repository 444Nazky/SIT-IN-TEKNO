#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ===== WIFI CONFIG =====
const char* SSID = "YOUR_SSID";        // torekkk
const char* PASSWORD = "YOUR_PASSWORD"; // torekk tokopedia

ESP8266WebServer server(80);

// ===== PIN CONFIG =====
const int MOTOR_PIN = D1;              // Conveyor motor
const int PROXIMITY_PIN = D4;          // Waste sensor
const int WEIGHT_PIN = A0;             // Weight sensor
const int ULTRASONIC_TRIG = D3;        // Distance sensor trigger
const int ULTRASONIC_ECHO = D2;        // Distance sensor echo
const int BUTTON_STOP = D8;            // Emergency stop

// ===== MOTOR SPEED =====
const int MOTOR_SPEED = 200;           // 0-255 (speed level)

// ===== STORAGE THRESHOLDS =====
const int WEIGHT_THRESHOLD = 500;      // Heavy vs light

// ===== WASTE TYPES =====
const int ORGANIC = 0;
const int INORGANIC = 1;

// ===== SYSTEM STATE =====
bool system_active = true;
bool emergency_stop = false;
bool waste_detected = false;
int waste_type = -1;
int organic_level = 0;
int inorganic_level = 0;
unsigned long total_waste = 0;
unsigned long organic_count = 0;
unsigned long inorganic_count = 0;

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize pins
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(PROXIMITY_PIN, INPUT);
  pinMode(BUTTON_STOP, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  Serial.println("\n\n=== ROBOT SAMPAH TORIK ===");
  
  // Connect WiFi
  connectWiFi();
  
  // Setup web server
  setupWebServer();
  
  // Startup test
  testSystem();
  
  Serial.println("✓ System Ready!\n");
}

// ===== MAIN LOOP =====
void loop() {
  server.handleClient();
  
  // Check emergency stop
  if (digitalRead(BUTTON_STOP) == LOW) {
    emergency_stop = true;
  }
  
  // Handle emergency stop
  if (emergency_stop) {
    digitalWrite(MOTOR_PIN, 0);
    system_active = false;
    delay(100);
    return;
  }
  
  // Update storage level
  updateStorage();
  
  // Check for waste
  if (digitalRead(PROXIMITY_PIN) == LOW && system_active) {
    handleWaste();
    delay(2000);
  } else {
    // Run motor normally
    digitalWrite(MOTOR_PIN, MOTOR_SPEED);
  }
  
  delay(50);
}

// ===== WIFI FUNCTIONS =====
void connectWiFi() {
  Serial.print("Connecting WiFi: ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PASSWORD);
  int count = 0;
  
  while (WiFi.status() != WL_CONNECTED && count < 20) {
    delay(500);
    Serial.print(".");
    count++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n⚠ WiFi Failed (continue without WiFi)");
  }
}

void setupWebServer() {
  // API: Get status
  server.on("/api/status", []() {
    String json = "{";
    json += "\"active\":" + String(system_active ? "true" : "false") + ",";
    json += "\"waste\":" + String(waste_detected ? "true" : "false") + ",";
    json += "\"total\":" + String(total_waste) + ",";
    json += "\"organic_level\":" + String(organic_level) + ",";
    json += "\"inorganic_level\":" + String(inorganic_level) + ",";
    json += "\"organic_count\":" + String(organic_count) + ",";
    json += "\"inorganic_count\":" + String(inorganic_count);
    json += "}";
    server.send(200, "application/json", json);
  });
  
  // API: Stop system
  server.on("/api/stop", []() {
    emergency_stop = true;
    server.send(200, "text/plain", "STOPPED");
  });
  
  server.begin();
  Serial.println("✓ Web Server Started");
}

// ===== WASTE HANDLING =====
void handleWaste() {
  Serial.println("\n>>> WASTE DETECTED!");
  
  // Stop conveyor
  digitalWrite(MOTOR_PIN, 0);
  delay(500);
  
  // Read weight
  int weight = analogRead(WEIGHT_PIN);
  
  // Classify
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
  digitalWrite(MOTOR_PIN, MOTOR_SPEED);
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
  // Measure distance using ultrasonic
  int distance = measureDistance();
  
  if (distance > 0) {
    // Map distance to percentage (50cm = empty, 5cm = full)
    organic_level = map(distance, 50, 5, 0, 100);
    organic_level = constrain(organic_level, 0, 100);
    inorganic_level = organic_level;
  }
  
  // Alert if full
  if (organic_level >= 90 || inorganic_level >= 90) {
    // Storage full alert
  }
}

int measureDistance() {
  // Send trigger pulse
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  // Measure echo time
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000);
  
  if (duration == 0) return -1;
  
  // Convert to distance (cm)
  return duration * 0.034 / 2;
}

// ===== SYSTEM TEST =====
void testSystem() {
  Serial.print("Testing motor... ");
  digitalWrite(MOTOR_PIN, MOTOR_SPEED);
  delay(500);
  digitalWrite(MOTOR_PIN, 0);
  Serial.println("✓");
  
  Serial.print("Testing sensors... ");
  digitalRead(PROXIMITY_PIN);
  analogRead(WEIGHT_PIN);
  measureDistance();
  Serial.println("✓");
}

// ===== KAPAL TOREKK =====
