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

#include <TwitterApi.h>
#include <WiFiClientSecure.h>
//#include <Arduino_JSON.h>
#include <time.h>
#include "tokens.h"

#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#define LEDPIN  13


int status = WL_IDLE_STATUS;

WiFiClientSecure client;
TwitterApi api(client);

unsigned long api_mtbs = 60000; //mean time between api requests
unsigned long api_lasttime = 0;   //last time api request has been done
bool firstTime = true;

String screenName = "bexben9";
bool haveBearerToken = false;

void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    delay(500);

    delay(2000);
    attemptConnection();
    Serial.println();

    api.setBearerToken(BEARER_TOKEN);
    haveBearerToken = true;
    loadTweet(screenName);
}



void loop() {
    //Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
        attemptConnection();
    }

    if (haveBearerToken) {
        if ((millis() > api_lasttime + api_mtbs)) {
            loadTweet(screenName);
            api_lasttime = millis();
        }
    }
    delay(1000);
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


void loadTweet(String name) {
    Serial.println("Getting twitter stats for " + name);
    String responseString = api.getUserStatistics(name);
    Serial.println(responseString);
    Serial.println("Test");

    DynamicJsonDocument doc(1024);
    auto error = deserializeJson(doc, responseString);
    if (error) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.print("failedhere");
        Serial.println(error.c_str());
        return;
    }
}
