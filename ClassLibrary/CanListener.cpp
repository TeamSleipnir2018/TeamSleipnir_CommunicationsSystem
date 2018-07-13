#include "CanListener.h"

inline float CanListener::CANIntToFloat(uint16_t floatValue) {
	return floatValue / 1000.0;
}

inline float CanListener::CANKelvinToFloat(uint16_t kelvinValue) {
	float result = kelvinValue / 10.0;
	result = result - 273.15;

	return result;
}

bool CanListener::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller) {

	switch (frame.id) {
	case 1:
		vehicle.rpm = frame.buf[0] | (frame.buf[1] << 8);
		vehicle.voltage = CANIntToFloat(frame.buf[2] | (frame.buf[3] << 8));
		vehicle.waterTemp = CANKelvinToFloat(frame.buf[4] | (frame.buf[5] << 8));
		vehicle.speed = frame.buf[6] | (frame.buf[7] << 8);
		break;
	case 2:
		vehicle.oilTemp = CANKelvinToFloat(frame.buf[0] | (frame.buf[1] << 8));
		vehicle.gear = frame.buf[2] | (frame.buf[3] << 8);
		vehicle.airTemp = CANKelvinToFloat(frame.buf[4] | (frame.buf[5] << 8));
		vehicle.map = frame.buf[6] | (frame.buf[7] << 8);
		break;
	case 3:
		vehicle.ecuTemp = CANKelvinToFloat(frame.buf[0] | (frame.buf[1] << 8));
		vehicle.fuelPressure = (frame.buf[2] | (frame.buf[3] << 8)) / 1000.0;
		vehicle.fanOn = frame.buf[5];
		vehicle.fuelPumpOn = frame.buf[7];
		break;
	}

	return true;
}