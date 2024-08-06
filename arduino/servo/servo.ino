#include <Servo.h>

#define encoderA 2
#define encoderB 3
#define servoPin 9
#define PI 3.1415926535897932384626433832795

Servo myservo;  // create Servo object to control a servo

const int armPPR = 446; // tested with francesca
const int gearRatio = ((2*PI)/(20))*38;
const int readNum = 4;
volatile double currentPos = 0; // increase is CW, decrease is CCW
int direction = 1; // UP = 1, DOWN = 0

void setup() {
  Serial.begin(115200);
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderA), changeA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB), changeB, CHANGE);
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object , or any other PWM Teensy pin
}

void loop() {
  // Serial.println("Idle");
  // if (Serial.available() > 0) {
  //   int rec = Serial.parseInt();
  //   Serial.println("Move to " + String(rec));
  //   setClaw(rec);
  // }
  Serial.print("currentPos: "); Serial.println(currentPos);
  Serial.print("dist: "); Serial.println((currentPos/(readNum*armPPR))*gearRatio);
  //Serial.print("rotational angle: "); Serial.println((360*currentPos)/(PPR*readNum*gearRatio));
  delay(100);
}

// void setClaw(int angle) {
//   int direction = 1;
//   if (currentPos/readNum < angle) {
//     direction = 180;
//   }
//   else if (currentPos/readNum > angle) {
//     direction = 0;
//   }
//   while ((direction == 180 && (currentPos/readNum) < angle) || 
//          (direction == 0 && (currentPos/readNum) > angle)) {
//     myservo.write(direction);
//   }
//   return;
// }

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