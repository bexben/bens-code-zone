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
#include "tokens.h" //contains ssid, pass, and BEARER_TOKEN

// Adafruit ESP32 feather pinouts for 2.4" TFT featherwing
#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#define LEDPIN   13

// Time values for EST no DST, -5:00 GMT
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 0;


void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    delay(500);

    // Connect to WiFi
    attemptConnection();
    Serial.println();

    // Setting time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


void loop() {
    //Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
        // If WiFi is disconnected, attempt to connect
        attemptConnection();
    }

    String imageUrl, text;
    loadTweet(&imageUrl, &text);
    Serial.println(imageUrl);
    Serial.println(text);
    Serial.println(ESP.getFreeHeap());
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
Example query, uses ISO 8601 format
https://api.twitter.com/2/tweets/search/recent?query=from%3Acolorschemez%20&start_time=YYYY-MM-DDTHH%3A00%3A00-05%3A00&expansions=attachments.media_keys&media.fields=url";
*/

void loadTweet(String *url, String *tweet) {
    // Getting time from time.h struct
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
      return;
    }

    // Setting custom time. Queries the past hour for tweets.
    char queryMidChar[50];
    strftime(queryMidChar, 50, "%Y-%m-%dT%H", &timeinfo);
    String queryMid = String(queryMidChar);

    // Rest of Query
    String queryStart = "https://api.twitter.com/2/tweets/search/recent?query=from%3Acolorschemez%20&start_time=";
    String queryEnd = "%3A00%3A00-05%3A00&expansions=attachments.media_keys&media.fields=url";

    // Sending HTTP request with authorization
    HTTPClient http;
    http.setAuthorizationType("Bearer");
    http.begin(queryStart + queryMid + queryEnd);
    http.setAuthorization(BEARER_TOKEN);
    int httpCode = http.GET();

    if (httpCode > 0) { //Check for the returning code
        // Get HTTP response payload
        String payload = http.getString();

        // Setting up JSON deserialization
        // Uses ArduinoJson, tutorials online
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(error.c_str());
            return;
        }

        // Code provided by ArduinoJson assistant
        JsonObject data_0 = doc["data"][0];
        const char* tweetChar = data_0["text"];
        JsonObject includes_media_0 = doc["includes"]["media"][0];
        const char* urlChar = includes_media_0["url"];

        // This section is for removing the extraneous URL that exists at
        // the end of the tweet for some reason.
        int textLen = strlen(tweetChar) - 23; //23 is length of url at end of tweet with null character
        char tweetStr[textLen];
        for (int i = 0; i < textLen; i++) {
            tweetStr[i] = tweetChar[i];
        }

        // This section is for changing the PNG to JPG. Only JPG can be handled
        // by arduino processors due to necessary computational capacities.
        // All URLs are the same # of characters, which makes this easy.
        char urlStr[48];
        for (int i = 0; i < 48; i++) {
            urlStr[i] = urlChar[i];
        }
        // png to jpg
        urlStr[44] = 'j';
        urlStr[45] = 'p';
        urlStr[46] = 'g';
        // extra null terminator since it was removed during the separation
        tweetStr[textLen] = '\0';

        // Assigning pointers to be "returned"
        *url = urlStr;
        *tweet = tweetStr;
    } else {
        Serial.println("Error on HTTP request");
        Serial.println(httpCode);
    }
    http.end(); //Free the resources
}
