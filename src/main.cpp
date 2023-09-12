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
#include <BlynkSimpleEsp32.h>


//blynk conf
#define BLYNK_TEMPLATE_ID "TMPL40BYmBR4x"
#define BLYNK_TEMPLATE_NAME "Smart Parking"
#define BLYNK_AUTH_TOKEN "3IjcRYKXAjF6sTcBB_uMkHJvXTdcDPuR"

// wifi
const char* ssid = "Arbri";
const char* password = "ardar123";

//mysql db
IPAddress mysqlServerAddress(127, 0, 0, 1);      // MySQL host address
char user[] = "root";  // MySQL username
char password_db[] = "";  // MySQL password
char db[] = "iot_members";     // MySQL database name

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

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  pinMode(irSensor1Pin, INPUT);
  pinMode(irSensor2Pin, INPUT);
  pinMode(irSensor3Pin, INPUT);
  pinMode(irSensor4Pin, INPUT);

  SPI.begin();
  mfrc522.PCD_Init();
  if (conn.connect(mysqlServerAddress, 3306, user, password_db, db)) {
    Serial.println("Connected to MySQL Server");
  } else {
    Serial.println("Connection to MySQL server failed");
    while (1);
  }
  servoMotor.attach(SERVO_PIN); //attach servo pin
  servoMotor.write(0); // Initialize the servo position to 0 degrees
  // Initialize my custom Wire instance
  myWire.begin(CUSTOM_SDA_PIN, CUSTOM_SCL_PIN);

}

void loop() {
  Blynk.run();
  double distanceUS1, distanceUS2, distanceUS3;
  bool gateOpen=false;

  distanceUS1 = ultrasonic1.read();
  distanceUS2 = ultrasonic2.read();
  distanceUS3 = ultrasonic3.read();

  //common infrared sensors
  int sensor1Value=digitalRead(irSensor1Pin);
  int sensor2Value=digitalRead(irSensor2Pin);
  int sensor3Value=digitalRead(irSensor3Pin);

  int highCommonSensorCount = 0;

  if(sensor1Value == HIGH) {
    highCommonSensorCount++;
  }
  if(sensor2Value == HIGH) {
    highCommonSensorCount++;
  }
  if(sensor3Value == HIGH) {
    highCommonSensorCount++;
  }


  int commonParkingCount = 3 - highCommonSensorCount;

  //premium infrared sensors
  int sensor4Value=digitalRead(irSensor4Pin);
  int highPremiumSensorCount = 0;
  if(sensor4Value == HIGH) {
    highPremiumSensorCount++;
  }

  int premiumParkingCount = 1 - highPremiumSensorCount;

  Blynk.virtualWrite(V5, sensor1Value);
  Blynk.virtualWrite(V6, sensor2Value);
  Blynk.virtualWrite(V7, sensor3Value);
  Blynk.virtualWrite(V8, sensor4Value);

  if (distanceUS1 <= 2.0 && commonParkingCount >= 1 && commonParkingCount <= 3 && gateOpen==false){
    servoMotor.write(90);
    delay(1000);
    gateOpen=true;
  }

  if (distanceUS2 <= 2.0 && gateOpen==false){
    servoMotor.write(90);
    delay(1000);
    gateOpen=true;
  }

  // Read card UID
  String cardUID = "";
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    
    // Check if card UID exists in the database
    MySQL_Cursor *cursor = new MySQL_Cursor(&conn);
    String query = "SELECT * FROM parking_members WHERE card_uid = '" + cardUID + "'";
    cursor->execute(query.c_str());

    // Check if there are any results
    row_values *row = NULL;

    row = cursor->get_next_row();
    if (row != NULL && premiumParkingCount==1 && gateOpen==false){
      // UID found in database
      delete cursor;
      servoMotor.write(90);
      gateOpen = true;
    }
    else{
      //UID  not found in database
      delete cursor;
    }
  }
  mfrc522.PICC_HaltA();    
  delay(2000);

  if (distanceUS3 >= 5.0 && gateOpen==true){
    delay(2000);
    servoMotor.write(0);
    gateOpen=false;
    delay(1000);
  }
}
