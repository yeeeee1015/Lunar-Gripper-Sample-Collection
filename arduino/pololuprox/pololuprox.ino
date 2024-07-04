const uint8_t sensorPin = 18;
uint8_t read;

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
}

void loop() {
  uint8_t read = digitalRead(sensorPin); 
  Serial.println(read);
}