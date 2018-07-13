#pragma once
#include <stdint.h>

/*
	Pin assignment
*/

const uint8_t CAN0TX_ALT = 1;
const uint8_t CAN0RX_ALT = 1;

// Translate float values from CAN BUS
inline float CANIntToFloat(uint16_t floatValue) {
	return floatValue / 1000.0;
}
// Translate kelvin temperature values from CAN BUS
inline float CANKelvinToFloat(uint16_t kelvinValue) {
	float result = kelvinValue / 10.0;
	result = result - 273.15;

	return result;
}

const int PAYLOAD_SIZE = 119;
const int NUMBER_OF_MESSAGES = 5;