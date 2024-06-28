const int fast = up = 1;
const int slow = down = 0;

void setup()
{
  Serial.begin(115200);       //Initialize hardware serial port (serial debug port)
  while (!Serial);            // wait for serial port to connect. Needed for native USB port only
  Serial.println ("Initializing...");
}
 
void loop()
{
  int instr;
  instr = getInstruction();
  doInstruction(instr);
}

int getInstruction() {
  int instr = 0;
  // get instruction from UI plugged into Teensy
  return instr;
}

void doInstruction(int instr) {
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
  while (getProxDist() > 100) {       // 10 cm, but in mm
    actuateArmMotor(down, slow);
  }
  if (getProxObj() > 100) {            // arbitrary
    Serial.print("Uh oh");
    break;
  }
  closeClaw();
  while (getLidarDist() <= 150) {
    actuateArmMotor(up, fast);
  }
  if (getProxObj() > 100) {
    Serial.print("Uh oh");
    break;
  }
}

void release() {
  openClaw();
}

void reset() {
  stopMotion();
  openClaw();
  while (!isTopLimit()) {
    raiseClaw();
  }
}

void closeClaw() {
  // motor stuff
}

void openClaw() {
  // motor stuff
}

void lowerClaw() {
  // actuate arm motor method by arbitrary amount
}

void raiseClaw() {
  // actuate arm motor method by arbitrary amount
}

void stopMotion() {
  // motor stuff
}

bool isTopLimit() {
  // yada yada 
}

void actuateArmMotor(int dir, int speed) {
  // yada yada
}

int getLidarDist() {
  // yada yada
}

int getProxDist() {
  // yada yada
}