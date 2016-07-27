## Synopsis
### Using Chariot with Blynk (blynk.cc) drag-n-drop IoT smartphone app builder
Our purpose here is to provide an example of how to create a Blynk sketch that uses Chariot as its web-of-things engine. This sketch is designed to run on the ESP8266-based WeMos D1 R2 "UNO" board. The ESP8266 is a WiFi processor and is used to move IoT data to and from the Blynk cloud (and therefore Blynk app). Blynk is orchestrated through a collection of widgets whose confederates run on an Arduino sketch.

See the "docs" section of the Blynk website for the how to's of setting up your smartphone app. They are not storable in the standard sense, but created through a drag-n-drop interface that is loaded onto your android or iPhone. We have put up a photo of our phone app widget layout and numbering scheme [here] so you can see what the sketch is accomplishing and how to create the app for yourself.
### Note-1 ###
The sleep function of the ESP8266 requires the addition of a switch between pin D0 and RESET (we wired this on the bottom of our WeMos D1 R2). The switch must be thrown or 'on' for the reset to occur during wakeup (see below). The switch must be 'off' while Upload is occurring.
### Note-2 ###
Blynk offers a "bridge" widget whose purpose is to enable device to device controls. It is very limited in comparison to CoAP, both in scale and functionality. We do not make use of it in our sketch, using instead the power of Chariot to create our example.

## Sketch code details
First, a picture of the Blynk widget layout and numbering that mates with this sketch is provided here:
[picture]
 
### Setup the mote connecting to the Chariot mesh and the Blynk cloud ###
This shows how to to use the library to find out which motes are currently operating in the mesh.
```c++
    static char auth[] = "token created on your smartphone";
    void setup() {  
        ...
        //---Put Arduino resources on the air---
        ChariotEP.begin();
        Blynk.begin(auth, "Router name", "Your passkey");
      
        while (Blynk.connect() == false) {
            // Wait until connected
        }
    
         timerCountDn = sliderVal;    // sliderVal read from widget 'V9' setting on your smartphone
         timer.setInterval(1000L, widgetServiceTimer); // Run function every sec. 
         SerialMon.println("Connected to Blynk Cloud!");
         ...
    }
```
### How the sketch gets *sliderVal* above ###
```c++
    // This function will be called every time the
    // app writes value to Virtual Pin 9 (top lhs slider)
    BLYNK_WRITE(V9)
    {
      Serial.print("Got a slider value: ");
      Serial.println(param.asStr());

      sliderVal = param.asInt();
      timerCountDn = sliderVal;
      Blynk.virtualWrite(V15, timerCountDn);
    }
```
### Get the temperature and display it on the smartphone ###
When the number of seconds set by the "SAMPLE FREQ(SEC)" slider have expired this sketch will read temperature, accelerometer, magnetometer, GPS, battery voltage, RSSI (Chariot RF signal strength), LQI (RF link quality), sending these values to the Blynk cloud which will deliver them to the smartphone app for display. Note that temp, gps, accel and mag sampling is governed by individual on/off buttons on the smartphone--the sketch looks at their values before acquiring and sending samples. We will show how temperature is gotten from Chariot and sent to the cloud for widget "V5" (see sketch for the rest):
```c++
  if (++timerCountUp >= sliderVal) 
  {
    if (tempButton) // This is the value of button widget "V0"
    {
      // See distributed temp ex. for sensing remote temps.
      cmd = "temp";
      if (ChariotEP.localChariotCmd(cmd, resp)) 
      {
        Blynk.virtualWrite(V5, resp);
      } 
      else
      {
        Serial.println("widgetServiceTimer() error in temp command");
        resp = "Sensor error";
        Blynk.virtualWrite(V5, resp);
      }
    } 
    else
    {
      resp = "off";
      Blynk.virtualWrite(V5, resp);
    }
...
    if (gpsButton) 
    {
        ...
```

## Other features of the sketch ##
This sketch can be configured on the smartphone to be *sleepy". There is a slider switch named *SAMPLES PER SLEEP* that determines when sleep will take place. There is also a slider named *SLEEP(SEC)* that will determine how long to go to sleep for. The setup for this sketch must include the following. To disable sleeps set this value to zero.
```c++
#define LOW_POWER_SLEEP_ENABLED (1)
```
When instructed to sleep for the interval set by the Blynk app, both the Arduino and Chariot are placed into a sleep for the requested interval (seconds). For this to be a timed sleep, the ESP8266's clock must be left running. This is also the case for Chariot. This means that this is not the absolutely lowest power sleep that is possible. At this time we have not measured the sleep current of the ESP8266 with clock running and with clock not running. The Chariot can sleep at approximately 90 uA in power-down sleep mode. We will have updates for this important capability in the next release of software and firmware. 
### Important! ###
When waking up as the sleep timer expires, the ESP8266 is reset, restarting the app. This means that setup() is rerun and the environment is completely reset. Here's how the sketch places the mote in sleep:
```c++
 if (sampleCountDn <= 0) {
      Serial.print("Sense read cycle complete. Set to sleep for ");
      Serial.print(sleepTimeSecs);
      Serial.println("(sec)");
      Blynk.virtualWrite(V15, "asleep");
      
      led1.off(); // Green blinking 'run' led on smartphone off
      digitalWrite(ledPin, HIGH); // optional led on Arduino (inverted in this case)
      led2.on(); // red "stop" led on smartphone
      
      cmd = "sleep=" + String(sleepTimeSecs+2);
      ChariotEP.localChariotCmd(cmd, resp);     // put Chariot to sleep
      Serial.println("Chariot is asleep");
      ESP.deepSleep(sleepTimeSecs*1000000, WAKE_RF_DEFAULT);
      delay(100);
    }
```

While this sketch has 400 lines of code, the vast majority are for Blynk widget IO. And this code is quite simple once you understand what it is doing. The combination of Chariot + Blynk + ESP8266 processor is a very powerful one. Also, Blynk offers a downloadable private cloud which can be very helpful for debugging.

# Additional requirements and considerations
- The **Serial Monitor**, when used, should be set to *74880 baud* and *Newline* line termination set.
- Chariot (ie., **ChariotClient**) communicates with this board on pins D4(to Chariot Tx) and D5(to Chartion RX).
- This sketch runs on the WeMos D1 R2 UNO equivalent board. It can be purchased from Amazon. 
- To use low power sleep feature, the WeMos board must have switch wired between its reset pin and digital pin D0 (see photo).

> Qualia Networks Incorporated -- Chariot IoT Shield and software for Arduino              
> Copyright 2016, Qualia Networks, Inc.
