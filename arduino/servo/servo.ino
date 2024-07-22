#include <Servo.h>

Servo myservo;  // create Servo object to control a servo

int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object , or any other PWM Teensy pin
} 
 
void loop() {
  // Servo spins forward at full speed for 1 second.
  myservo.write(1050);
  delay(1000);
}