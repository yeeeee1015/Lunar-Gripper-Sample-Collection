#define encoderA 2
#define encoderB 3

const int PPR = 24;
const int gearRatio = 1;
const int readNum = 4;
volatile long int currentPos = 0; // increase is CW, decrease is CCW

void setup() {
  Serial.begin(115200);
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderA), changeA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB), changeB, CHANGE);
}

void loop() {
  Serial.print("currentPos: "); Serial.println(currentPos/readNum);
  Serial.print("rotational angle: "); Serial.println((360*currentPos)/(PPR*readNum*gearRatio));
  delay(500);
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
