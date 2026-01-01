#include <Servo.h>

#define SERVO_PIN 9 
#define CONVEYOR_IN1 2
#define CONVEYOR_IN2 3
#define CONVEYOR_EN  4 


#define SORTER_IN3 5
#define SORTER_IN4 6
#define SORTER_EN  7 

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
  
  pinMode(CONVEYOR_IN1, OUTPUT);
  pinMode(CONVEYOR_IN2, OUTPUT);
  pinMode(CONVEYOR_EN, OUTPUT);
  Serial.println("✓ Pin Setup - Conveyor");
  
  pinMode(SORTER_IN3, OUTPUT);
  pinMode(SORTER_IN4, OUTPUT);
  pinMode(SORTER_EN, OUTPUT);
  Serial.println("✓ Pin Setup - Sorter");
  
  conveyor_stop();
  sorter_stop();
  Serial.println("✓ Motor Setup");
  
  Serial.println("=== SETUP COMPLETE ===");
  Serial.println("Smart Trash Wheel Ready!");
  delay(2000);
}

void conveyor_jalan(int speed) {
  digitalWrite(CONVEYOR_IN1, HIGH);
  digitalWrite(CONVEYOR_IN2, LOW);
  analogWrite(CONVEYOR_EN, speed);
}

void conveyor_stop() {
  digitalWrite(CONVEYOR_IN1, LOW);
  digitalWrite(CONVEYOR_IN2, LOW);
  analogWrite(CONVEYOR_EN, 0);
}

void sorter_jalan(int speed) {
  digitalWrite(SORTER_IN3, HIGH);
  digitalWrite(SORTER_IN4, LOW);
  analogWrite(SORTER_EN, speed);
}

void sorter_stop() {
  digitalWrite(SORTER_IN3, LOW);
  digitalWrite(SORTER_IN4, LOW);
  analogWrite(SORTER_EN, 0);
}

void sortir_ke_metal() {
  Serial.println("→ Sortir ke BOX 1 (METAL)");
  sortirGate.write(GATE_METAL);
  delay(800);
  
  sorter_jalan(200);
  delay(1500);
  sorter_stop();
  
  sortirGate.write(GATE_TENGAH); 
  delay(300);
}

void sortir_ke_plastic() {
  Serial.println("→ Sortir ke BOX 2 (PLASTIC)");
  sortirGate.write(GATE_PLASTIC);
  delay(800);
  
  sorter_jalan(200);  
  delay(1500);
  sorter_stop();
  
  sortirGate.write(GATE_TENGAH);  
  delay(300);
}

void loop() {
  // === MAIN LOOP ===
  Serial.println("");
  Serial.println("===== MAIN LOOP START =====");
  
  Serial.println("1. RUN CONVEYOR");
  conveyor_jalan(220);
  delay(3000); 
  conveyor_stop();
  delay(500);
  
  Serial.println("2. SERVO - Sampah #1 (METAL)");
  sortir_ke_metal();
  delay(1000);
  
  Serial.println("3. RUN CONVEYOR");
  conveyor_jalan(220);
  delay(3000);
  conveyor_stop();
  delay(300);

  Serial.println("4. SERVO - Sampah #2 (PLASTIC)");
  sortir_ke_plastic();
  delay(1000);
  
  Serial.println("5. RUN CONVEYOR");
  conveyor_jalan(220);
  delay(3000);
  conveyor_stop();
  delay(500);
  
  Serial.println("6. SERVO - Sampah #3 (METAL)");
  sortir_ke_metal();
  delay(1000);
  
  Serial.println("7. RUN CONVEYOR");
  conveyor_jalan(220);
  delay(3000);
  conveyor_stop();
  delay(500);
  
  Serial.println("8. SERVO - Sampah #4 (PLASTIC)");
  sortir_ke_plastic();
  delay(1000);
  
  Serial.println("===== LOOP BACK =====");
  delay(2000);
}
