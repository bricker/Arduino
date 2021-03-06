#include <Servo.h>

Servo servo;

const int potentiometerPin = A0,
          servoPin         = 9;

const int minInput         = 0,
          minOutput        = 0,
          maxInput         = 1023,
          maxOutput        = 179;

int potentiometerVal,
    angle;


void setup() {
  servo.attach(servoPin);
}

void loop() {
  potentiometerVal = analogRead(potentiometerPin);

  angle = map(
    potentiometerVal,
    minInput,
    maxInput,
    minOutput,
    maxOutput);

  servo.write(angle);
  delay(15);
}
