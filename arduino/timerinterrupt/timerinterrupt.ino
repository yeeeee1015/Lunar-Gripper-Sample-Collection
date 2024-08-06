#include <SoftwareSerial.h>
#include <TFMini.h>
#include "Adafruit_VL53L0X.h"
#include "IntervalTimer.h"

uint8_t header = 0x59;
TFMini tfmini;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
SoftwareSerial SerialTFMini(7, 8);

int TFMiniDist = 0;
int distance = 0;
int strength = 0;
VL53L0X_RangingMeasurementData_t measure;
IntervalTimer clk;

String status = "temp";

void updateLiDARs() {
  getTFminiData(&distance, &strength);
  distance = 0;
  while (!distance) {
    getTFminiData(&distance, &strength);
  }
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus == 4) {
    Serial.println("out of range");
  }
}

void sendOutput() {
  Serial.println("updating lidar");
  updateLiDARs();
  Serial.println(serialize(TFMiniDist , measure.RangeMilliMeter));
}

void setup()
{
  Serial.begin(115200);       //Initialize hardware serial port (serial debug port)
  
  VL53L0X_RangingMeasurementData_t measure;

  while (!Serial);            // wait for serial port to connect. Needed for native USB port only
  
  Serial.println ("Initializing...");
  SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  clk.begin(sendOutput, 15000);
}
 
void loop()
{
  Serial.println("Getting instruction");
  int instr = getInstruction();
  Serial.print("Got instruction: "); Serial.println(instr);
  delay(500);
  doInstruction(instr);
}

String serialize(int TFMiniDistint,int VL53L0X) {
  return String(TFMiniDist) + "@" + VL53L0X + "@" + status;
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
        TFMiniDist = *distance;
      }
      i = 0;
    }
    else {
      i++;
    }
  }
}

int getInstruction() {
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
    case 1:       // start
      start();
      break;
    case 2:       // release
      release();
      break;
    case 3:       // kill switch
      killswitch();
      break;
    case 4:
      pause();
      break;
    case 5:       // reset
      reset();
      break;
    case 6:       // move Arm
      setArm(0);
      break;
    case 7:       // move claw
      setClaw(0);
      break;
    default:
      Serial.println("invalid input");
  }
  Serial.println("executed instruction");
}

void killswitch() {
  // somehow kill power
  return;
}

void pause() {
  // interrupt
  return;
}

void start() {
  return;
}

void release() {
  return;
}

void reset() {
  return;
}

void setClaw(int angle) {
  // motor stuff
  return;
}

void setArm(int distance) {
  // actuate arm motor method by arbitrary amount
  return;
}

bool isTopLimit() {
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