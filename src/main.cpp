#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT.h>
#include "WiFi.h"
#include "wifi_secret.h"

const char* host = "10.0.0.42";
int port = 1883;
const char *mqtt_client_id = "light_dimmer_mqtt";

const char* rx_topic = "/dimmer/request";
const char* tx_topic = "/dimmer/response";
const char* status_topic = "/dimmer/status";

MQTTClient mqtt(1024);// 1KB
WiFiClient wifi;//needed to stay on global scope

RTC_DATA_ATTR uint32_t cycle_count = 0;

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

  Serial.print("\nmqtt connecting...");
  while (!mqtt.connect(mqtt_client_id)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");

  mqtt.subscribe(rx_topic);

  mqtt.publish(status_topic, "running");
}

void mqtt_loop(){
  mqtt.loop();
  if (!mqtt.connected()) {
    connect();
  }
}

void process_serial_2_mqtt(String str_message){
  mqtt.publish(tx_topic, str_message);
}


void messageReceived(String &topic, String &payload) {
  Serial.println(payload);
}

void setup() {
  
  Serial.begin(115200);
  //Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);//Rx does not work
  WiFi.begin(ssid, password);

  mqtt.begin(host,port, wifi);
  mqtt.onMessage(messageReceived);

  timelog("setup() done");
}

void loop() {
  static unsigned long lastTime = 0;
  mqtt_loop();
  delay(1000);

  if(Serial.available()>0){
    String msg = Serial.readString();
    process_serial_2_mqtt(msg);
  }

}
