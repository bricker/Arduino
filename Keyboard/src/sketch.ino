const int notes[] = { 262, 294, 330, 349 };

#define ANALOG A0
#define PIEZO 8


////////////////////////////////////////////////////

void setup() {}


////////////////////////////////////////////////////

void loop() {
  int keyVal = analogRead(ANALOG);

  if (keyVal == 1023) {
    play(notes[0]);
  } else if (isBetween(keyVal, 990, 1010)) {
    play(notes[1]);
  } else if (isBetween(keyVal, 505, 515)) {
    play(notes[2]);
  } else if (isBetween(keyVal, 5, 10)) {
    play(notes[3]);
  } else {
    noTone(PIEZO);
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
  tone(PIEZO, frequency);
}
