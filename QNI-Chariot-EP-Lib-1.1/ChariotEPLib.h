/*
 * ChariotLib.h - Library for interfacing to Chariot IoT Shield
 *               
 * Created by George Wayne for Qualia Networks, Inc., 2016.
 * BSD license, all text above must be included in any redistribution.
 */
#ifndef CHARIOT_EP_LIBINCLUDED
#define CHARIOT_EP_LIBINCLUDED

#include <Arduino.h>
#include <Wire.h>    			// the Arduino I2C library
#include <SoftwareSerial.h>
#include "coap-constants.h"

/*
 * Because this is Arduino, a maximum needs to be set for the number of nodes.
 */
#define MAX_MOTES			(8)

#define EP_DEBUG			0
#define SerialMon			if(debug)Serial

#define UNO					1
#define LEONARDO			2
#define MEGA_DUE			3
/*-------------------------------------------------------------------------------------------*/
# undef ESP8266_D1_R1		4  // #define if compiling for this board; #undef otherwise.
#if defined(ESP8266)		   // #defined on the compile line by Arduino IDE
#define ESP8266_D1_R2		5  // #Switch #stmts around if you're using the "R1 Retired" board.
#endif
/*--------------------------------------------------------------------------------------------*/

/*
 * Determine Chariot-Arduino channel and event resource setup
 */
 #if defined(ESP8266_D1_R1)    // WeMos D1 R1
	 /*
	  * For the WeMos D1/R1:
	  * 
	  *------PIN   GPIO-------*
	  */
	#define D2    16  //GPIO16
	#define D5    14  //GPIO14
	#define D6    12  //GPIO12
	#define D7    13  //GPIO13
	#define D8    0   //GPIO0
	#define D9    2   //Same for R1/2
	#define D10   15  //GPI15
	#define D11   13  //repeated SPI bus (MOSI)
	#define D12   12  //repeated SPI bus (MISO)
	#define D13   14  //repeated SPI bus (SCK)
	/*
	 *   Note: other pins are I2C
	 */
	 
	#define USABLE_D1_R1_PINS 7

	//# ESP8266 Host
    #define LEONARDO_HOST   0
    #define UNO_HOST      	0
    #define MEGA_DUE_HOST 	0
	#define ESP8266_D1_R1_HOST	1
	#define ESP8266_D1_R2_HOST	0
	#define RX_PIN			D6
	#define TX_PIN			D7
	#define MAX_RESOURCES	32

#elif defined(ESP8266_D1_R2)    // WeMos D1 R2
	 /*
	  * For the WeMos D1/R2:
	  * 
	  *------PIN   GPIO-------*
	  */
	#define D0    16  //GPIO16
	#define D1    5   //GPIO5 IO SCL
	#define D2    4   //GPIO4 IO SDA
	#define D3    0   //GPIO0 IO 10K pullup
	#define D4    2   //GPIO2 IO 10K pullup LED
	#define D5    14  //GPIO14 IO SCK
	#define D6    12  //GPIO12 IO MISO
	#define D7    13  //GPIO13 IO MOSI
	#define D8    15  //GPIO15 IO 10K pulldown

	/*
	 *   Note: all pins above D8 are repeats of lower pins or I2C.     
	 */

	//# ESP8266 Host--WeMos D1 R2
    #define LEONARDO_HOST   0
    #define UNO_HOST      	0
    #define MEGA_DUE_HOST 	0
	#define ESP8266_D1_R1_HOST	0
	#define ESP8266_D1_R2_HOST	1
	#define RX_PIN			D4		//Pin numbers shift down by 2 for R2
	#define TX_PIN			D5
	#define MAX_RESOURCES	32

#elif defined(HAVE_HWSERIAL3)
	// MEGA Host
    #define LEONARDO_HOST   0
    #define UNO_HOST    	0
    #define MEGA_DUE_HOST 	1
	#define ESP8266_D1_R1_HOST	0
	#define ESP8266_D1_R2_HOST	0
	#define MAX_RESOURCES	16	// dynamic limit of Chariot 
    #define ChariotClient Serial3
	
#elif !defined(HAVE_HWSERIAL0) && defined(HAVE_HWSERIAL1)
    // LEONARDO Host  
	#error Leonardo board type no longer supported by Chariot. If compiling for Yun, plz remove this #error.
    #define LEONARDO_HOST   1
    #define UNO_HOST    	0
    #define MEGA_DUE_HOST 	0
	#define ESP8266_D1_R1_HOST	0
	#define ESP8266_D1_R2_HOST	0
	#define RX_PIN			11
	#define TX_PIN			12//4 -- problem using pin 4?
	#define MAX_RESOURCES	6

#elif (defined(HAVE_HWSERIAL0) && !defined(HAVE_HWSERIAL1))
    // UNO Host
    #define LEONARDO_HOST   0
    #define UNO_HOST      	1
    #define MEGA_DUE_HOST 	0
	#define ESP8266_D1_R1_HOST	0
	#define ESP8266_D1_R2_HOST	0
	#define RX_PIN			11
	#define TX_PIN			12
	#define MAX_RESOURCES	4
#else
  #error Board type not supported by Chariot at this time--contact Qualia Networks Tech Support.
#endif

/*
 * Chariot HW parameters
 *  --events, serial, CoAP resource
 */
#if ESP8266_D1_R1_HOST==1
#define COAP_EVENT_INT_PIN  	D2 // requires Chariot pin3 cut and jumper between pins 2&3
#define RSRC_EVENT_INT_PIN  	D9 // initiate external event interrupt
#define CHARIOT_STATE_PIN   	D8 // driven HIGH when Chariot is online

#elif ESP8266_D1_R2_HOST==1
#define COAP_EVENT_INT_PIN  	D0 // requires Chariot pin3 cut and jumper between pins 2&3
#define RSRC_EVENT_INT_PIN  	D7 // initiate external event interrupt
#define CHARIOT_STATE_PIN   	D6 // driven HIGH when Chariot is online

#else
#define COAP_EVENT_INT_PIN  	3  // initiate external event interrupt
#define RSRC_EVENT_INT_PIN  	9  // initiate external event interrupt
#define CHARIOT_STATE_PIN   	8  // driven HIGH when Chariot is online
#endif

#define MAX_BUFLEN				64
#define MAX_URI_LEN				32
#define MAX_ATTR_LEN			48

#define	TMP275_ADDRESS			0x48
#define FAHRENHEIT    			1
#define CELSIUS       			2
#define KELVIN        			3

#define JSON          			50  // see CoAP RFC7252

#define LT            			1
#define GT            			2
#define EQ            			3
#define ADD						4
#define SUB						5
#define MPY						6
#define MOD						7
#define NEQ						8
#define VAL						9

#define ON            			true
#define OFF           			false

#define LF            			10
#define CR            			13

#define MINUTES       			1
#define SECONDS       			2

class ChariotEPClass
{
  public:
    ChariotEPClass();
	~ChariotEPClass();
    bool begin();
	bool begin(String& loc);
	int available();
	void process();
	bool coapRequest(coap_method_t method, String& host,  String& resource,  
					 coap_content_format_t content, String& opts, String& response);
	bool coapSearchResources(String& mote,  String& resource, String& response);
	int coapResponseGet(String& response);
	bool pinValParse(String& command, int *pin, int *value);
	int allocResource();
	int setResourceBuflen(int id, uint8_t maxBufLen);
	int setResourceUri(int id, const String& uri);
	int setResourceAttr(int id, const String& attr);
	int createResource(const String& uri, uint8_t maxBufLen, const String& attrib);
	int createResource(const __FlashStringHelper* uri, uint8_t maxBufLen, const __FlashStringHelper* attrib);
	bool triggerResourceEvent(int handle, String& event, bool signalChariot);
	
	void serialChariotCmd();
	bool localChariotCmd(String& command, String& response);
	bool chariotGetResponse(String& response);
	void serialChariotCmdHelp();
	int getIdFromURI(String& uri);
	int setPutHandler(int handle, String * (*putCallback)(String& putCmd));
	uint8_t getMotes(String (&motes)[MAX_MOTES]);
	uint8_t getArduinoModel();
	float readTMP275(uint8_t units);
	void enableDebugMsgs();
	void disableDebugMsgs();
	inline bool strip_205_CONTENT(String& response) {
	  int x;
	  if ((x = response.indexOf("2.05 CONTENT ")) != -1)
	  {
		response.remove(0, x+13);
		return true;
	  }
	  return false;
	}
  private:
	uint8_t arduinoType;
	bool chariotAvailable;
	uint8_t maxBufLen;
	bool 	debug;

	// Event resources--these are stored in Chariot
	int nextRsrcId;

	String rsrcURIs[MAX_RESOURCES];
	String rsrcATTRs[MAX_RESOURCES];
	String * (*putCallbacks[MAX_RESOURCES])(String& putCmd);

	uint8_t rsrcChariotBufSizes[MAX_RESOURCES];

	void digitalCommand(String& command);
	void analogCommand(String& command);
	void modeCommand(String& command);
	void chariotSignal(int pin);
	void chariotPrintResponse();
};

extern ChariotEPClass ChariotEP;   // the EndPoint object for Chariot
#if LEONARDO_HOST==1 ||UNO_HOST==1  || ESP8266_D1_R1_HOST==1 || ESP8266_D1_R2_HOST==1
    extern SoftwareSerial ChariotClient;
#endif

#endif
