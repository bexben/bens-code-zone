#include <WiFi.h>
//#include <ArduinoECCX08.h>
//#include <ArduinoBearSSL.h>
#include <ArduinoHttpClient.h>
#include <Arduino_JSON.h>
#include <tokens.h>

#define led = 13;

void setup {
  Serial.begin(921600);
  pinMode(led, OUTPUT);
  attemptConnection();
  httpsBegin();
}


void loop {
   // Wifi shit
   Serial.println(WiFi.status());
   if (WiFi.status() != WL_CONNECTED) {
     attemptConnection();
     httpsBegin();
   }

   // Retrieving data from twitter API


   delay(5000);
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


void httpsBegin() {
  HTTPClient http;
  Serial.println("HTTPS Begin...");

  http.begin
}
