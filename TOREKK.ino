  #include <Servo.h>

  // Pin dari pin-config.md
  #define SERVO_PIN 9 

  // Conveyor (DC Motor) - pin 22
  // NOTE: Pin 22 BUKAN PWM, jadi hanya bisa ON/OFF (kecepatan penuh)
  #define CONVEYOR_IN1 22  // Arah motor conveyor
  #define CONVEYOR_IN2 23  // Arah motor conveyor

  // Sorter (DC Motor) - tidak ada di pin-config, gunakan default
  #define SORTER_IN3 24
  #define SORTER_IN4 25

  Servo sortirGate;  
  #define GATE_METAL 45      
  #define GATE_PLASTIC 135   
  #define GATE_TENGAH 90     

  void setup() {
    Serial.begin(9600);
    
    Serial.println("=== START SETUP ===");
    
    sortirGate.attach(SERVO_PIN);
    sortirGate.write(GATE_TENGAH);
    Serial.println("✓ Servo Setup");
    delay(500);
    
    // Conveyor pins
    pinMode(CONVEYOR_IN1, OUTPUT);
    pinMode(CONVEYOR_IN2, OUTPUT);
    Serial.println("✓ Pin Setup - Conveyor (Pin 22-23)");
    
    // Sorter pins
    pinMode(SORTER_IN3, OUTPUT);
    pinMode(SORTER_IN4, OUTPUT);
    Serial.println("✓ Pin Setup - Sorter (Pin 24-25)");
    
    conveyor_stop();
    sorter_stop();
    Serial.println("✓ Motor Setup");
    
    Serial.println("=== SETUP COMPLETE ===");
    Serial.println("Smart Trash Wheel Ready!");
    delay(2000);
  }

  // ===== CONVEYOR MOTOR (DC Motor) =====
  void conveyor_jalan(int speed) {
    // Pin 22-23 bukan PWM, jadi kecepatan selalu penuh
    // speed parameter diabaikan
    digitalWrite(CONVEYOR_IN1, HIGH);
    digitalWrite(CONVEYOR_IN2, LOW);
    Serial.println("→ Conveyor ON");
  }

  void conveyor_stop() {
    digitalWrite(CONVEYOR_IN1, LOW);
    digitalWrite(CONVEYOR_IN2, LOW);
    Serial.println("← Conveyor OFF");
  }

  // ===== SORTER MOTOR (DC Motor) =====
  void sorter_jalan(int speed) {
    // Pin 24-25 bukan PWM, jadi kecepatan selalu penuh
    digitalWrite(SORTER_IN3, HIGH);
    digitalWrite(SORTER_IN4, LOW);
    Serial.println("→ Sorter ON");
  }

  void sorter_stop() {
    digitalWrite(SORTER_IN3, LOW);
    digitalWrite(SORTER_IN4, LOW);
    Serial.println("← Sorter OFF");
  }

  void loop() {
    // === MAIN LOOP === Tanpa jeda - servo berputar terus
    
    while(true) {
      // Metal → Tengah
      Serial.println("SERVO → METAL");
      sortirGate.write(GATE_METAL);
      delay(50);
      
      Serial.println("Sorter ON");
      sorter_jalan(255);
      delay(500);
      sorter_stop();
      
      Serial.println("SERVO → TENGAH");
      sortirGate.write(GATE_TENGAH);
      delay(50);
      
      // Tengah → Plastic
      Serial.println("SERVO → PLASTIC");
      sortirGate.write(GATE_PLASTIC);
      delay(50);
      
      Serial.println("Sorter ON");
      sorter_jalan(255);
      delay(500);
      sorter_stop();
      
      Serial.println("SERVO → TENGAH");
      sortirGate.write(GATE_TENGAH);
      delay(50);
      
      // Conveyor jalan
      Serial.println("Conveyor ON");
      conveyor_jalan(255);
      delay(2000);
      conveyor_stop();
    }
  }

