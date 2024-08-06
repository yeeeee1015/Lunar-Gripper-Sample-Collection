#define limit1 10
#define motorA 3
#define motorB 5
#define sdA 4
#define sdB 6
#define killSwitch 7
#define encoderA 11
#define encoderB 12
#define PI 3.1415926535897932384626433832795
#define encoderRatio 160.83
#include <SoftwareSerial.h>
#include <TFMini.h>
#include <Adafruit_VL53L0X.h>
#include "IntervalTimer.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
SoftwareSerial SerialTFMini(0, 1); // rx tx for tf mini lidar
TFMini tfmini;
uint8_t header = 0x59;
const int armPPR = 446; // tested with francesca
const int gearRatio = ((2*PI)/(20))*38;
const int readNum = 4;
volatile double currentPos = 0; // increase is CW, decrease is CCW
IntervalTimer clk;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);
  Serial.println ("Initializing...");
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(sdA, OUTPUT);
  pinMode(sdB, OUTPUT);
  pinMode(killSwitch, OUTPUT);
  digitalWrite(sdA, HIGH);
  digitalWrite(sdB, HIGH);
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, LOW);
  digitalWrite(killSwitch, HIGH);
  pinMode(limit1, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderA), changeA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB), changeB, CHANGE);
  // if (!lox.begin()) {
  //   Serial.println(F("Failed to boot VL53L0X"));
  //   while(1);
  // }
  clk.begin(sendOutput, 50000);
  Serial.println("setup done");
}

void actuateArmMotor(int freqA, int freqB) {
  analogWrite(motorA, freqA);
  analogWrite(motorB, freqB);
}

void setArm(double distance) {
  // Serial.print("distance: "); Serial.println(distance);
  if ((-1*(currentPos/encoderRatio)) < (distance*10)) {
    actuateArmMotor(0, 200);
    while ((-1*(currentPos/encoderRatio)) < (distance*10)) {
    }
    actuateArmMotor(0,0);
    Serial.println("Done: ");
    return;
  }
  else {
    while ((-1*(currentPos/encoderRatio)) > (distance*10)) {
      actuateArmMotor(200, 0);
    }
    actuateArmMotor(0,0);
    return;
  }
}

void loop() {
  // int groundDist = getAvgVl53L0X(30);
  // Serial.print("ground distance: "); Serial.println(groundDist);
  // while ((-1 * ((currentPos/(readNum*armPPR))*gearRatio)) < (150)) {
  //     analogWrite(motorB, 245);
  //   }
  // while (digitalRead(limit1) == LOW) {
  //   analogWrite(motorB, 240);
  //   Serial.print("currentPos: "); Serial.println(currentPos);
  //   Serial.print("dist: "); Serial.println(currentPos/160.83);
  // }
  // analogWrite(motorB, 0);
  // Serial.print("currentPos: "); Serial.println(currentPos);
  // Serial.print("dist: "); Serial.println(currentPos/160.83);
  // double thing = (currentPos/(readNum*armPPR))*gearRatio;
  // while ((thing - ((currentPos/(readNum*armPPR))*gearRatio)) > (-200)) {
  //   Serial.println((currentPos/(readNum*armPPR))*gearRatio);
  //   analogWrite(motorA, 240);
  // }
  // analogWrite(motorA, 0);
  // Serial.println(currentPos/160.83);


  // while (Serial.available() > 0) {
  //   int rec = Serial.parseInt();
  //   Serial.println("rec");
  //   setArm(rec);
  // }
  analogWrite(motorA, 200);
}

String serialize() {
  return "a@b@c@d@e";
}

void sendOutput() {
  Serial.println(serialize());
  Serial.print("encoder: "); Serial.println(currentPos/encoderRatio);
  while (Serial.available() > 0) {
    int rec = Serial.parseInt();
    if (rec == 4) {
      Serial.println("PAUSE 4");
    }
    else if (rec != 0) {
    }
  }
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

void changeA() {
  if (digitalRead(encoderA) != digitalRead(encoderB)) {
    currentPos++;
  }
  else {
    currentPos--;
  }
}

void changeB() {
  if (digitalRead(encoderA) == digitalRead(encoderB)) {
    currentPos++;
  }
  else {
    currentPos--;
  }
}