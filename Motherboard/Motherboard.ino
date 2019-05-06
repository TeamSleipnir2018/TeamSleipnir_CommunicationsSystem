/******************************************************************
RU Racing 2019 communications motherboard

Hardware:
- Teensy 3.5
- LoRa RF95
- Adafruit Ultimate GPS Breakout v3
- MCP2551 CAN transceiver

Written by Einar Arnason && Örlygur Ólafsson && Hregggi
******************************************************************/

#include <SPI.h>
#include <RH_RF95.h>
#include <stdint.h>
//#include <FlexCAN.h>
#include <SdFat.h>
#include <TimeLib.h>
//#include "constants.h"
//#include "CanListener.h"
#include "TeensyThreads.h"
#include <TinyGPS.h>
#include <SoftwareSerial.h>

// CAN bus driver
//CanListener canListener;
//CAN_filter_t mask;

// GPS object
TinyGPS gps;
const int NUMBER_OF_MESSAGES = 4;

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences.
const bool GPSE#include <SoftwareSerial.h>CHO = true;

// Variables for Copernicus II GPS module
float flat, flon;
bool newGpsData = false;

// this keeps track of whether we're using the interrupt off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

// SD card variables
SdFatSdio sd;
File outFile;
char filename[20];
File outFileimu;
char filenameimu[20];

// LoRa & Teensy 3.5 setup
#define RF95_FREQ 434.0
#define RFM95_CS 10
#define RFM_RST 25
#define RFM95_INT 24
#define ERRORLED 3
const int COMMAND_SIZE = RH_RF95_MAX_MESSAGE_LEN;
uint8_t COMMAND[COMMAND_SIZE];
const uint8_t Pit_ID = 7;
const uint8_t Car_ID = 6;

#define IMUserial Serial4
uint8_t IMU[19];
bool newIMUData = false;

// LoRa - Radio Frequency driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

const int PAYLOAD_SIZE = 20; //payloadLength();
char payload[PAYLOAD_SIZE];

time_t getTeensy3Time()
{
	return Teensy3Clock.get();
}

void gpsRead()
{
	unsigned long age;
	int year;
	byte month, day, hour, minute, second, hundredth;
	while (1)
	{
		//unsigned long chars;
		//unsigned short sentences, failed;
		// For one second we parse GPS data and report some key values
		for (unsigned long start = millis(); millis() - start < 1000;)
		{
			while (Serial3.available())
			{
				char c = Serial3.read();
				// Serial.write(c); // uncomment this line if you want to see the GPS data flowing
				if (gps.encode(c))
				{
					// Did a new valid sentence come in?
					newGpsData = true;
				}
			}
		}
		// Write the position and current time to variables
		gps.f_get_position(&flat, &flon, &age);
		gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredth, &age);
	}
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
	Serial3.begin(4800);
  	IMUserial.begin(115200);

	while (!Serial)
		;

	if (!rf95.init())
	{
		Serial.println("init failed");
	}

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
	Serial.println("RTC has set the system time");

	// Generate filename
	sprintf(filename, "%d_%d_%d_%d_%d_%d.json", year(), month(), day(), hour(), minute(), second());

	//Create the File
	outFile = sd.open(filename, FILE_WRITE);

	if (!outFile)
	{
		Serial.println("Error: failed to open file");
	};

	 sprintf(filenameimu, "IMU_%d_%d_%d_%d_%d_%d.json", year(), month(), day(), hour(), minute(), second());

	 //Create the File
	 outFileimu = sd.open(filenameimu, FILE_WRITE);

	 if (!outFileimu)
	 {
	   Serial.println("Error: failed to open file");
	 };

	threads.addThread(gpsRead);

	// Initialize the CAN bus
	/*mask.flags.extended = 0;
  	mask.flags.remote = 0;
  	mask.id = 0;
  	Can0.begin(500000, mask, CAN0TX_ALT, CAN0RX_ALT);
  	Can0.attachObj(&canListener);
  	canListener.attachGeneralHandler();*/
}

uint32_t timer = millis();

void loop()
{
	if (newGpsData)
	{
		// Copernicuse GPS if new data write new data
		// ToDo, breita serial print i SD.Write og Lora Send
		Serial.print("LAT=");
		Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
		Serial.print(" LON=");
		Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);

		Serial.print(" TIME=");
		Serial.print(hour(), DEC);
		Serial.print(":");
		Serial.print(minute(), DEC);
		Serial.print(":");
		Serial.print(second(), DEC);
		Serial.print(",");
		Serial.println("0.");
		newGpsData = false;
	}
	char payload[30];

	// TEMP ! for analog suspension sensors. This is a placeholder
	int FR = analogRead(A9);
	int FL = analogRead(A8);
	int RR = analogRead(A7);
	int RL = analogRead(A6);
	long time = getTeensy3Time();

	for (int i = 0; i < 1; i++)
	{
		sprintf(payload, "{\"time\": %ld, \"FR\": %d, \"FL\": %d, \"RR\": %d, \"RL\": %d}!", time, FR, FL, RR, RL); 
	}

	if (outFile)
	{
		outFile.write(payload);
		outFile.flush();
	}
	//Lesa af IMU
    int i=0;
    while (IMUserial.available() > 0)
    {
      uint8_t c = IMUserial.read();
      // Serial.write(c);   // uncomment this line if you want to see the IMU data flowing
      IMU[i] = c;
      i++;
      newIMUData = true;
      if(i > 18)
        break;        
    }
  	//Skrifa a sd kort imu data
	char payloadimu[50];
	if (newIMUData)
	{
	  if (IMU[0] == 170 && IMU[1] == 170)
	  {
	    short lsbyaw = IMU[3];
	    short msbyaw = (IMU[4]*256);
	    short yaw = (lsbyaw + msbyaw)*0.01;

	    short lsbpitch = IMU[5];
	    short msbpitch = (IMU[6]*256);
	    short pitch = (msbpitch + lsbpitch)*0.01;

	    short lsbroll = IMU[7];
	    short msbroll = (IMU[8]*256);
	    short roll = (msbroll + lsbroll)*0.01;

	    short lsbx = IMU[9];
	    short msbx = (IMU[10]*256);
	    short x = (msbx + lsbx)*(9.80665/1000);

	    short lsby = IMU[11];
	    short msby = (IMU[12]*256);
	    short y = (msby + lsby)*(9.80665/1000);

	    short lsbz = IMU[13];
	    short msbz = (IMU[14]*256);
	    short z = (msbz + lsbz)*(9.80665/1000);

	      for (int i = 0; i < 1; i++)
	      {
	        sprintf(payloadimu, "{\"time\": %ld, \"pitch\": %d, \"yaw\": %d, \"roll\": %d, \"x-axis\": %d, \"y-axis\": %d, \"z-axis\": %d}!", time, yaw, pitch, roll, x, y, z); 
	      } 
	  }    
	   newIMUData = false;
	}
	else
	{
	    delay(4);
	}
	if (outFileimu)
	{
	    outFileimu.write(payloadimu);
	    outFileimu.flush();
	}  

}

void sendMessageLoRa(uint8_t CMD)
{
	rf95.setHeaderId(Car_ID);
	uint8_t buf[COMMAND_SIZE] = {"message"}; //{CMD};
	uint8_t len = sizeof(buf);

	rf95.send(buf, sizeof(buf));
	rf95.waitPacketSent();

	if (rf95.waitAvailableTimeout(4000))
	{
		if (rf95.recv(buf, &len))
		{
			Serial.print("Got a reply: ");
			//Serial.println((char *)buf);
			//Serial.print("RSSI: ");
			//Serial.println(rf95.lastRssi(), DEC);
			for (int i = 0; i < len; i++)
			{
				Serial.print((char)buf[i]);
			}
			Serial.println(" ");
		}
		else
		{
			Serial.println("recv failed");
		}
	}
	else
	{
		Serial.println("No reply, is rf95_server running?");
	}
	delay(10);
}

void recieveMessage()
{
	uint8_t buf[COMMAND_SIZE];
	uint8_t len = sizeof(buf);
	//Serial.println("Waiting for reply...");

	// Max wait for data is 1 sec..
	if (rf95.waitAvailableTimeout(1000))
	{
		if (rf95.recv(buf, &len))
		{
			if (rf95.headerId() == Pit_ID)
			{
				switch (buf[0])
				{
				case 0:
					Serial.print("#ACK COMMAND: ");
					Serial.println(COMMAND[1]);
					break;

				default:
					Serial.println("#FAILED");
					break;
				}
				//sendMessage(REPLY_ACK);
				//Serial.println((char *)buf);
				//Serial.print("RSSI: ");
				//Serial.println(rf95.lastRssi(), DEC);
			}
			else
			{
				Serial.println("Not my message, ID: ");
				Serial.println(rf95.headerId());
			}
		}
		else
		{
			Serial.println("ERROR: Receive message failed");
		}
	}
	else
	{
		Serial.println("WARNING: ACK not received");
	}
	delay(1000);
}