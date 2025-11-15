#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>

#define LED_PIN 2
#define MAX_NETWORKS 10

typedef struct {
  String ssid;
  String password;
} WiFiNetwork;

WiFiNetwork knownNetworks[MAX_NETWORKS];
int networkCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("\nSmart WiFi Connector");
  Serial.println("====================\n");
  
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  loadNetworks();
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

void loadNetworks() {
  File file = SPIFFS.open("/networks.txt", "r");
  if (!file) {
    Serial.println("Failed to open networks.txt");
    return;
  }
  
  Serial.println("Loading networks:");
  while (file.available() && networkCount < MAX_NETWORKS) {
    String line = file.readStringUntil('\n');
    line.trim();
    
    if (line.length() == 0 || line.startsWith("#")) continue;
    
    int colonPos = line.indexOf(':');
    if (colonPos > 0) {
      knownNetworks[networkCount].ssid = line.substring(0, colonPos);
      knownNetworks[networkCount].password = line.substring(colonPos + 1);
      Serial.printf("  - %s\n", knownNetworks[networkCount].ssid.c_str());
      networkCount++;
    }
  }
  file.close();
  Serial.printf("\nLoaded %d networks\n\n", networkCount);
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
    
    for (int j = 0; j < networkCount; j++) {
      if (scannedSSID == knownNetworks[j].ssid) {
        Serial.printf("\nFound: %s (%d dBm)\n", scannedSSID.c_str(), rssi);
        Serial.println("Connecting...");
        
        WiFi.begin(knownNetworks[j].ssid.c_str(), knownNetworks[j].password.c_str());
        
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
