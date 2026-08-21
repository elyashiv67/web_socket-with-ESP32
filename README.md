# Smart Distress Button (ESP32 + WebSocket)

A real-time smart panic-button system built on an ESP32. Pressing a physical button (wired to a hardware interrupt) immediately triggers a local alarm (fast-blinking LED + rising/falling buzzer tone) and pushes an SOS notification to a Node.js WebSocket server, which relays it to a web dashboard. The alarm can only be cancelled from the web page — a second press of the button does nothing.

## How it works

```
[Physical button] --interrupt--> [ESP32]  <--WebSocket-->  [Node.js server]  <--WebSocket-->  [Web page]
                                     |
                              LED + Buzzer
```

- The ESP32 connects to Wi-Fi and opens a WebSocket connection to the server, identifying itself with `{"id": "esp"}`.
- The web page also connects to the server and identifies itself with `{"id": "web"}`.
- The server (`index.js`) keeps a map of connected clients by id and simply forwards any message that has a `to` field to the matching client — it's a thin relay, not a state owner.
- All state logic (armed / SOS / reset) lives on the ESP32.

## Device states

| State  | Value | Behavior |
|--------|-------|----------|
| `armed` | 100 | Routine mode. LED blinks slowly (once every ~1s) to show the device is alive and connected. No buzzer. |
| `sos`   | 101 | Distress mode, triggered by the button interrupt. LED blinks quickly (every 250ms) and the buzzer sounds a rising/falling tone. Stays active until reset. |
| `rst`   | 102 | Sent only from the web page. Silences the buzzer and returns the device to `armed`. |

Each time the state changes, the ESP32 sends a JSON message to the server:

```json
{ "from": "esp", "to": "web", "state": 101, "message": "esp sends an sos" }
```

The web page can reset the device by sending:

```json
{ "from": "web", "to": "esp", "state": 102, "message": "reset button" }
```

## Hardware

| Component | ESP32 Pin |
|-----------|-----------|
| Push button (INPUT_PULLUP, FALLING interrupt) | GPIO 23 |
| Status LED | GPIO 17 |
| Buzzer (active-low module) | GPIO 16 |

## Project structure

```
esp/webSocket/
  webSocket.ino        # setup, Wi-Fi/WebSocket connection, button interrupt, JSON message builder
  wifi.ino              # Wi-Fi credentials and connection logic
  state_manager.ino     # LED/buzzer handling and the armed/sos/rst state machine
index.js                 # Node.js WebSocket relay server
public/
  index.html             # Web dashboard
  webSocket.js            # Browser-side WebSocket client logic
  style.css               # Dashboard styling
```

## Requirements

**Server**
- Node.js
- npm packages: `ws`, `express`, `nodemon` (see `package.json`)

**ESP32 firmware** (Arduino IDE / arduino-cli)
- `WiFi.h` (bundled with the ESP32 board package)
- [`ArduinoWebsockets`](https://github.com/gilmaimon/ArduinoWebsockets)
- [`ArduinoJson`](https://arduinojson.org/)

## Setup

### 1. Run the WebSocket server

```bash
npm install
npm start
```

This starts the WebSocket server on port `8080` (via `nodemon`, auto-restarting on changes).

### 2. Configure and flash the ESP32

In `esp/webSocket/wifi.ino`, set your Wi-Fi credentials:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* pass = "YOUR_WIFI_PASSWORD";
```

In `esp/webSocket/webSocket.ino`, point the client at the machine running the server (same IP used above):

```cpp
while (!client.connect("YOUR_SERVER_IP", 8080, "/")) {
```

Wire the button, LED, and buzzer to GPIO 23, 17, and 16 respectively, then flash the sketch to the ESP32.

### 3. Open the web dashboard

Serve/open `public/index.html` in a browser. By default it connects to `ws://localhost:8080` (edit the URL in `public/webSocket.js` if the server runs elsewhere). The page shows the latest status received from the ESP32 and has a **Reset** button to cancel an active alarm.

## Notes

- The buzzer module used is **active-low**: driving the pin `HIGH` turns it off.
- The button interrupt is debounced with a simple 100ms timer check.
