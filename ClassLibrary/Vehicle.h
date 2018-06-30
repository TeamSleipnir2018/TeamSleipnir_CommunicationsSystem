#pragma once
#include <stdint.h>

class Vehicle {
public:
	// Vehicle values
	uint16_t rpm;
	uint16_t prevRPM;

	float oilTemp;
	float prevOilTemp;

	float waterTemp;
	float prevWaterTemp;

	float ecuTemp;
	float prevEcuTemp;

	uint8_t gear;
	uint8_t prevGear;

	uint16_t speed;
	uint16_t prevSpeed;

	float voltage;
	float prevVoltage;

	bool fanOn;
	bool prevFanOn;

	uint16_t fuelPressure;
	uint16_t prevFuelPressure;

	Vehicle();
	virtual ~Vehicle();
};

