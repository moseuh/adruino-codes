#include <SoftwareSerial.h>

SoftwareSerial A9GSerial(12,13); // RX, TX pins

void setup() {
  Serial.begin(9600);
  A9GSerial.begin(9600);
  delay(1000);
  // Disable command echoing
  sendATCommand("ATE0");
  delay(3000);
  // Set SMS message format to text mode
  sendATCommand("AT+CMGF=1");
  delay(3000);
  // Set the APN (Access Point Name) for GPRS connection
  sendATCommand("AT+CGDCONT=1,\"IP\",\"safaricom.com\"");
  delay(3000);
  // Connect to GPRS network
  sendATCommand("AT+CGATT=1");
  delay(3000);
  // Activate PDP context/
  sendATCommand("AT+CGACT=1,1");
  delay(3000);
}

void loop() {
  // Send POST request
  sendPOSTRequest();
  delay(5000); // Wait for the POST request to be processed
  
  // Send SMS after posting data
  sendSMS("+254711596697", "Data has been posted successfully.");
  delay(5000); // Adjust delay as needed
}

void sendATCommand(String command) {
  Serial.print("Sending AT command: ");
  Serial.println(command);
  
  A9GSerial.println(command);
  delay(1000); // Adjust delay if needed

  while (A9GSerial.available()) {
    char c = A9GSerial.read();
    Serial.write(c);
  }
  Serial.println();
}

bool establishConnection() {
  for (int i = 0; i < 5; i++) { // Retry connection 5 times
    A9GSerial.println("AT+CIPSTART=\"TCP\",\"www.dialapadafrica.co.ke\",80");
    delay(2000);
    if (A9GSerial.find("CONNECT OK")) {
      return true;
    } else {
      Serial.println("Failed to establish connection. Retrying...");
      delay(5000); // Wait before retrying
    }
  }
  Serial.println("Max retries reached. Failed to establish connection.");
  return false;
}

void sendPOSTRequest() {
  // Connect to server
  if (establishConnection()) {
    // Send POST request header
    String postData = "phone_number=0746778298&amount=10"; // Modify as per your requirements
    String postRequest = "POST /mpesa/stk.php HTTP/1.1\r\n";
    postRequest += "Host: dialapadafrica.co.ke\r\n";
    postRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
    postRequest += "Content-Length: " + String(postData.length()) + "\r\n";
    postRequest += "Connection: close\r\n\r\n";
    postRequest += postData;

    sendData(postRequest);
  }
}

void sendData(String data) {
  int len = data.length();
  A9GSerial.print("AT+CIPSEND=");
  A9GSerial.println(len);
  delay(1000);

  // Send data
  A9GSerial.print(data);

  // Wait for response
  delay(5000);

  // Print server response
  receiveResponse();

  // Close connection
  closeConnection();
}

void receiveResponse() {
  Serial.println("Server Response:");
  while (A9GSerial.available()) {
    char c = A9GSerial.read();
    Serial.write(c);
  }
  Serial.println();
}

void closeConnection() {
  A9GSerial.println("AT+CIPCLOSE");
  delay(2000);
}

void sendSMS(String phoneNumber, String message) {
  sendATCommand("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);
  A9GSerial.print(message);
  delay(100);
  A9GSerial.write(0x1A);
  delay(1000);
}
