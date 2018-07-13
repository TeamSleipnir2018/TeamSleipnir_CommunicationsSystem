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

	float airTemp;
	float prevAirTemp;

	uint8_t gear;
	uint8_t prevGear;

	uint16_t speed;
	uint16_t prevSpeed;

	float voltage;
	float prevVoltage;

	bool fanOn;
	bool prevFanOn;

	bool fuelPumpOn;
	bool prevFuelPumpOn;

	float fuelPressure;
	float prevFuelPressure;

	uint16_t map;
	uint16_t prevMap;

	Vehicle();
	virtual ~Vehicle();
};

