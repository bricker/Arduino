const int bitRate = 9600;

const int redLEDPin   = 11;
const int blueLEDPin  = 10;
const int greenLEDPin = 9;

const int redSensorPin   = A0;
const int blueSensorPin  = A2;
const int greenSensorPin = A1;

int redSensorValue   = 0;
int blueSensorValue  = 0;
int greenSensorValue = 0;

int redValue   = 0;
int blueValue  = 0;
int greenValue = 0;

void setup() {
  Serial.begin(bitRate);

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

  Serial.print("Raw Sensor Values:\t");
  Serial.print("Red: ");
  Serial.print(redSensorValue);
  Serial.print("\t");
  Serial.print("Blue: ");
  Serial.print(blueSensorValue);
  Serial.print("\t");
  Serial.print("Green: ");
  Serial.print(greenSensorValue);
  Serial.println();
  
  redValue   = redSensorValue / 4;
  blueValue  = blueSensorValue / 4;
  greenValue = greenSensorValue / 4;

  Serial.print("Mapped Sensor Values:\t");
  Serial.print("Red: ");
  Serial.print(redValue);
  Serial.print("\t");
  Serial.print("Blue: ");
  Serial.print(blueValue);
  Serial.print("\t");
  Serial.print("Green: ");
  Serial.print(greenValue);
  Serial.println();

  analogWrite(redLEDPin, redValue);
  analogWrite(blueLEDPin, blueValue);
  analogWrite(greenLEDPin, greenValue);
}

