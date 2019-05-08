#pragma once
#include "Vehicle.h"
#include <FlexCAN.h>

class CanListener : public CANListener
{
public:
	// Vehicle class instance
	Vehicle vehicle;
	//overrides the parent version
	bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller);
	// Translate float values from CAN BUS
	inline float CANIntToFloat(uint16_t floatValue);
	// Translate kelvin temperature values from CAN BUS
	inline float CANKelvinToFloat(uint16_t kelvinValue);
};
