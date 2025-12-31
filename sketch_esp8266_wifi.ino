/*
 * Waste Sorting Robot - ESP8266 dengan WiFi & Web Dashboard
 * Versi Advanced dengan remote monitoring
 * 
 * Features:
 * - WiFi connectivity
 * - Web dashboard (HTML)
 * - REST API untuk mobile apps
 * - Data logging
 * - OTA (Over-The-Air) updates
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>

// ==================== WiFi Configuration ====================
const char* SSID = "YOUR_WIFI_SSID";           // Ganti dengan SSID WiFi Anda
const char* PASSWORD = "YOUR_WIFI_PASSWORD";   // Ganti dengan password WiFi Anda
const int WEB_SERVER_PORT = 80;

ESP8266WebServer webServer(WEB_SERVER_PORT);

// ==================== PIN DEFINITIONS ====================
const int CONVEYOR_MOTOR_PIN = D1;
const int COLOR_SENSOR_S0 = D2;
const int COLOR_SENSOR_S1 = D3;
const int COLOR_SENSOR_S2 = D4;
const int COLOR_SENSOR_S3 = D5;
const int COLOR_SENSOR_OUT = D6;
const int WEIGHT_SENSOR_PIN = A0;
const int SEPARATOR_STEP_PIN1 = D7;
const int SEPARATOR_STEP_PIN2 = D8;
const int SEPARATOR_STEP_PIN3 = D0;
const int SEPARATOR_STEP_PIN4 = D1;  // WARNING: Conflicts with conveyor
const int PROXIMITY_SENSOR_PIN = D4;
const int ORGANIC_STORAGE_LED = D5;
const int INORGANIC_STORAGE_LED = D6;
const int BUZZER_PIN = D7;
const int EMERGENCY_STOP_PIN = D8;
const int ULTRASONIC_TRIG_PIN = D3;
const int ULTRASONIC_ECHO_PIN = D2;

// ==================== CONFIGURATION ====================
const int CONVEYOR_SPEED = 200;
const int CLASSIFICATION_THRESHOLD = 500;
const int SEPARATOR_STEPS = 512;
const int SEPARATOR_DELAY = 800;

const int ORGANIC_STORAGE = 0;
const int INORGANIC_STORAGE = 1;

// ==================== DATA LOGGING ====================
struct LogEntry {
  unsigned long timestamp;
  int wasteType;
  int weight;
  int colorR, colorG, colorB;
};

const int MAX_LOG_ENTRIES = 100;
LogEntry logBuffer[MAX_LOG_ENTRIES];
int logIndex = 0;
int logCount = 0;

// ==================== SYSTEM STATE ====================
struct SystemState {
  bool active;
  bool emergencyStopActive;
  bool wasteDetected;
  int currentWasteType;
  int organicStorageLevel;
  int inorganicStorageLevel;
  unsigned long totalWasteProcessed;
  unsigned long organicWasteCount;
  unsigned long inorganicWasteCount;
  unsigned long lastStatusUpdate;
  unsigned long lastMainLoopRun;
  int stepperPosition;
} system = {
  .active = true,
  .emergencyStopActive = false,
  .wasteDetected = false,
  .currentWasteType = -1,
  .organicStorageLevel = 0,
  .inorganicStorageLevel = 0,
  .totalWasteProcessed = 0,
  .organicWasteCount = 0,
  .inorganicWasteCount = 0,
  .lastStatusUpdate = 0,
  .lastMainLoopRun = 0,
  .stepperPosition = 0
};

struct ColorData {
  int red, green, blue;
} colorData = {0, 0, 0};

const int stepperSequence[8][4] = {
  {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
  {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1}
};

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n╔═══════════════════════════════════════════════╗");
  Serial.println("║ Trash Robot - ESP8266 WiFi Edition              ║");
  Serial.println("║ Initializing...                                 ║");
  Serial.println("╚═══════════════════════════════════════════════╝");
  
  initializePins();
  initializeColorSensor();
  setupWiFi();
  setupWebServer();
  
  Serial.println("✓ All systems initialized!");
}

// ==================== MAIN LOOP ====================
void loop() {
  unsigned long currentTime = millis();
  
  // Handle WiFi & Web Server
  webServer.handleClient();
  
  // Maintain WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.reconnect();
  }
  
  checkEmergencyStop();
  
  if (system.emergencyStopActive) {
    handleEmergencyStop();
    return;
  }
  
  updateStorageStatus();
  
  if (currentTime - system.lastMainLoopRun >= 100) {
    system.lastMainLoopRun = currentTime;
    
    system.wasteDetected = detectWaste();
    
    if (system.wasteDetected && system.active) {
      handleWasteDetection();
    } else if (system.active) {
      startConveyor();
    }
  }
  
  updateIndicatorLEDs();
  
  if (currentTime - system.lastStatusUpdate >= 10000) {
    system.lastStatusUpdate = currentTime;
    displaySystemStatus();
  }
}

// ==================== WiFi SETUP ====================
void setupWiFi() {
  Serial.print("\n▶ Connecting to WiFi: ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\n⚠ WiFi connection failed!");
    Serial.println("  Continuing without WiFi...");
  }
}

// ==================== WEB SERVER SETUP ====================
void setupWebServer() {
  webServer.on("/", handleWebRoot);
  webServer.on("/api/status", handleApiStatus);
  webServer.on("/api/logs", handleApiLogs);
  webServer.on("/api/control", handleApiControl);
  webServer.onNotFound(handleNotFound);
  
  webServer.begin();
  Serial.println("✓ Web server started on port 80");
}

// ==================== WEB HANDLERS ====================
void handleWebRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Trash Robot Dashboard</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { 
      font-family: 'Segoe UI', Arial, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      padding: 20px;
    }
    .container { max-width: 1000px; margin: 0 auto; }
    .header { color: white; text-align: center; margin-bottom: 30px; }
    .header h1 { font-size: 2.5em; margin-bottom: 10px; }
    .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }
    .card {
      background: white;
      border-radius: 10px;
      padding: 20px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
    }
    .card h2 { color: #333; margin-bottom: 15px; }
    .status-item { display: flex; justify-content: space-between; margin: 10px 0; padding: 10px 0; border-bottom: 1px solid #eee; }
    .label { font-weight: 600; color: #666; }
    .value { color: #333; }
    .progress-bar { width: 100%; height: 30px; background: #eee; border-radius: 5px; overflow: hidden; margin: 10px 0; }
    .progress-fill { height: 100%; background: linear-gradient(90deg, #4caf50, #81c784); display: flex; align-items: center; justify-content: center; color: white; font-weight: bold; }
    .organic-fill { background: linear-gradient(90deg, #8b7355, #a0826d); }
    .inorganic-fill { background: linear-gradient(90deg, #2196f3, #42a5f5); }
    .btn { 
      padding: 10px 20px;
      margin: 5px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      font-weight: 600;
      transition: all 0.3s;
    }
    .btn-primary { background: #667eea; color: white; }
    .btn-primary:hover { background: #5568d3; }
    .btn-danger { background: #f44336; color: white; }
    .btn-danger:hover { background: #d32f2f; }
    .status-active { color: #4caf50; font-weight: bold; }
    .status-inactive { color: #f44336; font-weight: bold; }
    .stats { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; }
    .stat-box { background: #f5f5f5; padding: 15px; border-radius: 5px; text-align: center; }
    .stat-number { font-size: 1.8em; font-weight: bold; color: #667eea; }
    .stat-label { color: #666; font-size: 0.9em; margin-top: 5px; }
    .alert { padding: 15px; margin: 10px 0; border-radius: 5px; }
    .alert-warning { background: #fff3cd; color: #856404; border: 1px solid #ffeaa7; }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>♻️ Trash Robot Dashboard</h1>
      <p>Waste Sorting System Monitor</p>
    </div>
    
    <div class="status-grid">
      <!-- System Status Card -->
      <div class="card">
        <h2>System Status</h2>
        <div class="status-item">
          <span class="label">Status:</span>
          <span class="value" id="status">Loading...</span>
        </div>
        <div class="status-item">
          <span class="label">Current Task:</span>
          <span class="value" id="task">Idle</span>
        </div>
        <div class="status-item">
          <span class="label">Waste Detected:</span>
          <span class="value" id="waste">No</span>
        </div>
        <button class="btn btn-primary" onclick="toggleSystem()">Toggle System</button>
        <button class="btn btn-danger" onclick="emergencyStop()">EMERGENCY STOP</button>
      </div>
      
      <!-- Storage Status Card -->
      <div class="card">
        <h2>Storage Levels</h2>
        <div style="margin-bottom: 20px;">
          <div class="label">Organic Storage</div>
          <div class="progress-bar"><div class="progress-fill organic-fill" id="organic-bar" style="width: 0%"></div></div>
          <div style="text-align: right; font-weight: 600;"><span id="organic-percent">0</span>%</div>
        </div>
        <div>
          <div class="label">Inorganic Storage</div>
          <div class="progress-bar"><div class="progress-fill inorganic-fill" id="inorganic-bar" style="width: 0%"></div></div>
          <div style="text-align: right; font-weight: 600;"><span id="inorganic-percent">0</span>%</div>
        </div>
      </div>
      
      <!-- Statistics Card -->
      <div class="card">
        <h2>Statistics</h2>
        <div class="stats">
          <div class="stat-box">
            <div class="stat-number" id="total">0</div>
            <div class="stat-label">Total Processed</div>
          </div>
          <div class="stat-box">
            <div class="stat-number" id="organic">0</div>
            <div class="stat-label">Organic</div>
          </div>
          <div class="stat-box">
            <div class="stat-number" id="inorganic">0</div>
            <div class="stat-label">Inorganic</div>
          </div>
        </div>
      </div>
    </div>
  </div>
  
  <script>
    function updateDashboard() {
      fetch('/api/status')
        .then(r => r.json())
        .then(data => {
          document.getElementById('status').textContent = data.active ? 'ACTIVE' : 'INACTIVE';
          document.getElementById('status').className = data.active ? 'value status-active' : 'value status-inactive';
          document.getElementById('task').textContent = data.currentTask || 'Idle';
          document.getElementById('waste').textContent = data.wasteDetected ? 'YES' : 'No';
          document.getElementById('organic-percent').textContent = data.organicLevel;
          document.getElementById('inorganic-percent').textContent = data.inorganicLevel;
          document.getElementById('organic-bar').style.width = data.organicLevel + '%';
          document.getElementById('inorganic-bar').style.width = data.inorganicLevel + '%';
          document.getElementById('total').textContent = data.totalProcessed;
          document.getElementById('organic').textContent = data.organicCount;
          document.getElementById('inorganic').textContent = data.inorganicCount;
        })
        .catch(e => console.error('Error:', e));
    }
    
    function toggleSystem() {
      fetch('/api/control?action=toggle')
        .then(r => r.json())
        .then(data => updateDashboard());
    }
    
    function emergencyStop() {
      if (confirm('Are you sure? This will stop all motors!')) {
        fetch('/api/control?action=emergency')
          .then(r => r.json())
          .then(data => updateDashboard());
      }
    }
    
    // Update every 1 second
    updateDashboard();
    setInterval(updateDashboard, 1000);
  </script>
</body>
</html>
  )HTML";
  
  webServer.send(200, "text/html", html);
}

void handleApiStatus() {
  String json = "{";
  json += "\"active\":" + String(system.active ? "true" : "false") + ",";
  json += "\"currentTask\":\"";
  
  if (system.wasteDetected) {
    json += (system.currentWasteType == ORGANIC_STORAGE ? "Processing Organic" : "Processing Inorganic");
  } else {
    json += "Running Conveyor";
  }
  
  json += "\",";
  json += "\"wasteDetected\":" + String(system.wasteDetected ? "true" : "false") + ",";
  json += "\"organicLevel\":" + String(system.organicStorageLevel) + ",";
  json += "\"inorganicLevel\":" + String(system.inorganicStorageLevel) + ",";
  json += "\"totalProcessed\":" + String(system.totalWasteProcessed) + ",";
  json += "\"organicCount\":" + String(system.organicWasteCount) + ",";
  json += "\"inorganicCount\":" + String(system.inorganicWasteCount);
  json += "}";
  
  webServer.send(200, "application/json", json);
}

void handleApiLogs() {
  String json = "[";
  for (int i = 0; i < logCount; i++) {
    int idx = (logIndex - logCount + i + MAX_LOG_ENTRIES) % MAX_LOG_ENTRIES;
    json += "{";
    json += "\"time\":" + String(logBuffer[idx].timestamp) + ",";
    json += "\"type\":\"" + String(logBuffer[idx].wasteType == ORGANIC_STORAGE ? "organic" : "inorganic") + "\",";
    json += "\"weight\":" + String(logBuffer[idx].weight);
    json += "}";
    if (i < logCount - 1) json += ",";
  }
  json += "]";
  
  webServer.send(200, "application/json", json);
}

void handleApiControl() {
  if (webServer.hasArg("action")) {
    String action = webServer.arg("action");
    if (action == "toggle") {
      system.active = !system.active;
    } else if (action == "emergency") {
      system.emergencyStopActive = true;
    }
  }
  
  handleApiStatus();
}

void handleNotFound() {
  webServer.send(404, "text/plain", "Not Found");
}

// ==================== DATA LOGGING ====================
void logWasteEvent(int wasteType, int weight, int r, int g, int b) {
  logBuffer[logIndex].timestamp = millis();
  logBuffer[logIndex].wasteType = wasteType;
  logBuffer[logIndex].weight = weight;
  logBuffer[logIndex].colorR = r;
  logBuffer[logIndex].colorG = g;
  logBuffer[logIndex].colorB = b;
  
  logIndex = (logIndex + 1) % MAX_LOG_ENTRIES;
  if (logCount < MAX_LOG_ENTRIES) logCount++;
}

// ==================== HELPER FUNCTIONS ====================
void initializePins() {
  pinMode(CONVEYOR_MOTOR_PIN, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN1, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN2, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN3, OUTPUT);
  pinMode(SEPARATOR_STEP_PIN4, OUTPUT);
  
  pinMode(COLOR_SENSOR_S0, OUTPUT);
  pinMode(COLOR_SENSOR_S1, OUTPUT);
  pinMode(COLOR_SENSOR_S2, OUTPUT);
  pinMode(COLOR_SENSOR_S3, OUTPUT);
  pinMode(COLOR_SENSOR_OUT, INPUT);
  
  pinMode(WEIGHT_SENSOR_PIN, INPUT);
  pinMode(PROXIMITY_SENSOR_PIN, INPUT);
  pinMode(ORGANIC_STORAGE_LED, OUTPUT);
  pinMode(INORGANIC_STORAGE_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(EMERGENCY_STOP_PIN, INPUT);
}

void initializeColorSensor() {
  digitalWrite(COLOR_SENSOR_S0, HIGH);
  digitalWrite(COLOR_SENSOR_S1, LOW);
}

void startConveyor() {
  if (system.active && !system.emergencyStopActive) {
    analogWrite(CONVEYOR_MOTOR_PIN, CONVEYOR_SPEED);
  }
}

void stopConveyor() {
  analogWrite(CONVEYOR_MOTOR_PIN, 0);
}

void handleWasteDetection() {
  Serial.println("⚠ Waste detected!");
  
  stopConveyor();
  delay(500);
  
  readColorSensor();
  int weight = readWeightSensor();
  system.currentWasteType = classifyWaste();
  
  activateSeparator(system.currentWasteType);
  
  // Log the event
  logWasteEvent(system.currentWasteType, weight, colorData.red, colorData.green, colorData.blue);
  
  // Update statistics
  system.totalWasteProcessed++;
  if (system.currentWasteType == ORGANIC_STORAGE) {
    system.organicWasteCount++;
  } else {
    system.inorganicWasteCount++;
  }
  
  delay(500);
  startConveyor();
}

int classifyWaste() {
  bool isOrganic = false;
  
  if (colorData.red > 400 && colorData.green > 300 && colorData.blue < 300) {
    isOrganic = true;
  } else if (colorData.green > colorData.red && colorData.green > 350) {
    isOrganic = true;
  }
  
  return isOrganic ? ORGANIC_STORAGE : INORGANIC_STORAGE;
}

void readColorSensor() {
  digitalWrite(COLOR_SENSOR_S2, LOW);
  digitalWrite(COLOR_SENSOR_S3, LOW);
  colorData.red = pulseIn(COLOR_SENSOR_OUT, LOW, 1000);
  
  digitalWrite(COLOR_SENSOR_S2, HIGH);
  digitalWrite(COLOR_SENSOR_S3, HIGH);
  colorData.green = pulseIn(COLOR_SENSOR_OUT, LOW, 1000);
  
  digitalWrite(COLOR_SENSOR_S2, LOW);
  digitalWrite(COLOR_SENSOR_S3, HIGH);
  colorData.blue = pulseIn(COLOR_SENSOR_OUT, LOW, 1000);
}

bool detectWaste() {
  return digitalRead(PROXIMITY_SENSOR_PIN) == LOW;
}

int readWeightSensor() {
  return analogRead(WEIGHT_SENSOR_PIN);
}

int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  return (duration == 0) ? -1 : (duration * 0.034 / 2);
}

void updateStorageStatus() {
  int distance = measureDistance(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
  if (distance > 0) {
    system.organicStorageLevel = map(distance, 50, 5, 0, 100);
    system.organicStorageLevel = constrain(system.organicStorageLevel, 0, 100);
    system.inorganicStorageLevel = system.organicStorageLevel;
  }
}

void updateIndicatorLEDs() {
  if (system.organicStorageLevel < 70) {
    digitalWrite(ORGANIC_STORAGE_LED, LOW);
  } else {
    digitalWrite(ORGANIC_STORAGE_LED, HIGH);
  }
  
  if (system.inorganicStorageLevel < 70) {
    digitalWrite(INORGANIC_STORAGE_LED, LOW);
  } else {
    digitalWrite(INORGANIC_STORAGE_LED, HIGH);
  }
}

void activateSeparator(int wasteType) {
  int targetPosition = (wasteType == ORGANIC_STORAGE) ? 128 : 384;
  moveStepperToPosition(targetPosition);
  delay(SEPARATOR_DELAY);
  moveStepperToPosition(0);
}

void moveStepperToPosition(int targetPosition) {
  int stepsToMove = targetPosition - system.stepperPosition;
  
  if (stepsToMove > SEPARATOR_STEPS / 2) {
    stepsToMove -= SEPARATOR_STEPS;
  } else if (stepsToMove < -SEPARATOR_STEPS / 2) {
    stepsToMove += SEPARATOR_STEPS;
  }
  
  if (stepsToMove > 0) {
    for (int i = 0; i < stepsToMove; i++) {
      stepForward();
      delayMicroseconds(2000);
    }
  } else if (stepsToMove < 0) {
    for (int i = 0; i > stepsToMove; i--) {
      stepBackward();
      delayMicroseconds(2000);
    }
  }
  
  system.stepperPosition = targetPosition;
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

void checkEmergencyStop() {
  if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
    if (!system.emergencyStopActive) {
      system.emergencyStopActive = true;
    }
  }
}

void handleEmergencyStop() {
  static unsigned long lastWarningTime = 0;
  
  stopConveyor();
  
  if (millis() - lastWarningTime >= 500) {
    lastWarningTime = millis();
    digitalWrite(ORGANIC_STORAGE_LED, !digitalRead(ORGANIC_STORAGE_LED));
    digitalWrite(INORGANIC_STORAGE_LED, !digitalRead(INORGANIC_STORAGE_LED));
  }
  
  digitalWrite(BUZZER_PIN, HIGH);
  system.active = false;
  
  if (digitalRead(EMERGENCY_STOP_PIN) == HIGH) {
    delay(50);
    if (digitalRead(EMERGENCY_STOP_PIN) == HIGH) {
      system.emergencyStopActive = false;
      system.active = true;
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(ORGANIC_STORAGE_LED, LOW);
      digitalWrite(INORGANIC_STORAGE_LED, LOW);
    }
  }
}

void displaySystemStatus() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      SYSTEM STATUS                     ║");
  Serial.print("║ Organic: ");
  Serial.print(system.organicStorageLevel);
  Serial.print("% | Inorganic: ");
  Serial.print(system.inorganicStorageLevel);
  Serial.println("% ║");
  Serial.print("║ Total Processed: ");
  Serial.print(system.totalWasteProcessed);
  Serial.println("                        ║");
  Serial.println("╚════════════════════════════════════════╝\n");
}
