#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT.h>
#include "WiFi.h"
#include "M5Atom.h"

//recommended approach : create 'wifi_secret.h' which is git ignored
//fallback approach, remove the comments of the following 3 lines
//#define WIFI_CREDENTIAL_OVERRIDE
//const char* ssid = "MySSID";
//const char* password =  "MyPassword";
#ifndef WIFI_CREDENTIAL_OVERRIDE
  #include "wifi_secret.h"
#endif


const char* host = "10.0.0.42";
int port = 1883;
const char *mqtt_client_id = "light_dimmer_mqtt";

const char* rx_topic = "esp/dimmer/request";
const char* tx_topic = "esp/dimmer/response";
const char* main_topic = "esp/dimmer";

MQTTClient mqtt(1024);// 1KB
WiFiClient wifi;//needed to stay on global scope

//      G
//      5V
#define TX_PIN 25
#define RX_PIN 21

void timelog(String Text){
  Serial.println(String(millis())+" : "+Text);//micros()
}

void mqtt_publish_config(){
  String str_config;
  mqtt.publish("config",str_config);
  mqtt.loop();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  M5.dis.drawpix(0, CRGB(0,0,50));

  Serial.print("\nmqtt connecting...");
  while (!mqtt.connect(mqtt_client_id)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  M5.dis.drawpix(0, CRGB(50,0,0));

  mqtt.subscribe(rx_topic);
  String payload = "{\"status\":\"running\",\"time\":"+String(millis())+"}";
  mqtt.publish(main_topic, payload.c_str());
}

void mqtt_loop(){
  mqtt.loop();
  if (!mqtt.connected()) {
    connect();
  }
}

void process_serial_2_mqtt(String str_message){
  mqtt.publish(tx_topic, str_message);
  Serial.println("\nSerial > MQTT");
  Serial.println(str_message);
}


void messageReceived(String &topic, String &payload) {
  Serial1.println(payload);
  Serial.println("\nMQTT > Serial");
  Serial.println(payload);
}

void setup() {
  
  M5.begin(false, false, true);
  delay(50);
  M5.dis.drawpix(0, CRGB(0,50,0));

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  WiFi.begin(ssid, password);

  mqtt.begin(host,port, wifi);
  mqtt.onMessage(messageReceived);

  timelog("setup() done");
}

void switch_led(){
  static uint8_t state = 0;
  switch (state)
  {
  case 0:
      M5.dis.drawpix(0, CRGB(0,0,50));
      Serial.println("\nButton => LED: Blue");
      break;
  case 1:
      M5.dis.drawpix(0, CRGB(50,0,0));
      Serial.println("\nButton => LED: Green");
      break;
  default:
      break;
  }
  state++;
  if (state == 2)
  {
      state = 0;
  }
}

void loop() {
  mqtt_loop();
  if (M5.Btn.wasPressed()){
    switch_led();
  }

  delay(50);
  M5.update();
  if(Serial1.available()>0){
    String msg = Serial1.readString();
    process_serial_2_mqtt(msg);
  }

}
