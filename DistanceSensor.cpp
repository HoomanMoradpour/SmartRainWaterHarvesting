#include "DistanceSensor.h"
#include <cmath>

DistanceSensor::DistanceSensor() {
    // Constructor
}

DistanceSensor::~DistanceSensor() {
    cleanup();
}

bool DistanceSensor::initialize() {
    wiringPiSetupGpio(); // Use BCM pin numbering
    pinMode(4, OUTPUT);
    pinMode(17, INPUT);
    return true;
}

void DistanceSensor::cleanup() {
    // Cleanup resources if needed
}

double DistanceSensor::getDistance() {
    // Clear the TRIG_PIN
    digitalWrite(4, LOW);
    QThread::sleep(2); // Wait for 2 seconds

    // Send a trigger pulse
    digitalWrite(4, HIGH);
    delayMicroseconds(10);
    digitalWrite(4, LOW);

    // Wait for ECHO_PIN to go HIGH
    while (digitalRead(17) == LOW);
    long pulseStart = micros();

    // Wait for ECHO_PIN to go LOW
    while (digitalRead(17) == HIGH);
    long pulseEnd = micros();

    // Calculate the distance
    double pulseDuration = pulseEnd - pulseStart;
    double distance = pulseDuration * 0.01715; // Distance in cm

    return std::round(distance * 100.0) / 100.0; // Round to 2 decimal places
}
