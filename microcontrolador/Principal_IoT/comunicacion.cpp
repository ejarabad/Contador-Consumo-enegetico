#include <Arduino.h>
#include "sensores.h"
#include <TimeLib.h>
int estado = 0;
int contador = 0;
int j=0;
char  option[50];  // tamaño del buffer para recepcion de datos

 enum{
   	MODEM_OFF, //AT+CFUN=0 // Apago y espero 3 Segs
	  MODEM_CFG_APN, 	 //AT+CGDCONT=1,"IP","internet.comcel.com.co"  // y espero 2 Segs
	  MODEM_ON,  		 // AT+CFUN=1 // enciende y espera 5 Segs
	  MODEM_PIN,  		 // AT+CPIN? // Mando y espero RX READY , sino volver al estado de entender
	  MODEM_CHK_REG,    //AT+CREG?  // Mando y espero Registro "0,1" Home y espero 2 Segs
	  MODEM_CFG_TEXT,	 //AT+CMGF=1  // Configura para SMS en modo Texto
	  MODEM_SMS_TEL,	 //AT+CMGS="nroTel" // Envia mensaje a Tel y espera ">"
	  MODEM_MSG,//"mensaje" CTRL Z (0x1A) // "CMGS:"
  };




void  task1() {
    //{period}: Periodo de Tiempo en el cual se va a ejecutar esta tarea
    unsigned long period=200; //En Milisegundos

    static unsigned long previousMillis=0;


    if((millis()-previousMillis)>period){
       if (hour() == 12 && minute() == 0 && second() == 0) {
            switch(estado){
              case MODEM_OFF:
                Serial.flush();
                Serial1.flush();
                Serial.print("AT+CFUN=0\r\n");
                Serial1.print("AT+CFUN=0\r\n");
                delay(1000);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                    for(j=0; j<option ; j++){
                      if(option[j] == 79 && option[j+1] == 75){
                        estado = MODEM_CFG_APN;	
                          break;
                      }
                    }
                }
              break;
              case MODEM_CFG_APN:
                Serial.flush();
                Serial1.flush();
                Serial.print("AT+CGDCONT=1,\"IP\",\"internet.comcel.com.co\"\r\n");
                Serial1.print("AT+CGDCONT=1,\"IP\",\"internet.comcel.com.co\"\r\n");
                delay(1000);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                    for(j=0; j<option ; j++){
                      if(option[j] == 79 && option[j+1] == 75){
                        estado = MODEM_ON;	
                          break;
                      }
                    }
              }
              break;
              case MODEM_ON:
                Serial.flush();
                Serial1.flush();
                Serial.print("AT+CFUN=1\r\n");
                Serial1.print("AT+CFUN=1\r\n");
                delay(5000);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                    for(j=0; j<option ; j++){
                      if(option[j] == 79 && option[j+1] == 75){
                        estado = MODEM_PIN;	
                          break;
                      }
                    }
                }  
              break; 
              case MODEM_PIN:
                Serial.flush();
                Serial1.flush();
                Serial.print("AT+CPIN?\r\n");
                Serial1.print("AT+CPIN?\r\n");
                delay(1000);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                    for(j=0; j<option ; j++){
                      if(option[j] == 79 && option[j+1] == 75){
                        estado = MODEM_CHK_REG;	
                          break;
                      }
                    }
                } 
              break; 
              case MODEM_CHK_REG:
                Serial.flush();
                Serial1.flush();
                Serial.print("AT+CREG?\r\n");
                Serial1.print("AT+CREG?\r\n");
                delay(1000);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                    for(j=0; j<option ; j++){
                      if(option[j] == 79 && option[j+1] == 75){
                        estado = MODEM_CFG_TEXT;	
                          break;
                      }
                    }
                }
              break;
              case MODEM_CFG_TEXT:
                Serial.flush();
                Serial1.flush();
                Serial.print("AT+CMGF=1\r\n");
                Serial1.print("AT+CMGF=1\r\n");
                delay(1000);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                    for(j=0; j<option ; j++){
                      if(option[j] == 79 && option[j+1] == 75){
                        estado = MODEM_SMS_TEL;	
                          break;
                      }
                    }
                }
              break;   
              case MODEM_SMS_TEL:
                Serial.flush();
                Serial1.flush();
                Serial.print("AT+CMGS=\"3024070841\"\r\n");
                Serial1.println("AT+CMGS=\"3024070841\"\r\n");
                delay(1000);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                  estado = MODEM_MSG;
                }
              break;   
              case MODEM_MSG:
                Serial.flush();
                Serial1.flush();
                Serial.print(costo());
                Serial1.println("El consumo del dia de ayer fue: ");
                delay(500);
                Serial1.println(costo());
                delay(500);
                Serial.println((char)26);
                Serial1.println((char)26);
                if(Serial1.available() != 0){
                  Serial1.readBytes(option,50);
                    for(j=0; j<option ; j++){
                      if(option[j] == 79 && option[j+1] == 75){
                        estado = MODEM_OFF;	
                          break;
                      }
                    }
                }  
              break; 
            }
                
        previousMillis += period;
    }  
    }
}