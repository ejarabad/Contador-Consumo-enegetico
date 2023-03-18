#include <Wire.h>
#include <TimeLib.h>
#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include "sensores.h" 


#define WIFI_SSID "iPhone de Maite"
#define WIFI_PASSWORD "life2109"

// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(4, 204, 226, 150)
// For a cloud MQTT broker, type the domain name
//#define MQTT_HOST "example.com"
#define MQTT_PORT 1883

// Temperature MQTT Topics
#define MQTT_PUB_CORRIENTE "corriente"
#define MQTT_PUB_VOLTAJE "voltaje"
#define MQTT_PUB_POTENCIA "potencia"
#define MQTT_PUB_COSTO "costo"
#define MQTT_PUB_PROMCONSU "promedioConsumo"
#define MQTT_PUB_PROMCOST "promedioCosto"

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}


void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setupp() {


  
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);


  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
  connectToWifi();
}

void mainnn() {
  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;

    // Publish an MQTT message on topic esp32/BME2800/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_CORRIENTE, 1, true, String(corriente_sensor()).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_CORRIENTE, packetIdPub1);
    Serial.printf("corriente: %.2f \n", corriente_sensor());

    // Publish an MQTT message on topic esp32/BME2800/humidity
    uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_VOLTAJE, 1, true,  String(voltaje()).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_VOLTAJE, packetIdPub2);
    Serial.printf("voltaje: %.2f \n", voltaje());

    // Publish an MQTT message on topic esp32/BME2800/pressure
    uint16_t packetIdPub3 = mqttClient.publish(MQTT_PUB_POTENCIA, 1, true, String(potencia()).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_POTENCIA, packetIdPub3);
    Serial.printf("potencia: %.2f \n", potencia());


    uint16_t packetIdPub4 = mqttClient.publish(MQTT_PUB_COSTO, 1, true, String(costo()).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_COSTO, packetIdPub4);
    Serial.printf("costo en pesos: %.2f \n", costo());
  }
}
