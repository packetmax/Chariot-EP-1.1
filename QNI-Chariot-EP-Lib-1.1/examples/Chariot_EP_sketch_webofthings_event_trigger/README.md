## Synopsis
This sketch uses the **ChariotEPLib** and the **Chariot** *Web-of-Things shield* for
Arduino. It uses Chariot's builtin high-precision TI TMP275 temp sensor,
residing on the Chariot's I2C bus that is shared with the Arduino, to show how
triggerable events may be easily constructed and delivered to subscribers
anywhere.
 
The temp sensor is available to the outside world at */sensors/tmp275-c*, using
the mDNS (*multicast DNS*) name of your board. For example, if you were using a Chariot owning the
serial number *c350e*, you could issue a RESTful GET to retrieve its current reading
simply by sending this Chariot the URL: 

> coap://chariot.c350e.local/sensors/tmp275-c/?get
	
In this URL, *"coap:"* or *"coaps:*" is the internet-of-things analogue to *"http:"*. In addition
to this, Chariot can also perform a great number of other RESTful operations
and even enable your sketch to do new and exciting ones. For more on this, check
the top level documentation of this library.

The sketch establishes its connection to the web-of-things with the line of code
in setup():
```c++
	//---Put Arduino resources on the air---
	ChariotEP.begin();
```
		
This makes all of Arduino's pin resources and Chariot's resources available remotely for reading,
setting and publishing. 

## Sketch details
This sketch creates a triggerable event resource, making it
possible to publish events concerning the temp sensor to all subscribers. This
is accomplished by the library via the sketch code:

```c++
	String trigger = "event/tmp275-c/trigger";
	String attributes = "title=\"Trigger\?get|obs|put\""; // text description
	
	eventHandle = ChariotEP.createResource(trigger, 63, attributes);
```

The trigger is now available for GET, PUT and OBS(erve) service at *chariot.c350e.local*:

> coap://chariot.c350e.local/event/tmp275-c/trigger
	
The final setup step is to register a PUT handler. Line 110 of the example
sketch does this:
```c++
    ChariotEP.setPutHandler(eventHandle, triggerPutCallback);
```
### PUTting parameters to resources from outside the mesh
Chariot's CoAP parser will create a CoAP request to the addressed mote when
it receives properly formed URLs over its communication channel with Arduino, **ChariotClient**. 
This is very useful when sending parameters and commands to the event trigger from a remote app. Our websocket server
example receives URLs from our Chrome JS frontend and passes them (and result strings) over this channel.

PUT URLs are constructed as name/value pairs. In this example, the following
pairs are supported:

> coap://chariot.c350e.local/event/tmp275-c/trigger?put&param=triggerval&val=30
	
For this resource, the action of the PUT handler will be to pass the 
new parameter to the sketch's registered handler:
```c++
    triggerPutCallback("triggerval=30");
```	
It is the responsibility of the handler to parse the string and set the value. 
See sketch for more details.

### PUTting parameters to resources from mesh neighbors
Remote Chariot-equipped Arduino motes can put parameters to event resources by way of 
the RESTful API. Our distributed temp sketch shows how one Arduino generates URLs
that are passed to the Chariot parser to be acted upon. For this sketch, an Arduino neighbor
can achieve the result above by simply calling the library function:
```c++
    String mote = "chariot.c350e.local";
    String resource = "event/tmp275-c/trigger";
    String opts = "param=triggerval&val=30";
    String response;
    ChariotEP.coapRequest(COAP_PUT, mote, resource, TEXT_PLAIN, opts, response);
```
When the event trigger condition is met, the example sends a message
that is received by all that have subscribed to the notification previously vin
the url:

> coap://chariot.c350e.local/event/tmp275-c/trigger?obs

Note that, as immediately above, a sketch might have been the subscriber, generating the
OBServe request via the RESTful API:
```c++
    String mote = "chariot.c350e.local";
    String resource = "event/tmp275-c/trigger";
    String opts = "";
    String response;
    ChariotEP.coapRequest(COAP_OBSERVE, mote, resource, TEXT_PLAIN, opts, response);
```

### URI and API PUT semantics for this sketch
The parameterization of the temp trigger resource is completely determined by this sketch. It can be easily
modified, and its pattern used for other resources generating events that must by monitored (i.e., **OBS**erved).

| PUT URI                  | Resulting action by this sketch      |
|:---------------------------------------------------------------|--------------------------------|
| coap://chariot.c350e.local/arduino/mode?put&param=state&val={on,off} | enable or disable trigger    |
| coap://chariot.c350e.local/arduino/mode?put&param=triggerval&val=30   | resource triggers at 30C    |
| coap://chariot.c350e.local/arduino/mode?put&param=func&val={lt,gt}    | func sets '>' or '<' of triggerval     |
| coap://chariot.c350e.local/arduino/mode?put&param=caloffset&val=integer| calibrates sensor by integer offset (pos or neg)|
| coap://chariot.c350e.local/arduino/mode?put&param=fetch&val=*don't care* | returns JSON string of all params |
|**PUT API used in sketches to create and send above URIs:**|     **Example parameters** |
| ChariotEP.coapRequest(COAP_PUT, mote, resource, 0, opts, response);|`opts="param=state&val={on,off}"`    |
| ChariotEP.coapRequest(COAP_PUT, mote, resource, 0, opts, response);|`opts=param=triggerval&val=30`    |
| ChariotEP.coapRequest(COAP_PUT, mote, resource, 0, opts, response);|`opts=param=func&val=gt`    |
| ChariotEP.coapRequest(COAP_PUT, mote, resource, 0, opts, response);|`opts=param=fetch&val=noise`    |

The trigger message we constructed for this example is coded as JSON (line 89):
```c++
    String triggeredVal = "{\"ID\":\"Trigger\",\"Triggered\":\"Yes\",\"State\":\"Off\"}";
```	
The Chariot EP library handles the delivery via the call for the sketch (line 90):
```c++
    ChariotEP.triggerResourceEvent(eventHandle, triggeredVal, true);
```
# Additional requirements and considerations
- The **Serial Monitor**, when used, should be set to *9600 baud* and *Newline* line termination set.
- Chariot (ie., **ChariotClient**) communicates with the *Mega 2560* on **Serial3** port.
- This sketch runs on the Mega 2560, or any of the current knock-offs, which are available on Amazon. 
- This sketch will also run on the Genuino/UNO, but leaves little memory or pin space for I/O.
-  This sketch will run on the UNO-like ESP8266 WeMos D1 R1 or R2 WiFi board sold on Amazon. When using this board, the ESP8266 WiFi library for Arduino is required. There is also a websocket library available for this board; *we are yet to try it out*.

> Qualia Networks Incorporated -- Chariot IoT Shield and software for Arduino              
> Copyright 2016, Qualia Networks, Inc.
