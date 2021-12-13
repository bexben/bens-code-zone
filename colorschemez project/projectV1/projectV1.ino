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
#include <WiFiClientSecure.h>
//#include <ArduinoECCX08.h>
#include <ArduinoBearSSL.h>
#include <ArduinoHttpClient.h>
#include <Arduino_OAuth.h>
#include <Arduino_JSON.h>
#include <time.h>
#include "tokens.h"

#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#define LEDPIN  13


int status = WL_IDLE_STATUS;

WiFiClientSecure client;
OAuthClient oauthClient(client, "https://api.twitter.com", 443);
String twitterHandle = "colorschemez";

void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    delay(500);

    attemptConnection();
    Serial.println();

    oauthClient.setCredentials(CONSUMER_KEY, CONSUMER_KEY_SECRET, ACCESS_TOKEN, ACCESS_TOKEN_SECRET);
    oauthClient.onGetTime(getTime);
}


unsigned long getTime() {
    time_t now;
    struct tm timeinfo;

    while (!getLocalTime(&timeinfo)) {
        Serial.print(".");
    }

    Serial.println();
    time(&now);
    return now;
}


void loop() {
    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
     attemptConnection();
    }

    loadTweet();

    delay(5000);
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
    Serial.println("test1");
    oauthClient.get("/2/users/3045203078/tweets");

    Serial.println("test2");
    int statusCode = oauthClient.responseStatusCode();
    String response = oauthClient.responseBody();
    Serial.println("test3");

    if (statusCode != 200) {
        Serial.println(statusCode);
        Serial.println(response);
    } else {
        JSONVar statusesObject = JSON.parse(response);

        Serial.print("@");
        Serial.print(twitterHandle);
        Serial.println("'s twitter status: ");

        Serial.println(statusesObject[0]["text"]);
        Serial.print("Retweets: ");
        Serial.println(statusesObject[0]["retweet_count"]);
        Serial.print("Likes: ");
        Serial.println(statusesObject[0]["favorite_count"]);
    }
}
