#include <SoftwareSerial.h>
#include <TFMini.h>
#include <Adafruit_VL53L0X.h>
#include <ezButton.h>

uint8_t header = 0x59;
TFMini tfmini;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
const uint8_t proxPin = 11;
const uint8_t pressurePin = 14;
ezButton limitSwitch(21);

SoftwareSerial SerialTFMini(0, 1);
 
void getTFminiData(int* distance, int* strength)
{
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static double rx[9];
  if (SerialTFMini.available())
  {
    rx[i] = SerialTFMini.read();
    if (rx[0] != header) {
      i = 0;
    }
    else if (i == 1 && rx[1] != header) {
      i = 0;
    }
    else if (i == 8) {
      for (j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if (rx[8] == (checksum % 256)) {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
      }
      i = 0;
    }
    else {
      i++;
    }
  }
}
 
 
void setup()
{
  Serial.begin(115200);       //Initialize
  pinMode(proxPin, INPUT);
  pinMode(pressurePin, INPUT);
  limitSwitch.setDebounceTime(50);

  // wait for serial port to connect
  while (!Serial) {
    delay(1);
  }

  Serial.println ("Initializing...");
  SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor

  // if (!lox.begin()) {
  //   Serial.println(F("Failed to boot VL53L0X"));
  //   while(1);
  // }
}

void loop()
{
  int distance = 0;
  int strength = 0;
  cyclect = 0;

  VL53L0X_RangingMeasurementData_t measure;
  
  while (!distance) {
    getTFminiData(&distance, &strength);
  }
  Serial.print("TFMini: "); Serial.println(distance);

  // lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  // if (measure.RangeStatus == 4) {
  //   Serial.println("out of range");
  // }
  // Serial.print("VL53L0X: "); Serial.println(measure.RangeMilliMeter);

  Serial.print("Prox: "); Serial.println(digitalRead(proxPin));

  Serial.print("Pressure: "); Serial.println(analogRead(pressurePin));

  // limitSwitch.loop();
  // Serial.print("Limit Switch "); Serial.println(limitSwitch.getState());
  // }
}