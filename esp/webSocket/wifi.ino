#include <WiFi.h>

// const char* ssid = "Kinneret College";
// const char* ssid = "Magi";

const char* ssid = "Elyashiv";
const char* pass = "12345622es";
const char* ipAddrress = "10.64.88.123";

// const char* pass = "0587127028";
// const char* ipAddrress = "10.0.0.10";

// const char* ipAddrress = "10.9.12.112";

void wifi_setup() {
  Serial.println("wifi setup");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("trying...");
    delay(100);
  }

  Serial.println("connected to wifi");
}
