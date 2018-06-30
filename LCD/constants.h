#pragma once

/*
	Pin assignment
*/
// LCD
const uint8_t RA8875_INT = 8;
const uint8_t RA8875_CS = 10;
const uint8_t RA8875_RESET = 9;
// Shift register
const uint8_t SR_CLOCK_OUT = 17;
const uint8_t SR_DATA_OUT = 18;
const uint8_t SR_LATCH = 19;
const uint8_t SR_OUTPUT_ENABLE = 16;

// LCD positioning
const uint16_t lcdWidth = 800;
const uint16_t lcdHeight = 480;
const uint16_t logoPos[] = { ((lcdWidth - logoWidth) / 2), ((lcdHeight - logoHeight) / 2) };
const bool xPos = 0;
const bool yPos = 1;
const uint16_t gearSize = 20;
const uint16_t gearPos[] = { ((lcdWidth - (5 * gearSize)) / 2), ((lcdHeight - (8 * gearSize)) / 2) };
const uint16_t oilLabelPos[] = { 10, 15 };
const uint16_t oilTempPos[] = { oilTempWidth + 30, 5 };
const uint8_t oilTempDispLen = 4;
const uint16_t waterLabelPos[] = { 10, 105 };
const uint16_t waterTempPos[] = { waterTempWidth + 30, 100 };
const uint8_t waterTempDispLen = 4;
const uint16_t ecuLabelPos[] = { 10, 200 };
const uint16_t ecuTempPos[] = { ecuTempWidth + 30, 190 };
const uint8_t ecuTempDispLen = 4;
const uint16_t batteryIconPos[] = { 10, 295, };
const uint16_t voltagePos[] = { batteryWidth + 30, 285 };
const uint8_t voltageDispLen = 5;
const uint16_t speedLabelPos[] = { 580, 240 };
const uint16_t speedPos[] = { 590, 190 };
const uint8_t speedDispLen = 3;
const uint16_t rpmLabelPos[] = { 480, 380 };
const uint16_t rpmPos[] = { 320, 380 };
const uint8_t rpmDispLen = 5;
const uint16_t fanIconPos[] = { (lcdWidth / 2) - (fanWidth / 2), 10 };
const uint16_t fuelPressurePos[] = { fuelPressureWidth + 30, 380 };
const uint8_t fuelPressureDispLen = 4;
const uint16_t fuelPressureIconPos[] = { 10, 380, };
const uint16_t disabledIconPos[] = { (uint16_t)(fanIconPos[xPos] - 15), (uint16_t)(fanIconPos[yPos] - 15) };

// Circular speedometer vector
int cX = 650;
int cY = 250;
uint16_t speedoOffsetRadius = 100;
uint16_t speedoBarRadius = 30;

// Shift register values
const uint8_t WARNING_LIGHT1 = 128;
const uint8_t WARNING_LIGHT2 = 64;
const uint8_t WARNING_LIGHT3 = 32;
const uint8_t WARNING_LIGHT4 = 16;
const uint8_t WARNING_LIGHT5 = 8;
const uint8_t WARNING_LIGHT6 = 4;
const uint8_t WARNING_LIGHT7 = 2;
const uint8_t WARNING_LIGHT8 = 1;
const uint8_t SR_LEDBITS = 40;
const uint8_t SR_WARNINGBITS = 8;
const uint16_t RPM_SCALE = 270;
const uint8_t TOTAL_LEDS = 48;

const uint16_t MAX_RPM = 14000;
const uint16_t IDLE_RPM = 4000;

// Celcius symbol
const static char celcius[3] = { 0xb0, 0x43 };