#include <Arduino.h>
#include <ESP32Servo.h>

const int triggerPin = 5; // Connect SRF05 Trigger Pin to ESP32 GPIO 4
const int echoPin = 18;    // Connect SRF05 Echo Pin to ESP32 GPIO 5
const int servoPin = 19;  // Connect Servo Signal Pin to ESP32 GPIO 13

Servo myservo;

void setup() {
  Serial.begin(9600);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myservo.attach(servoPin);
}

void loop() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= 10.0) {
    myservo.write(90); // Move the servo to 90 degrees
  } else {
    myservo.write(0); // Move the servo to 0 degrees
  }

  delay(1000);
}
