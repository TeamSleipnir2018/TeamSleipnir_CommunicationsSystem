#pragma once
#include <stdint.h>

/*
	Pin assignment
*/
#define RFM95_CS 10
#define RFM_RST 25
#define RFM95_INT 24
#define ERRORLED 3

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
const int NUMBER_OF_MESSAGES = 4;