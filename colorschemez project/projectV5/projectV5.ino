/*
    Benjamin Jones
    12/23/21

    Significant amounts of this code is based off the work by:
        https://github.com/Bodmer
    thank u bodmer

    This code is for a christmas present, designed to present @colorschemez's
    latest tweet picture and text on a 2.4" TFT display. It is implemented
    with an Arduino ESP32 Feather, and the 2.4" TFT FeatherWing

    Necessary changes:
    * If time-getting fails, then try again. FIXED I THINK
    * make code prettier, better error codes, more verbose checkpoints
        - Blit error code to display, delay 5 seconds

    Wanted changes:
    * Should only redownload if tweet is new, but idk if necessary yet
        - importance depends on if graphical glitches appear during a tweet update
    * run loadTweet() based on micros(), not delay. This will allow interaction
        with the display.
    * Button to save image and send to email or something
    * Tap display to randomize tweets
    * Change query to only past hour. Decreases query size from 1024 to 768
*/
// For TFT
#include <SPI.h>
#include <TFT_eSPI.h>

// For ESP32
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h> // Jpg decoder to TFT display
#include "SPIFFS.h"
#include "time.h"

// Custom headers
#include "tokens.h" //contains ssid, pass, and BEARER_TOKEN
#include "List_SPIFFS.h"
#include "Load_Tweet.h"
#include "Download_Image.h"

// Adafruit ESP32 feather pinouts for 2.4" TFT featherwing
#define STMPE_CS 32
#define SD_CS    14
#define LEDPIN   13

// Time values for EST no DST, -5:00 GMT
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 0;

TFT_eSPI tft = TFT_eSPI();

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if ( y >= tft.height() ) return 0;

    // This function will clip the image block rendering automatically at the TFT boundaries
    tft.pushImage(x, y, w, h, bitmap);

    // Return 1 to decode next block
    return 1;
}

void setup() {
    Serial.begin(115200);
    pinMode(LEDPIN, OUTPUT);
    delay(500);

    SPIFFS.begin(true);
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS initialisation failed!");
        while (1) yield(); // Stay here twiddling thumbs waiting
    }

    // Initialize displayTweet
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);

    TJpgDec.setJpgScale(2);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);

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

    if (SPIFFS.exists("/image.jpg") == true) {
        Serial.println("Remove file after every check");
        SPIFFS.remove("/image.jpg");
    }

    listSPIFFS();

    String imageUrl, text;
    if (loadTweet(&imageUrl, &text)) {
        Serial.println(imageUrl);
        Serial.println(text);

        bool loaded = imageToMem(imageUrl, "/image.jpg");

        // Listing file
        listSPIFFS();
        displayTweet(text);

        Serial.print("Free Memory: ");
        Serial.println(ESP.getFreeHeap());
        delay(60000);
    }
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

void displayTweet(String tweet) {
    tft.fillScreen(ILI9341_BLACK);

    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);

    tft.println(tweet);
    TJpgDec.drawFsJpg(0, 80, "/image.jpg");
    Serial.println("Drawn JPG");

}
