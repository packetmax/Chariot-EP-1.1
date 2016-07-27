#include <ChariotEPLib.h>
#include <coap-constants.h>
 /*
   This example for the Arduino enables access and use of the
   digital and analog pins via Qualia's Chariot/CoAP shield library
   through REST calls. It also shows how sketches 
   can interoperate over the Chariot 6LoWPAN/CoAP network to
   also control other sketches in a connected mesh. Chariot's temp, accel,
   magnetometer and radio sensors are also supported. Standard resources available
   on the web-of-things are:
   
  - /sensors/battery
  - /arduino/analog
  - /arduino/digital
  - /arduino/mode
  - /sensors/tmp275-c
  - /sensors/radio
  - /sensors/fxos8700cq-c/accel
  - /sensors/fxos8700cq-c/mag
  - /search
  - /sleep 
  
  Possible commands to this sketch (handled automatically):
 
  Remote webapp:                                               Resulting action by ChariotEPLib
  --------------------------------------------------------------------------------------------------
  coap://chariot.c350e.local/arduino/mode?put&pin=13&val=input    -> pinMode(13, INPUT)
  coap://chariot.c350e.local/arduino/digital?put&pin=13&val=0     -> digitalWrite(13, LOW)
  coap://chariot.c350e.local/arduino/digital?get&pin=13           -> digitalRead(13)           
  coap://chariot.c350e.local/arduino/analog?get&pin=5             -> analogRead(5)
  coap://chariot.c350e.local/arduino/analog?put&pin=13&val=128    -> analogWrite(2, 123) //set PWM duty cycle
 * 
 * by George Wayne, Qualia Networks Incorporated
 */

#define SerialMon if(debug)Serial

// If using the Serial port--type an integer within 5 secs to activate.
static bool debug = false;

void setup() {  
  // arduino IDE/Tools/Port for console and debug:
  Serial.begin(9600);
  Serial.println(F("Serial port is active"));
  debug = true;
  ChariotEP.enableDebugMsgs();
  
  //---Put Arduino resources on the air---
  ChariotEP.begin();
  
  SerialMon.println(F("Setup complete."));
}

void loop() {
  
  //---answer RESTful API calls automatically---
  if (ChariotEP.available()) {
   ChariotEP.process();
  }

  //---type 'sys/help' for available
  if (Serial.available()) {
    ChariotEP.serialChariotCmd();
  }
  
  delay(50);
}

