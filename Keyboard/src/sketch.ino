const int notes[] = { 262, 294, 330, 349 };

const int analogPin = A0,
          piezoPin  = 8;


////////////////////////////////////////////////////

void setup() {
}


////////////////////////////////////////////////////

void loop() {
  int keyVal = analogRead(analogPin);

  if (keyVal == 1023) {
    play(notes[0]);
  } else if (isBetween(keyVal, 800, 900)) {
    play(notes[1]);
  } else if (isBetween(keyVal, 50, 100)) {
    play(notes[2]);
  } else if (isBetween(keyVal, 1, 10)) {
    play(notes[3]);
  } else {
    noTone(piezoPin);
  }
}


////////////////////////////////////////////////////

// Check if the value is between two other values.
boolean isBetween(int val, int low, int high) {
  return val >= low && val <= high;
}


////////////////////////////////////////////////////

// Play a tone on the piezo.
void play(int frequency) {
  tone(piezoPin, frequency);
}
