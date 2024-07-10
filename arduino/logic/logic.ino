#include <SoftwareSerial.h>
#include <TFMini.h>

uint8_t header = 0x59;
TFMini tfmini;

SoftwareSerial SerialTFMini(10, 11);

const int fast = 1;
const int up = 1;
const int slow = 0;
const int down = 0;

const byte proxPin = 2;
volatile byte proxOn = LOW;

void setup()
{
  attachInterrupt(digitalPinToInterrupt(proxPin), blink, CHANGE);
  Serial.begin(115200);       //Initialize hardware serial port (serial debug port)
  pinMode(proxPin, INPUT);
  while (!Serial);            // wait for serial port to connect. Needed for native USB port only
  Serial.println ("Initializing...");
  SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
}
 
void loop()
{
  int instr = getInstruction();
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
  int instr = 0;
  // get instruction from UI plugged into Teensy
  return instr;
}

void doInstruction(int instr) {
  switch (instr) {
    case 0:       // start
      Serial.println("Case 0:");
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
  }
}

void killswitch() {
  // somehow kill power
}

void start() {
  int groundDist;

  reset();
  while (getLidarDist() >= 150) {     // arbitrary 
    actuateArmMotor(down, fast);
  }
  Serial.println("Moved down");

  while (!proxOn) {       // 10 cm, but in mm
    actuateArmMotor(down, slow);
  }
  Serial.println("Prox On");

  if (!detectObj()) {            // arbitrary
    Serial.print("Uh oh");
    return;
  }
  closeClaw();
  while (getLidarDist() <= 150) {
    actuateArmMotor(up, fast);
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

void actuateArmMotor(int dir, int speed) {
  // yada yada
  return;
}

int getLidarDist() {
  int distance = 0;
  int strength = 0;
  while (!distance) {
    getTFminiData(&distance, &strength);
  }
  Serial.println(distance);
  return distance;
}

bool detectObj() {
  return true;
}

void blink() {
  proxOn = !proxOn;
}