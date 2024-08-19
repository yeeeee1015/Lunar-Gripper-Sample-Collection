#define sdA 4
#define sdB 6
#define pwmA 3
#define pwmB 5
#define killSwitch 7

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(sdA, OUTPUT);
  pinMode(sdB, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(killSwitch, OUTPUT);
  digitalWrite(killSwitch, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(sdA, HIGH);
  digitalWrite(sdB, HIGH);
  digitalWrite(pwmB, LOW);
  digitalWrite(pwmA, LOW);
}

void loop()
{
  Serial.println()
  analogWrite(pwmA, 25);
}
