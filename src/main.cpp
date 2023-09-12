#include <Arduino.h>
#include <Ultrasonic.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFi.h>


// wifi
const char* ssid = "Arbri";
const char* password = "ardar123";

//mysql db
const char* host = "localhost";      // MySQL host address
const char* user = "root";  // MySQL username
const char* password_db = "";  // MySQL password
const char* db = "iot_members";     // MySQL database name

MySQL_Connection conn((Client *)&Serial);

//defining ultrasonic sensor pins
Ultrasonic ultrasonic1(17, 16); //Trig pin: 17, Echo Pin: 16
Ultrasonic ultrasonic2(4, 21); //Trig pin: 4, Echo Pin: 21
Ultrasonic ultrasonic3(22, 25); //Trig pin: 22, Echo Pin: 25

//RFID RC522 pins
#define SDA_PIN 5
#define SCK_PIN 18
#define MOSI_PIN 23
#define MISO_PIN 19
#define RST_PIN 27

MFRC522 mfrc522(SDA_PIN, RST_PIN);

//servo motor pin
#define SERVO_PIN 13
Servo servoMotor; // servo object

//infrared sensors pins
const int irSensor1Pin = 26;
const int irSensor2Pin = NaN;
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

  while (!Serial);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  servoMotor.attach(SERVO_PIN); //attach servo pin
  servoMotor.write(0); // Initialize the servo position to 0 degrees
  // Initialize my custom Wire instance
  myWire.begin(CUSTOM_SDA_PIN, CUSTOM_SCL_PIN);

}

void loop() {
  double distanceUS1, distanceUS2, distanceUS3;
  bool gateOpen;
  int commonParkingCount = 3;
  int premiumParkingCount = 1;

  distanceUS1 = ultrasonic1.read();
  distanceUS2 = ultrasonic2.read();
  distanceUS3 = ultrasonic3.read();

  if (distanceUS1 <= 2.0 && commonParkingCount >= 1 && commonParkingCount <= 3){
    servoMotor.write(90);
    delay(1000);
    gateOpen=true;
  }

  if (distanceUS2 <= 2.0){
    servoMotor.write(90);
    delay(1000);
    gateOpen=true;
  }

  if (distanceUS3 >= 5.0 && gateOpen==true){
    delay(2000);
    servoMotor.write(0);
    gateOpen=false;
    delay(1000);
  }
}