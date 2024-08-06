#include <ArduinoQueue.h>
#include <IntervalTimer.h>

ArduinoQueue<int> q(100);
IntervalTimer clk;
bool isPaused = false;

void pause() {
  clk.end();
  while (isPaused) {
    while (Serial.available() > 0) {
      int rec = Serial.parseInt();
      if (rec == 4) {
        Serial.println("Unpause 4");
        isPaused = false;
        clk.begin(readSerial, 50000);
        break;
      }
      else if (rec != 0) {
        Serial.print("enqueuing "); Serial.println(rec);
        q.enqueue(rec);
      }
    }
  }
}

void readSerial() {
  while (Serial.available() > 0) {
    int rec = Serial.parseInt();
    Serial.print("Parsed "); Serial.println(rec);
    if (rec == 4) {
      Serial.println("PAUSE 4");
      isPaused = true;
      pause();
    }
    else if (rec != 0) {
      Serial.print("enqueuing "); Serial.println(rec);
      q.enqueue(rec);
    }
  }
}

void setup() {
	Serial.begin(115200);
  clk.begin(readSerial, 50000);
}

void loop() {
  while (!q.isEmpty()) {
    Serial.println(q.dequeue());
  }
}