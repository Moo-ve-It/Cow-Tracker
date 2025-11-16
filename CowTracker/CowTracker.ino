#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

#define LED_PIN 2
#define NUM_COWS 30
#define CENTER_LAT 40.7128
#define CENTER_LON -74.0060
#define MAX_DISTANCE_MILES 5.0
#define MAX_ERRORS 10

struct Cow {
  int id;
  char tag[16];
  float lat;
  float lon;
  float temperature;
  int heartRate;
  int batteryLevel;
  unsigned long nextReport;
};

Cow cows[NUM_COWS];
int consecutiveErrors = 0;
bool sendingDisabled = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("\nCow Tracker - 30 Cows Simulator");
  Serial.println("================================\n");
  
  initializeCows();
  connectToStrongestNetwork();
  
  // Configure NTP time
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync");
  while (time(nullptr) < 1000000000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" done!");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToStrongestNetwork();
  }
  
  if (WiFi.status() == WL_CONNECTED && !sendingDisabled) {
    unsigned long now = millis();
    
    for (int i = 0; i < NUM_COWS; i++) {
      if (now >= cows[i].nextReport) {
        sendCowData(i);
        updateCowData(i);
        cows[i].nextReport = now + random(30000, 45000); // 30-45 seconds
      }
    }
    
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(950);
  } else if (sendingDisabled) {
    // Slow blink to indicate disabled state
    digitalWrite(LED_PIN, HIGH);
    delay(2000);
    digitalWrite(LED_PIN, LOW);
    delay(2000);
  }
}

void initializeCows() {
  Serial.println("Initializing 30 cows...\n");
  
  for (int i = 0; i < NUM_COWS; i++) {
    cows[i].id = i + 1;
    sprintf(cows[i].tag, "COW-%03d", i + 1);
    
    // Random position within 5 miles (0.072 degrees ≈ 5 miles)
    float angle = random(0, 360) * PI / 180.0;
    float distance = random(0, 72) / 1000.0; // 0-0.072 degrees
    cows[i].lat = CENTER_LAT + distance * cos(angle);
    cows[i].lon = CENTER_LON + distance * sin(angle);
    
    cows[i].temperature = random(380, 390) / 10.0; // 38.0-39.0°C
    cows[i].heartRate = random(60, 80);
    cows[i].batteryLevel = random(70, 100);
    cows[i].nextReport = random(0, 30000); // Stagger initial reports
    
    Serial.printf("Cow %d: %s at (%.4f, %.4f)\n", 
                  cows[i].id, cows[i].tag, cows[i].lat, cows[i].lon);
  }
  Serial.println();
}

void updateCowData(int index) {
  // Simulate movement (small random walk)
  cows[index].lat += random(-10, 11) / 10000.0;
  cows[index].lon += random(-10, 11) / 10000.0;
  
  // Update vitals
  cows[index].temperature += random(-5, 6) / 10.0;
  cows[index].temperature = constrain(cows[index].temperature, 37.5, 39.5);
  
  cows[index].heartRate += random(-3, 4);
  cows[index].heartRate = constrain(cows[index].heartRate, 55, 85);
  
  cows[index].batteryLevel = max(50, cows[index].batteryLevel - (int)random(0, 2));
}

void sendCowData(int index) {
  HTTPClient http;
  
  String url = "https://o4510371565273088.ingest.us.sentry.io/api/" + String(SENTRY_PROJECT) + "/store/";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Sentry-Auth", "Sentry sentry_key=" + String(SENTRY_AUTH_TOKEN) + 
                 ", sentry_version=7");
  
  StaticJsonDocument<512> doc;
  
  // Generate 32-char hex event_id (UUID format without dashes)
  char eventId[33];
  sprintf(eventId, "%08x%08x%08x%08x", 
          cows[index].id, (unsigned int)millis(), 
          (unsigned int)time(nullptr), random(0xFFFFFFFF));
  doc["event_id"] = eventId;
  doc["timestamp"] = time(nullptr);
  doc["platform"] = "other";
  doc["level"] = "info";
  doc["message"] = "Cow " + String(cows[index].tag) + " location update";
  
  JsonObject extra = doc.createNestedObject("extra");
  extra["cow_id"] = cows[index].id;
  extra["tag"] = cows[index].tag;
  extra["latitude"] = cows[index].lat;
  extra["longitude"] = cows[index].lon;
  extra["temperature"] = cows[index].temperature;
  extra["heart_rate"] = cows[index].heartRate;
  extra["battery_level"] = cows[index].batteryLevel;
  extra["health_status"] = (cows[index].temperature > 39.0) ? "sick" : "healthy";
  
  String payload;
  serializeJson(doc, payload);
  
  Serial.printf("[%s] Sending: ", cows[index].tag);
  Serial.println(payload);
  
  int httpCode = http.POST(payload);
  Serial.printf("Response: %d\n", httpCode);
  
  if (httpCode >= 200 && httpCode < 300) {
    consecutiveErrors = 0;
  } else {
    consecutiveErrors++;
    Serial.printf("Error count: %d/%d\n", consecutiveErrors, MAX_ERRORS);
    
    if (consecutiveErrors >= MAX_ERRORS) {
      sendingDisabled = true;
      Serial.println("\n!!! TOO MANY ERRORS - SENDING DISABLED !!!\n");
    }
  }
  
  Serial.println();
  http.end();
}

void connectToStrongestNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("Scanning networks...");
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No networks found");
    return;
  }
  
  for (int i = 0; i < n; i++) {
    String scannedSSID = WiFi.SSID(i);
    
    for (int j = 0; j < NETWORK_COUNT; j++) {
      if (scannedSSID == networks[j][0]) {
        Serial.printf("Connecting to %s...\n", scannedSSID.c_str());
        
        WiFi.begin(networks[j][0], networks[j][1]);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
          delay(500);
          Serial.print(".");
          attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("\nConnected!");
          Serial.printf("IP: %s\n\n", WiFi.localIP().toString().c_str());
          return;
        }
      }
    }
  }
  
  Serial.println("No known networks available");
}
