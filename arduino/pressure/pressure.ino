const uint8_t sensorPin = A0;

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
}

void loop() {
  uint16_t read = analogRead(sensorPin); 
  Serial.println(read);
  delay(50);
}