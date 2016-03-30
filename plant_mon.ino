// Plant monitor v0.1 - using grove moisture sensor
#include "Serial2/Serial2.h"

// attach the yellow data wire from the grove sensor to this pin
const int readPin = A2;

// attach the positive (longer) lead from the LED to this pin
const int led1 = D0;

// this is the tiny blue onboard LED built into the particle board
const int onboardLed = D7;

// Not entirely sure what readings can be obtained from the Grove moisture
// sensor. Zero is bone dry, but when freshly watered, I was getting readings
// around 1500-1600
int moistureReading = 0;

// Keep track of how many consecutive readings we have where the dryness level
// was too low, this is mostly to prevent false readings.
int lastState = 0;

int alertLedOn = 0;  // just a little hack so we don't write LOW to the led all
                     // the time (0=Off,1=On)

// The setup() method is run once and we just tell it what pins we'll be using
// and whether we'll be using them for INPUT or OUTPUT.  We'll read from the
// sensor and write to the LEDs.  We can listen to the serial port while the
// Particle is attached to the IDE
void setup() {

  Serial.begin(9600);

  pinMode(led1, OUTPUT);

  pinMode(onboardLed, OUTPUT);

  pinMode(readPin, INPUT);

}

// This routine gets repeated over and over, as quickly as possible and as many times as possible, after the setup function is called.
// Note: Code that blocks for too long (like more than 5 seconds), can make weird things happen (like dropping the network connection).
// The built-in delay function shown below safely interleaves required background activity, so arbitrarily long delays can safely be
// done if you need them.
void loop() {

  delay(60 * 1000);  // only check once a minute

  // pulse the onboard led so we know our program is running
  digitalWrite(onboardLed, HIGH);
  delay(100);
  digitalWrite(onboardLed, LOW);

  // read from the moisture sensor
  moistureReading = analogRead(readPin);

  // echo the reading to the serial port for debugging purposes
  Serial.println(moistureReading);

  // if the plant is really dry...
  if(moistureReading <= 75) {

    if( alertLedOn == 0 ){ // if the alert LED is off, turn it on
      alertLedOn = 1;
      digitalWrite(led1, HIGH);
    }

    // increment the number of times we've registered a low moisture reading
    lastState += 1;

  } else {  // moisture level is fine

    if( alertLedOn == 1){ // if the alert LED is on, turn it off
      alertLedOn = 0;
      digitalWrite(led1, LOW);
    }

    // reset the counter for low moisture readings, either previous readings
    // were a false positive, or we've watered the plant and it's fine again.
    lastState = 0;
  }

  // if we've gotten 30 consecutive low moisture readings, we can assume it's
  // time to alarm (and the signal isn't just bouncing)
  if(lastState == 30){
    Serial.println("Sending signal to trigger IFTT recipe");
    Particle.publish("water_level", "low");
  }

}
