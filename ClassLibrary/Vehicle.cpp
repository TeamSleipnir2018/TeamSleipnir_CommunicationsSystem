#include "Vehicle.h"

Vehicle::Vehicle() {
	// Initialize car values
	rpm = 0;
	prevRPM = 1;
	oilTemp = 0.0;
	prevOilTemp = 1.0;
	waterTemp = 0.0;
	prevWaterTemp = 1.0;
	ecuTemp = 0.0;
	prevEcuTemp = 1.0;
	airTemp = 0.0;
	prevAirTemp = 1.0;
	gear = 0;
	prevGear = 1;
	speed = 0;
	prevSpeed = 1;
	fanOn = false;
	prevFanOn = true;
	fuelPumpOn = false;
	prevFuelPumpOn = true;
	voltage = 0.0;
	prevVoltage = 1.0;
	fuelPressure = 0;
	prevFuelPressure = 1;
	map = 0;
	prevMap = 1;
}


Vehicle::~Vehicle() {}
