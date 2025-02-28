/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"

bool modeAuto=false;
bool pomp=false;
float s=60;
float soilhumidity;
String strsoil;
const int pum = 15;
const int sensor_pin = A0;
int sensor_analog;

unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 3000;           // interval at which to blink (milliseconds)

EspMQTTClient client(
  "TOPNET_4C28",
  "7zwu82g6vl",
  "broker.hivemq.com",  // MQTT Broker server ip
  "MQTTUsername",   // Can be omitted if not needed
  "MQTTPassword",   // Can be omitted if not needed
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  Serial.begin(115200);

  // Optional functionalities of EspMQTTClient
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("khouloudsyrine/mode", [](const String & payload) {
    if(payload=="1") modeAuto=true;
    else if (payload=="0") modeAuto=false;
    Serial.println(payload);
  });

  client.subscribe("khouloudsyrine/pompe", [](const String & payload) {
    if(payload=="1") pomp=true;
    else if (payload=="0") pomp=false;
    Serial.println(payload);
  });

  
  // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
  client.subscribe("mytopic/wildcardtest/#", [](const String & topic, const String & payload) {
    Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
  });

  
// Publish humidity to "khouloudsyrine/hum"
  client.publish("khouloudsyrine/hum", strsoil); //converti soilhumidity to strsoil

  
  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  });
}
void soilmoisture(){
   sensor_analog = analogRead(sensor_pin);
  soilhumidity = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
  strsoil = String(soilhumidity, 2);
  Serial.print("soilhumidity = ");
  Serial.print(soilhumidity);  /* Print soilhumidity on the serial window */
  Serial.println("%");
  delay(1000); 
 }
void loop(){
  soilmoisture();
    client.loop();
//      if(modeAuto){
//        if(soilhumidity<s){
//          digitalWrite(pum, HIGH);
//          Serial.print(" Faible humidite");
//          Serial.print("La Pump est:ON");
//        }else{
//          digitalWrite(pum, LOW );
//          Serial.print("Humidite elevee");
//          Serial.print("La Pump est:OFF");
//        }//else {
//          //digitalWrite(pum, LOW );
//         //}
//      }else{
//        if(pomp){
//          digitalWrite(pum, HIGH );
//        }else{
//          digitalWrite(pum, LOW );
//        }
//      }

      unsigned long currentMillis = millis();
     if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    client.publish("khouloudsyrine/hum", "60"); 
    
     }

     
    
 
}
