#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "tu_ssid";
const char* password = "tu_contraseña";
const char* mqtt_server = "tu_servidor_mqtt";
const int mqtt_port = 1883;

void call_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe("voltaje");
      client.subscribe("corriente");
      client.subscribe("potencia");
      client.subscribe("costo");
    } else {
      delay(5000);
    }
  }
}

void mqtt() {
  call_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void conect() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}