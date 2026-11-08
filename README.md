# GPS Tracker (ESP32 + NEO-6M)

An ESP32-based GPS tracker that reads location data from a NEO-6M GPS module and sends a **Google Maps link** to your phone through [ntfy](https://ntfy.sh). Tapping the notification opens the location directly in Google Maps.

The project was built and tested in the **Wokwi simulator**, so no physical hardware is needed to try it.

**Simulation:** https://wokwi.com/projects/480016485491910657

---

## Features

- Reads latitude, longitude, speed and satellite count from a NEO-6M GPS module
- Connects to Wi-Fi and sends a push notification to your phone via ntfy
- Notification contains a Google Maps link and opens the map when tapped
- Sends the first location as soon as a valid GPS fix is available, then repeats at a set interval (default: every 60 seconds)
- Prints status and coordinates to the Serial Monitor for debugging
- Reconnects to Wi-Fi automatically if the connection drops

## How it works

```
NEO-6M GPS  --UART-->  ESP32  --Wi-Fi-->  ntfy.sh  --push-->  Phone (ntfy app)
                                                                    |
                                                          tap notification
                                                                    v
                                                              Google Maps
```

1. The GPS module streams NMEA sentences to the ESP32 over a serial (UART) connection at 9600 baud.
2. The [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus) library parses them into latitude and longitude.
3. When a valid fix exists and the send interval has passed, the ESP32 builds a link like `https://maps.google.com/?q=LAT,LNG`.
4. The ESP32 sends the link to your ntfy topic as an HTTP POST request. The `Click` header makes the notification open the map when tapped.
5. The ntfy app on your phone, subscribed to the same topic, shows the notification.

## Components

| Part | Purpose |
|---|---|
| ESP32 dev board | Microcontroller with built-in Wi-Fi |
| NEO-6M GPS module | Provides location data |

## Wiring

| NEO-6M pin | ESP32 pin |
|---|---|
| VCC | 3V3 |
| GND | GND |
| TX | GPIO 16 (RX2) |
| RX | GPIO 17 (TX2) |

## Software and libraries

- Arduino framework for ESP32 (Arduino IDE or Wokwi)
- `TinyGPS++` (TinyGPSPlus) by Mikal Hart
- `WiFi.h` and `HTTPClient.h` (included with the ESP32 board package)

## Setup

### 1. Run in Wokwi (no hardware)

1. Open the simulation link above, or create a new ESP32 project in [Wokwi](https://wokwi.com) and add a NEO-6M GPS module.
2. Wire the parts as shown in the wiring table.
3. Add the `TinyGPSPlus` library in Wokwi's Library Manager.
4. Paste in the code from `gps-tracker.ino`.
5. Click **Start**, then click the GPS module to set a location.

### 2. Set up ntfy

1. Install the ntfy app on your phone (Android or iOS).
2. Choose a long, random topic name, for example `gps-tracker-x7k2m9`.
3. In the app, tap **+**, enter the topic name and subscribe.
4. In `gps-tracker.ino`, replace `CHANGE-THIS-TOPIC` with your topic name.

> **Keep your topic private.** Topics on the public ntfy.sh server have no password, so anyone who knows the name can read your alerts or send fake ones. Do not commit your real topic name to a public repository.

### 3. Configuration

These values are at the top of the sketch:

| Setting | Default | Meaning |
|---|---|---|
| `WIFI_SSID` | `Wokwi-GUEST` | Wi-Fi network name (use your own on real hardware) |
| `WIFI_PASS` | empty | Wi-Fi password (never commit a real password) |
| `NTFY_URL` | `http://ntfy.sh/CHANGE-THIS-TOPIC` | Your ntfy topic address |
| `SEND_INTERVAL` | `60000` | Time between alerts in milliseconds |

## Example output

Serial Monitor:

```
GPS tracker started
Connecting to Wi-Fi... connected
ntfy response: 200
https://maps.google.com/?q=LATITUDE,LONGITUDE
Lat: LATITUDE  Lng: LONGITUDE  Sats: N
```

Phone notification: **GPS tracker location**, with a Google Maps link. Tapping it opens the map.

