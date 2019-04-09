/*
 *  Function library for the testbed 
 *
 *  Version 1.0
 *  Author: Soledad Escolar
 */


// include this library's description file

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>
#include <SHT1x.h>
#include <SPI.h>
#include <RH_RF95.h>
#include "platform.h"
//***************************************************************
// Variables and definitions					*
//***************************************************************
//! Set the version of the class
	const int vs = 1;
	
	#define	ADDRESS0 0x40
	#define ADDRESS1 0x41
	#define	ADDRESS2 0x44
	Adafruit_INA219 ina0(ADDRESS0);
	Adafruit_INA219 ina1(ADDRESS1);
	Adafruit_INA219 ina2(ADDRESS2);
	
	#define BATTERY  	A0 
	#define ANENOMETER 	A1
	#define AUX1 		(3.3/1023)
	#define AUX2 		(32.4/1.6)
	#define	WRITE		0
	#define READ		1
	

	#define LED           	13
	#define PINSET		11
	#define PINUNSET 	12
	#define CS_SD 		10
	#define OLED_RESET 	4
	#define RFM95_CS        8
	#define RFM95_RST       4
	#define RFM95_INT       3
	#define	RFM95_TIMEOUT	1000
	// Change to 433.0 or other frequency, must match RX's freq!
	#define RF95_FREQ 	433.0

	
	#define	SHT1X_ADDRESS	A1
	#define	SHT1X_CONTROL	A2
	#define	TEMPERATURE 	"TEMPERATURE\n"
	#define	HUMIDITY	"HUMIDITY\n"
	#define	BATTERYVOLT 	"BATTERYVOLT\n"
	#define DISPLAY_ADDRESS	0x3C	
	#define WELCOME_MSG	"Starting Platform"	
	#define VERSION_MSG	(vs)
	
	
	File platformClass::file;
	bool platformClass::initializedSD=false;
	bool platformClass::initializedRTC=false;
	bool platformClass::initializedRFMLoRa=false;
	
	// Singleton instance of the rfm95
	RH_RF95 rf95(RFM95_CS, RFM95_INT);

	// Singleton instance of the rtc
	RTC_PCF8523 platformClass::rtc;
	
	// SHT1x sensor
	SHT1x sht1x(SHT1X_ADDRESS,SHT1X_CONTROL);
	
	// display
	Adafruit_SSD1306 display(OLED_RESET);
	
//***************************************************************
// Constructor of the class					*
//***************************************************************

	//! Function that handles the creation and setup of instances
	platformClass::platformClass(void) { 
		
		pinMode(LED,OUTPUT);
		pinMode(CS_SD, OUTPUT);
		pinMode(RFM95_CS, OUTPUT);
		pinMode(PINSET, OUTPUT);
		pinMode(PINUNSET, OUTPUT);
		digitalWrite(PINSET, LOW);
		digitalWrite(PINUNSET, LOW);
		digitalWrite(LED,HIGH);	
	}
	
	
//***************************************************************
// Public Methods						*
//***************************************************************

	//!******************************************************************************
	//!		Name:	initIoTNode()						*
	//!		Description: initializes the pins for IoT node			*
	//!		Param : void							*
	//!		Returns: void							*
	//!		Example: platform.initIoTnode();				*
	//!******************************************************************************

	void platformClass::initIoTNode(void)
	{
		pinMode(LED,OUTPUT);
		pinMode(BATTERY, INPUT);
		digitalWrite(LED,HIGH);
	}

	
	//!******************************************************************************
	//!		Name:	version()						*
	//!		Description: It check the version of the library		*
	//!		Param : void							*
	//!		Returns: void							*
	//!		Example: platform.version();					*
	//!******************************************************************************

	int platformClass::version(void)
	{
		return vs;
	}

	//!******************************************************************************
	//!	Name:	initializeRTC()							*
	//!	Description: Initializes the Real Time Clock (RTC)			*
	//!	Param : void								*
	//!	Returns: int with the success (0) or fail (-1) of the initialization	*													
	//!	Example: platform.initializeRTC();					*
	//!******************************************************************************

	int platformClass::initializeRTC(void)
	{
		if (! platformClass::rtc.begin()) {
			Serial.println("DEBUG: RTC not found!");
			return -1;
		}
		platformClass::initializedRTC = true;
		return int(platformClass::initializedRTC);
	}

	//!******************************************************************************
	//!	Name:	adjustRTC()							*
	//!	Description: Set the RTC to the date & time this sketch was compiled	*
	//!	Param : void								*
	//!	Returns: void								*
	//!	Example: platform.adjustRTC();						*
	//!******************************************************************************
	void platformClass::adjustRTC(void)
	{
		if (! platformClass::rtc.initialized()) {
			Serial.println("DEBUG: RTC is NOT running!");
			platformClass::rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		}else{
			Serial.println("DEBUG: RTC initialized!");
		}
	}

	//!******************************************************************************
	//!	Name:	initializeLoRa()						*
	//!	Description: Initializes the LORA communication module.			*
	//!	Param : void								*
	//!	Returns: int with the success (0) or fail (-1) of the initialization	*
	//!	Example: platform.initializeLoRa();					*
	//!******************************************************************************
	int platformClass::initializeLoRa(void)
	{
		digitalWrite(CS_SD, HIGH);	   //Disable SD
		digitalWrite(RFM95_CS, LOW);	   //Enable Lora 
		while (!rf95.init()) {
			Serial.println("DEBUG: RFM LoRa not initialized!");
			while (1);
		}
		platformClass::initializedRFMLoRa = true;
		Serial.println("DEBUG: RFM LoRa initialized!");

		// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	  	if (!rf95.setFrequency(RF95_FREQ)) {
			Serial.println("DEBUG: Setting LoRa frequency failed!");
    			while (1);
  		}
		Serial.println("DEBUG: Setting LoRa frequency !");

  		// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  		// you can set transmitter powers from 5 to 23 dBm:
		rf95.setTxPower(23, false);
		return int(platformClass::initializedRFMLoRa);
	}
	
	//!******************************************************************************
	//!	Name:	sendLoRa()							*
	//!	Description: send data through the LORA communication module.		*
	//!	Param : data to send							*
	//!	Returns: int with the success (0) or fail (-1) of the initialization	*
	//!	Example: platform.sendLoRa();						*
	//!******************************************************************************
	int platformClass::sendLoRa(String data)
	{
		const char *msg=data.c_str();

		digitalWrite(CS_SD, HIGH);	   //Disable SD
		digitalWrite(RFM95_CS, LOW);	   //Enable Lora 

		Serial.print("DEBUG: Sending Message: ");
		Serial.println(msg);
		rf95.send((uint8_t*)msg, sizeof(msg));
	 	delay(10);
		rf95.waitPacketSent();
		return 0;
	}
	
	//!******************************************************************************
	//!	Name:	receiveLoRa()							*
	//!	Description: receive data through the LORA communication module.	*
	//!	Param : void								*
	//!	Returns: String with the data received					*
	//!	Example: platform.receiveLoRa();					*
	//!******************************************************************************
	String platformClass::receiveLoRa(void)
	{
		uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  		uint8_t len = sizeof(buf);
 
		digitalWrite(CS_SD, HIGH);	   //Disable SD
		digitalWrite(RFM95_CS, LOW);	   //Enable Lora 
    		// Should be a reply message for us now   
		if (rf95.waitAvailableTimeout(RF95_FREQ)) 
		{	
			if (rf95.recv(buf, &len))
   			{
      				Serial.print("DEBUG: received from LoRA : ");
				Serial.println((char*)buf);
			      	//Serial.print("RSSI: ");
			      	//Serial.println(rf95.lastRssi(), DEC);    
			}else{
				Serial.println("DEBUG: recv failed!");
			}
		}else{
			Serial.println("DEBUG: No reply recv from LoRa module!");
		}
		return (char*)buf;
	}
		
	//!******************************************************************************
	//!	Name:	getTemperature()						*
	//!	Description: Read the temperature sensor				*
	//!	Param : void								*
	//!	Returns: float with the temperature					*
	//!	Example: platform.getTemperature();					*
	//!******************************************************************************
	float platformClass::getTemperature(void)
	{
		
		char inChar;
		String temp="";
		Serial1.print(TEMPERATURE);
		bool end=false;
		while (!end){
			while (Serial1.available()) {
				inChar = (char)Serial1.read();
				temp += inChar;
				if (inChar == '\n') {
					end = true;
					break;
				}
				
			}
			if (end) {break;}
		}
		const char* foo = temp.c_str();
		return (atof(foo));
	}
 	
	//!******************************************************************************
	//!	Name:	getHumidity()							*
	//!	Description: Read the humidity						*
	//!	Param : void								*
	//!	Returns: float with the humidity					*
	//!	Example: platform.getHumidity();					*
	//!******************************************************************************
	float platformClass::getHumidity(void)
	{

		char inChar;
		String hum="";
		Serial1.print(HUMIDITY);
		bool end=false;
		while (!end){
			while (Serial1.available()) {
				inChar = (char)Serial1.read();
				hum += inChar;
				if (inChar == '\n') {
					end = true;
					break;
				}
				
			}
			if (end) {break;}
		}
		const char* foo = hum.c_str();
		return (atof(foo));
	}
	//!******************************************************************************
	//!	Name:	getBatteryVoltage()						*
	//!	Description: Read the battery voltage					*
	//!	Param : void								*
	//!	Returns: float with the battery voltage					*
	//!	Example: platform.getBatteryVoltage();					*
	//!******************************************************************************
	float platformClass::getBatteryVoltage(void)
	{
		char inChar;
		String batt="";
		Serial1.print(BATTERYVOLT);
		bool end=false;
		while (!end){
			while (Serial1.available()) {
				inChar = (char)Serial1.read();
				batt += inChar;
				if (inChar == '\n') {
					end = true;
					break;
				}
				
			}
			if (end) {break;}
		}
		const char* foo = batt.c_str();
		return (atof(foo));
	}
	//!******************************************************************************
	//!	Name:	getTime()							*
	//!	Description: get the time in which a sample is collected		*
	//!	Param : void								*
	//!	Returns: String								*
	//!	Example: platform.getTime();						*
	//!******************************************************************************
	String  platformClass::getTime()
	{
		char date[20];
		
		if (!platformClass::initializedRTC){
			return "";
		}
		DateTime now = platformClass::rtc.now(); //Obtener fecha y hora actual.

		int day = now.day();
		int month = now.month();
		int year = now.year();
		int hour = now.hour();
		int minute = now.minute();
		int second = now.second();

		sprintf( date, "%.2d.%.2d.%.4d %.2d:%.2d:%.2d", day, month, year, hour, minute, second); 
		return String( date );
	}  
	
	//!******************************************************************************
	//!	Name:	initINA0()							*
	//!	Description: Initializes sensor INA0					*
	//!	Param : void								*
	//!	Returns: void								*
	//!	Example: platform.initINA0();						*
	//!******************************************************************************
	void platformClass::initINA0(void)
	{
		ina0.begin();
	}


	//!******************************************************************************
	//!	Name:	getPanelCurrent()						*
	//!	Description: Read the current sensor INA0				*
	//!	Param : void								*
	//!	Returns: float with the current of the panel				*
	//!	Example: platform.getPanelCurrent();					*
	//!******************************************************************************
	float platformClass::getPanelCurrent(void)
	{
		float current=0.0;
		ina0.begin();
	
		relay(PINSET);
		delay(1000);   
		current = ina0.getCurrent_mA();
		relay(PINUNSET);  
		return current;
	}
	
	
	//!******************************************************************************
	//!	Name:	initINA1()							*
	//!	Description: Initializes sensor INA1					*
	//!	Param : void								*
	//!	Returns: void								*
	//!	Example: platform.initINA1();						*
	//!******************************************************************************
	void platformClass::initINA1(void)
	{
		ina1.begin();
	}
	//!******************************************************************************
	//!	Name:	getLoadCurrent()						*
	//!	Description: Read the current sensor INA1				*
	//!	Param : void								*
	//!	Returns: float with the load current 					*
	//!	Example: platform.getLoadCurrent();					*
	//!******************************************************************************
	float platformClass::getLoadCurrent(void)
	{
		float current=0.0;
		ina1.begin();
		current = ina1.getCurrent_mA();
		return current;
	}
	
	//!******************************************************************************
	//!	Name:	initINA2()							*
	//!	Description: Initializes sensor INA2					*
	//!	Param : void								*
	//!	Returns: void								*
	//!	Example: platform.initINA2();						*
	//!******************************************************************************
	void platformClass::initINA2(void)
	{
		ina2.begin();
	}
	//!******************************************************************************
	//!	Name:	getBatteryCurrent()						*
	//!	Description: Read the current sensor INA2				*
	//!	Param : void								*
	//!	Returns: float with the current of the battery				*
	//!	Example: platform.getBatteryCurrent();					*
	//!******************************************************************************
	float platformClass::getBatteryCurrent(void)
	{
		float current=0.0;
		ina2.begin();
		current = ina2.getCurrent_mA();
		return current;
	}	



	//!******************************************************************************
	//!	Name:	getPanelPower()							*
	//!	Description: Read the power from sensor INA0				*
	//!	Param : void								*
	//!	Returns: float with the power of the panel				*
	//!	Example: platform.getPanelPower();					*
	//!******************************************************************************
	float platformClass::getPanelPower(void)
	{
		float power=0.0;
		relay(PINSET);
		delay(1000);   
		power = ina0.getPower_mW();
		relay(PINUNSET);  
		return power;
	}
	
	
	//!******************************************************************************
	//!	Name:	getLoadPower()							*
	//!	Description: Read the power from sensor INA1				*
	//!	Param : void								*
	//!	Returns: float with the load power 					*
	//!	Example: platform.getLoadPower();					*
	//!******************************************************************************
	float platformClass::getLoadPower(void)
	{
		float power=0.0;

		power = ina1.getPower_mW();
		return power;
	}
	
	//!******************************************************************************
	//!	Name:	getBatteryPower()						*
	//!	Description: Read the power from sensor INA2				*
	//!	Param : void								*
	//!	Returns: float with the power of the battery				*
	//!	Example: platform.getBatteryPower();					*
	//!******************************************************************************
	float platformClass::getBatteryPower(void)
	{
		float power=0.0;

		power = ina2.getPower_mW();
		return power;
	}	
	

	
	//!******************************************************************************
	//!	Name:	getSpeedOfWind()						*
	//!	Description: Returns the speed of the wind (ANENOMETER)			*
	//!	Param : void								*
	//!	Returns: float with the ANENOMETER value in meters per second		*
	//!	Example: platform.getSpeedOfWind();					*
	//!******************************************************************************
	float  platformClass::getSpeedOfWind(void)
	{
		float windOfSpeed = 0.0;
		
		//Reading from ANENOMETER and conversion to meters/second according to the datasheet 
		windOfSpeed = ((AUX1*analogRead(ANENOMETER))-0.4)*AUX2;
		return windOfSpeed;
	}
	

	//!******************************************************************************
	//!	Name:	open()								*
	//!	Description: open a file on the memory card				*
	//!	Param : filename, mode							*
	//!	Returns: int 0 if ok and -1 if not ok					*
	//!	Example: platform.open();						*
	//!******************************************************************************
	int  platformClass::open(String filename, int mode)
	{
		
		digitalWrite(RFM95_CS, HIGH);      //Disable LORA
		digitalWrite(CS_SD, LOW);	   //Enable SD
		if (platformClass::initializedSD){
			if (mode == WRITE){
				platformClass::file = SD.open(filename,FILE_WRITE);
				if (!platformClass::file){
					Serial.println("DEBUG1: Open file failed!");
					return -1;
				}
			}else{
				platformClass::file = SD.open(filename);
				if (!platformClass::file){
					Serial.println("DEBUG: Open file failed!");
					return -1;
				}
			}
			return 0;
		}else{
			Serial.println("DEBUG: Open file failed!");
		}
		return -1;
		
	
	}
	
	
	//!******************************************************************************
	//!	Name:	close()								*
	//!	Description: close a file 						*
	//!	Param : void								*
	//!	Returns: void								*
	//!	Example: platform.close();						*
	//!******************************************************************************
	void  platformClass::close()
	{
		digitalWrite(RFM95_CS, HIGH);      //Disable LORA
		digitalWrite(CS_SD, LOW);	   //Enable SD
		if (platformClass::file){
			platformClass::file.close();
		}
	}  
	
	//!******************************************************************************
	//!	Name:	write()								*
	//!	Description: write data into memory card SD				*
	//!	Param : String to write							*
	//!	Param : File to write							*
	//!	Returns: 0 if success or -1 if fail					*
	//!	Example: platform.writeln();						*
	//!******************************************************************************
	int  platformClass::writeline(String data)
	{
		digitalWrite(RFM95_CS, HIGH);      //Disable LORA
		digitalWrite(CS_SD, LOW);	   //Enable SD
		if (platformClass::file){
			file.println(data); 	
			return 0;
		}
		return -1;
		
	}
	
	//!******************************************************************************
	//!	Name:	readline()							*
	//!	Description: read data from memory card SD				*
	//!	Param : File to read							*
	//!	Returns: String with the data 						*
	//!	Example: platform.readline();						*
	//!******************************************************************************
	String  platformClass::readline()
	{
		String data="";
		digitalWrite(RFM95_CS, HIGH);      //Disable LORA
		digitalWrite(CS_SD, LOW);	   //Enable SD
		if ((platformClass::file) and (platformClass::file.available())){
			data = file.read();
		}
		return data;
	}
	

	//!******************************************************************************
	//!	Name:	initializeDisplay()						*
	//!	Description: Initialize the LCD						*
	//!	Param : void								*
	//!	Returns: void								*
	//!	Example: platform.initializeDisplay();					*
	//!******************************************************************************
	void  platformClass::initializeDisplay(void)
	{
		display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
   
		delay(2000);
		clean();
		display.println(WELCOME_MSG);
		display.println(VERSION_MSG);
		display.display();
		
		Serial.println("DEBUG: Display Initialized!");
	}
	
	//!******************************************************************************
	//!	Name:	displayLCD()							*
	//!	Description: Display data on screen					*
	//!	Param : String with the title and 					*
	//!	Param : String with the data 						*
	//!	Returns: void								*
	//!	Example: platform.displayLCD();						*
	//!******************************************************************************
	void  platformClass::displayLCD(String title, String data)
	{	
		clean();
		
		display.print(title);
		display.print(':');
		display.print(data);
		display.display();
	}

	//! This function will read the temperature sensor of IoTnode 
	float platformClass::readTemperature(){
		float value;
		value = sht1x.readTemperatureC();
		if (isnan(value)) {  // check if 'is not a number'
			return -1000.0;
		} 
		return value;
	}
	//! This function will read the humidity sensor of IoTnode 
	float platformClass::readHumidity(){	
		float value;
		value = sht1x.readHumidity();
		if (isnan(value)) {  // check if 'is not a number'
			return -1.0;
		} else { 
    		return value;
		}
	}
	//! This function will read the battery voltage of IoTnode 
	
	float platformClass::readBatteryVoltage(){	
		float value;
		value = analogRead(BATTERY) * AUX1;
		return value;
	}
	
//***************************************************************
// Private Methods						*
//***************************************************************

		
	//! This function will change the status of the relay:  
	// Short circuit current of the panel: we close relay and wait 
	//current be stablished again, then it puts the relay in its original state

	void platformClass::relay(int status)
	{
		digitalWrite(status,HIGH);
		delay(10);
		digitalWrite(status,LOW);
	}
	
	//! This function will prepare the display for visualization
	void platformClass::clean(void)
	{
		display.clearDisplay();
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(0,0);
	}
	
	//!******************************************************************************
	//!	Name:	initializeSD()							*
	//!	Description: Initialize the memory card SD				*
	//!	Param : void								*
	//!	Returns: int with the success (0) or fail (-1) of the initialization 	*
	//!	Example: platform.initializeSD();					*
	//!******************************************************************************
	int  platformClass::initializeSD(void)
	{
		digitalWrite(RFM95_CS, HIGH);      //Disable LORA
		digitalWrite(CS_SD, LOW);	   //Enable SD
		if (!SD.begin(CS_SD)){
			Serial.println("DEBUG: SD initialization failed!");
			platformClass::initializedSD = false;
			return -1;
		}
		platformClass::initializedSD = true;
		Serial.println("DEBUG: SD Initialized!");
		return 0;
	}

	
	
//***************************************************************
// Preinstantiate Objects					*
//***************************************************************

	platformClass platform = platformClass();





