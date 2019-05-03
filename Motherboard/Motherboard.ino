/******************************************************************
Team Sleipnir communications motherboard

Hardware:
- Teensy 3.6
- XBee-PRO S2C
- Adafruit Ultimate GPS Breakout v3
- MCP2551 CAN transceiver

Written by Einar Arnason
******************************************************************/

#include <SPI.h>
#include <RH_RF95.h>
//#include <XBee.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <FlexCAN.h>
#include <SdFat.h>
#include <TimeLib.h>
//#include <Adafruit_GPS.h>
#include "constants.h"
#include "CanListener.h"

// CAN bus driver
CanListener canListener;
CAN_filter_t mask;

// GPS object
//Adafruit_GPS GPS(&Serial3);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences.
const bool GPSECHO = true;

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

// SD card variables
SdFatSdio sd;
File outFile;
char filename[20];

// LoRa & Teensy 3.5 setup
#define RF95_FREQ 434.0
#define RFM95_CS 15
#define RFM_RST 25
#define RFM95_INT 2
#define ERRORLED 3
const int COMMAND_SIZE = RH_RF95_MAX_MESSAGE_LEN;
uint8_t COMMAND[COMMAND_SIZE];
const uint8_t Pit_ID = 7;
const uint8_t Car_ID = 6;

// LoRa - Radio Frequency driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// XBee driver
//XBee xbee = XBee();
// SH + SL Address of receiving XBee
//XBeeAddress64 addr64 = XBeeAddress64(0xFF, 0xFE);
//char payload[PAYLOAD_SIZE];

time_t getTeensy3Time()
{
	return Teensy3Clock.get();
}

uint8_t payloadLength()
{
	for (int i = 0; i < PAYLOAD_SIZE; i++)
	{
		if (payload[i] == '!')
		{
			return i;
		}
	}
	return PAYLOAD_SIZE;
}

void setup()
{
	delay(500);
	Serial.begin(9600);

	while (!Serial)
		;

	if (!rf95.init())
		Serial.println("init failed");

	//init SD Card
	while (!sd.begin())
	{
		Serial.println("Error: SD connection failed");
		delay(1000);
	}

	// set the Time library to use Teensy 3.0's RTC to keep time
	setSyncProvider(getTeensy3Time);
	if (timeStatus() != timeSet)
	{
		Serial.println("Unable to sync with the RTC");
	}
	else
	{
		Serial.println("RTC has set the system time");
	}

	// Generate filename
	sprintf(filename, "%d_%d_%d_%d_%d_%d.json", year(), month(), day(), hour(), minute(), second());

	//Create the File
	outFile = sd.open(filename, FILE_WRITE);
	if (!outFile)
	{
		Serial.println("Error: failed to open file");
	};

	// Initialize the CAN bus
	mask.flags.extended = 0;
	mask.flags.remote = 0;
	mask.id = 0;
	Can0.begin(500000, mask, CAN0TX_ALT, CAN0RX_ALT);
	Can0.attachObj(&canListener);
	canListener.attachGeneralHandler();

	// Initialize XBee serial
	//Serial2.begin(9600);
	//xbee.setSerial(Serial2);

	/*
	// 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
	GPS.begin(9600);
	Serial3.begin(9600);

	// uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	// uncomment this line to turn on only the "minimum recommended" data
	//GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
	// For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
	// the parser doesn't care about other sentences at this time
  
	// Set the update rate
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
	// For the parsing code to work nicely and have time to sort thru the data, and
	// print it out we don't suggest using anything higher than 1 Hz

	// Request updates on antenna status, comment out to keep quiet
	GPS.sendCommand(PGCMD_ANTENNA);
#ifdef __arm__
	usingInterrupt = false;  //NOTE - we don't want to use interrupts on the Due
#else
	useInterrupt(true);
#endif
*/
}

#ifdef __AVR__
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
	char c = GPS.read();
// if you want to debug, this is a good time to do it!
#ifdef UDR0
	if (GPSECHO)
		if (c)
			UDR0 = c;
// writing direct to UDR0 is much much faster than Serial.print
// but only one character can be written at a time.
#endif
}

void useInterrupt(boolean v)
{
	if (v)
	{
		// Timer0 is already used for millis() - we'll just interrupt somewhere
		// in the middle and call the "Compare A" function above
		OCR0A = 0xAF;
		TIMSK0 |= _BV(OCIE0A);
		usingInterrupt = true;
	}
	else
	{
		// do not call the interrupt function COMPA anymore
		TIMSK0 &= ~_BV(OCIE0A);
		usingInterrupt = false;
	}
}
#endif //#ifdef__AVR__

uint32_t timer = millis();

void loop()
{
	/*
	// in case you are not using the interrupt above, you'll
	// need to 'hand query' the GPS, not suggested :(
	if (! usingInterrupt) {
	// read data from the GPS in the 'main loop'
	char c = GPS.read();
	// if you want to debug, this is a good time to do it!
	if (GPSECHO)
		if (c) Serial.print(c);
	}
  
	// if a sentence is received, we can check the checksum, parse it...
	if (GPS.newNMEAreceived()) {
	// a tricky thing here is if we print the NMEA sentence, or data
	// we end up not listening and catching other sentences! 
	// so be very wary if using OUTPUT_ALLDATA and trytng to print out data
	//Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
	if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
		return;  // we can fail to parse a sentence in which case we should just wait for another
	}

	// if millis() or timer wraps around, we'll just reset it
	if (timer > millis())  timer = millis();

	// approximately every 2 seconds or so, print out the current stats
	if (millis() - timer > 2000) { 
		timer = millis(); // reset the timer
    
		Serial.print("\nTime: ");
		Serial.print(GPS.hour, DEC); Serial.print(':');
		Serial.print(GPS.minute, DEC); Serial.print(':');
		Serial.print(GPS.seconds, DEC); Serial.print('.');
		Serial.println(GPS.milliseconds);
		Serial.print("Date: ");
		Serial.print(GPS.day, DEC); Serial.print('/');
		Serial.print(GPS.month, DEC); Serial.print("/20");
		Serial.println(GPS.year, DEC);
		Serial.print("Fix: "); Serial.print((int)GPS.fix);
		Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
		if (GPS.fix) {
			Serial.print("Location: ");
			Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
			Serial.print(", "); 
			Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      
			Serial.print("Speed (knots): "); Serial.println(GPS.speed);
			Serial.print("Angle: "); Serial.println(GPS.angle);
			Serial.print("Altitude: "); Serial.println(GPS.altitude);
			Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
		}
	}
	*/
	long time = getTeensy3Time();

	for (int i = 0; i < NUMBER_OF_MESSAGES; i++)
	{
		switch (i)
		{
		case 0:
			sprintf(payload,
					"{\"time\": %ld, \"rpm\": %d, \"volt\": %0.2f, \"waterTemp\": %0.2f, \"speed\": %d}!",
					time,
					canListener.vehicle.rpm,
					canListener.vehicle.voltage,
					canListener.vehicle.waterTemp,
					canListener.vehicle.speed);
			break;
		case 1:
			sprintf(payload,
					"{\"time\": %ld, \"oilTemp\": %0.2f, \"gear\": %d, \"airTemp\": %0.2f, \"map\": %d}!",
					time,
					canListener.vehicle.oilTemp,
					canListener.vehicle.gear,
					canListener.vehicle.airTemp,
					canListener.vehicle.map);
			break;
		case 2:
			sprintf(payload,
					"{\"time\": %ld, \"ecuTemp\": %0.2f, \"fuelPressure\": %0.2f, \"fanOn\": %s, \"fuelPumpOn\": %s}!",
					time,
					canListener.vehicle.ecuTemp,
					canListener.vehicle.fuelPressure,
					canListener.vehicle.fanOn ? "true" : "false",
					canListener.vehicle.fuelPumpOn ? "true" : "false");
			break;
		case 3:
			sprintf(payload,
					"{\"time\": %ld, \"cylcontrib1\": %d, \"cylcontrib2\": %d, \"cylcontrib3\": %d, \"cylcontrib4\": %d}!",
					time,
					canListener.vehicle.cylcontrib1,
					canListener.vehicle.cylcontrib2,
					canListener.vehicle.cylcontrib3,
					canListener.vehicle.cylcontrib4);
			break;
		}

		ZBTxRequest zbTx = ZBTxRequest(addr64, (uint8_t *)payload, payloadLength());
		ZBTxStatusResponse txStatus = ZBTxStatusResponse();
		//xbee.send(zbTx);
		// after sending a tx request, we expect a status response
		// wait up to half second for the status response
		/*if (xbee.readPacket(500))
		{
			// got a response!

			// should be a znet tx status
			if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE)
			{
				xbee.getResponse().getZBTxStatusResponse(txStatus);

				// get the delivery status, the fifth byte
				if (txStatus.getDeliveryStatus() == SUCCESS)
				{
					// success.  time to celebrate
					Serial.println("Success!");
				}
				else
				{
					// the remote XBee did not receive our packet. is it powered on?
					Serial.println("the remote XBee did not receive packet");
				}
			}
		}
		else if (xbee.getResponse().isError())
		{
			Serial.print("Error reading packet.  Error code: ");
			Serial.println(xbee.getResponse().getErrorCode());
		}
		else
		{
			// local XBee did not provide a timely TX Status Response -- should not happen
			Serial.println("local XBee did not provide a timely TX Status Response");
		}*/

		if (outFile)
		{
			outFile.write(payload);
			outFile.flush();
		}
	}
}