#include <Arduino.h>
#include <Ultrasonic.h>
#include <MFRC522.h>
#include <Servo.h>
#include <U8g2lib.h>
#include <Wire.h>

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
const int servoPin = 13;

//infrared sensors pins
const int irSensor1Pin = 26;
const int irSensor2Pin = 27;
const int irSensor3Pin = 32;
const int irSensor4Pin = 33;


//oled sh1106 display definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define CUSTOM_SDA_PIN 14
#define CUSTOM_SCL_PIN 15

TwoWire myWire(0); //0 indicates the first I2C bus

// Create an instance of the U8g2 SH1106 display library with your custom Wire instance
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ CUSTOM_SCL_PIN, /* data=*/ CUSTOM_SDA_PIN, /* reset=*/ U8X8_PIN_NONE);


void setup() {
  Serial.begin(115200);
  // Initialize my custom Wire instance
  myWire.begin(CUSTOM_SDA_PIN, CUSTOM_SCL_PIN);

}

void loop() {
  // put your main code here, to run repeatedly:
}