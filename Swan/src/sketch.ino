// Swan Station Countdown
// Program Specification: http://lostpedia.wikia.com/wiki/Countdown_timer

#include <LiquidCrystal.h>

// Pin assignment
const int   switchPin    = 7,
            alarmPin     = 6,
            failsafePin  = A0,
            fsLed1       = 8,
            fsLed2       = 9,
            fsLed3       = 10;

// LCD Pin Assignment
const int   lcdRsPin = 12,
            lcdEnPin = 11,
            lcdD4Pin = 5,
            lcdD5Pin = 4,
            lcdD6Pin = 3,
            lcdD7Pin = 2;

// lcd size
const int   lcdWide = 16,
            lcdTall = 2,
            lcdRow1 = 0,
            lcdRow2 = 1;

// Initialize LiquidCrystal
LiquidCrystal lcd(lcdRsPin, lcdEnPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);

// Delay to make the tone consistent, for the tone() function.
const int consistentToneDelay = 20;

// failsafe values
const int   failsafeLow   = 10, // Low threshold for triggering completion
            failsafeHigh  = 1013; // High threshold for triggering completion



///// CONFIGURATION /////

// Full interval in MINUTES
// Needs to be unsigned long so we can perform maths.
const unsigned long minInterval = 108;

// "Hieroglyph" characters
// This is an array of binary codes for ASCII characters.
// The size of this array should be greater than the timeDisplayChars
// integer set in the setup() function.
// See page 17 of https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
#define GLYPH_SIZE 64

const byte glyphs[GLYPH_SIZE] = {
    B10110000,  B11000000,  B11010000,  B11100000,
    B10100001,  B10110001,  B11000001,  B11010001,
    B10100010,  B10110010,  B11000010,  B11010010,
    B10100011,  B10110011,  B11000011,  B11010011,
    B10100100,  B10110100,  B11000100,  B11010100,
    B10100101,  B10110101,  B11000101,  B11010101,
    B10100110,  B10110110,  B11000110,  B11010110,
    B10100111,  B10110111,  B11000111,  B11010111,
    B10101000,  B10111000,  B11001000,  B11011000,
    B10101001,  B10111001,  B11001001,  B11011001,
    B10101010,  B10111010,  B11001010,  B11011010,
    B10101011,  B10111011,  B11001011,  B11011011,
    B10101100,  B10111100,  B11001100,  B11011100,
    B10101101,  B10111101,  B11001101,  B11011101,
    B10101110,  B10111110,  B11001110,  B11011110,
    B10101111,  B10111111,  B11001111,  B11011111
};

// Configure the beep for the tone() function.
const int   beepLength     = 20,
            baseBeepFreq   = 440,
            baseBeepDelay  = 2000; // Delay between beeps, not for tone()

// Configure the beep for each level.
// Frequency multipliers
// Multiplied by the baseBeepFreq for each level
// Higher number results in a higher frequency from the piezo.
const int   l2BeepFreqMult    = 2,
            l3BeepFreqMult    = 2,
            sfBeepFreqMult    = 2;

// Delay multipliers
// Multiplied by the baseBeepDelay for each level
// A lower number results in a shorter delay, i.e. a faster beep.
const float l2BeepDelayMult = 1.0,
            l3BeepDelayMult = 0.5;

// Number of MILLISECONDS to delay between writing each glyph.
// unsigned long is for the maths
const unsigned long msGlyphDelay = 1000;

// failsafe checks default values
bool    failsafeStep1        = false,
        failsafeStep2        = false,
        failsafeStep3        = false,
        failureAlertStarted  = false; // Whether or not the alert has started
                                      // This is so we only do certain things
                                      // once, like write to the LCD and turn
                                      // on the LEDs.

// Flag to check if the countdown is finished so we can start
// writing hieroglyphics in that space.
bool countdownFinished = false;

// Time holders
unsigned long   msCurrentTime  = 0,
                msPreviousTime = 0;

// Relative Thresholds from intervalSeconds in MILLISECONDS
unsigned long   msInterval,
                msBeepLevelOne,   // level 1 relative threshold
                msBeepLevelTwo,   // level 2 relative threshold
                msBeepLevelThree, // level 3 relative threshold
                msHieroglyphs,    // Hieroglyphs relative threshold
                msSystemFailure;  // system failure relative threshold

// Keep track of the last time we showed on the display.
long secPreviousLeft = 0;

// Keep track of the last time an event occurred.
// This is so we don't have to use delay().
// delay() is a bummer because it prevents anything from
// happening, such as writing to the LCD.
unsigned long   msLastBeep  = 0,
                msLastGlyph = 0;

// Where the last glyph was written to on the LCD, so we can do it
// gradually instead of all at once.
int lastGlyphPosition = 0,
    timeDisplayChars,  // How many characters the time/glyphs will need
    timeDisplayOffset; // How many spaces to offset from the left.


/////////////////////////////////////

void setup() {
    lcd.begin(lcdWide, lcdTall);

    pinMode(switchPin, INPUT);
    pinMode(alarmPin, OUTPUT);
    pinMode(fsLed1, OUTPUT);
    pinMode(fsLed2, OUTPUT);
    pinMode(fsLed3, OUTPUT);

    // Seed rand() with the value of the failsafePin (an analog-in),
    // so we don't have to load a time library for this one thing.
    srand(digitalRead(failsafePin));

    // Set the number of characters to allocate for the time display.
    // The glyph display will also use this count to overwrite the
    // numbers.
    if (minInterval >= 1000) {
        timeDisplayChars = 7; // 1000:00
    } else if (minInterval >= 100) {
        timeDisplayChars = 6; // 100:00
    } else if (minInterval >= 10) {
        timeDisplayChars = 5; // 10:00
    } else {
        timeDisplayChars = 4; // 1:00
    }

    // Set the offset space from the left of the LCD.
    timeDisplayOffset = (lcdWide - timeDisplayChars) / 2;

    // Set the relative thresholds from the intervalSeconds.
    // These variables should be set to MILLISECONDS.
    // Examples are with T = 108m
    // We add an extra second so we can go all the way from
    // 108 minutes to 0 seconds. That's technically an extra second,
    // but the aesthetics are what's important here.
    msInterval = (minInterval * 60 * 1000) + 1000; // Extra second

    msBeepLevelOne    = msInterval - (msInterval / 27);  // T-4m
    msBeepLevelTwo    = msInterval - (msInterval / 108); // T-1m
    msBeepLevelThree  = msInterval - (msInterval / 648); // T-10s
    msHieroglyphs     = msInterval;
    msSystemFailure   = msInterval + (msInterval / 648); // T+10s
}


/////////////////////////////////////

void loop() {
    msCurrentTime = millis();

    if (!countdownFinished) {
        writeCountdownToLCD();
    }

    // Setup variables so we know when to trigger each event.
    unsigned long   msL1th    = msPreviousTime + msBeepLevelOne,
                    msL2th    = msPreviousTime + msBeepLevelTwo,
                    msL3th    = msPreviousTime + msBeepLevelThree,
                    msHGth    = msPreviousTime + msHieroglyphs,
                    msSFth    = msPreviousTime + msSystemFailure;

    // Beeps level one (T-4m - T-1m)
    // Steady alarm
    if (isBetween(msL1th, msL2th)) {
        if (shouldBeep(baseBeepDelay)) {
            beep(baseBeepFreq);
            msLastBeep = msCurrentTime;
        }
    }

    // Beeps level two (T-1m - T-0s)
    // Intense alarm
    if (isBetween(msL2th, msL3th)) {
        if (shouldBeep(baseBeepDelay * l2BeepDelayMult)) {
            beep(baseBeepFreq * l2BeepFreqMult);
            msLastBeep = msCurrentTime;
        }
    }

    // Beeps level three (T-10s - T+10s)
    // Faster, intense alarm
    if (isBetween(msL3th, msSFth)) {
        if (shouldBeep(baseBeepDelay * l3BeepDelayMult)) {
            beep(baseBeepFreq * l3BeepFreqMult);
            msLastBeep = msCurrentTime;
        }
    }

    // EXECUTE!! EXECUTE!!!! (T-0s)
    // Same alarm, hieroglyphs start showing
    if (msCurrentTime >= msHGth) {
        if (countdownFinished) {
            writeHieroglyphs();
        }
    }

    // System Failure! RUN FOR YOUR LIFE BROTHA! (T+10s +)
    // Alarm ends, "System Failure" plays on loop.
    // Since we don't have recorded audio, we'll just blare a
    // solid tone and show "System Failure" on the LCD.
    if (msCurrentTime >= msSFth) {
        alertSystemFailure();

        // DESMOND WHERE ARE YOU GOING?!?!?
        if (failsafeEngaged()) {
            resetFailsafe();
            resetTimer();
        }
    }

    // 4 8 15 16 23 42
    if (isBetween(msL1th, msSFth) && isOn(switchPin)) {
        resetTimer();
    }
}


/////////////////////////////////////

// Write the remaining time to the LCD.
void writeCountdownToLCD() {
    // Get the Time To Zero for the LCD Display,
    // and separate it into minutes and seconds.
    // Then write to the display, padding with
    // zero as necessary for seconds and minutes.
    long msToZero = msInterval - (msCurrentTime - msPreviousTime);
    long secCurrentLeft = msToZero / 1000;

    // If we've passed 0, then do nothing.
    // Set the flag that we're finished in this function so it doesn't
    // get called again.
    // The numbers get replaced by hieroglyphs in the loop() function.
    // Using secCurrentLeft instead of msToZero so we can get a nice
    // "000:00" display for a short time.
    if (secCurrentLeft < 0) {
        countdownFinished = true;
        return;
    }

    if (secCurrentLeft != secPreviousLeft) {
        int mins = secCurrentLeft / 60;
        int secs = secCurrentLeft - (mins * 60);

        // Overwrite the previous time display.
        // We don't want to clear the screen because
        // there may be other stuff on another line.
        lcd.setCursor(timeDisplayOffset, lcdRow1);

        // Pad with 0's.
        // We only allow between 4 and 7 slots,
        // so we can assume some things here.
        // We only pad the 7, 6, and 5 spots (omitting 4) because
        // a single 0 will always be printed in the last spot if
        // the mins is 0.
        for (int i = 1000, x = 7; x > 4; i /= 10, x--) {
            if (timeDisplayChars >= x && mins < i) {
                lcd.print(0);
            }
        }

        // A single 0 will always be printed here if mins is 0,
        // so we don't need to pad this spot.
        lcd.print(mins);
        lcd.print(":");

        if (secs < 10) {
            lcd.print("0");
        }

        lcd.print(secs);
    }

    secPreviousLeft = secCurrentLeft;
}


/////////////////////////////////////

// Gradually replace the time display with hieroglyphs.
void writeHieroglyphs() {
    int currentGlyphPosition = timeDisplayOffset + lastGlyphPosition;

    // If we've already covered up all the numbers,
    // then stop writing glyphs.
    if (lastGlyphPosition >= timeDisplayChars) {
        return;
    }

    if (msCurrentTime >= msLastGlyph + msGlyphDelay) {
        lcd.setCursor(currentGlyphPosition, lcdRow1);
        lcd.write(glyphs[rand() % GLYPH_SIZE]);

        msLastGlyph = msCurrentTime;
        lastGlyphPosition++;
    }
}


/////////////////////////////////////

// Turn on the System Failure alerts.
void alertSystemFailure() {
    // SOUND THE ALARM!!!
    tone(alarmPin, baseBeepFreq * sfBeepFreqMult, consistentToneDelay);

    if (!failureAlertStarted) {
        // WARN THE DEAF!!!!!
        turnOn(fsLed1);
        turnOn(fsLed2);
        turnOn(fsLed3);

        // Center the message, and will also overwrite the time so
        // we don't have to worry about clearing it out.
        lcd.setCursor(1, lcdRow2);
        lcd.print("System Failure");

        // Set this to true so we don't run this block again.
        failureAlertStarted = true;
    }
}


/////////////////////////////////////

// Check the failsafe return whether or not it's been completed.
// Also handles failsafe progress LEDs.
bool failsafeEngaged() {
    int failsafeVal = analogRead(failsafePin);

    if (!failsafeStep1 && !failsafeStep2 && !failsafeStep3) {
        if (failsafeVal <= failsafeLow) {
            turnOff(fsLed3);
            failsafeStep1 = true;
        }
    } else if (failsafeStep1 && !failsafeStep2 && !failsafeStep3) {
        if (failsafeVal >= failsafeHigh) {
            turnOff(fsLed2);
            failsafeStep2 = true;
        }
    } else if (failsafeStep1 && failsafeStep2 && !failsafeStep3) {
        if (failsafeVal <= failsafeLow) {
            turnOff(fsLed1);
            failsafeStep3 = true;
        }
    }

    return failsafeStep1 && failsafeStep2 && failsafeStep3;
}


/////////////////////////////////////

// Reset the timer.
void resetTimer() {
    lcd.clear();
    countdownFinished = false;
    msPreviousTime = msCurrentTime;
    lastGlyphPosition = 0;
}


/////////////////////////////////////

// Reset the failsafe.
void resetFailsafe() {
    failsafeStep1 = false;
    failsafeStep2 = false;
    failsafeStep3 = false;
    failureAlertStarted = false;
}

/////////////////////////////////////

// Do a beep
void beep(int freq) {
    tone(alarmPin, freq, beepLength);
}


/////////////////////////////////////

// Whether or not to perform a beep on this loop.
// This is used instead of delay() so we get i/o while waiting
// for the next beep.
bool shouldBeep(int msBeepDelay) {
    return msCurrentTime >= msLastBeep + msBeepDelay;
}


/////////////////////////////////////

// Determine whether the current time is between the two other numbers.
// * t1 - lower threshold (compared using >=)
// * t2 - upper threshold (compared using <)
bool isBetween(unsigned long t1, unsigned long t2) {
    return msCurrentTime >= t1 && msCurrentTime < t2;
}


/////////////////////////////////////

// Check if the pin is currently on
bool isOn(int pin) {
    return digitalRead(pin) == HIGH;
}


/////////////////////////////////////

// Activate the given pin.
void turnOn(int pin) {
    digitalWrite(pin, HIGH);
}


/////////////////////////////////////

// Deactivate the given pin.
void turnOff(int pin) {
    digitalWrite(pin, LOW);
}
