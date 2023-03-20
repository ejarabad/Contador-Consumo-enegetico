#include "MQTT.h"
#include "sensores.h" 

void connectToWifi();
void connectToMqtt();
void  WiFiEvent();
void  onMqttConnect();
void onMqttDisconnect();
void  onMqttPublish();

void setup() {
  

  Serial.begin(115200);
  Serial.println();
  setupp();
  pin_vol();
}

void loop() {
  // put your main code here, to run repeatedly:
  mainnn();
  voltaje ();
  corriente_sensor();
  potencia();
  costo();
}
