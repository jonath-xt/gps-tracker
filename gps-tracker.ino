#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPS++.h>

// ---- Settings: change these ----
const char* WIFI_SSID = "Wokwi-GUEST";   // your home Wi-Fi name on real hardware
const char* WIFI_PASS = "";              // your Wi-Fi password on real hardware
const char* NTFY_URL  = "http://ntfy.sh/CHANGE-THIS-AS-YOUR-TOPIC";  // use your own long random topic

const unsigned long SEND_INTERVAL = 60000;  // send a location every 60 seconds

// ---- GPS wiring ----
const int GPS_RX = 16;  // ESP32 pin connected to GPS TX
const int GPS_TX = 17;  // ESP32 pin connected to GPS RX

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

unsigned long lastSend = 0;
bool sentOnce = false;

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASS, 6);  // channel 6 is needed for Wokwi-GUEST
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void sendLocation(double lat, double lng) {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  String link = "https://maps.google.com/?q=" + String(lat, 6) + "," + String(lng, 6);

  HTTPClient http;
  http.begin(NTFY_URL);
  http.addHeader("Title", "GPS tracker location");
  http.addHeader("Click", link);  // tapping the notification opens the map
  int code = http.POST("Current location: " + link);
  Serial.printf("ntfy response: %d\n", code);
  Serial.println(link);
  http.end();
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("GPS tracker started");
  connectWiFi();
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  bool timeToSend = !sentOnce || (millis() - lastSend >= SEND_INTERVAL);

  if (gps.location.isValid() && timeToSend) {
    sendLocation(gps.location.lat(), gps.location.lng());
    lastSend = millis();
    sentOnce = true;
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 5000) {
    lastPrint = millis();
    if (gps.location.isValid()) {
      Serial.printf("Lat: %.6f  Lng: %.6f  Sats: %d\n",
                    gps.location.lat(), gps.location.lng(), gps.satellites.value());
    } else {
      Serial.println("Waiting for GPS fix...");
    }
  }
}