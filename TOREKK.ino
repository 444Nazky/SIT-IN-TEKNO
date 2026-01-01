#include <Servo.h>

#define SERVO_PIN 9 

#define CONVEYOR_IN1 22
#define CONVEYOR_IN2 23
#define SORTER_IN3 24
#define SORTER_IN4 25

Servo sortirGate;  
#define GATE_METAL 45      
#define GATE_PLASTIC 135   
#define GATE_TENGAH 90     

void setup() {
  Serial.begin(9600);
  
  Serial.println("START SETUP");
  
  sortirGate.attach(SERVO_PIN);
  sortirGate.write(GATE_TENGAH);
  Serial.println("SERVO OK");
  delay(500);
  
  pinMode(CONVEYOR_IN1, OUTPUT);
  pinMode(CONVEYOR_IN2, OUTPUT);
  Serial.println("CONVEYOR PIN OK");
  
  pinMode(SORTER_IN3, OUTPUT);
  pinMode(SORTER_IN4, OUTPUT);
  Serial.println("SORTER PIN OK");
  
  conveyor_stop();
  sorter_stop();
  Serial.println("MOTOR STOP");
  
  Serial.println("SYSTEM READY");
  delay(1000);
}

void conveyor_jalan(int speed) {
  digitalWrite(CONVEYOR_IN1, HIGH);
  digitalWrite(CONVEYOR_IN2, LOW);
}

void conveyor_stop() {
  digitalWrite(CONVEYOR_IN1, LOW);
  digitalWrite(CONVEYOR_IN2, LOW);
}

void sorter_jalan(int speed) {
  digitalWrite(SORTER_IN3, HIGH);
  digitalWrite(SORTER_IN4, LOW);
}

void sorter_stop() {
  digitalWrite(SORTER_IN3, LOW);
  digitalWrite(SORTER_IN4, LOW);
}

void loop() {
  while(true) {
    Serial.println("CONVEYOR ON");
    conveyor_jalan(255);
    delay(2000);
    conveyor_stop();
    delay(500);
    
    Serial.println("SERVO METAL");
    sortirGate.write(GATE_METAL);
    delay(100);
    
    Serial.println("SORTER ON");
    sorter_jalan(255);
    delay(500);
    sorter_stop();
    
    Serial.println("SERVO TENGAH");
    sortirGate.write(GATE_TENGAH);
    delay(100);
    
    Serial.println("SERVO PLASTIC");
    sortirGate.write(GATE_PLASTIC);
    delay(100);
    
    Serial.println("SORTER ON");
    sorter_jalan(255);
    delay(500);
    sorter_stop();
    
    Serial.println("SERVO TENGAH");
    sortirGate.write(GATE_TENGAH);
    delay(100);
  }
}

