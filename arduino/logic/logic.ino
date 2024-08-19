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
Servo claw;
Statistic stats;
 
uint8_t header = 0x59;                // used to tell if tf mini is reading garbage or not
volatile long int currentPosArm = 0;  // increase is CW, decrease is CCW
volatile long int currentPosClaw = 0;
 
int clawDist = 0;  // obj detection value when no obj
double clawDev;
String status = "initializing";
String prepause;  // status before pause var
int motorApre;
int motorBpre;
int motorClawpre;
int motorApost;
int motorBpost;
int motorClawpost;
int lidarVal = "";
ArduinoQueue<int> q(100);  // queue for ui instrs
bool isPaused = false;
bool isKilled = false;
bool isParam = false;
//encoder consts
const int armPPR = 480;
const int clawPPR = 24;
const int readNum = 4;
const double clawRatio = 0.375;
 
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
        clk.begin(sendOutput, 75000);
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
  status = "paused";
  while (isPaused) {
    Serial.println(serialize());
    while (Serial.available() > 0) {
      int rec = Serial.parseInt();
      if (rec == 4) {
        Serial.println("Unpause 4");
        isPaused = false;
        clk.begin(sendOutput, 75000);
        actuateArmMotor(motorApost, motorBpost);
        status = prepause;
        return;
      } else if (rec != 0) {
        // Serial.print("enqueuing ");
        // Serial.println(rec);
        q.enqueue(rec);
      }
    }
  }
}
 
void sendOutput() {
  Serial.println(serialize());
  // Serial.println(String(q.getHead()));
  // Serial.print("Encoder: "); Serial.println(currentPosArm/encoderRatio);
  while (Serial.available() > 0) {
    int rec = Serial.parseInt();
    //Serial.print("Parsed "); Serial.println(rec);
    if (isParam == true) {
      return;
    } else if (rec == 3) {
      Serial.println("Dead");
      status = "killed";
      isKilled = true;
      killswitch();
    } else if (rec == 4) {
      Serial.println("PAUSE 4");
      prepause = status;
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
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(encoderArmA), changeArmA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderArmB), changeArmB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderClawA), changeClawA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderClawB), changeClawB, CHANGE);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("initializing");
  SerialTFMini.begin(115200);   //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);  //Initialize the TF Mini sensor
  digitalWrite(killSwitchPin, HIGH);
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  clk.begin(sendOutput, 75000);
  digitalWrite(sdA, HIGH);
  digitalWrite(sdB, HIGH);
  Serial.println("Initialized");
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
  return String(getTFMiniDist()) + "@" + String(digitalRead(prox1)) + "@" + String(lidarVal) + "@" + String(digitalRead(limit1)) + "@" + String(digitalRead(limit2)) + "@" + String(digitalRead(limit3)) + "@" + String(clawDist) + "@b@c@" + String(currentPosArm / encoderRatio) + "@" + String(currentPosClaw*clawRatio) + "@" + status;
  // return "a@b@c@d@e@f@g@" + status;
}
 
int getInstruction() {
  if (q.isEmpty()) {
    status = "idle";
    return -1;
  }
  status = String(q.getHead());
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
      setArm(q.dequeue(), 0.5);
      // setArm();
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
  // groundDist = getTFMiniDist();
  // status = "ground distance: " + String(groundDist);
  // setArm(groundDist * 0.90, .90);
  // Serial.println("Moved down");
  // actuateArmMotor(0, 0);
  // delay(1000);
  // actuateArmMotor(0, 127);
  // while (digitalRead(prox1)) {  // 15 cm
  // }
  // actuateArmMotor(0, 0);
  // Serial.println("Prox On");
  // delay(5000);
  // if (!detectObj()) {
  //   Serial.print("Object not detected");
  //   return;
  // }
  // closeClaw();
  // // setArm(15, 0.5);
  // Serial.println("Moved up");
 
  // if (!detectObj()) {
  //   Serial.print("Object not detected");
  //   return;
  // }
  // Serial.println("Done");
  delay(500);
  actuateArmMotor(0, 127);
  while (digitalRead(prox1)) {  // 15 cm
  }
  actuateArmMotor(0, 0);
  closeClaw();
  if (!detectObj()) {
    actuateArmMotor(245, 0);  // move claw up
    status = "Object not detected";
    while (!digitalRead(limit2)) {
    }
    actuateArmMotor(0, 0);
    return;
  }
  else {
    actuateArmMotor(245, 0);  // move claw up
    delay(5000);
    if (!detectObj()) {
      status = "Object slipped out";
      while (!digitalRead(limit2)) {
      }
      actuateArmMotor(0, 0);
      return;
    }
    else {
      status = "object still in";
      while (!digitalRead(limit2)) {
      }
      actuateArmMotor(0, 0);
      return;
    }
  }
}
 
void release() {
  status = "releasing";
  resetClaw();
  return;
}
 
void reset() {
  status = "resetting";
  actuateArmMotor(240, 0);  // move claw up
  while (!digitalRead(limit2)) {
  }
  actuateArmMotor(0, 0);
  currentPosArm = 0;
  closeClaw();
  clawDist = getVL53L0XStats(10);
  status = String(clawDev);
  delay(1000);
  release();
  return;
}
 
void resetClaw() {
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
  claw.attach(motorClaw);
  claw.write(180);
  delay(750);
  claw.write(90);
  claw.write(120);
  delay(6000);
  claw.write(90);
  status = "clawclosed";
  claw.detach();
  return;
}
 
void setClaw(int angle) {
  status = "closingclaw";
  // isParam = false;
  // int direction = 1;
  // status = "moving claw to " + String(angle);
  // if (currentPosClaw*clawRatio > angle) {
  //   direction = 60;  // opens gripper
  // } else if (currentPosClaw*clawRatio < angle) {
  //   direction = 120;  // closes gripper
  // }
  // while ((direction == 120 && (currentPosClaw*clawRatio < angle)) || (direction == 60 && currentPosClaw*clawRatio > angle)) {
  //   claw.write(direction);
  // }
  // claw.write(90);
  // return;
  closeClaw();
  return;
}
 
void setArm(double distance, double maxSpeedPercent) {
  // isParam = false;
  // int setSpeed = int(245 * maxSpeedPercent);
  // // Serial.print("distance: "); Serial.println(distance);
  // // Serial.print("encoder: "); Serial.println(currentPosArm / encoderRatio);
  // if ((-1 * (currentPosArm / encoderRatio)) < (distance * 10 - 1)) {
  //   actuateArmMotor(0, setSpeed);
  //   status = "moving arm";
  //   while ((-1 * (currentPosArm / encoderRatio)) < (distance * 10 - 1)) {
  //   }
  //   status = "arm stopped";
  //   actuateArmMotor(0, 0);
  //   return;
  // } else {
  //   actuateArmMotor(setSpeed, 0);
  //   while ((-1 * (currentPosArm / encoderRatio)) > (distance * 10)) {
  //   }
  //   actuateArmMotor(0, 0);
  //   return;
  // }
  status = "moving claw up";
  actuateArmMotor(245, 0);  // move claw up
  while (!digitalRead(limit2)) {
  }
  actuateArmMotor(0, 0);
  delay(500);
  status = "moving claw down";
  actuateArmMotor(0, 245);
  while (digitalRead(prox1)) {  // 15 cm
  }
  actuateArmMotor(0, 0);
  return;
}
 
// void rampArm(int distance) {
//   setArm(distance * 0.1, 0.5);
//   setArm(distance * 0.8, 1);
//   setArm(distance * 0.1, 0.5);
// }
 
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
  return measure.RangeMilliMeter;
}
 
int getVL53L0XStats(int numAvg) {
  int ct = 0;
  status = "getting avg";
  stats.clear();
  while (ct <= numAvg) {
    stats.add(lidarVal);
    ct = ct + 1;
    delay(50);
  }
  status = String(stats.average());
  clawDev = stats.pop_stdev();
  return stats.average();
}
 
bool detectObj() {
  status = "detecting object";
  //double clawThreshold = ((clawDist + clawDev) * 1.1) - clawDist;
  double clawThreshold = clawDev;
  if (abs(getVL53L0XStats(10)-clawDist) > clawThreshold) {
    return true;
  }
  return false;
}