void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // int incomingByte = 0;
  pinMode(LED_BUILTIN, OUTPUT);
}
 
void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  if (Serial.available() > 0) {
    if (Serial.readString() == "on") {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(5000);
    } 
  }
}