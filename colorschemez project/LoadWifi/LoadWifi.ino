// Code paraphrased from dronebotworkshop.com for
// learning and testing purposes
/*
  DroneBot Workshop 2020
  https://dronebotworkshop.com
*/

#include "WiFi.h"

void setup() {
  Serial.begin(921600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup Done");
}

void loop() {
  int n = WiFi.scanNetworks();
  Serial.println("Scan Done");

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int ii = 0; i < n; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
    }
  }
  Serial.println("Scan Done, Scanning again in 10 seconds");
  delay(10000);
}
