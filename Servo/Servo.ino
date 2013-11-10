#include <Servo.h>

Servo servo;

const int potentiometerPin = A0,
          servoPin         = 9,
          bitRate          = 9600,
          minInput         = 0,
          minOutput        = 0,
          maxInput         = 1023,
          maxOutput        = 179;

int potentiometerVal, angle;

void setup() {
  servo.attach(servoPin);
  Serial.begin(bitRate);  
}

void loop() {
  potentiometerVal = analogRead(potentiometerPin);
  Serial.print("potentiometer value: ");
  Serial.print(potentiometerVal);

  angle = map(
    potentiometerVal,
    minInput,
    maxInput,
    minOutput,
    maxOutput);

  Serial.print(" , ");
  Serial.print("angle: ");
  Serial.print(angle);
  Serial.println();

  servo.write(angle);
  delay(15);
}

