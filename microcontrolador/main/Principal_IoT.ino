#include <Arduino.h>
#include "comunicacion.h"
#include "sensores.h" 
#include "wifi.h"




void setup() {
  // inicializo comunicacion
  Serial.begin(115200);
  Serial1.begin(115200);
  call_wifi();
  pin_vol();
  
}


void loop() {
    
    voltaje ();
    task1();
    corriente_sensor();
    potencia();
    
}
