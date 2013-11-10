int switchState = 0;

int switchPin   = 2;
int greenLedPin = 3;
int redLed1Pin  = 4;
int redLed2Pin  = 5;

void setup() {
  pinMode(switchPin,   INPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLed1Pin,  OUTPUT);
  pinMode(redLed2Pin,  OUTPUT);
}

void loop() {
  switchState = digitalRead(switchPin);
  
  if (switchState == LOW) {
    digitalWrite(redLed1Pin, LOW);
    digitalWrite(redLed2Pin, LOW);
    
    blinkGreen();
  } else {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLed1Pin,  LOW);
    digitalWrite(redLed2Pin,  HIGH);
    
    delay(250);
    digitalWrite(redLed1Pin, HIGH);
    digitalWrite(redLed2Pin, LOW);
    delay(250);
  }
}

void blinkGreen() {
    digitalWrite(greenLedPin, HIGH);
    delay(150);
    digitalWrite(greenLedPin, LOW);
    delay(3000);
}
