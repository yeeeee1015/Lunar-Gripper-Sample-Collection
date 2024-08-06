#include <SoftwareSerial.h>
#include <TFMini.h>
#include <Adafruit_VL53L0X.h>
uint8_t header = 0x59;
TFMini tfmini;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

int distance;
int strength;

SoftwareSerial SerialTFMini(18,19);
 
void getTFminiData(int* distance, int* strength)
{
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static double rx[9];
  if (SerialTFMini.available())
  {
    Serial.println("data avail");
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
        Serial.println("ya");
      }
      i = 0;
    }
    else {
      i++;
    }
  }
  Serial.println("nothing avail");
}

void setup()
{
  Serial.begin(115200);       //Initialize

  // wait for serial port to connect
  while (!Serial) {
    delay(1);
  }

  Serial.println ("Initializing...");
  SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
  Serial.println("yo");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  Serial.println("not failed to boot");
}
 
void loop()
{ 
  // distance = 0;
  // strength = 0;
  // while (!distance) {
  //   getTFminiData(&distance, &strength);
  // }
  // Serial.print("TFMini: "); Serial.println(distance);

  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus == 4) {
    Serial.println("out of range");
  }
  Serial.print("VL53L0X: "); Serial.println(measure.RangeMilliMeter);
}