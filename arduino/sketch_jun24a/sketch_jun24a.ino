#include <SoftwareSerial.h>
#include <TFMini.h>

uint8_t header = 0x59;
TFMini tfmini;
 
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
  Serial.begin(115200);       //Initialize hardware serial port (serial debug port)
  while (!Serial);            // wait for serial port to connect. Needed for native USB port only
  Serial.println ("Initializing...");
  SerialTFMini.begin(115200);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
}
 
void loop()
{
  int distance = 0;
  int strength = 0;
 
  getTFminiData(&distance, &strength);
  while (!distance)
  {
    getTFminiData(&distance, &strength);
    if (distance)
    {
      Serial.println(distance);
    }
  }
  delay(100);
}