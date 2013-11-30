const int switchPin = 8;

const long intervalSeconds = 600;

const int   led1 = 2,
            led2 = 3,
            led3 = 4,
            led4 = 5,
            led5 = 6,
            led6 = 7;

const int lastLed = led6;

int switchState      = 0,
    prevSwitchState  = 0,
    nextLed          = led1;

unsigned long   currentTime  = 0,
                previousTime = 0;


/////////////////////////////////////

void setup() {
    pinMode(switchPin, INPUT);

    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(led4, OUTPUT);
    pinMode(led5, OUTPUT);
    pinMode(led6, OUTPUT);
}


/////////////////////////////////////

void loop() {
    currentTime = millis();

    // If it has been the specified amount of time since the last
    // LED was turned on, then turn on the next one and set the
    // previousTime to the currentTime.
    if (currentTime > previousTime + (intervalSeconds * 1000)) {
        previousTime = currentTime;

        if (nextLed > lastLed) {
            turnOffAll();
        } else {
            turnOn(nextLed);
            nextLed++;
        }
    }

    // Check to see if the switchState is different from last time,
    // and reset the LEDs to OFF if so.
    switchState = digitalRead(switchPin);

    if (switchState != prevSwitchState) {
        turnOffAll();
        nextLed = led1;
        previousTime = currentTime;
    }

    // Set the previous switchState to the current one.
    prevSwitchState = switchState;
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


/////////////////////////////////////

// Deactivate all pins.
void turnOffAll() {
    turnOff(led1);
    turnOff(led2);
    turnOff(led3);
    turnOff(led4);
    turnOff(led5);
    turnOff(led6);
}
