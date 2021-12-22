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


#include <WiFiClientSecure.h>
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
const char* server = "api.twitter.com";
//const char* server = "howsmyssl.com";

void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    delay(500);

    attemptConnection();
    Serial.println();

    client.setCACert(test_root_ca);
    //client.setPrivateKey(CONSUMER_KEY_SECRET);
}



void loop() {
    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
        attemptConnection();
    }

    loadTweet();
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
    if (!client.connect(server, 443)) {
        Serial.println("Connection failed!");
        Serial.println(client.connect(server, 443));
    } else {
        Serial.println("Connected to server!");

        //client.println("GET https://api.twitter.com/2/users/by/username/colorschemez HTTP/1.1");
        client.println("GET https://www.howsmyssl.com/a/check HTTP/1.1");
        client.println("Host: www.howsmyssl.com");
        client.println("Connection: close");
        client.println();

        while (client.connected()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
            Serial.println("headers received");
            break;
            }
        }
        // if there are incoming bytes available
        // from the server, read them and print them:
        while (client.available()) {
            char c = client.read();
            Serial.write(c);
        }
    }


    /*
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

    }*/
}
