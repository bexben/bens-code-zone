#include <WiFi.h>
#include <ArduinoECCX08.h>
#include <ArduinoBearSSL.h>
#include <ArduinoHttpClient.h>
#include <Arduino_OAuth.h>
#include <Arduino_JSON.h>

#define led = 13;

const char* ssid = "" // enter SSID
const char* pass = "" // enter password

void setup {
  Serial.begin(921600);
  pinMode(led, OUTPUT);
  attemptConnection();
}

void loop {
   // Wifi shit
   delay(1000);
   Serial.println(WiFi.status());
   if (WiFi.status() != WL_CONNECTED) {
     attemptConnection();
   }



}

void attemptConnection() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(led, HIGH);
    Serial.print(".");
    delay(250);
    digitalWrite(led, LOW);
  }

  Serial.println("");
  Serial.println("Wifi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(led, HIGH);
}
