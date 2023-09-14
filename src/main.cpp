//blynk conf
#define BLYNK_TEMPLATE_ID "TMPL40BYmBR4x"
#define BLYNK_TEMPLATE_NAME "Smart Parking"
#define BLYNK_AUTH_TOKEN "3IjcRYKXAjF6sTcBB_uMkHJvXTdcDPuR"

#include <Arduino.h>
#include <Ultrasonic.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
// #include <U8g2lib.h>
#include <SPI.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>



// wifi
const char* ssid = "Arbri";
const char* password = "ardar123";

const char* server = "localhost";
const int serverPort = 8080;


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
// const int irSensor1Pin = 26;
// const int irSensor2Pin = NaN;
// const int irSensor3Pin = 32;
// const int irSensor4Pin = 33;


// //oled sh1106 display definitions
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// #define CUSTOM_SDA_PIN 14
// #define CUSTOM_SCL_PIN 15

// // Create an instance of the U8g2 SH1106 display library with your custom Wire instance
// U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ CUSTOM_SCL_PIN, /* data=*/ CUSTOM_SDA_PIN, /* reset=*/ U8X8_PIN_NONE);

int commonParkingCount=3;
int premiumParkingCount=1;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  // u8g2.begin();
  // u8g2.setFont(u8g2_font_ncenB10_tr);

  // Clear the display buffer
  // u8g2.clearBuffer();

  while (!Serial);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  // pinMode(irSensor1Pin, INPUT);
  // pinMode(irSensor2Pin, INPUT);
  // pinMode(irSensor3Pin, INPUT);
  // pinMode(irSensor4Pin, INPUT);

  servoMotor.attach(SERVO_PIN); //attach servo pin
  servoMotor.write(90); // Initialize the servo position to 0 degrees

}

void loop() {
  Blynk.run();
  double distanceUS1, distanceUS2, distanceUS3;
  bool gateOpen=false;

  // u8g2.clearBuffer();


  distanceUS1 = ultrasonic1.read();
  distanceUS2 = ultrasonic2.read();
  distanceUS3 = ultrasonic3.read();

  // //common infrared sensors
  // int sensor1Value=digitalRead(irSensor1Pin);
  // int sensor2Value=digitalRead(irSensor2Pin);
  // int sensor3Value=digitalRead(irSensor3Pin);

  // int highCommonSensorCount = 0;

  // if(sensor1Value == HIGH) {
  //   highCommonSensorCount++;
  // }
  // if(sensor2Value == HIGH) {
  //   highCommonSensorCount++;
  // }
  // if(sensor3Value == HIGH) {
  //   highCommonSensorCount++;
  // }


  // commonParkingCount = 3 - highCommonSensorCount;

  // //premium infrared sensors
  // int sensor4Value=digitalRead(irSensor4Pin);

  // int highPremiumSensorCount = 0;

  // if(sensor4Value == HIGH) {
  //   highPremiumSensorCount++;
  // }

  // premiumParkingCount = 1 - highPremiumSensorCount;

  // Blynk.virtualWrite(V5, sensor1Value);
  // Blynk.virtualWrite(V6, sensor2Value);
  // Blynk.virtualWrite(V7, sensor3Value);
  // Blynk.virtualWrite(V8, sensor4Value);


  // // Create a message with the updated variables
  // char message[100];
  // snprintf(message, sizeof(message), "%d / 3 vende normale te lira.\n%d / 1 vende premium te lira.", commonParkingCount, premiumParkingCount);

  // Draw the message on the screen
  // u8g2.drawStr(0, 20, "Parking Status:");
  // u8g2.drawStr(0, 40, message);

  // Send the buffer to the OLED display
  // u8g2.sendBuffer();

  // delay(1000);

  //open gate when entering
  if (distanceUS1 <= 2.0 && commonParkingCount >= 1 && commonParkingCount <= 3 && gateOpen==false){
    servoMotor.write(20);
    gateOpen=true;
    delay(1000);
  }

  //open gate when leaving
  if (distanceUS2 <= 2.0 && gateOpen==false){
    servoMotor.write(20);
    gateOpen=true;
    delay(1000);
  }

  // Read card UID
  String rfid_uid = "";
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfid_uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      rfid_uid += String(mfrc522.uid.uidByte[i], HEX);
    }
  }
  
  Serial.println("Scanned UID: " + rfid_uid);

  //open gate using rfid tag (premium membership)
  if (checkUID(rfid_uid) && premiumParkingCount==1) {
    Serial.println("UID exists in the database");
    servoMotor.write(20);
    gateOpen=true;
    delay(1000);
  }

  mfrc522.PICC_HaltA();

  //close gate (wait if the car is below the gate)
  if (distanceUS3 >= 5.0 && gateOpen==true){
    delay(2000);
    servoMotor.write(125);
    gateOpen=false;
    delay(1000);
  }
}


bool checkUID(String uid) {
  WiFiClient client;
  
  if (client.connect(server, serverPort)){
    String postRequest = "POST /iot_fiek/check_rfid.php HTTP/1.1\r\n";
    postRequest += "Host: " + String(server) + "\r\n";
    postRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
    postRequest += "Content-Length: " + String(uid.length()) + "\r\n\r\n";
    postRequest += "rfid_uid=" + uid;
    
    client.print(postRequest);

    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        line.trim();
        if (line == "true") {
          return true; //UID exists in the db
        }
      }
    }

    client.stop();
  } else {
    Serial.println("Failed to connect to server");
  }
  return false; //UID does not exist in the db
}
