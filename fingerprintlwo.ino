/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <Servo.h>
#include <EEPROM.h>

// Define GSM module pins
#define GSM_TX_PIN 12 // Connect GSM module TX to Arduino pin 2
#define GSM_RX_PIN 13 // Connect GSM module RX to Arduino pin 3

SoftwareSerial mySerial(7, 2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo myservo;
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;

int sensor1 = 8;
int sensor2 = 9;
int sensor3= 10;
int sensor4 = 11;

int val1 = 0;
int val2 = 0;
int val3 = 0;
int val4 = 0;

unsigned long previousMillis = 0;
const unsigned long uploadInterval = 180000; // 3 minutes
String apn = "safaricom.com";
String url = "http://dialapadafrica.co.ke/mpesa/stk.php/";
String apiURL = "http://dialapadafrica.co.ke/mpesa/queryapi.php";

void setup() {
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  
 myservo.attach(1);
  myservo.write(180);
  myservo1.attach(3);
  myservo1.write(180);
  myservo2.attach(4);
  myservo2.write(180);
  myservo3.attach(5);
  myservo3.write(180);
 myservo4.attach(6);
  myservo4.write(180);

  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop() {
  val1 = digitalRead(sensor1);
  Serial.print(val1);
  
  val3 = digitalRead(sensor3);
  Serial.print(val3);
  val4 = digitalRead(sensor4);
  Serial.print(val4);


  // Check if the sensor is triggered (finger placed)

    // Attempt to read the fingerprint
    uint8_t fingerprintID = getFingerprintID();

    // If a fingerprint is detected and it's within the valid range
    if (fingerprintID != FINGERPRINT_NOTFOUND) {
      Serial.println("Fingerprint detected. Pick pad.");

      // Check if the fingerprint matches any stored templates
      if (finger.fingerID >= 1 && finger.fingerID <= 127 && val1 == LOW &&  val3 == LOW && val4==LOW ) {
        Serial.println("Fingerprint matched.");
        myservo4.write(90);
        Serial.println("CHAMBER 1");  // Assuming this is your servo control
        delay(6000);         // Delay for 6 seconds
        myservo4.write(180);   // Reset servo position
      } 
      else if (finger.fingerID >= 1 && finger.fingerID <= 127 && val1 == HIGH &&  val3 == LOW && val4==LOW ) {
        Serial.println("Fingerprint matched.");
        myservo2.write(90);
        Serial.println("CHAMBER 1");  // Assuming this is your servo control
        delay(6000);         // Delay for 6 seconds
        myservo2.write(180);   // Reset servo position
      }
      else if (finger.fingerID >= 1 && finger.fingerID <= 127 && val1 == HIGH &&  val3 == HIGH && val4==LOW ) {
        Serial.println("Fingerprint matched.");
        myservo.write(90);
        Serial.println("CHAMBER 2");  // Assuming this is your servo control
        delay(6000);         // Delay for 6 seconds
        myservo.write(180);   // Reset servo position
      }
      else {
        Serial.println("Fingerprint not recognized.");
      }
    } else {
      Serial.println("No valid fingerprint detected.");
    }
  
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println("No finger detected");
    return FINGERPRINT_NOTFOUND;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Imaging error");
    return FINGERPRINT_NOTFOUND;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    Serial.println("Fingerprint not found");
    return FINGERPRINT_NOTFOUND;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
void uploadData() {
  // Read data from EEPROM and send it to server API using GSM AT commands
  for (int i = 0; i < EEPROM.length(); ++i) {
    char data = EEPROM.read(i);
    Serial.print(data);
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= uploadInterval) {
    previousMillis = currentMillis;
   // uploadData();
  }

  delay(500);
}
