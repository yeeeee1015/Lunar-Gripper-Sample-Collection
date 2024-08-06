#define limit1 2

volatile bool topLimitState = false;

void setup() {
  Serial.begin(115200);
  pinMode(limit1, INPUT);
  attachInterrupt(digitalPinToInterrupt(limit1), changeLimit1, CHANGE);
}

void changeLimit1() {
  topLimitState = !topLimitState;
}

void loop() {
  //Serial.print("interrupt: "); Serial.println(topLimitState);
  Serial.print("digitalRead: "); Serial.println(digitalRead(limit1));

}