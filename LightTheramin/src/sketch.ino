int sensorValue,
    pitch,
    sensorLow  = 1023,
    sensorHigh = 0;

const int ledPin     = 13,
          tonePin    = 8,
          sensorPin  = A0;

const int freqLow    = 50,
          freqHigh   = 4000,
          toneLength = 20;


void setup() {
  pinMode(ledPin, OUTPUT);
  turnOn(ledPin);

  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin);

    if (sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }

    if (sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }

  turnOff(ledPin);
}


void loop() {
  sensorValue = analogRead(sensorPin);

  pitch = map(
    sensorValue,
    sensorLow,
    sensorHigh,
    freqLow,
    freqHigh);

  tone(tonePin, pitch, toneLength);
  delay(10);
}


void turnOn(int pin) {
  digitalWrite(pin, HIGH);
}


void turnOff(int pin) {
  digitalWrite(pin, LOW);
}

