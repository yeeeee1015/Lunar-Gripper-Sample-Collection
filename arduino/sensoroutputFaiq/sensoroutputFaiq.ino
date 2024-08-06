#define sdA 4
#define sdB 6
#define pwmA 3
#define pwmB 5

void setup()
{
  Serial.begin(115200);
  pinMode(sdA, OUTPUT);
  pinMode(sdB, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  digitalWrite(sdA, HIGH);
  digitalWrite(sdB, HIGH);
  digitalWrite(pwmB, LOW);
}

void loop()
{
  analogWrite(pwmA, 240);
}
