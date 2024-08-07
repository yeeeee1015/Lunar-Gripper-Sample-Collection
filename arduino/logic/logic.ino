// imports
#include <SoftwareSerial.h>
#include <TFMini.h>
#include "Adafruit_VL53L0X.h"
#include <ezButton.h>
#include "IntervalTimer.h"
#include <ArduinoQueue.h>
#include <PWMServo.h>
// pin definitions
#define prox1 15
#define press1 22
#define press2 21
#define press3 20
#define limit1 10
#define limit2 9
#define limit3 8
#define encoderArmA 11
#define encoderArmB 12
#define motorArmA 3  // pulse to move up
#define sdA 4
#define motorArmB 5  // pulse to move down
#define sdB 6
#define motorClaw 2
#define encoderClawA 17
#define encoderClawB 16
#define PI 3.1415926535897932384626433832795
#define killSwitchPin 7
#define encoderRatio 80.1
SoftwareSerial SerialTFMini(0, 1);  // rx tx for tf mini lidar
 
TFMini tfmini;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;
IntervalTimer clk;
PWMServo claw;
 
uint8_t header = 0x59;                // used to tell if tf mini is reading garbage or not
volatile long int currentPosArm = 0;  // increase is CW, decrease is CCW
volatile long int currentPosClaw = 0;

double clawDist = 0;  // obj detection value when no obj
String status = "initializing";
String prepause;  // status before pause var
int motorApre;
int motorBpre;
int motorClawpre;
int motorApost;
int motorBpost;
int motorClawpost;
ArduinoQueue<int> q(100);  // queue for ui instrs
bool isPaused = false;
bool isKilled = false;
bool isParam = false;
//encoder consts
const int armPPR = 480;
const int clawPPR = 24;
const int readNum = 4;
const int gearRatio = 20;
volatile double clawAngle = 0; //((360*currentPosClaw)/(clawPPR*readNum*gearRatio));

// void gripperSwitch() {
//   claw.write(90);
//   delay(500);
//   claw.write(180);
//   delay(2000);
//   claw.write(90);
// }
 
void changeArmA() {
  if (digitalRead(encoderArmA) != digitalRead(encoderArmB)) {
    currentPosArm++;
  } else {
    currentPosArm--;
  }
}
 
void changeArmB() {
  if (digitalRead(encoderArmA) == digitalRead(encoderArmB)) {
    currentPosArm++;
  } else {
    currentPosArm--;
  }
}
 
void changeClawA() {
  if (digitalRead(encoderClawA) != digitalRead(encoderClawB)) {
    currentPosClaw++;
  } else {
    currentPosClaw--;
  }
}
 
void changeClawB() {
  if (digitalRead(encoderClawA) == digitalRead(encoderClawB)) {
    currentPosClaw++;
  } else {
    currentPosClaw--;
  }
}
 
void updateVL53() {
  lox.rangingTest(&measure, false);  // pass in 'true' to get debug data printout!
  if (measure.RangeStatus == 4) {
    Serial.println("out of range");
  }
}
 
void killswitch() {
  digitalWrite(killSwitchPin, LOW);
  clk.end();
  while (!q.isEmpty()) {
    q.dequeue();
  }
  while (isKilled) {
    Serial.println("Powerkilled@Powerkilled@Powerkilled@Powerkilled@Powerkilled");
    while (Serial.available() > 0) {
      int rec = Serial.parseInt();
      if (rec == 3) {
        Serial.println("Back Alive");
        isKilled = false;
        status = "idle";
        clk.begin(sendOutput, 50000);
        break;
      }
    }
  }
}
 
void actuateArmMotor(int freqA, int freqB) {
  status = "pwm A is " + String(freqA) + " pwm B is " + String(freqB);
  motorApre = freqA;
  motorBpre = freqB;
  analogWrite(motorArmA, freqA);
  analogWrite(motorArmB, freqB);
}
 
void pause() {
  clk.end();
  while (isPaused) {
    //updateVL53();
    while (Serial.available() > 0) {
      int rec = Serial.parseInt();
      if (rec == 4) {
        Serial.println("Unpause 4");
        isPaused = false;
        status = prepause;
        clk.begin(sendOutput, 50000);
        actuateArmMotor(motorApost, motorBpost);
        return;
      } else if (rec != 0) {
        Serial.print("enqueuing "); Serial.println(rec);
        q.enqueue(rec);
      }
    }
  }
}
 
void sendOutput() {
  //updateVL53();
  Serial.println(serialize());
  // Serial.print("Encoder: "); Serial.println(currentPosArm/encoderRatio);
  while (Serial.available() > 0) {
    int rec = Serial.parseInt();
    //Serial.print("Parsed "); Serial.println(rec);
    if (isParam == true) {
      return;
    }
    else if (rec == 3) {
      Serial.println("Dead");
      status = "killed";
      isKilled = true;
      killswitch();
    }
    else if (rec == 4) {
      Serial.println("PAUSE 4");
      prepause = status;
      status = "paused";
      isPaused = true;
      motorApost = motorApre;
      motorBpost = motorBpre;
      actuateArmMotor(0, 0);
      pause();
    } else if (rec != 0) {
      Serial.print("enqueuing ");
      Serial.println(rec);
      q.enqueue(rec);
    }
  }
}
 
void setup() {
  Serial.begin(115200);  //Initialize hardware serial port (serial debug port)
  Serial.setTimeout(50);
  pinMode(prox1, INPUT);
  pinMode(encoderArmA, INPUT);
  pinMode(encoderArmB, INPUT);
  pinMode(encoderClawA, INPUT);
  pinMode(encoderClawB, INPUT);
  pinMode(killSwitchPin, OUTPUT);
  pinMode(motorArmA, OUTPUT);
  pinMode(motorArmB, OUTPUT);
  pinMode(motorClaw, OUTPUT);
  pinMode(sdA, OUTPUT);
  pinMode(sdB, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(encoderArmA), changeArmA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderArmB), changeArmB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderClawA), changeClawA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderClawB), changeClawB, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(limit3), gripperSwitch, RISING);
  claw.attach(motorClaw);
 
  Serial.println("Initializing...");
  SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
  // if (!lox.begin()) {
  //   Serial.println(F("Failed to boot VL53L0X"));
  //   while(1);
  // }
  clk.begin(sendOutput, 50000);
  digitalWrite(killSwitchPin, HIGH);
  digitalWrite(sdA, HIGH);
  digitalWrite(sdB, HIGH);
}
 
void loop() {
  status = "idle";
  int instr = getInstruction();
  delay(50);
  if (instr != -1) {
    Serial.print("Got instruction: ");
    Serial.println(instr);
    doInstruction(instr);
    delay(50);
  }
}
 
String serialize() {
  return String(getTFMiniDist()) + "@" + String(digitalRead(prox1)) + "@c@" + String(currentPosArm/encoderRatio) + "@e@" + 
  String(digitalRead(limit1)) + "@" + String(digitalRead(limit2)) + "@" + String(digitalRead(limit3)) + "@" + status;
}
 
int getInstruction() {
  if (q.isEmpty()) {
    return -1;
  }
  Serial.print("Instruction: ");
  Serial.println(q.getHead());
  return q.dequeue();
}
 
void doInstruction(int instr) {
  Serial.print("Case ");
  Serial.println(instr);
  switch (instr) {
    case 1:  // start
      start();
      break;
    case 2:  // release
      release();
      break;
    case 5:  // reset
      reset();
      break;
    case 6:  // move Arm
      isParam = true;
      setArm(q.dequeue(), 0.5);
      break;
    case 7:  // move claw
      isParam = true;
      setClaw(q.dequeue());
      break;
    default:
      Serial.println("invalid input");
  }
  Serial.println("executed instruction");
}
 
void start() {
  status = "starting";
  int groundDist;
  reset();
  groundDist = getTFMiniDist();
  status = "ground distance: " + String(groundDist);
  setArm(groundDist * 0.90, .90);
  Serial.println("Moved down");
  actuateArmMotor(0, 0);
  delay(1000);
  actuateArmMotor(0, 127);
  while (digitalRead(prox1)) {  // 15 cm
  }
  actuateArmMotor(0, 0);
  Serial.println("Prox On");
  delay(5000);
  // // if (!detectObj()) {
  // //   Serial.print("Object not detected");
  // //   return;
  // // }
  // setClaw(90);
  setArm(15, 0.5);
  Serial.println("Moved up");
 
  // if (!detectObj()) {
  //   Serial.print("Object not detected");
  //   return;
  // }
  Serial.println("Done");
}
 
void release() {
  status = "releasing";
  resetClaw();
}
 
void reset() {
  status = "resetting";
  actuateArmMotor(245, 0);  // move claw up
  while (!digitalRead(limit2)) {
  }
  actuateArmMotor(0, 0);
  currentPosArm = 0;
  // setClaw(90);
  // clawDist = getAvgVl53L0X(15);
  // release();
  return;
}

void resetClaw() {
  claw.write(65);
  currentPosClaw = 0;
  while (!digitalRead(limit3)) {
  }
  claw.write(90);
  delay(500);
  claw.write(180);
  delay(750);
  claw.write(90);
  return;
}
 
void setClaw(int angle) {
  int direction = 1;
  status = "moving claw to " + String(angle);
  if (clawAngle < angle) {
    direction = 0; // opens gripper
  } else if (clawAngle > angle) {
    direction = 180; // closes gripper
  }
  while ((direction == 180 && (clawAngle > angle)) || (direction == 0 && clawAngle < angle)) {
    claw.write(direction);
  }
  claw.write(90);
  isParam = false;
  return;
}
 
void setArm(double distance, double maxSpeedPercent) {
  int setSpeed = int(245 * maxSpeedPercent);
  // Serial.print("distance: "); Serial.println(distance);
  // Serial.print("encoder: "); Serial.println(currentPosArm / encoderRatio);
  if ((-1 * (currentPosArm / encoderRatio)) < (distance * 10 - 1)) {
    actuateArmMotor(0, setSpeed);
    while ((-1 * (currentPosArm / encoderRatio)) < (distance * 10 - 1)) {
    }
    actuateArmMotor(0, 0);
    isParam = false;
    return;
  } else {
    actuateArmMotor(setSpeed+100, 0);
    while ((-1 * (currentPosArm / encoderRatio)) > (distance * 10)) {
    }
    actuateArmMotor(0, 0);
    isParam = false;
    return;
  }
}
 
void rampArm(int distance) {
  setArm(distance * 0.1, 0.5);
  setArm(distance * 0.8, 1);
  setArm(distance * 0.1, 0.5);
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
 
  lox.rangingTest(&measure, false);  // pass in 'true' to get debug data printout
  if (measure.RangeStatus != 4) {    // phase failures have incorrect data
    return measure.RangeMilliMeter;
  } else {
    return -1;
  }
}
 
int getAvgVl53L0X(int numAvg) {
  int ct = 0;
  int dist;
  int sum = 0;
 
  while (ct <= numAvg) {
    dist = getVL53L0XDist();
    if (dist != -1) {
      sum = sum + dist;
      ct = ct + 1;
    }
  }
  return (sum / numAvg);
}
 
bool detectObj() {
  status = "detecting object";
  if (abs(clawDist - getAvgVl53L0X(15)) >= 5) {  // arbitrary 5
    return true;
  }
  return false;
}