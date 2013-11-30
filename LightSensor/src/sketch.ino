const int redLEDPin      = 11,
          blueLEDPin     = 10,
          greenLEDPin    = 9,
          redSensorPin   = A0,
          blueSensorPin  = A2,
          greenSensorPin = A1;

int redSensorValue   = 0,
    blueSensorValue  = 0,
    greenSensorValue = 0;

int redValue   = 0,
    blueValue  = 0,
    greenValue = 0;

void setup() {
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
}

void loop() {
  redSensorValue = analogRead(redSensorPin);
  delay(5);

  blueSensorValue = analogRead(blueSensorPin);
  delay(5);

  greenSensorValue = analogRead(greenSensorPin);

  redValue   = redSensorValue / 4;
  blueValue  = blueSensorValue / 4;
  greenValue = greenSensorValue / 4;

  analogWrite(redLEDPin, redValue);
  analogWrite(blueLEDPin, blueValue);
  analogWrite(greenLEDPin, greenValue);
}

