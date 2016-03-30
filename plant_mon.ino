// Plant monitor v0.1 - using grove moisture sensor
#include "Serial2/Serial2.h"

const int readPin = A2;
const int led1 = D0;
const int onboardLed = D7;

int moistureReading = 0;

int lastState = 0;

void setup() {

  Serial.begin(9600);

  pinMode(led1, OUTPUT);

  pinMode(onboardLed, OUTPUT);

  pinMode(readPin, INPUT);

}

// Next we have the loop function, the other essential part of a microcontroller program.
// This routine gets repeated over and over, as quickly as possible and as many times as possible, after the setup function is called.
// Note: Code that blocks for too long (like more than 5 seconds), can make weird things happen (like dropping the network connection).  The built-in delay function shown below safely interleaves required background activity, so arbitrarily long delays can safely be done if you need them.

void loop() {

  moistureReading = analogRead(readPin);

  Serial.println(moistureReading);

  if(moistureReading <= 300) {
    digitalWrite(led1, HIGH);  //turn alert led on
    delay(5000); // leave it on for 5 seconds
    /*digitalWrite(led1, LOW);  //turn alert led off*/
    lastState += 1;
  } else {
    // just pulse the onboard led so we know the program is running
    digitalWrite(onboardLed, HIGH);
    delay(1000);
    digitalWrite(onboardLed, LOW);
    lastState = 0;  // reset the consecutive readings count, since we must have filled it
  }

  delay(1000);  // only read every n/1000 seconds

  if(lastState == 30){
    Serial.println("Last state triggered for 30 consecutive readings, assuming it's the real deal and triggering IFTT");
    Particle.publish("water_level", "low");
    delay(60*60*1000);
  }

}
