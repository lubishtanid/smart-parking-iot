#include <Arduino.h>
#include <Ultrasonic.h>
#include <MFRC522.h>
#include <Servo.h>

//defining ultrasonic sensor pins
Ultrasonic ultrasonic1(17, 16); //Trig pin: 17, Echo Pin: 16
Ultrasonic ultrasonic2(4, 21); //Trig pin: 4, Echo Pin: 21
Ultrasonic ultrasonic3(22, 25); //Trig pin: 22, Echo Pin: 25

//RFID RC522 pins
#define SDA_PIN 5
#define SCK_PIN 18
#define MOSI_PIN 23
#define MISO_PIN 19
#define RST_PIN 0

MFRC522 mfrc522(SDA_PIN, RST_PIN);

//servo motor pin
Servo servoMotor;
int servoPin = 13;


void setup() {
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
}