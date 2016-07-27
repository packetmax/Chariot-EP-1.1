#include <ChariotEPLib.h>
#include <coap-constants.h>

 /*
 arduino-to-arduino sensor exchanges using Chariot

 This example sets up a Mega2560 to perform a simple but
 powerful distributed temperature sensor application.
 Every Qualia Chariot includes an onboard TI TMP275-c temp
 sensor. For any Chariot, this sensor can be read over 
 the wireless 6LoWPAN network as resource "/sensors/tmp275-c."
 It can also be read over the Arduino's I2C bus or the Chariot
 serial channel.

 Chariot includes, in its REST API a resource for finding resources. 
 It is located at "/search." It can return full and partial matches in
 response to searches initiated by any Chariot or connected internet
 device. Chariot also includes a resource that names the mote location
 so that a meaniful temp response can be reported.

 The app:
 1.) looks through all currently visible motes for "/sensors/tmp275-c"
 2.) for those found, it acquires the location string and current temp (C)
 3.) displays the mote address, location string and temperature on the serial monitor.
 
 by George Wayne, Qualia Networks Incorporated
 */

#define SerialMon if(debug)Serial
static bool debug;

void setup() {  
  Serial.begin(9600);
  Serial.println(F("Serial port is active"));
  debug = true;
  ChariotEP.enableDebugMsgs();
    
  //---Put Arduino resources on the air---
  String location = "Mega-RHS-lab-bench";
  ChariotEP.begin(location);
  
  Serial.println(F("Setup complete."));
}

static uint32_t loops = 0;

void loop() {
  
  //---answer RESTful API calls to access sense and actuator resources automatically---
  if (ChariotEP.available()) {
   ChariotEP.process();
  }

  //---type 'sys/help' to see available commands
  if (Serial.available()) {
    ChariotEP.serialChariotCmd();
  }

  //---approximately every 80 sec.
  if ((++loops % 2000)==0)
  {
    String motes[MAX_MOTES];
    uint8_t status;

    SerialMon.println(F("\n@----------------------------------------------------------------------------------------------@"));
    SerialMon.print(F("...loops="));
    SerialMon.println(loops/2000);

#if UNO_HOST==1
    delay(50);
#endif

    status = ChariotEP.getMotes(motes);
    if (status > 0 && status <= 10)
    {
      int i, x;
      String resource = "sensors/tmp275-c";
      String location = "location";
      String response;
      String opts = "";
     
#if UNO_HOST==1
          delay(100);
#endif     
      for (i=0; i<status; i++)
      {
          SerialMon.print(F("...searching mote: ")); SerialMon.print(motes[i]);
          SerialMon.print(F(" for ")); SerialMon.println(resource);

          // Search for resource to see if temp sensor exists 
          if (ChariotEP.coapSearchResources(motes[i], resource, response))
          {
            // Get location of mote[i]
            delay(50); 
            ChariotEP.coapRequest(COAP_GET, motes[i], location, TEXT_PLAIN, opts, response);
            ChariotEP.strip_205_CONTENT(response);
            SerialMon.print(response);
            SerialMon.print(F("--->"));

            // Get temp for mote[i] 
            delay(50);
            ChariotEP.coapRequest(COAP_GET, motes[i], resource, TEXT_PLAIN, opts, response);
            ChariotEP.strip_205_CONTENT(response);
            SerialMon.println(response);
            SerialMon.println();
          }
          else {
            SerialMon.print(F("...error: search error returned for mote: "));
            SerialMon.println(motes[i]);
          }
#if UNO_HOST==1
          delay(100);
#endif
      }
    }
    else { 
      SerialMon.println(F("...no motes visible this pass."));
    }
  }
  delay(75);
}

