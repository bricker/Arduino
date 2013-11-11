#include <cmath>

const int bitRate    = 9600, // For logging
          tonePin    = 8,    // Output pin, piezo
          potPin     = A0,   // Input pin, potentiometer
          volumePin  = 3,    // Input pin, switch
          octavePin  = 4,    // Input pin, switch
          auxPin     = 5,    // Input pin, switch
          greenLed   = 6,    // Green LED
          redLed     = 7,    // Red LED
          inputMin   = 0,    // Input minimum for potentiometer
          inputMax   = 1023, // Input maximum for potentiometer
          toneLength = 20,   // The length of the tone
          rootFreq   = 440,  // Root frequency (A2)
          octaves    = 1,    // Number of octaves to allow. Only 1 for now.
          octaveMult = 2;    // Number of octaves to jump
                             // when octave switch is held.
                             // Positive number go up,
                             // negative numbers go down.

// This is the twelfth root of 2, which is the number
// we need to multiply by to get the frequencies.
// The formula is: rootFreq * (halfStepMultiplier^n)
// where n is the number of half-steps from the root pitch.                     
const float halfStepMultiplier = 1.059463094359;

// Scales
// Each array contains the notes of a scale,
// as number of half-steps from the root of the scale.
// Leave off the octave - we'll handle that in the loops.
const int majorSize          = 7,
          harmonicMinorSize  = 7,
          naturalMinorSize   = 7,
          pentatonicSize     = 5,
          bluesSize          = 6;

const int majorSteps[majorSize]                  = { 0, 2, 4, 5, 7, 9, 11 },
          harmonicMinorSteps[harmonicMinorSize]  = { 0, 2, 3, 5, 7, 8, 11 },
          naturalMinorSteps[naturalMinorSize]    = { 0, 2, 3, 5, 7, 8, 10 },
          pentatonicSteps[pentatonicSize]        = { 0, 2, 4, 7, 9 },
          bluesSteps[bluesSize]                  = { 0, 3, 5, 6, 7, 10 };
 
int majorFrequencies[majorSize]                  = { },
    harmonicMinorFrequencies[harmonicMinorSize]  = { },
    naturalMinorFrequencies[naturalMinorSize]    = { },
    pentatonicFrequencies[pentatonicSize]        = { },
    bluesFrequencies[bluesSize]                  = { };

int potValue,
    pitch,
    pos,
    highFreq;

boolean toneIsOn = true,
        volumeSwitchWasPressed = false;


///////////////////////////////////////////////////////////


// To change to another scale, change:
// * majorSize
// * majorSteps (in setup)
// * majorFrequencies (in setup)

const int scaleSize = bluesSize; // HERE
int scaleSteps[scaleSize];
int scaleFrequencies[scaleSize];

void setup() {
  Serial.begin(bitRate);

  memcpy(
    scaleSteps,
    bluesSteps, // HERE
    sizeof(bluesSteps) // HERE
  );

  memcpy(
    scaleFrequencies,
    bluesFrequencies, // HERE
    sizeof(bluesFrequencies) // HERE
  ); // HERE

  for (int octave = 0; octave < octaves; octave++) {
    for (
      int noteLoopIndex = 0;
      noteLoopIndex < scaleSize;
      noteLoopIndex++
    ) {
      pos = scaleSteps[noteLoopIndex];

      if (octave > 0) {
        pos += 12*octave;
      }

      scaleFrequencies[noteLoopIndex] = getFrequency(pos);
    }

    // If this loop is going over the last octave,
    // then we should add the final octave note to
    // the array of frequencies, to get a full
    // octave range.
    if (octave == octaves - 1) {
      highFreq = getFrequency(scaleSteps[0] + 12*octaves);
    }
  }

  pinMode(volumePin, INPUT);
  pinMode(octavePin, INPUT);
  pinMode(auxPin, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
}

///////////////////////////////////////////////////////////


void loop() {
  if (isPressed(volumePin)) {
    if (!volumeSwitchWasPressed) {
      // If the switch is currently being pressed, but we
      // haven't already registered that it was pressed,
      // then let the program know that it was pressed
      // and toggle the tone.
      volumeSwitchWasPressed = true;
      toggleTone();
    }
  } else {
    if (volumeSwitchWasPressed) {
      // If the volume switch isn't currently
      // begin pressed and we haven't already registered
      // it, set the switch status to false.
      volumeSwitchWasPressed = false;
    }
  }

  // If the tone isn't on, then we don't want to write
  // anything to the piezo. So, we can just short-circuit
  // here, since there's no point in getting the frequency
  // or checking the other switches or anything.
  if (!toneIsOn) {
    return;
  }

  potValue = analogRead(potPin);
  pitch = map(
    potValue,
    inputMin,
    inputMax,
    rootFreq,
    highFreq);

  pitch = snapFrequency(pitch);
  handleVolumeLeds(pitch);

  // Check and handle the Octave Switch
  // It just makes the tone jump up or down an octave.
  if (isPressed(octavePin)) {
    if (octaveMult < 0) {
      pitch /= -octaveMult; // This doesn't work.
    } else {
      pitch *= octaveMult;
    }
  }

  // Check an handle the Auxiliary Switch
  // What does it do? Nobody knows!
  if (isPressed(auxPin)) {
    pitch += 2;
  }

  tone(tonePin, pitch, toneLength);
  delay(10);
}


///////////////////////////////////////////////////////////

// Get the frequency for the given step from the defined root frequency.
int getFrequency(int steps) {
  return rootFreq * pow(halfStepMultiplier, steps);
}


///////////////////////////////////////////////////////////

int snapFrequency(int pitch) {
  int thisFrequency,
      nextFrequency;

  int newPitch = pitch;

  for (int i = 0; i < scaleSize; i++) {
    thisFrequency = scaleFrequencies[i];

    // If pitch is exactly this frequency, then we can just
    // return it. No need to snap in this case.
    if (pitch == thisFrequency) {
      newPitch = thisFrequency;
      break;
    }

    // If we're on the last frequency, then the "next" frequency
    // should just be our highest one (ie, the octave).
    if(thisFrequency == scaleFrequencies[scaleSize - 1]) {
      nextFrequency = highFreq;
    } else {
      nextFrequency = scaleFrequencies[i+1];
    }

    // If the pitch is not between this frequency and the
    // next one, then go on to the next set.
    // We don't have to check >= here because we already
    // caught the == case above.
    if (!(pitch > thisFrequency && pitch < nextFrequency)) {
      continue;
    }

    // Check if the pitch is above the half-way point between
    // the two frequencies. If it's above, then snap to the next
    // frequency. If it's below, then snap to the lower one.
    if (pitch > (nextFrequency - thisFrequency) / 2) {
      newPitch = nextFrequency;
    } else {
      newPitch = thisFrequency;
    }
  }

  return newPitch;
}


///////////////////////////////////////////////////////////

// Check if a switch is being held down.
boolean isPressed(int pin) {
  return digitalRead(pin) == HIGH;
}


///////////////////////////////////////////////////////////

// Toggle the tone on/off, based on the current status of the tone.
void toggleTone() {
  if (toneIsOn) {
    deactivateTone();
  } else {
    activateTone();
  }
}


///////////////////////////////////////////////////////////

// Handle the LED indicators.
void handleVolumeLeds(int pitch) {
  if (pitch <= rootFreq) {
    turnOff(redLed);
    turnOn(greenLed);
  } else if (pitch >= highFreq) {
    turnOff(greenLed);
    turnOn(redLed);
  } else {
    turnOff(redLed);
    turnOff(greenLed);
  }
}


///////////////////////////////////////////////////////////

// Turn On the tone.
void activateTone() {
  digitalWrite(tonePin, HIGH);
  toneIsOn = true;
}


///////////////////////////////////////////////////////////

// Turn Off the tone.
void deactivateTone() {
  digitalWrite(tonePin, LOW);
  toneIsOn = false;
}


///////////////////////////////////////////////////////////

// Turn On the given pin.
void turnOn(int pin) {
  digitalWrite(pin, HIGH);
}


///////////////////////////////////////////////////////////

// Turn Off the given pin.
void turnOff(int pin) {
  digitalWrite(pin, LOW);
}

