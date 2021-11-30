// Code edited from original for learning and testing purposes

/*
 Original code:
 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32
31.01.2017 by Jan Hendrik Berlin

 */

#include <WiFi.h>
#include <tokens.h>

int led = 13;

void setup() {
   Serial.begin(921600);
   pinMode(led, OUTPUT);
   attemptConnection();
}

void loop() {
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
