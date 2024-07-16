#include <SoftwareSerial.h>
#include <TFMini.h>
#include "Adafruit_VL53L0X.h"
#define proxPin 4
#define encoderArmA 2
#define encoderArmB 3
#define encoderClawA 18 
#define encoderClawB 19

uint8_t header = 0x59;
TFMini tfmini;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

SoftwareSerial SerialTFMini(10, 11);

volatile byte proxOn = LOW;

const int PPR = 24;
const int gearRatio = 1;
const int readNum = 4;
volatile long int currentPosArm = 0; // increase is CW, decrease is CCW
volatile long int currentPosClaw = 0; 

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
  while (!Serial.available()) {

  }
  int thing = Serial.println(Serial.parseInt());
  Serial.print("thing: "); Serial.println(thing);
  return thing;
}

void doInstruction(int instr) {
  Serial.print("Case "); Serial.println(instr);
  switch (instr) {
    case 0:       // start
      start();
    case 1:       // release
      release();
    case 2:       // kill switch
      killswitch();
    case 3:       // reset
      reset();
    case 4:       // close claw
      closeClaw();
    case 5:       // open claw
      openClaw();
    case 6:       // lower claw
      lowerClaw();
    case 7:       // raise claw
      raiseClaw();
    default:
      Serial.println("bruh");
  }
}

void killswitch() {
  // somehow kill power
}

void start() {
  int groundDist;

  reset();
  groundDist = getTFMiniDist();

  while (currentPosArm <= groundDist * 0.90) {     // arbitrary 
    actuateArmMotor();
  }
  Serial.println("Moved down");

  while (!proxOn) {       // 15 cm
    actuateArmMotor();
  }
  Serial.println("Prox On");

  if (!detectObj()) {            // arbitrary
    Serial.print("Uh oh");
    return;
  }
  closeClaw();
  while (getTFMiniDist() <= 150) {
    actuateArmMotor();
  }
  Serial.println("Moved up");

  if (!detectObj()) {
    Serial.print("Uh oh");
    return;
  }

  Serial.println("Done");
}

void release() {
  openClaw();
}

void reset() {
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
  // yada yada 
  return;
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
  return true;
}

void blink() {
  proxOn = !proxOn;
}

void changeArmA() {
  if (digitalRead(encoderArmA) != digitalRead(encoderArmB)) {
    currentPos1++;
  }
  else {
    currentPos1--;
  }
}

void changeArmB() {
  if (digitalRead(encoderArmA) == digitalRead(encoderArmB)) {
    currentPos1++;
  }
  else {
    currentPos1--;
  }
}

void changeClawA() {
  if (digitalRead(encoderClawA) != digitalRead(encoderClawB)) {
    currentPos2++;
  }
  else {
    currentPos2--;
  }
}

void changeClawB() {
  if (digitalRead(encoderClawA) == digitalRead(encoderClawB)) {
    currentPos2++;
  }
  else {
    currentPos2--;
  }
}