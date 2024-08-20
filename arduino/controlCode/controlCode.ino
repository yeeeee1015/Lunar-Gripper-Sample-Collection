// imports
#include <SoftwareSerial.h>
#include <TFMini.h>
#include "Adafruit_VL53L0X.h"
#include <ezButton.h>
#include "IntervalTimer.h"
#include <ArduinoQueue.h>
#include <Servo.h>
#include "Statistic.h"
// pin definitions
#define prox1 15
#define press1 22
#define press2 21
#define press3 20
#define limit1 10
#define limit2 9
#define limit3 8
#define motorArmA 3  // pulse to move up
#define sdA 4
#define motorArmB 5  // pulse to move down
#define sdB 6
#define motorClaw 2
#define killSwitchPin 7
#define encoderRatio 80.1
SoftwareSerial SerialTFMini(0, 1);  // rx tx for tf mini lidar

TFMini tfmini;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;
IntervalTimer clk;
Servo claw;
Statistic stats;

uint8_t header = 0x59;                // used to tell if tf mini is reading garbage or not
volatile long int currentPosArm = 0;  // increase is CW, decrease is CCW
volatile long int currentPosClaw = 0;

int clawDist = 0;  // obj detection value when no obj
String status = "initializing";
int lidarVal = "";
ArduinoQueue<int> q(100);  // queue for ui instrs
bool isKilled = false;
//encoder consts
const double clawRatio = 0.375;

void killswitch() {
  digitalWrite(killSwitchPin, LOW);
  clk.end();
  while (!q.isEmpty()) {
    q.dequeue();
  }
  while (isKilled) {
    Serial.println("Powerkilled@Powerkilled@Powerkilled@Powerkilled@Powerkilled");
  }
}

void actuateArmMotor(double freqA, double freqB) {
  if (freqA > freqB) {
    double power = (freqA/255.0) * 100.0;
    status = "Arm moving up at " + String(power) + " percent power";
  }
  else if (freqB > freqA) {
    double power = (freqB/255.0) * 100.0;
    status = "Arm moving down at " + String(power) + " percent power";
  }
  else {
    status = "Arm stopped";
  }
  // status = "pwm A is " + String(freqA) + " pwm B is " + String(freqB);
  analogWrite(motorArmA, freqA);
  analogWrite(motorArmB, freqB);
}

// ISR which communicates sensor data and checks for ks/other instrs
void sendOutput() {
  Serial.println(serialize());
  while (Serial.available() > 0) {
    int rec = Serial.parseInt();
    if (rec == 3) {
      status = "killed";
      isKilled = true;
      killswitch();
    } 
    else if (rec != 0) {
      q.enqueue(rec);
    }
  }
}

void setup() {
  Serial.begin(115200);  // Initialize hardware serial port (serial debug port)
  Serial.setTimeout(50); // only wait 50 ms for serial inputs
  pinMode(prox1, INPUT);
  pinMode(killSwitchPin, OUTPUT);
  pinMode(motorArmA, OUTPUT);
  pinMode(motorArmB, OUTPUT);
  pinMode(motorClaw, OUTPUT);
  pinMode(sdA, OUTPUT);
  pinMode(sdB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  SerialTFMini.begin(115200);   // Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);  // Initialize the TF Mini sensor
  digitalWrite(killSwitchPin, HIGH); // KS needs to be pulled high
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  clk.begin(sendOutput, 75000);
  digitalWrite(sdA, HIGH);
  digitalWrite(sdB, HIGH);
}

void loop() {
  int instr = getInstruction();
  delay(50);
  if (instr != -1) {
    doInstruction(instr);
    delay(50);
  }
}

String serialize() {
  lidarVal = getVL53L0XDist();
  return String(getTFMiniDist()) + "@" + String(digitalRead(prox1)) + "@" + String(lidarVal) + "@" + String(digitalRead(limit1)) + "@" + String(digitalRead(limit2)) + "@" + String(digitalRead(limit3)) + "@" + status;
}

int getInstruction() {
  if (q.isEmpty()) {
    status = "idle";
    return -1;
  }
  return q.dequeue();
}

void doInstruction(int instr) {
  switch (instr) {
    case 1:  // start
      start();
      break;
    case 2:  // release
      release();
      break;
    case 5:  // reset claw
      reset();
      break;
    case 6:  // arm moves up til limit switch then down til prox
      setArm();
      break;
    case 7:  // close claw
      closeClaw();
      break;
    default:
      Serial.println("invalid input");
  }
}

void start() {
  status = "Starting start process";
  int groundDist;
  reset();
  actuateArmMotor(0, 127);
  while (digitalRead(prox1)) {  // 15 cm
  }
  actuateArmMotor(0, 0);
  if (!detectObj()) {
    actuateArmMotor(245, 0);  // move claw up
    status = "Object not detected";
    while (!digitalRead(limit2)) {
    }
    actuateArmMotor(0, 0);
    return;
  } 
  else {
    status = "Object detected";
    closeClaw();
    actuateArmMotor(245, 0);  // move claw up
    delay(500);
    if (!detectObj()) {
      status = "Object slipped out";
      while (!digitalRead(limit2)) {
      }
      actuateArmMotor(0, 0);
      return;
    } 
    else {
      status = "Object still in gripper";
      while (!digitalRead(limit2)) {
      }
      actuateArmMotor(0, 0);
      return;
    }
  }
}

void release() {
  status = "Starting release process";
  releaseClaw();
  return;
}

void reset() {
  status = "Starting reset process";
  actuateArmMotor(240, 0);  // move claw up
  while (!digitalRead(limit2)) {
  }
  actuateArmMotor(0, 0);
  currentPosArm = 0;
  releaseClaw();
  closeClaw();
  clawDist = getVL53L0XStats(10);
  release();
  return;
}

void releaseClaw() {
  claw.attach(motorClaw);
  currentPosClaw = 0;
  int time = millis();
  while (!digitalRead(limit3) && millis() <= time + 2500) {
    claw.write(0);
  }
  while (!digitalRead(limit3)) {
    claw.write(65);
  }
  claw.write(90);
  claw.detach();
  return;
}

void closeClaw() {
  status = "Closing Claw";
  claw.attach(motorClaw);
  claw.write(180);
  delay(750);
  claw.write(120);
  delay(6000);
  claw.write(90);
  claw.detach();
  status = "Claw Closed";
  return;
}

void setArm() {
  status = "Starting lower arm Process";
  actuateArmMotor(245, 0);  // move claw up
  while (!digitalRead(limit2)) {
  }
  actuateArmMotor(0, 0);
  delay(500);
  actuateArmMotor(0, 127);
  while (digitalRead(prox1)) {  // 15 cm
  }
  actuateArmMotor(0, 0);
  return;
}

void getTFminiData(int* distance, int* strength) {
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static double rx[9];
  if (SerialTFMini.available()) {
    rx[i] = SerialTFMini.read();
    if (rx[0] != header) {
      i = 0;
    } else if (i == 1 && rx[1] != header) {
      i = 0;
    } else if (i == 8) {
      for (j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if (rx[8] == (checksum % 256)) {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
      }
      i = 0;
    } else {
      i++;
    }
  }
}

int getTFMiniDist() {
  int distance = 0;
  int strength = 0;
  for (int i = 0; i < 3; i++) {  // need a for loop to incr amount of measurements lidar takes and remove lag
    distance = 0;
    strength = 0;
    while (!distance) {
      getTFminiData(&distance, &strength);
    }
  }
  return distance;
}

int getVL53L0XDist() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  return measure.RangeMilliMeter;
}

int getVL53L0XStats(int numAvg) {
  int ct = 0;
  status = "Getting claw average";
  stats.clear();
  while (ct <= numAvg) {
    stats.add(lidarVal);
    ct = ct + 1;
    delay(50);
  }
  return stats.average();
}

bool detectObj() {
  status = "Detecting object";
  double clawThreshold = 15;
  if (abs(getVL53L0XStats(10)-clawDist) > clawThreshold) { 
    return true;
  }
  return false;
}