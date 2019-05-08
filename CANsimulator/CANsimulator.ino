#include <stdint.h>
#include <FlexCAN.h>

// Vehicle values
uint16_t rpm;
uint16_t oilTemp;
uint16_t waterTemp;
uint16_t brakeTemp;
uint8_t gear;
uint8_t speed;
uint16_t voltage;
bool fanOn;

uint16_t speedCount;
bool reverse;
const uint16_t MAX_RPM = 14000;

static CAN_message_t msg1;
static CAN_message_t msg2;

void demo()
{
	if (reverse)
	{
		if (rpm != 0)
		{
			rpm -= 25;
			Serial.print("RPM : ");
			Serial.println(rpm);
		}
		else
		{
			oilTemp = random(4730, 4830);
			waterTemp = random(4030, 4230);
			brakeTemp = random(3730, 4230);
			voltage = random(11900, 12900);

			if (gear > 0)
			{
				gear--;
				rpm = 9000;
			}
			else
			{
				rpm = 0;
				speed = 0;
			}
		}
	}
	else
	{
		if (rpm != MAX_RPM)
		{
			rpm += 25;
		}
		else
		{
			oilTemp = random(4730, 4830);
			waterTemp = random(4030, 4230);
			brakeTemp = random(3730, 4230);
			voltage = random(11900, 12900);

			if (gear < 6)
			{
				gear++;
				rpm = 2000;
			}
			else
			{
				rpm = 13500;
			}
		}
	}

	if (reverse)
	{
		if (speed > 0)
		{
			if (speedCount == 15)
			{
				speed--;
				speedCount = 0;
			}
			else
			{
				speedCount++;
			}
		}
	}
	else
	{
		if (speed < 200 && gear != 0)
		{
			if (speedCount == 15)
			{
				speed++;
				speedCount = 0;
			}
			else
			{
				speedCount++;
			}
		}
	}
}

void setup()
{
	// Initialize the CAN bus
	Can0.begin(500000);
	Serial.begin(9600);

	rpm = 0;
	speed = 0;
	oilTemp = 130;
	waterTemp = 130;
	voltage = 12000;

	reverse = false;

	// CAN message headers
	msg1.id = 0x1;
	msg1.len = 8;
	//msg1.flags.extended = 0;
	//msg1.flags.remote = 0;

	msg2.id = 0x2;
	msg2.len = 8;
	//msg2.flags.extended = 0;
	//msg2.flags.remote = 0;
	msg2.ext = 0;
	msg2.rtr = 0;
}

void loop()
{
	demo();
	// CAN message 1 payload
	msg1.buf[0] = (uint8_t)rpm;
	msg1.buf[1] = rpm >> 8;
	msg1.buf[2] = (uint8_t)voltage;
	msg1.buf[3] = voltage >> 8;
	msg1.buf[4] = (uint8_t)waterTemp;
	msg1.buf[5] = waterTemp >> 8;
	msg1.buf[6] = (uint8_t)speed;
	msg1.buf[7] = 0;

	/*while (!Can0.write(msg1, Can0.getFirstTxBox())) {
  }*/

	// CAN message 2 payload
	msg2.buf[0] = (uint8_t)oilTemp;
	msg2.buf[1] = oilTemp >> 8;
	msg2.buf[2] = gear;
	msg2.buf[3] = 0;
	msg2.buf[4] = (uint8_t)waterTemp;
	msg2.buf[5] = waterTemp >> 8;
	msg2.buf[6] = speed;
	msg2.buf[7] = speed >> 8;

	while (!Can0.write(msg2, Can0.getLastTxBox())) {
  }

	if (gear == 6 && rpm == 14000)
	{
		reverse = true;
	}
	if (reverse && gear == 0 && rpm == 0)
	{
		reverse = false;
	}

	delay(5);
}
