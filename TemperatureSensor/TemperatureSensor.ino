const int   bitRate = 9600;
const int   sensorPin    = A0;
const float baselineTemp = 20.0;

const int blue   = 2;
          yellow = 3;
          red    = 4;


void setup() {
  Serial.begin(bitRate);

  for (int pin = blue; pin <= red; pin++) {
    pinMode(pin, OUTPUT);
    turnOff(pin);
  }
}

void loop() {
  int   sensorVal    = analogRead(sensorPin);
  float voltage      = (sensorVal/1024.0) * 5.0;
  float temperature  = (voltage - 0.5) * 100;

  turnOffAll();

  if (temperature < baselineTemp+4) {
    turnOn(blue);
  } else if (temperature < baselineTemp + 8) {
    turnOn(yellow);
  } else {
    turnOn(red);
  }

  delay(1);
}

void turnOn(int pin) {
  digitalWrite(pin, HIGH);
}

void turnOff(int pin) {
  digitalWrite(pin, LOW);
}

void turnOffAll() {
  turnOff(blue);
  turnOff(yellow);
  turnOff(red);
}

