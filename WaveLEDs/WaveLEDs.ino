int switchPin = 2;

int led1 = 7;
int led2 = 6;
int led3 = 5;
int led4 = 4;
int led5 = 3;

int switchState = LOW;
int delayAmt    = 100;

void setup() {
  pinMode(switchPin, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
}

void loop() {
  switchState = digitalRead(switchPin);

  if(switchState == LOW) {
    return;
  }

  turnOn(led1);
  turnOn(led3);
  turnOn(led5);
  delay(delayAmt);

  turnOff(led1);
  turnOff(led3);
  turnOff(led5);  
  turnOn(led2);
  turnOn(led4);
  delay(delayAmt);
  
  turnOff(led2);
  turnOff(led4);
  delay(delayAmt);
}

void turnOn(int led) {
  digitalWrite(led, HIGH);
}

void turnOff(int led) {
  digitalWrite(led, LOW);
}

