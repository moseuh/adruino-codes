#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Servo.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; //R1,R2,R3,R4
byte colPins[COLS] = {6, 7, 8, 9}; //C1,C2,C3,C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String phoneNumber = "";
String amount = "10";

SoftwareSerial SWserial(10, 11);
Servo doorServo;  // Move servo declaration outside setup to make it global

String apn = "safaricom.com";
String url = "http://dialapadafrica.co.ke/mpesa/stk.php/";
String apiURL = "http://dialapadafrica.co.ke/mpesa/queryapi.php";
bool doorOpen = false;
unsigned long doorOpenStartTime = 0;

void setup() {
  closeDoor();
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  SWserial.begin(9600);
  pinMode(13, OUTPUT);   // Set pin 13 as output

  digitalWrite(13, LOW); // Initially keep the servo closed
  delay(15000);

  gsm_config_gprs();
  delay(5000);

  lcd.setCursor(0, 0);
  lcd.print("Enter Phone:");
  lcd.setCursor(0, 1);
  lcd.print(phoneNumber);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == '#') {
      lcd.setCursor(0, 0);
      lcd.print("Phone:        ");

      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Loading...");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      formatPhoneNumber(phoneNumber);

      String postData = "phone_number=" + phoneNumber + "&amount=" + amount;
      String postdata2 = "phone_number=" + phoneNumber;
long wtimer = millis();
      if (gsm_http_post(postData)) {
        String response = gsm_http_get(postdata2);

        delay(3000);

        if (response == "open") {
          openDoor();
          doorOpenStartTime = millis();
          doorOpen = true;
        }
      }

      phoneNumber = "";
      amount = "10";

      lcd.setCursor(0, 1);
      lcd.print("Done!       ");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Phone:");
    } else if (isDigit(key)) {
      if (phoneNumber.length() < 13) {
        phoneNumber += key;
        lcd.setCursor(phoneNumber.length() - 1, 1);
        lcd.print(key);
      }
    } else if (key == 'C') {
      phoneNumber = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
    } else if (isDigit(key)) {
      if (phoneNumber.length() < 13) {
        phoneNumber += key;
        lcd.setCursor(phoneNumber.length() - 1, 1);
        lcd.print(key);
      }
    }
  }

  if (doorOpen && millis() - doorOpenStartTime >= 10000) {
    closeDoor();
    doorOpen = false;
  }
}

bool gsm_http_post(String postdata) {
  Serial.println(" --- Start GPRS & HTTP --- ");
  gsm_send_serial("AT+SAPBR=1,1");
  delay(5000);
  gsm_send_serial("AT+SAPBR=2,1");
  gsm_send_serial("AT+HTTPINIT");
  gsm_send_serial("AT+HTTPPARA=CID,1");
  gsm_send_serial("AT+HTTPPARA=URL," + url);
  gsm_send_serial("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  gsm_send_serial("AT+HTTPDATA=" + String(postdata.length()) + ",6000");
  gsm_send_serial(postdata);
  gsm_send_serial("AT+HTTPACTION=1");

  gsm_send_serial("AT+HTTPREAD");
  gsm_send_serial("AT+HTTPTERM");
  gsm_send_serial("AT+SAPBR=0,1");

  return true;
}

void formatPhoneNumber(String &number) {
  number.replace(" ", "");

  if (number.startsWith("07")) {
    number.replace("07", "2547");
  } else if (number.startsWith("011")) {
    number.replace("011", "25411");
  }
}

void gsm_config_gprs() {
  //Serial.print("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
  SWserial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(5000);
  ShowSerialData();
  delay(5000);

//Serial.print("AT+SAPBR=3,1,\"APN\",\"safaricom.com\"\r\n");
  SWserial.println("AT+SAPBR=3,1,\"APN\",\"safaricom.com\"");
  delay(5000);
  

 // Serial.print("AT+SAPBR=1,1\r\n");
  SWserial.println("AT+SAPBR=1,1");
  ShowSerialData();
  delay(5000);
 SWserial.println("AT+CREG?");
  
  ShowSerialData();
  delay(5000);
  SWserial.println("AT+CSQ");
  
  ShowSerialData();
  delay(5000);
  
}

void ShowSerialData() {
  while (SWserial.available() != 0)
    Serial.write(char(SWserial.read()));
}

String gsm_http_get(String postdata2) {
  Serial.println(" --- SENDING HTTP GET REQUEST2 --- ");
  gsm_send_serial1("AT+SAPBR=1,1");
  gsm_send_serial1("AT+SAPBR=2,1");
  gsm_send_serial1("AT+HTTPINIT");
  gsm_send_serial1("AT+HTTPPARA=CID,1");
  gsm_send_serial1("AT+HTTPPARA=URL," + apiURL);
  gsm_send_serial1("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  gsm_send_serial1("AT+HTTPDATA=" + String(postdata2.length()) + ",5000");
  gsm_send_serial1(postdata2);
  gsm_send_serial1("AT+HTTPACTION=1");
  delay(5000);

  String line = gsm_send_serial1("AT+HTTPREAD");

  String response = "";

  if (line.indexOf("open") != -1) {
    response = "open";
  } else {
    response = line;
  }

  gsm_send_serial("AT+HTTPTERM");
  gsm_send_serial("AT+SAPBR=0,1");

  return response;
}

String gsm_send_serial1(String command) {
  Serial.println("Send ->: " + command);
  SWserial.println(command);
  long wtimer = millis();
  String response = "";

  while (wtimer + 3000 > millis()) {
    while (SWserial.available()) {
      char c = SWserial.read();
      response += c;
      Serial.write(c);
    }
  }

  Serial.println();
  return response;
}

void sendCommand(String command) {
  Serial.println("Sending command: " + command);
  SWserial.println(command);
  delay(3000);
  ShowSerialData();
}

void gsm_send_serial(String command) {
  Serial.println("Send ->: " + command);
  SWserial.println(command);
  long wtimer = millis();
  while (wtimer + 3000 > millis()) {
    while (SWserial.available()) {
      Serial.write(SWserial.read());
    }
  }
  Serial.println();
}

void openDoor() {
  doorServo.attach(13);  // Attach servo to pin 13
  doorServo.write(90);
  digitalWrite(13, HIGH);
  delay(6000);  // Keep the door open for 6 seconds

  // Detach the servo after keeping the door open
  doorServo.detach();
  digitalWrite(13, LOW);
}

void closeDoor() {
  doorServo.attach(13);  // Attach servo to pin 13
  doorServo.write(0);
  delay(2000);  // Allow 2 seconds for the door to close

  // Detach the servo after closing the door
  doorServo.detach();
  digitalWrite(13, LOW);
}

