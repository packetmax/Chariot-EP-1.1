## Synopsis
### Using Chariot to find and manipulate resources of other sketches
The purpose of this sketch is to provide an example of how Arduino can use its local Chariot to access sensors and other resources that are distributed by other remote Chariot-Arduino motes using CoAP over Chariot's 6LoWPAN, IEEE 802.15.4 wireless mesh.  This example sets up a Mega2560 to perform a simple but powerful distributed temperature sensor application. Every Qualia Chariot includes an onboard TI TMP275-c temp sensor. For any Chariot, this sensor can be read over the wireless 6LoWPAN network at *"coap://DNS-name/sensors/tmp275-c."* Arduino can read the local Chariot's temp sensor (or its accelerometer/magnetometer) over either the Arduino's local I2C bus or the Chariot serial channel.

Chariot includes in its RESTful API a resource for finding resources. It is located at *"/search."* It can return full and partial matches in response to search requests initiated by any Chariot or connected internet device. Chariot also includes a resource that names the mote location so that a meaniful temp response can be reported.
 
This sketch doesn't depend on the sketches running on any of the motes it finds for gathering its temperature data. This is because the temp sensor resides on Chariot, which makes the resources it provides wirelessly available to all other clients operating in the mesh. 

The basic tasks performed by this sketch:
 - 1.) searches all currently visible motes for resource: *"/sensors/tmp275-c"*
 - 2.) for those found, it requests the location string and current temp in C 
 - 3.) displays the mote address, location string and temperature on the serial monitor.
 
## Sketch code details
 
### Obtain the (DNS) names of available motes ###
This shows how to to use the library to find out which motes are currently operating in the mesh.
```c++
    String motes[MAX_MOTES];
    uint8_t status;

    status = ChariotEP.getMotes(motes);
    if (status > 0) {
    // do processing for motes that are found 
    }
```
### Search each mote for  temp resource ###
```c++
    String resource = "sensors/tmp275-c";
    
    // Search *motes[i]* for resource to see if temp sensor exists 
    if (ChariotEP.coapSearchResources(motes[i], resource, response))
    {
        // Request location name and temp
```
### Get the data and display it on the Arduino serial monitor ###
```c++
    // Get location string of mote[i]
    ChariotEP.coapRequest(COAP_GET, motes[i], location, TEXT_PLAIN, opts, response);
    ChariotEP.strip_205_CONTENT(response);
    SerialMon.print(response);
    SerialMon.print(F("--->"));

    // Get temp for mote[i] 
    ChariotEP.coapRequest(COAP_GET, motes[i], resource, TEXT_PLAIN, opts, response);
    ChariotEP.strip_205_CONTENT(response);
    SerialMon.println(response);
    SerialMon.println();
```
### Output display
This example run in our lab shows the Arduino running this sketch meshed with two other motes:
- *chariot.c351e.local*
- *chariot.c350d.local*

Additionally, to appear in the output, these motes had to answer the **search** request in the positive for the temp sensor we are seeking. 
```
@----------------------------------------------------------------------------------------------@
...loops=4
...searching mote: chariot.c351e.local for sensors/tmp275-c
 mega-webskt-srvr-lab---> 26.8(C)

...searching mote: chariot.c350d.local for sensors/tmp275-c
 esp8266-blynk-lab---> 25.6(C)

```
## Other features of the sketch ##
This sketch will also answer */arduino/digital*, */arduino/analog*, */arduino/mode* GET/PUT/OBS pin requests or requests to triggerable */event* resources received from remote sketches. Our loop() function accomplishes this for these requests arriving on the mesh or the serial monitor in the following way:
```c++
void loop() {
  //---answer remote RESTful requests to access sense and actuator resources automatically---
  if (ChariotEP.available()) {
   ChariotEP.process();
  }

  //---type 'sys/help' to see available commands
  if (Serial.available()) {
    ChariotEP.serialChariotCmd();
  }
  ...
```
All this functionality is accomplished by a sketch of just over 100 lines of code. It has been kept as simple as possible to highlight the power of Arduino-to-Arduino resource sharing made possible by **Chariot**. The other example sketches that demonstrate dynamic triggerable event resources, Websocket servers, Blynk cloud interface to Chariot wireless mesh represent just a few of the many possibilites that exist for distributing Arduino sense and actuation use cases using Chariot. Their features can be added to create sophisticated web-of-things systems quickly and inexpensively!

# Additional requirements and considerations
- The **Serial Monitor**, when used, should be set to *9600 baud* and *Newline* line termination set.
- Chariot (ie., **ChariotClient**) communicates with the *Mega 2560* on **Serial3** port.
- This sketch runs on the Mega 2560, or any of the current knock-offs, which are available on Amazon. 
- This sketch will also run on the *UNO* (always check the message window of the Arduino IDE which provides memory usage for flash program and data storage use when compiling).



> Qualia Networks Incorporated -- Chariot IoT Shield and software for Arduino              
> Copyright 2016, Qualia Networks, Inc.
