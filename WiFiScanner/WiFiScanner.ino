#include <WiFi.h>

#define LED_PIN 2  // Built-in LED on ESP32-WROVER

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("\nWiFi Network Scanner");
  Serial.println("====================");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("\nScanning networks...");
  int n = WiFi.scanNetworks();
  
  digitalWrite(LED_PIN, LOW);
  
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.printf("\n%d networks found:\n\n", n);
    
    for (int i = 0; i < n; i++) {
      Serial.printf("%2d: ", i + 1);
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm) ");
      Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Encrypted");
      
      if (WiFi.SSID(i) == "Sentry-Guest") {
        Serial.print(" <-- TARGET");
      }
      
      Serial.println();
    }
  }
  
  Serial.println("\nWaiting 10 seconds before next scan...");
  
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}
