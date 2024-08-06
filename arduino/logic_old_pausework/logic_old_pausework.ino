// imports
#include <SoftwareSerial.h>
#include <TFMini.h>
#include "Adafruit_VL53L0X.h"
#include <ezButton.h>
#include "IntervalTimer.h"
#include <ArduinoQueue.h>
#include <Servo.h>
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

uint8_t header = 0x59;                // used to tell if tf mini is reading garbage or not
volatile long int currentPosArm = 0;  // increase is CW, decrease is CCW
volatile long int currentPosClaw = 0;
volatile int topLimitState = 0;
volatile int bottomLimitState = 0;
volatile int gripperLimitState = 0;
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
//encoder consts
const int armPPR = 480;
const int clawPPR = 24;
const int gearRatio = 684;
const int readNum = 4;

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

void changeLimit1() {
  topLimitState = !topLimitState;
}

void changeLimit2() {
  bottomLimitState = !bottomLimitState;
}

void changeLimit3() {
  gripperLimitState = !gripperLimitState;
}

void updateVL53() {
  lox.rangingTest(&measure, false);  // pass in 'true' to get debug data printout!
  if (measure.RangeStatus == 4) {
    Serial.println("out of range");
  }
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
        // actuateArmMotor(motorApost, motorBpost);
        break;
      } else if (rec != 0) {
        Serial.print("enqueuing "); Serial.println(rec);
        q.enqueue(rec);
      }
    }
  }
}

void sendOutput() {
  // updateVL53();
  Serial.println(serialize());
  // Serial.print("Encoder: "); Serial.println(currentPosArm/encoderRatio);
  while (Serial.available() > 0) {
    int rec = Serial.parseInt();
    //Serial.print("Parsed "); Serial.println(rec);
    if (rec == 3) {
      Serial.println("Dead");
      status = "killed";
      isKilled = true;
      killswitch();
    } 
    if (rec == 4) {
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

void setup()
{
  Serial.begin(115200);       //Initialize hardware serial port (serial debug port)
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
  attachInterrupt(digitalPinToInterrupt(limit1), changeLimit1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(limit2), changeLimit2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(limit3), changeLimit3, CHANGE);
  
  Serial.println("Initializing...");
  // SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  // tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
  // if (!lox.begin()) {
  //   Serial.println(F("Failed to boot VL53L0X"));
  //   while(1);
  // }
  clk.begin(sendOutput, 50000);
  digitalWrite(killSwitchPin, HIGH);
  digitalWrite(sdA, HIGH);
  digitalWrite(sdB, HIGH);
}
 
void loop()
{
  int instr = getInstruction();
  delay(100);
  if (instr != -1) {
    Serial.print("Got instruction: "); Serial.println(instr);
    doInstruction(instr);
    delay(50);
  }
  else {
    Serial.println("Waiting for instruction");
  }
}

String serialize() {
  // return String(getTFMiniDist()) + "@" + String(digitalRead(prox1)) + "@" + String(analogRead(press1)) + "@" + measure.RangeMilliMeter + "@" + digitalRead(limit1) + "@" + status;
  return status;
}

void getTFminiData(int* distance, int* strength)
{
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static double rx[9];
  if (SerialTFMini.available())
  {
    rx[i] = SerialTFMini.read();
    if (rx[0] != header) {
      i = 0;
    }
    else if (i == 1 && rx[1] != header) {
      i = 0;
    }
    else if (i == 8) {
      for (j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if (rx[8] == (checksum % 256)) {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
      }
      i = 0;
    }
    else {
      i++;
    }
  }
}

int getInstruction() {
  if (q.isEmpty()){
    return -1;
  }
  Serial.print("Instruction: "); Serial.println(q.getHead());
  return q.dequeue();
}

void doInstruction(int instr) {
  Serial.print("Case "); Serial.println(instr);
  switch (instr) {
    case 1:       // start
      status = "1";
      start();
      break;
    case 2:       // release
      status = "2";
      release();
      break;
    case 3:       // kill switch
      status = "3";
      killswitch();
      break;
    case 4:
      status = "4";
      pause();
      break;
    case 5:       // reset
    status = "5";
      reset();
      break;
    case 6:       // move Arm
    status = "6";
      setArm(0);
      break;
    case 7:       // move claw
    status = "7";
      setClaw(0);
      break;
    default:
      Serial.println("invalid input");
  }
  Serial.println("executed instruction");
}

void killswitch() {
  digitalWrite(killSwitchPin, LOW);
  return;
}

void start() {
  int groundDist;
  reset();
  Serial.println("Reset");
  groundDist = getTFMiniDist();
  while (currentPosArm <= groundDist * 0.90) {     // arbitrary 
    actuateArmMotor(0, 255);
  }
  Serial.println("Moved down");
  while (!prox1) {       // 15 cm
    actuateArmMotor(0, 127);
  }
  Serial.println("Prox On");
  if (!detectObj()) {
    Serial.print("Object not detected");
    return;
  }
  setClaw(0);
  int bottom = currentPosArm;
  while (currentPosArm >= bottom - 15) { // arbitrary 15
    actuateArmMotor(255, 0);
  }
  Serial.println("Moved up");

  if (!detectObj()) {
    Serial.print("Object not detected");
    return;
  }
  Serial.println("Done");
}

void release() {
  setClaw(90);
}

void reset() {
  while (!limitOne()) {
    actuateArmMotor(127, 0); // move claw up
  }
  setClaw(0);
  clawDist = getAvgVl53L0X(15);
  release();
  return;
}

void setClaw(int angle) {
  delay(3000);
  return;
}

void setArm(int distance) {
  delay(3000);
  return;
}

bool limitOne() {
  return digitalRead(limit1);
}

void actuateArmMotor(int freqA, int freqB) {
  status = "pwm A is " + String(freqA) + " pwm B is " + String(freqB);
  motorApre = freqA;
  motorBpre = freqB;
  analogWrite(motorArmA, freqA);
  analogWrite(motorArmB, freqB);
}

void actuateClawMotor(int freq) {
  analogWrite(motorClaw, freq);
  return;
}

int getTFMiniDist() {
  int distance = 0;
  int strength = 0;
  for (int i=0; i<3; i++) {       // need a for loop to incr amount of measurements lidar takes and remove lag
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

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    return measure.RangeMilliMeter;
  }
  else {
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
  return (sum/numAvg);
}

bool detectObj() {
  if (abs(clawDist - getAvgVl53L0X(15)) >= 5) { // arbitrary 5
    return true;
  }
  //return false;
  return true;
}
