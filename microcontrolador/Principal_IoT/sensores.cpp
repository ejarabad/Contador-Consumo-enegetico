#include <Arduino.h>
#include <Wire.h> 
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
#include "EmonLib.h"             // Include Emon Library
#define VOLT_CAL 345.3

EnergyMonitor emon1;             // Create an instance

void pin_vol(){
  emon1.voltage(1, VOLT_CAL, 1.7);  // Voltage: input pin, calibration, phase_shift
     // Inicializar el LCD
  lcd.init();
  //Encender la luz de fondo.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("INICIALIZANDO...");
  lcd.setCursor(0,1);
  lcd.print("MEDIDOR DE POTENCIA.");
  lcd.setCursor(5,2);
  lcd.print(">>> IoT <<<");
  //espera 5 seg
   delay(2000);
   lcd.clear();
}

///////////// sensor voltaje ///////////////

float  voltaje() {
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=5000; //En Milisegundos
    static unsigned long previousMillis=0;
   //procesos sensor de voltaje
   emon1.calcVI(25,100);         // Calculate all. No.of half wavelengths (crossings), time-out
   float supplyVoltage   = emon1.Vrms;             //extract Vrms into

  if((millis()-previousMillis)>period){   

    if(supplyVoltage >=40){
      lcd.setCursor(0,0);
      lcd.print("Voltaje:");
      lcd.setCursor(8,0);
      lcd.print(supplyVoltage);
      lcd.setCursor(15,0);
      lcd.print("VAC");
      delay(1000);
    }
    else{
     supplyVoltage=supplyVoltage-supplyVoltage;
      lcd.setCursor(0,0);
      lcd.print("Voltaje:");
      lcd.setCursor(8,0);
      lcd.print(supplyVoltage);
      lcd.setCursor(15,0);
      lcd.print("VAC");
      delay(1000);
    }
      
  }
        previousMillis += period;

    return supplyVoltage;
}  

///////// sensor corriente ///////////

float corriente = 0;
const int analogInPin = A0;
int sensorValue = 0; 

float corriente_sensor(){
  
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=5000; //En Milisegundos

    static unsigned long previousMillis=0;

    if((millis()-previousMillis)>period){
        float voltage = (float) analogRead(A0) * 5.0 / 1023.0;
    corriente= (float) (voltage - 0.500) / 0.066;
   sensorValue = analogRead(analogInPin);
   lcd.setCursor(0,1);
   lcd.print("sensorcorri:");
   lcd.setCursor(13,1);
   lcd.print(sensorValue);
   lcd.setCursor(0,2);
   lcd.print("corriente:");
   lcd.setCursor(11,2);
   lcd.print(corriente);
   lcd.setCursor(17,2);
   lcd.print("A");
   delay(1000);

        previousMillis += period;
     } 

    return corriente; 
 }




 float potencia(){
  
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=5000; //En Milisegundos

    static unsigned long previousMillis=0;
      float pot;
    if((millis()-previousMillis)>period){
         float voltage = (float) analogRead(A0) * 5.0 / 1023.0;
         corriente= (float) (voltage - 0.500) / 0.066;
         float supplyVoltage   = emon1.Vrms;
          pot = supplyVoltage  * corriente;

          lcd.setCursor(0,3);
          lcd.print("potencia:");
          lcd.setCursor(10,3);
          lcd.print(pot);
          lcd.setCursor(18,3);
          lcd.print("W");
          delay(1000);

        previousMillis += period;
     }
     return pot ;
 }


float costo(){
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=5000; //En Milisegundos

    static unsigned long previousMillis=0;
      float costo_consumo;
     if((millis()-previousMillis)>period){
          float total_potencia = (total_potencia + potencia());
           if (hour() == 12 && minute() == 0 && second() == 0) {
           costo_consumo = 858.000 * total_potencia;
           delay(3000);
           total_potencia = 0;
           }
        previousMillis += period;
     }
     return costo_consumo ;
}
