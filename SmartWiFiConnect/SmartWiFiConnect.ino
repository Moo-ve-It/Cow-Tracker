#include <WiFi.h>
#include "networks.h"

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("\nSmart WiFi Connector");
  Serial.println("====================\n");
  
  Serial.printf("Loaded %d networks\n\n", NETWORK_COUNT);
  
  connectToStrongestNetwork();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
  } else {
    Serial.println("Connection lost, reconnecting...");
    connectToStrongestNetwork();
    delay(5000);
  }
}

void connectToStrongestNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("Scanning networks...");
  digitalWrite(LED_PIN, HIGH);
  int n = WiFi.scanNetworks();
  digitalWrite(LED_PIN, LOW);
  
  if (n == 0) {
    Serial.println("No networks found");
    return;
  }
  
  Serial.printf("Found %d networks\n", n);
  
  for (int i = 0; i < n; i++) {
    String scannedSSID = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);
    
    for (int j = 0; j < NETWORK_COUNT; j++) {
      if (scannedSSID == networks[j][0]) {
        Serial.printf("\nFound: %s (%d dBm)\n", scannedSSID.c_str(), rssi);
        Serial.println("Connecting...");
        
        WiFi.begin(networks[j][0], networks[j][1]);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
          digitalWrite(LED_PIN, HIGH);
          delay(250);
          digitalWrite(LED_PIN, LOW);
          delay(250);
          Serial.print(".");
          attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("\n\nConnected!");
          Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
          Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
          Serial.printf("Signal: %d dBm\n", WiFi.RSSI());
          return;
        } else {
          Serial.println("\nFailed");
        }
      }
    }
  }
  
  Serial.println("\nNo known networks available");
}
