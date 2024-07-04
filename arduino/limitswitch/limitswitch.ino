#include <ezButton.h>

ezButton limitSwitch(21);

void setup() {
  Serial.begin(115200);
  limitSwitch.setDebounceTime(100);
}

void loop() {
  limitSwitch.loop();
  Serial.println(limitSwitch.getState());
}