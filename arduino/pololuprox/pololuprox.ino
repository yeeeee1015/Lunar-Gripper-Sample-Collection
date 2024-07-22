uint8_t read;

const byte proxPin = 2;
volatile byte proxOn = LOW;


void setup() {
  Serial.begin(115200);
  pinMode(proxPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(proxPin), blink, CHANGE);
}

void loop() {
  while (true) {
    Serial.println("go");
    delay(1000);
  }
}

void blink() {
  Serial.println("detected");
  proxOn = !proxOn;
}