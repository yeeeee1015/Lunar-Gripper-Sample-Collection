#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
}


void loop() {
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.println(measure.RangeMilliMeter);
  }
  else {
    Serial.println("out of range");
  }

  // int ct = 0;
  // unsigned int sum = 0;
  // int numAvg = 50;
  // VL53L0X_RangingMeasurementData_t measure;

  // while (ct <= numAvg) {
  //   lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout
  //   if (measure.RangeStatus != 4) {  // phase failures have incorrect data
  //     Serial.println(measure.RangeMilliMeter);
  //     sum = sum + measure.RangeMilliMeter;
  //     ct = ct + 1;
  //   }
  //   else {
  //     Serial.println(" out of range ");
  //   }
  // }
  // int avg = sum / numAvg;
  //Serial.print("Average dist: "); Serial.println(avg);
}
