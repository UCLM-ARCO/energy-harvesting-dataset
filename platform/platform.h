/*
 *  Function library for the testbed  
 *
 *
 *  Version 1.0
 *  Author: Soledad Escolar
 *  Date: February 2019
 */


// Ensure this library description is only included once
#ifndef platformClass_h
#define platformClass_h
#define ARDUINO_FEATHER_M0

#include "Arduino.h"
#include <SD.h>
#include "RTClib.h"
#include <SPI.h>
#include <RH_RF95.h>

// Library interface description
class platformClass {
	// Singleton instance of the SD
	static File file;
	static bool initializedSD;
	// Singleton instance of the rtc
	static RTC_PCF8523 rtc;
	static bool initializedRTC;
	static bool initializedRFMLoRa;
	public: 
	//***************************************************************
	// Constructor of the class					*
	//***************************************************************
  
		//! Class constructor.
		platformClass(void);
		 
	//***************************************************************
	// Public Methods						*
	//***************************************************************
		//!  Returns the library version 
		/*!
		\param void
		\return int : The library version. 
		*/	int version(void);
		
		//! Initializes the Real Time Clock (RTC)
		/*!
		\param void
		\return int with the success (0) or fail (-1) of the initialization
		*/	int initializeRTC(void);
	
		//! Adjust the time for Real Time Clock (RTC)
		/*!
		\param void
		\return void
		*/	void adjustRTC(void);
		
		//! Initialize Lora module
		/*!
		\param void
		\return int with the success (0) or fail (-1) of the initialization
		*/	int initializeLoRa(void);

		//! Send data through Lora module
		/*!
		\param String with the data
		\return int with the success (0) or fail (-1) of the initialization
		*/	int sendLoRa(String);

		//! Receive data through Lora module
		/*!
		\param void
		\return String with the data received
		*/	String receiveLoRa(void);

		//! Returns the temperature
		/*!
		\param void
		\return float : The temperature value.   
		*/	float getTemperature( void );
		
		//! Returns the humidity
		/*!
		\param void
		\return float : The humidity value.   
		*/	float getHumidity( void );

		//! Returns the battery voltage
		/*!
		\param void
		\return float : The battery voltage.   
		*/	float getBatteryVoltage( void );

		//!  Initializes INA0
		/*!
		\param void
		\return void
		*/	void initINA0(void);
		//!  Initializes INA1
		/*!
		\param void
		\return void
		*/	void initINA1(void);
		//!  Initializes INA2
		/*!
		\param void
		\return void
		*/	void initINA2(void);
		//! Returns the load power (ina1)
		/*!
		\param void
		\return float with the load power (mW)
		*/	float getLoadPower( void );
		
		//! Returns the battery power (ina3)
		/*!
		\param void
		\return float with the power of the battery (mW)
		*/	float getBatteryPower( void );
	

		//! Returns the panel power (ina0)
		/*!
		\param void
		\return float with the power of the panel (mW)
		*/	float getPanelPower( void );
		
		//! Returns the load current (ina1)
		/*!
		\param void
		\return float with the load current (mA)
		*/	float getLoadCurrent( void );
		
		//! Returns the battery current (ina3)
		/*!
		\param void
		\return float with the current of the battery (mA)
		*/	float getBatteryCurrent( void );
	

		//! Returns the panel current (ina0)
		/*!
		\param void
		\return float with the current of the panel (mA)
		*/	float getPanelCurrent( void );
		
		//! Returns the speed of the wind (anenometer)
		/*!
		\param void
		\return float : The anenometer value 
		*/	float getSpeedOfWind( void );
		
	
		//! Open a file to read/write on SD
		/*!
		\param String : filename
		\param mode : open mode (READ|WRITE)
		\return int: 0 if success and -1 if fail
		*/	static int open( String , int);
	
		//! Close a file
		/*!
		\param void 
		\return void
		*/	static void close();
	
		//! Store data in SD
		/*!
		\param String : data to be stored
		\return int: number of bytes written 
		*/	static int writeline( String );
		
		//! Read data from SD	
		/*!
		\param File : file descriptor
		\return string: number of bytes written . 
		*/	String readline();
	
		//! Activate debug by means of display
		/*!
		\param void
		\return void
		*/	void initializeDisplay();
	
		//! Display data on screen
		/*!
		\param String: title
		\param String: data
		\return void
		*/	void displayLCD(String,String);
	
		//! Initialize IoTnode
		/*!
		\param void
		\return void
		*/	void initIoTNode();

		//! Read Temperature Sensor
		float readTemperature();
		
		//! Read Humidity Sensor
		float readHumidity();
		
		//! Read battery voltage sensor
		float readBatteryVoltage();
		/* Initialize the micro SD
		\param void
		\return int with the success (0) or fail (-1) of the initialization 
		*/	static int initializeSD( void );

		//! Get the time 
		String getTime();
		/*
		\return String with the date and time */
		
		
		
	private:

	//***************************************************************
	// Private Methods						*
	//***************************************************************
	
		//! Change the state of the relay
		void relay(int status);
		
		//! Prepare the screen to display data
		/*!
		\param void
		\return void
		*/	void clean( void );		 
		
	//***************************************************************
	// Private Variables						*
	//***************************************************************
};
extern platformClass platform;

#endif

