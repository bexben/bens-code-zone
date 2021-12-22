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
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "time.h"
#include "tokens.h"

#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#define LEDPIN   13

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 0;


void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    delay(500);

    attemptConnection();
    Serial.println();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


void loop() {
    //Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
        attemptConnection();
    }
    loadTweet();
    delay(60000);
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

/*

https://api.twitter.com/2/tweets/search/recent?query=from%3Acolorschemez%20&start_time=2021-12-21T10%3A00%3A00-05%3A00
*/

void loadTweet() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
      return;
    }

    char queryMidChar[50];
    strftime(queryMidChar, 50, "%Y-%m-%dT%H", &timeinfo);
    String queryMid = String(queryMidChar);

    String queryStart = "https://api.twitter.com/2/tweets/search/recent?query=from%3Acolorschemez%20&start_time=";
    String queryEnd = "%3A00%3A00-05%3A00";

    HTTPClient http;
    http.setAuthorizationType("Bearer");
    http.begin(queryStart + queryMid + queryEnd);
    http.setAuthorization(BEARER_TOKEN);
    int httpCode = http.GET();

    if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(error.c_str());
            return;
        }

        const char* tweetChar = doc["data"][0]["text"];
        int textLen = strlen(tweetChar) - 23; //23 is length of url at end of tweet with null character
        char tweet[textLen];
        char url[23];
        Serial.println(tweetChar[0]);
        Serial.println(tweetChar);
        Serial.println(strlen(tweetChar));
        for (int i = 0; i < textLen; i++) {
            tweet[i] = tweetChar[i];
            Serial.println(tweet);
        }
        for (int i = textLen; i < strlen(tweetChar)+1; i++) {
            url[i-textLen] = tweetChar[i];
            Serial.println(url);
        }
        Serial.println(tweet);
        Serial.println(url);
    } else {
        Serial.println("Error on HTTP request");
        Serial.println(httpCode);
    }
    http.end(); //Free the resources
}
