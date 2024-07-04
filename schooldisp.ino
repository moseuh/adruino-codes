#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <String.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

SoftwareSerial mySerial(7, 2);
volatile int finger_status = -1;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
Servo myservo;
Servo myservo1;
Servo myservo2;

int sensor1 = 9;
int sensor2 = 8;
int val = 0;
int val1 = 0;
int val2 = 0;
String apn = "safaricom.com";
String apn_u = "saf";
String apn_p = "data";
String url = "http://dialapadafrica.co.ke/mpesa/postdata.php/";
String apiURL = "http://dialapadafrica.co.ke/mpesa/queryapi.php";

void setup() {
  
   Serial.begin(9600);
 
  pinMode(sensor1, INPUT);
  myservo.attach(12);
  myservo1.attach(5);
  myservo2.attach(6);
  
  myservo.write(180);
  myservo1.write(180);
  myservo2.write(189);
  Serial.println("\n\nAdafruit finger detect test");
  finger.begin(9600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    
  }
  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void loop() {
  finger_status = getFingerprintIDez();
  if (finger_status != -1 && finger_status != -2) {
    Serial.print("Match");
    val1 = digitalRead(sensor1);
    if (finger.fingerID >= 1 && finger.fingerID <= 127) {
      Serial.println(" pick pad");
      myservo.write(90);
      delay(3000);
      myservo.write(180);
    } else {
      Serial.println("No finger detected");
    }
    if (finger.fingerID >= 1 && finger.fingerID <= 127) {
      Serial.println(finger.fingerID);
      Serial.println(" pick pad2");
      myservo1.write(90);
      delay(3000);
      myservo1.write(180);
    }
    if (finger.fingerID >= 1 && finger.fingerID <= 127 && val1 == HIGH && val == HIGH && val2 == LOW) {
      Serial.println(" CHAMBER 3");
      myservo2.write(90);
      delay(3000);
      myservo2.write(180);
    }
  } else {
    if (finger_status == -2) {
      for (int ii = 0; ii < 5; ii++) {
        Serial.print("Not Match");
      }
    }
  }
  
  
}


int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println(p);
    return -1;
  }
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println(p);
    return -1;
  }
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    return -2;
  }
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}


