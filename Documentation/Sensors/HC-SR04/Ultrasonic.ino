// Include NewPing Library
#include "NewPing.h"

// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN A4
#define ECHO_PIN A5

// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400	

// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
	Serial.begin(9600);
}

void loop() {
	Serial.print("Distance = ");
	float dist = sonar.ping_cm();
	Serial.println(dist);
	delay(500);
}