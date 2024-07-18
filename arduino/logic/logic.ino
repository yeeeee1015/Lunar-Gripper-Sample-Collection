#include <SoftwareSerial.h>
#include <TFMini.h>
#include "Adafruit_VL53L0X.h"
#include <ezButton.h>
#define proxPin 4
#define encoderArmA 2
#define encoderArmB 3
#define encoderClawA 18 
#define encoderClawB 19


uint8_t header = 0x59;
TFMini tfmini;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
ezButton limitSwitch(21);
SoftwareSerial SerialTFMini(10, 11);

volatile byte proxOn = LOW;

const int PPR = 24;
const int gearRatio = 1;
const int readNum = 4;
volatile long int currentPosArm = 0; // increase is CW, decrease is CCW
volatile long int currentPosClaw = 0;
double clawDist = 0;

void blink() {
  proxOn = !proxOn;
}

void changeArmA() {
  if (digitalRead(encoderArmA) != digitalRead(encoderArmB)) {
    currentPosArm++;
  }
  else {
    currentPosArm--;
  }
}

void changeArmB() {
  if (digitalRead(encoderArmA) == digitalRead(encoderArmB)) {
    currentPosArm++;
  }
  else {
    currentPosArm--;
  }
}

void changeClawA() {
  if (digitalRead(encoderClawA) != digitalRead(encoderClawB)) {
    currentPosClaw++;
  }
  else {
    currentPosClaw--;
  }
}

void changeClawB() {
  if (digitalRead(encoderClawA) == digitalRead(encoderClawB)) {
    currentPosClaw++;
  }
  else {
    currentPosClaw--;
  }
}

void setup()
{
  Serial.begin(115200);       //Initialize hardware serial port (serial debug port)

  pinMode(proxPin, INPUT);
  pinMode(encoderArmA, INPUT);
  pinMode(encoderArmB, INPUT);
  pinMode(encoderClawA, INPUT);
  pinMode(encoderClawB, INPUT);
  attachInterrupt(digitalPinToInterrupt(proxPin), blink, CHANGE); // init interrupt for prox
  attachInterrupt(digitalPinToInterrupt(encoderArmA), changeArmA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderArmB), changeArmB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderClawA), changeClawA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderClawB), changeClawB, CHANGE);
  limitSwitch.setDebounceTime(100);
  while (!Serial);            // wait for serial port to connect. Needed for native USB port only
  
  Serial.println ("Initializing...");
  SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
}
 
void loop()
{
  delay(1000);
  Serial.println("Getting instruction");
  int instr = getInstruction();
  Serial.print("Got instruction: "); Serial.println(instr);
  delay(500);
  doInstruction(instr);
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
  delay(1000);
  Serial.println("Type please");
  while (Serial.available() <= 1) {
  }
  Serial.print("Number of bytes available: "); Serial.println(Serial.available());
  int thing = (Serial.parseInt());
  Serial.print("thing: "); Serial.println(thing);
  return thing;
}

void doInstruction(int instr) {
  Serial.print("Case "); Serial.println(instr);
  switch (instr) {
    case 0:       // start
      start();
      break;
    case 1:       // release
      release();
      break;
    case 2:       // kill switch
      killswitch();
      break;
    case 3:       // reset
      reset();
      break;
    case 4:       // close claw
      closeClaw();
      break;
    case 5:       // open claw
      openClaw();
      break;
    case 6:       // lower claw
      lowerClaw();
      break;
    case 7:       // raise claw
      raiseClaw();
      break;
    default:
      Serial.println("bruh");
  }
  Serial.println("executed instruction");
}

void killswitch() {
  // somehow kill power
}

void start() {
  int groundDist;
  reset();
  Serial.println("Reset");
  groundDist = getTFMiniDist();
  while (currentPosArm <= groundDist * 0.90) {     // arbitrary 
    actuateArmMotor();
  }
  Serial.println("Moved down");
  while (!proxOn) {       // 15 cm
    actuateArmMotor();
  }
  Serial.println("Prox On");
  if (!detectObj()) {
    Serial.print("Object not detected");
    return;
  }
  closeClaw();
  int bottom = currentPosArm;
  while (currentPosArm >= bottom - 15) { // arbitrary 15
    actuateArmMotor();
  }
  Serial.println("Moved up");

  if (!detectObj()) {
    Serial.print("Object not detected");
    return;
  }
  Serial.println("Done");
}

void release() {
  openClaw();
}

void reset() {
  while (!isTopLimit()) {
    actuateArmMotor(); // move claw up
  }
  closeClaw();
  clawDist = getAvgVl53L0X(15);
  openClaw();
  return;
}

void closeClaw() {
  // motor stuff
  return;
}

void openClaw() {
  // motor stuff
  return;
}

void lowerClaw() {
  // actuate arm motor method by arbitrary amount
  return;
}

void raiseClaw() {
  // actuate arm motor method by arbitrary amount
  return;
}

void stopMotion() {
  // motor stuff
  return;
}

bool isTopLimit() {
  limitSwitch.loop();
  return !limitSwitch.getState();
}

void actuateArmMotor() {
  // yada yada
  return;
}

int getTFMiniDist() {
  int distance = 0;
  int strength = 0;
  while (!distance) {
    getTFminiData(&distance, &strength);
  }
  Serial.println(distance);
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
  //return false;
  return true;
}
}