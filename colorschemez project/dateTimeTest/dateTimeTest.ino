/*
    Benjamin Jones
    12/2/21

    Used following code as template for my implementation:
    https://github.com/arduino-libraries/Arduino_OAuth/blob/master/examples/GetTwitterStatus/GetTwitterStatus.ino
    dronebotworkshop.com (wifi tutorials)

    This code is for a christmas present, designed to present @colorschemez's
    latest tweet picture and text on a 2.4" TFT display. It is implemented
    with an Arduino ESP32 Feather, and the 2.4" TFT FeatherWing
*/


#include <WiFi.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include "time.h"
#include "tokens.h"

#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#define LEDPIN  13

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    delay(500);

    attemptConnection();
    Serial.println();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

}


void loop() {
    //Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
        attemptConnection();
    }
    //loadTweet();
    printLocalTime();
    delay(15000);
}


void attemptConnection() {
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        digitalWrite(LEDPIN, HIGH);
        Serial.print(".");
        delay(250);
        digitalWrite(LEDPIN, LOW);
    }
    Serial.println("");
    Serial.println("Wifi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LEDPIN, HIGH);

    delay(2000);
    Serial.println();
    Serial.println();
}


void loadTweet() {
    HTTPClient http;
    http.begin("https://www.google.com/");
    //http.begin("https://www.howsmyssl.com/a/check");

    //http.sendHeader();
    int httpCode = http.GET();
    //http.GET();

    if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
    } else {
        Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
}


void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //Serial.println(&timeinfo, "%Y/%m/%d, %H:%M:%S");
  String currentYear;
  strftime(currentYear, 80, "%Y", &timeinfo);
  Serial.println(currentYear);
  
}
