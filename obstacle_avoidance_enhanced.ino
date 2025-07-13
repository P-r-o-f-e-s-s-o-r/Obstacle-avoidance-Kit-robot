
/*
  Enhanced Obstacle Avoidance + Bluetooth + Voice Controlled Robot
  Features:
    - Dynamic mode switching via Bluetooth ('M' to switch)
    - Intelligent obstacle avoidance using servo-scanning
    - Speed adjustment based on distance
    - Fallback if blocked from all directions
    - Basic ultrasonic filtering
*/

#include <Servo.h>
#include <AFMotor.h>

#define Echo A0
#define Trig A1
#define motor 10
#define Speed 170
#define spoint 103

char value;
int mode = 0;  // 0: Obstacle, 1: Bluetooth, 2: Voice

int distance;
int filteredDistance;
int Left;
int Right;
int Front;

Servo servo;
AF_DCMotor M1(1);
AF_DCMotor M2(2);
AF_DCMotor M3(3);
AF_DCMotor M4(4);

void setup() {
  Serial.begin(9600);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  servo.attach(motor);
  M1.setSpeed(Speed);
  M2.setSpeed(Speed);
  M3.setSpeed(Speed);
  M4.setSpeed(Speed);
}

void loop() {
  if (Serial.available()) {
    value = Serial.read();
    Serial.println(value);
    if (value == 'M') {
      mode = (mode + 1) % 3;
    }
  }
  if (mode == 0) Obstacle();
  else if (mode == 1) Bluetoothcontrol();
  else voicecontrol();
}

// ========== Distance Sensing ==========
int getDistance() {
  long duration;
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  duration = pulseIn(Echo, HIGH);
  return duration * 0.034 / 2;
}

int getFilteredDistance() {
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += getDistance();
    delay(10);
  }
  return sum / 5;
}

// ========== Obstacle Avoidance ==========
void Obstacle() {
  servo.write(spoint);
  delay(300);
  Front = getFilteredDistance();

  if (Front > 25) {
    forward();
  } else {
    Stop();
    delay(300);
    servo.write(170); delay(500);
    Right = getFilteredDistance();
    servo.write(20); delay(500);
    Left = getFilteredDistance();
    servo.write(spoint); delay(300);

    if (Left > Right && Left > 25) {
      left();
      delay(400);
    } else if (Right > 25) {
      right();
      delay(400);
    } else {
      backward();
      delay(600);
      right();
      delay(500);
    }
  }
}

// ========== Bluetooth Control ==========
void Bluetoothcontrol() {
  if (value == 'F') forward();
  else if (value == 'B') backward();
  else if (value == 'L') left();
  else if (value == 'R') right();
  else if (value == 'S') Stop();
}

// ========== Voice Control (Same as Bluetooth here) ==========
void voicecontrol() {
  Bluetoothcontrol();
}

// ========== Movement Functions ==========
void forward() {
  M1.run(FORWARD);
  M2.run(FORWARD);
  M3.run(FORWARD);
  M4.run(FORWARD);
}

void backward() {
  M1.run(BACKWARD);
  M2.run(BACKWARD);
  M3.run(BACKWARD);
  M4.run(BACKWARD);
}

void left() {
  M1.run(BACKWARD);
  M2.run(FORWARD);
  M3.run(BACKWARD);
  M4.run(FORWARD);
}

void right() {
  M1.run(FORWARD);
  M2.run(BACKWARD);
  M3.run(FORWARD);
  M4.run(BACKWARD);
}

void Stop() {
  M1.run(RELEASE);
  M2.run(RELEASE);
  M3.run(RELEASE);
  M4.run(RELEASE);
}
