
#include <ChariotEPLib.h>

 /*
 Chariot Sleep-REST-Blynk(ESP8266 WiFi) tracking example.
 
 This example for the Arduino ESP8266 WeMos D1 R1 or R2 Uno shows how to use 
 Chariot to provide remote access tracking resources. This example also uses the ESP8266's deep sleep.
 Before sleeping, Chariot is instructed to go into a timed or powerdown sleep (your choice).
 When the ESP8266 comes out of its timed sleep it pulls the reset line, cause both boards to 
 restart. This sketch sets up a wireless network of arduinos through REST calls. It also shows  
 how sketches can interoperate over the Chariot 6LoWPAN/CoAP network to 
 also control other sketches in a connected mesh.

 The ESP8266 provides a WiFi service that sends tracking sensor values 
 through a Blynk cloud to a smartphone app for it.

 * 
 * by George Wayne, Qualia Networks Incorporated
 */
 
//#define BLYNK_DEBUG

/*
 * If sleep intervals are enabled, the WeMos board must have an
 * SPST switched wired up between the RESET pin and digital pin D0. On the R2,
 * D0 is not the first pin, but the third; numbering starts following RX and TX.
 * A photo is provided in the examples folder as well as a more detailed README.md.
 * 
 * IMPORTANT: if sleeping is enabled, this switch must be switched to the off
 * position during upload and on afterwards. The ESP8266 resets itself when it
 * emerges from sleeping. This fact means also that the sketch restarts immediately 
 * following wakeup.
 */
#define LOW_POWER_SLEEP_ENABLED (0)

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

WidgetLED led1(V10);
WidgetLED led2(V16);
SimpleTimer timer;

extern void blinkLedWidget();
extern void updateTempDisplay();
extern void widgetServiceTimer();

static uint8_t ledPin = D3;
static int sliderVal = 0L;
static int timerCountUp = 0L;
static int timerCountDn;
static int sampleCountDn;
static int sleepTimeSecs;
static int samplesPerSleep;

static bool tempButton = false;
static bool gpsButton = false;
static bool accelButton = false;
static bool magButton = false;

#define SerialMon if(debug)Serial

// If using the Serial port--type an integer within 5 secs to activate.
static bool debug = false;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "your-authorization-token-here";

void setup() {  
  
  // Check for availability of arduino IDE/Tools/Port for console and debug:
  Serial.begin(74880);
  Serial.println();
  Serial.println(F("Serial port is active"));
  led2.off();
  debug = true;
  ChariotEP.enableDebugMsgs();
  
  //---Put Arduino resources on the air---
  ChariotEP.begin();

  // See blynk.cc for documentation on the Blynk interface
  Blynk.begin(auth, "your-router-name-here", "security-key");
  
  while (Blynk.connect() == false) {
    // Wait until connected
  }

  pinMode(ledPin, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  timerCountDn = sliderVal;
  timer.setInterval(1000L, widgetServiceTimer); // Run every sec. Slider val determines interval.
  SerialMon.println("Connected to Blynk Cloud!");
}

/*
 * BLYNK specific handlers follow
 */
 
static String resp;
static String cmd = "";

// Service time interval driven by slider
void widgetServiceTimer()
{
  // Show 'not sleeping' on dashboard
  Blynk.virtualWrite(V15, --timerCountDn);
  blinkLedWidget();
  led2.off();
  
  if (++timerCountUp >= sliderVal) 
  {
    if (tempButton)
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

    if (gpsButton) 
    {
      // Waiting for Adafruit Ultimate GPS to arrive :)
      resp = "35.665367 N, -78.709994 W";
      Blynk.virtualWrite(V6, resp);
    }
    else
    {
      resp = "off";
      Blynk.virtualWrite(V6, resp);
    }
    
    if(accelButton)
    {
      cmd = "accel";
      if (ChariotEP.localChariotCmd(cmd, resp))
      {
        Blynk.virtualWrite(V7, resp);
      } 
      else
      {
        Serial.println("widgetServiceTimer() error in accel command");
        resp = "Sensor error";
        Blynk.virtualWrite(V7, resp);
      }
    } 
    else
    {
      resp = "off";
      Blynk.virtualWrite(V7, resp);
    }

    if(magButton)
    {
      cmd = "mag";     
      if (ChariotEP.localChariotCmd(cmd, resp))
      {
        Blynk.virtualWrite(V8, resp);
      } 
      else
      {
        Serial.println("widgetServiceTimer() error in mag command");
        resp = "Sensor error";
        Blynk.virtualWrite(V8, resp);
      }
    } 
    else
    {
      resp = "off";
      Blynk.virtualWrite(V8, resp);
    }

    cmd = "battery";
    if (ChariotEP.localChariotCmd(cmd, resp))
    {
      Blynk.virtualWrite(V12, resp);
    } 
    else
    {
      Serial.println("widgetServiceTimer() error in battery command");
      resp = "N/A";
      Blynk.virtualWrite(V12, resp);
    }

    cmd = "rssi";
    if (ChariotEP.localChariotCmd(cmd, resp))
    {
      Blynk.virtualWrite(V13, resp);
    } 
    else
    {
      Serial.println("widgetServiceTimer() error in rssi command");
      resp = "N/A";
      Blynk.virtualWrite(V13, resp);
    }

    cmd = "lqi";
    if (ChariotEP.localChariotCmd(cmd, resp))
    {
      Blynk.virtualWrite(V14, resp);
    } 
    else
    {
      Serial.println("widgetServiceTimer() error in lqi command");
      resp = "N/A";
      Blynk.virtualWrite(V14, resp);
    }

    // Reset countdown seconds
    timerCountUp = 0L;
    timerCountDn = sliderVal;
#if LOW_POWER_SLEEP_ENABLED
    sampleCountDn--;
    Blynk.virtualWrite(V1, sampleCountDn);
#endif
    
    if (sampleCountDn <= 0) {
      Serial.print("Sense read cycle complete. Set to sleep for ");
      Serial.print(sleepTimeSecs);
      Serial.println("(sec)");
      Blynk.virtualWrite(V15, "asleep");
      
      led1.off();
      digitalWrite(ledPin, HIGH);
      led2.on(); // red "stop"
      
      cmd = "sleep=" + String(sleepTimeSecs+2);
      ChariotEP.localChariotCmd(cmd, resp);
      Serial.println("Chariot is asleep");
      ESP.deepSleep(sleepTimeSecs*1000000, WAKE_RF_DEFAULT);
      delay(100);
    }
  }
}

// V10 LED Widget is blinking
void blinkLedWidget()
{
  if (led1.getValue()) {
    led1.off();
    digitalWrite(ledPin, HIGH);
#ifdef BLYNK_DEBUG
    SerialMon.println("LED on V10: off");
#endif
  } else {
    led1.on();
    digitalWrite(ledPin, LOW);
#ifdef BLYNK_DEBUG
    SerialMon.println("LED on V10: on");
#endif
  }
}

// This function will be called every time
// when App writes value to Virtual Pin 0 (temp button)
BLYNK_WRITE(V0)
{
  Serial.print("Got a temp button value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
  tempButton = (bool)param.asInt();
}

// This function will be called every time
// when App writes value to Virtual Pin 2 (gps button)
BLYNK_WRITE(V2)
{
  Serial.print("Got a gps button value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
  gpsButton = (bool)param.asInt();
}

// This function will be called every time
// when App writes value to Virtual Pin 3 (accel button)
BLYNK_WRITE(V3)
{
  Serial.print("Got an accel button value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
  accelButton = (bool)param.asInt();
}

// This function will be called every time
// when App writes value to Virtual Pin 3 (magnetometer button)
BLYNK_WRITE(V4)
{
  Serial.print("Got a magnetometer button value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
 magButton = (bool)param.asInt();
}

// This function will be called every time
// when App writes value to Virtual Pin 9 (top slider)
BLYNK_WRITE(V9)
{
  Serial.print("Got a slider value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
  sliderVal = param.asInt();
  timerCountDn = sliderVal;
  Blynk.virtualWrite(V15, timerCountDn);
}

// This function will be called 
// when the App writes a new 'samples per sleep' value
BLYNK_WRITE(V11)
{
  Serial.print("Got a samples per sleep slider value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
  samplesPerSleep = param.asInt();
  sampleCountDn = samplesPerSleep;
  Blynk.virtualWrite(V1, sampleCountDn);
}

// This function will be called 
// when the App writes a new 'sleep secs' value
BLYNK_WRITE(V17)
{
  Serial.print("Got a samples per sleep slider value: ");
  Serial.println(param.asStr());
  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
  sleepTimeSecs = param.asInt();
}

// Keep this flag not to re-sync on every reconnection
static bool isFirstConnect = true;

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() 
{
  
  if (isFirstConnect) {
    // Request Blynk server to re-send latest values for all pins
    Blynk.syncAll();

    // You can also update an individual Virtual pin like this:
    //Blynk.syncVirtual(V0);

    isFirstConnect = false;
  }

  // Show slider val after reconnect:
  Blynk.syncVirtual(V9);
  BLYNK_WRITE(V9);
}

void loop() {

  /*
   * Answer remote RESTful GET, PUT, DELETE, OBSERVE API calls
   * transparently.
   */
  if (ChariotEP.available()) {
   ChariotEP.process();
  }

  /* 
   *  Filter your own inputs first--pass everthing else here.
   *   --try typing 'sys/help' into the Serial window
   */
  if (debug && Serial.available()) {
    delay(50); // ESP8266 sending partial commands off to Chariot--slow it down
    ChariotEP.serialChariotCmd();
  }
  
  /*
   * Run Blynk
   */
  Blynk.run();
  timer.run();
}
