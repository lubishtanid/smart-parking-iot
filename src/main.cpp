//blynk conf
#define BLYNK_TEMPLATE_ID "TMPL40BYmBR4x"
#define BLYNK_TEMPLATE_NAME "Smart Parking"
#define BLYNK_AUTH_TOKEN "3IjcRYKXAjF6sTcBB_uMkHJvXTdcDPuR"


#include <Arduino.h>
#include <Ultrasonic.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>


// wifi
const char* ssid = "Arbri";
const char* password = "ardar123";


//defining ultrasonic sensor pins
Ultrasonic ultrasonic1(17, 16); //Trig pin: 17, Echo Pin: 16
Ultrasonic ultrasonic2(4, 26); //Trig pin: 4, Echo Pin: 26
Ultrasonic ultrasonic3(33, 25); //Trig pin: 33 Echo Pin: 25


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


//photoresistor pins
const int commonPhotoresistorPin1 = 34;
const int commonPhotoresistorPin2 = 35;
const int commonPhotoresistorPin3 = 36;
const int premiumPhotoresistorPin = 39;


//U8g2 display object with custom pins
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


int commonParkingCount=3;
int premiumParkingCount=1;

bool gateOpen;


void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  Wire.begin();
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);  //font size


  // Clear the display buffer
  u8g2.clearBuffer();

  while (!Serial);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");



  servoMotor.attach(SERVO_PIN); //attach servo pin
  servoMotor.write(127); // Initialize the servo position to 127 degrees
}


void loop() {
  Blynk.run();  
  double distanceUS1, distanceUS2, distanceUS3;

  u8g2.clearBuffer();


  distanceUS1 = ultrasonic1.read();
  Serial.print("Ultrasonic sensor 1 distance: ");
  Serial.println(distanceUS1);

  distanceUS2 = ultrasonic2.read();
  Serial.print("Ultrasonic sensor 2 distance: ");
  Serial.println(distanceUS2);

  distanceUS3 = ultrasonic3.read();
  Serial.print("Ultrasonic sensor 3 distance: ");
  Serial.println(distanceUS3);


  //common photoresistors
  int photoresistorValue1=analogRead(commonPhotoresistorPin1);
  Serial.print("Common photoresistor1 value: ");
  Serial.println(photoresistorValue1);

  int photoresistorValue2=analogRead(commonPhotoresistorPin2);
  Serial.print("Common photoresistor2 value: ");
  Serial.println(photoresistorValue2);
  
  int photoresistorValue3=analogRead(commonPhotoresistorPin3);
  Serial.print("Common photoresistor3 value: ");
  Serial.println(photoresistorValue3);

  int lowCommonResistorCount = 0;

  if(photoresistorValue1 <= 1400) {
    lowCommonResistorCount++;
    Blynk.virtualWrite(V5, LOW);
  }
  else{
    Blynk.virtualWrite(V5, HIGH);
  }
  if(photoresistorValue2 <= 1400) {
    lowCommonResistorCount++;
    Blynk.virtualWrite(V6, LOW);
  }
  else{
    Blynk.virtualWrite(V6, HIGH);
  }
  if(photoresistorValue3 <= 1400) {
    lowCommonResistorCount++;
    Blynk.virtualWrite(V7, LOW);
  }
  else{
    Blynk.virtualWrite(V7, HIGH);
  }


  commonParkingCount = 3 - lowCommonResistorCount;

  Serial.print("Common parking count: ");
  Serial.println(commonParkingCount);

  //premium infrared sensors
  int photoresistorValue4=analogRead(premiumPhotoresistorPin);
  Serial.print("Premium photoresistor value: ");
  Serial.println(photoresistorValue4);

  int lowPremiumResistorCount = 0;

  if(photoresistorValue4 <= 1400) {
    lowPremiumResistorCount++;
    Blynk.virtualWrite(V8, LOW);
  }
  else{
    Blynk.virtualWrite(V8, HIGH);
  }

  premiumParkingCount = 1 - lowPremiumResistorCount;
  Serial.print("Premium parking count: ");
  Serial.println(premiumParkingCount);


  // Message to display parking slots availability
  char commonMessage[50];
  snprintf(commonMessage, sizeof(commonMessage), "%d / 3 vende normale", commonParkingCount);

  char premiumMessage[50];
  snprintf(premiumMessage, sizeof(premiumMessage), "%d / 1 vende premium", premiumParkingCount);

  // Draw the message on the screen
  u8g2.drawStr(0, 10, "Statusi i vendeve te");
  u8g2.drawStr(0, 20, "lira ne parking:");
  u8g2.drawStr(0, 40, commonMessage);
  u8g2.drawStr(0, 60, premiumMessage);

  // Send the buffer to the OLED display
  u8g2.sendBuffer();

  delay(1000);

  //open gate when entering
  if (distanceUS1 <= 2.0 && commonParkingCount >= 1 && commonParkingCount <= 3){
    servoMotor.write(30);
    gateOpen=true;
    delay(2000);
  }

  //open gate when leaving
  if (distanceUS2 <= 2.0){
    servoMotor.write(30);
    gateOpen=true;
    delay(2000);
  }

  // Read card UID
  String rfidUID = "";
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfidUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      rfidUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println("Scanned UID: " + rfidUID);

    //open gate using rfid tag (premium membership)
    if (checkUID(rfidUID) && premiumParkingCount==1) {
      Serial.println("UID exists in the database");
      delay(1000);
      servoMotor.write(30);
      gateOpen=true;
      delay(2000);
    }
  }
  

  mfrc522.PICC_HaltA();

  //close gate (wait if the car is below the gate)

  while (distanceUS3 >= 13.0 && gateOpen==true){
    delay(1000);
    servoMotor.write(127);
    gateOpen=false;
    break;
  }

}


bool checkUID(String rfid_uid) {
  HTTPClient http;

  String serverURL = "http://192.168.0.46:8080/iot_fiek/check_rfid.php"; // Replace with your server URL

  http.begin(serverURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "rfid_uid=" + rfid_uid;

  int httpCode = http.POST(postData);

  Serial.print("URL: ");
  Serial.println(serverURL);
  Serial.print("Data: ");
  Serial.println(postData);
  Serial.print("httpCode: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("HTTP Response: " + payload);

    if (payload == "true") {
      return true; // UID exists in the database
    }
  } else {
    Serial.println("Error sending POST request. HTTP Response code: " + String(httpCode));
  }

  http.end();
  return false; // UID does not exist in the database
}
