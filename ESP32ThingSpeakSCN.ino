
#include <WiFi.h>
#include "ThingSpeak.h"


const char* ssid = "FBI";   // your network SSID (name) 
const char* password = "screw2017";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 538228;
const char * myWriteAPIKey = "536VBJVHBSLYYJ1I";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;





//Input Voltage = Digtal Reading * (Ref Volt/Total Bits)
//Ref Volt = 5v, Total Bits = 4095

int AN_Light = 36;    //Light on GPIO36
int AN_pH = 39;       //pH on GPIO39
int AN_Moisture = 34; //Moisture on GPIO34
int AN_Fert = 35;     //Fertility on GPIO35

int i = 0;

float Light_Scale = 0;
float pH_Scale = 0;
float Moisture_Scale = 0;
float Fert_Scale = 0;






void setup() {
  Serial.begin(115200);  //Initialize serial
  
  
  WiFi.mode(WIFI_STA);   
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
 // put your main code here, to run repeatedly:
float AN_Light_Result = 0;
float AN_pH_Result = 0;
float AN_Moisture_Result = 0;
float AN_Fert_Result = 0;

//Takes 5 readings from the probes in 1 sec increments  
  for (i=0; i<4; i++){
    AN_Light_Result = analogRead(AN_Light) + AN_Light_Result;
    AN_pH_Result = analogRead(AN_pH) + AN_pH_Result;
    AN_Moisture_Result = analogRead(AN_Moisture) + AN_Moisture_Result;
    AN_Fert_Result = analogRead(AN_Fert) + AN_Fert_Result;
    delay(1000);  
  }

//Converts to the scale used on the Rapitest Unit
//These where calculated by comparing a second unmodified rapitest unit
//with the modified unit and graphing the digital input vs the scale value
//of the light sensor
  Light_Scale = (AN_Light_Result/5)*0.004;
  pH_Scale = ((AN_pH_Result/5)*(-0.0026))+7;
  Moisture_Scale = (AN_Moisture_Result/5)*0.0016;
  Fert_Scale = (AN_Fert_Result/5)*0.004;

  Serial.print("Light,");
  Serial.print(AN_Light_Result/5);
  Serial.print(",");
  Serial.print(Light_Scale);
  Serial.print(", pH,");
  Serial.print(AN_pH_Result/5);
  Serial.print(",");
  Serial.print(pH_Scale);
  Serial.print(", Moist,");
  Serial.print(AN_Moisture_Result/5);
  Serial.print(",");
  Serial.print(Moisture_Scale);
  Serial.print(", Fert,");
  Serial.print(AN_Fert_Result/5);
  Serial.print(",");
  Serial.println(Fert_Scale);  

float light=Light_Scale;
float pHn=pH_Scale;
float moist=Moisture_Scale;
float fert=Fert_Scale;

  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

   


    // set the fields with the values
    ThingSpeak.setField(1, light);
    ThingSpeak.setField(2, pHn);
    ThingSpeak.setField(3, moist);
    ThingSpeak.setField(4, fert);
    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}
