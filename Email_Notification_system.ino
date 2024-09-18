/*



*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32_MailClient.h>  // Include ESP32 Mail Client Library (Install this via the Arduino Library Manager)

const char *ap_ssid = "GenZonix";  // AP SSID
const char *ap_password = "thanksforyoursupport";        // AP Password

const int irSensorPin = 13;  // Example pin for IR sensor

// WebServer on port 80
WebServer server(80);

// Variables to store form input
String wifi_ssid = "";
String wifi_password = "";
String sender_email = "";
String sender_password = "";
String receiver_email = "";

// SMTP Mail Setup (Use your SMTP provider or Gmail's)
SMTPData smtpData;

// Handle the root HTML form
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>WiFi & Email Setup</title></head><body>";
  html += "<h1>WiFi & Email Setup</h1>";
  html += "<form action=\"/save\" method=\"post\">";
  html += "WiFi Name (SSID): <input type=\"text\" name=\"ssid\"><br>";
  html += "WiFi Password: <input type=\"password\" name=\"password\"><br>";
  html += "Sender Email Address: <input type=\"email\" name=\"sender_email\"><br>";
  html += "Sender Email Password: <input type=\"password\" name=\"sender_password\"><br>";
  html += "Receiver Email Address: <input type=\"email\" name=\"receiver_email\"><br>";
  html += "<input type=\"submit\" value=\"Submit\">";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

// Handle form submission
void handleSave() {
  // Capture data from the form
  wifi_ssid = server.arg("ssid");
  wifi_password = server.arg("password");
  sender_email = server.arg("sender_email");
  sender_password = server.arg("sender_password");
  receiver_email = server.arg("receiver_email");

  // Respond to user
  server.send(200, "text/html", "<h1>Configuration Saved! ESP32 is connecting to WiFi...</h1>");

  // Connect to WiFi
  WiFi.softAPdisconnect(true);
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 20) {
    delay(500);
    Serial.print(".");
    count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.println("IP Address: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nFailed to connect.");
  }
}

// Function to send an email
void sendEmail() {
  smtpData.setLogin("smtp.gmail.com", 465, sender_email.c_str(), sender_password.c_str()); // Change to your SMTP server
  smtpData.setSender("ESP32 Security", sender_email.c_str());
  smtpData.setPriority("High");
  smtpData.setSubject("Security Alert");
  smtpData.setMessage("Movement detected!", false);
  smtpData.addRecipient(receiver_email.c_str());

  // Send the email
  if (!MailClient.sendMail(smtpData)) {
    Serial.println("Error sending email, try again later.");
  } else {
    Serial.println("Email sent successfully!");
  }

  smtpData.empty();  // Clear the message buffer to free memory
}

void setup() {
  Serial.begin(115200);

  // Set IR sensor pin as input
  pinMode(irSensorPin, INPUT);

  // Start ESP32 as an access point
  WiFi.softAP(ap_ssid, ap_password);

  // Start the web server
  server.on("/", handleRoot);   // Serve the root page
  server.on("/save", handleSave);  // Handle form submissions
  server.begin();
  Serial.println("HTTP server started");

  // Print AP IP
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();  // Handle web server requests

  // Check if IR sensor is triggered
  if (digitalRead(irSensorPin) == HIGH) {
    Serial.println("Movement detected!");
    sendEmail();  // Send email alert when motion is detected
    delay(10000);  // Delay to prevent sending too many emails
  }
}
