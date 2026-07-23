#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define btn 23
#define led 17
#define buzz 16

#define armed 100
#define sos 101
#define rst 102

volatile unsigned long timer = 0;

bool led_state = false;
bool is_blink = false;
unsigned long led_timer = 0;

bool buzz_state = false;
unsigned long buzz_timer = 0;

int state = 100;
int LastState = -1;

JsonDocument obj;
using namespace websockets;
WebsocketsClient client;

// void message(payload){
//   Serial.println(payload);
//   Serial.println("this message was sent by the server");
// }

void IRAM_ATTR btn_press() {
  if (millis() - timer > 100 && digitalRead(btn) == LOW) {
    Serial.println("btn is pressed");
    state = sos;
    timer = millis();
  }
}

String setObj(int state, String message) {
  obj["from"] = "esp";
  obj["to"] = "web";
  obj["state"] = state;
  obj["message"] = message;
  String json;
  serializeJson(obj, json);

  return json;
}

void setup() {
  Serial.begin(115200);
  wifi_setup();

  pinMode(btn, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzz, OUTPUT);
  noTone(buzz);
  digitalWrite(buzz, HIGH);  // active-low buzzer module: HIGH = off
  attachInterrupt(digitalPinToInterrupt(btn), btn_press, FALLING);



  //kinneret
  // while(!client.connect("10.9.12.112", 8080 , "/")){
  //home
  // while (!client.connect("10.0.0.10", 8080, "/")) {
    //phone
    while (!client.connect("10.64.88.123", 8080, "/")) {
    Serial.print(".");
  }
  client.send("{\"id\": \"esp\"}");
  Serial.println("websocket connected");


  // client.send("this will be sended to the server");

  client.onMessage([](WebsocketsMessage message) {
    String msgData = message.data();
    Serial.println(
      "got message: " + msgData 
    );

    JsonDocument temp;
    DeserializationError error = deserializeJson(temp, msgData);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str()); 
      return; 
    }
    
    if(temp["state"] == 102){
      //need to figure out why this is not working
      state = rst;
      Serial.println("ESP state updated to RST");
    }
  });

}

void loop() {
  client.poll();
  state_loop();
}
