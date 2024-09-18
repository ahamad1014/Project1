/*
Open a new sketch file
Goto File -> Preference -> Additional board manager URL:
[paste this URL]
http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://dl.espressif.com/dl/package_esp32_index.json
Install esp8266 on board manager.
Tools -> board -> ESP8266 -> LONIN(WEMOS)D1 Mini(clone)
Choose the COM port -> eg., COM5


->sign_in to your sender email google account.
->Goto Security -> Enable 2-step verification
->In search bar enter "app_password" and click it.
->create App Name,Eg., ESP_mail -> it shown 16_digit password "copy and paste the[sender_app_password]"
*/


#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>
//Ir_sensor_pin
#define Ir_pin D5    //define the IR_sensor Pin connect to the D5 pin


#define WiFi_SSID "your wifi name"     //replace your wifi name
#define WiFi_pwd "wifi password"    //replace your wifi password
#define sender_mail "sender@gmail.com"    //replace your email address
#define sender_app_password "elar krhu xwzs iezb" //follow the Gmail_Auth_steps/refer our project1 document.
#define receiver_mail "receiver@gmail.com"    //replace the receiver mail address to get notification.


SMTPSession smtp;


void setup() {
    Serial.begin(115200);
    pinMode(Ir_pin, INPUT);
    WiFi.begin(WiFi_SSID, WiFi_pwd);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
    Serial.println("IP Address: " + WiFi.localIP().toString());
    sendMail();
}


void loop() {
  int sensor=digitalRead(Ir_pin);
  if(sensor==1){
    sendMail();
    delay(10000);
  }else{
    Serial.println("All are safe...");
  }
  Serial.flush();
}


void sendMail(){
  Session_Config config;
  config.server.host_name = "smtp.gmail.com";
  config.server.port = 587;
  config.login.email = sender_mail;
  config.login.password = sender_app_password;


  SMTP_Message message;
  // Set the message content
  message.sender.name = "Motion Security Notification System...";
  message.sender.email = sender_mail;
  message.subject = "Security Alert";
  message.addRecipient("Receiver", receiver_mail);
  message.text.content = "Movement detected!";


  // Set SMTP server settings
  smtp.callback([](SMTP_Status status) {
    Serial.println(status.info());
  });


   if (!smtp.connect(&config))
  {
    MailClient.printf("Connection error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }


  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending email, try again later.");
  } else {
    Serial.println("Email sent successfully!");
  }


}
