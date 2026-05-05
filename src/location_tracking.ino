#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

// GPS Module RX pin to NodeMCU D1, TX pin to NodeMCU D2
const int GPS_RXPin = 4, GPS_TXPin = 5;
SoftwareSerial neo6m(GPS_RXPin, GPS_TXPin);

TinyGPSPlus gps;

// Wi-Fi and SMTP credentials
const char *ssid = "SWEETY 6915";
const char *password = "4b2P=688";
const char *smtp_server = "smtp.gmail.com";
const int smtp_port = 465; // or 587 for STARTTLS
const char *email_user = "klupro147@gmail.com"; // Replace with your Gmail address
const char *email_pass = "kmhs iomd lpuo soyc"; // Replace with your App Password
const char *recipient_email = "recipient@example.com"; // Replace with recipient's email

SMTPSession smtp;
SMTP_Message message;

void setup() {
  Serial.begin(115200);
  Serial.println();
  neo6m.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Read GPS data and send email
  smartdelay_gps(2000); // Wait for a while to get a GPS fix

  if (gps.location.isValid()) {
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);
    String msg = "Location: http://maps.google.com/maps?q=" + latitude + "," + longitude;

    sendEmail("GPS Coordinates", msg);
  } else {
    Serial.println("GPS location not valid");
  }
}

void loop() {
  smartdelay_gps(1000);
  // Add any other functionality you need here
}

void smartdelay_gps(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}

void sendEmail(String subject, String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    smtp.callback(smtpCallback);
    
    ESP_Mail_Session session;
    session.server.host_name = smtp_server;
    session.server.port = smtp_port;
    session.login.email = email_user;
    session.login.password = email_pass;

    // Set the email sender and receiver
    message.sender.name = "Sender";
    message.sender.email = email_user;
    message.subject = subject;
    message.addRecipient("Recipient", recipient_email);

    // Set the email content
    message.text.content = msg;
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

    // Send the email
    if (!MailClient.sendMail(&smtp, &message)) {
      Serial.println("Failed to send email");
    } else {
      Serial.println("Email sent successfully");
    }
  } else {
    Serial.println("WiFi not connected");
  }
}

void smtpCallback(SMTP_Status status) {
  if (status.success()) {
    Serial.println("Email sent successfully");
  } else {
    Serial.println("Email failed to send. Error: ");
    Serial.println(status.info());
  }		
}
