#include <Arduino.h>
#include "MQTT.h"
#include <TimeLib.h>
#include <Wire.h> 
#include "EmonLib.h"
#define VOLT_CAL 345.3
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

EnergyMonitor emon1;

void pin_vol(){
 emon1.current(34, 107.5); // input pin, calibration
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

float  voltaje() {
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=5000; //En Milisegundos
    static unsigned long previousMillis=0;
   //procesos sensor de voltaje
    
   double supplyVoltage = emon1.calcIrms(4065);            //extract Vrms into

  if((millis()-previousMillis)>period){   

    if(supplyVoltage >=50){
      lcd.setCursor(0,0);
      lcd.print("Voltaje:");
      lcd.setCursor(10,0);
      lcd.print(supplyVoltage);
      lcd.setCursor(17,0);
      lcd.print("VAC");
      delay(1000); 
    }
    else{
     supplyVoltage=supplyVoltage-supplyVoltage;
      
      lcd.setCursor(0,0);
      lcd.print("Voltaje:");
      lcd.setCursor(10,0);
      lcd.print(supplyVoltage);
      lcd.setCursor(17,0);
      lcd.print("VAC");
      delay(1000);
    }
      
  }
        previousMillis += period;

    return supplyVoltage;
}  


float corriente = 0;
float offset=0.700;
float corr=0.000;
float corriente_sensor(){
  
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=5000; //En Milisegundos
    static unsigned long previousMillis=0;

    if((millis()-previousMillis)>period){
       float voltage =  analogRead(35) * 5.0 / 1023.0;
       corriente= (0.9*0.1*((voltage-0.523) / 0.139))-offset;
        if( voltaje()==0){
          lcd.setCursor(0,1);
          lcd.print("corriente:");
          lcd.setCursor(10,1);
          lcd.print(corr, 3);
          lcd.setCursor(19,1);
          lcd.print("A");
          delay(1000);
        }
        else{
           if( corriente >=0){
          lcd.setCursor(0,1);
          lcd.print("corriente:");
          lcd.setCursor(10,1);
          lcd.print(corriente, 3);
          lcd.setCursor(19,1);
          lcd.print("A");
          delay(1000);
        }
        else  {
             corriente=corriente-corriente;
             lcd.setCursor(0,1);
             lcd.print("corriente:");
             lcd.setCursor(10,1);
             lcd.print(corriente, 3);
             lcd.setCursor(19,1);
             lcd.print("A");
             delay(1000);
        
        } 
        }
       
     previousMillis += period;
  return corriente; 
 }
}


float pot;

float potencia(){
  
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=5000; //En Milisegundos
    static unsigned long previousMillis=0;

    if((millis()-previousMillis)>period){
          pot = voltaje() * corriente_sensor();
          lcd.setCursor(0,2);
          lcd.print("potencia:");
          lcd.setCursor(10,2);
          lcd.print(pot, 3);
          lcd.setCursor(17,2);
          lcd.print("WAT");
          delay(1000);
          

        previousMillis += period;
     }
     return pot ;
 }

float costo_consumo;

float costo(){
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=6000; //En Milisegundos
    static unsigned long previousMillis=0;

     if((millis()-previousMillis)>period){
          float total_potencia = (total_potencia + potencia());
           if (hour() == 12 && minute() == 0 && second() == 0) {
           costo_consumo = 858.000 * total_potencia;
           delay(3000);
           total_potencia = 0;
           }
           else {
             costo_consumo=0;
           }
        previousMillis += period;
     }
     return costo_consumo ;
}

 