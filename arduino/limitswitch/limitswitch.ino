#define limitOne 21

volatile byte limitState = LOW;

void blink() {
  limitState = !limitState;
}

void setup() {
  Serial.begin(115200);
  pinMode(limitOne, INPUT);
  attachInterrupt(digitalPinToInterrupt(limitOne), blink, CHANGE); // init interrupt for prox
}

void loop() {
  Serial.println(limitState);
}