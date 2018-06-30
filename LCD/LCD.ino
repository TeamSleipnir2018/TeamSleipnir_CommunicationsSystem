/******************************************************************
Team Sleipnir steering wheel LCD

Hardware:
	- Teensy 3.2
	- Adafruit RA8875 touch LCD controller
	- 800 x 480 LCD
	- NPIC6C4894 shift registers
	- MCP2551 CAN transceiver

Written by Einar Arnason
******************************************************************/

#include <SPI.h>
#include <stdint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>
#include "./images/logo.h"
#include "./images/fanIcon.h"
#include "./images/ecuTempIcon.h"
#include "./images/oilTempIcon.h"
#include "./images/waterTempIcon.h"
#include "./images/batteryIcon.h"
#include "./images/fuelPressureIcon.h"
#include "./images/disabledIcon.h"
#include "constants.h"
#include "CanListener.h"

// LCD driver
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

// CAN BUS driver
CanListener canListener;

// Shift register variables
uint8_t warningSetBits;
uint8_t ledBarSetBits;
uint8_t srWarningCounter;
uint8_t srLedCounter;

void printIcons() {
	tft.graphicsMode();
	// Draw the icon for cooling fan
	tft.drawXBitmap(
		fanIconPos[xPos], 
		fanIconPos[yPos], 
		fanIcon, 
		fanWidth, 
		fanHeight, 
		RA8875_WHITE
	);
	// Draw battery icon
	tft.drawXBitmap(
		batteryIconPos[xPos], 
		batteryIconPos[yPos], 
		batteryIcon, 
		batteryWidth, 
		batteryHeight, 
		RA8875_WHITE
	);
	// Draw engine oil temperature icon
	tft.drawXBitmap(
		oilLabelPos[xPos], 
		oilLabelPos[yPos], 
		oilTempIcon, 
		oilTempWidth, 
		oilTempHeight, 
		RA8875_WHITE
	);
	// Draw coolant temperature icon
	tft.drawXBitmap(
		waterLabelPos[xPos],
		waterLabelPos[yPos],
		waterTempIcon,
		waterTempWidth,
		waterTempHeight,
		RA8875_WHITE
	);
	// Draw ECU temperature icon
	tft.drawXBitmap(
		ecuLabelPos[xPos],
		ecuLabelPos[yPos],
		ecuTempIcon,
		ecuTempWidth,
		ecuTempHeight,
		RA8875_WHITE
	);
	// Draw fuel pressure icon
	tft.drawXBitmap(
		fuelPressureIconPos[xPos],
		fuelPressureIconPos[yPos],
		fuelPressureIcon,
		fuelPressureWidth,
		fuelPressureHeight,
		RA8875_WHITE
	);
	tft.textMode();
}

void printLabels() {
	tft.textEnlarge(3);
	tft.textSetCursor(speedLabelPos[xPos], speedLabelPos[yPos]);
	tft.textWrite("km/h");
	tft.textSetCursor(rpmLabelPos[xPos], rpmLabelPos[yPos]);
	tft.textColor(RA8875_WHITE, RA8875_BLACK);
	tft.textWrite(" RPM");
}

void printFrames() {
	tft.drawFastVLine(280, 0, 480, RA8875_WHITE);
	tft.drawFastVLine(500, 0, 480 - (480 - 350), RA8875_WHITE);
	tft.drawFastHLine(280, 350, (800 - 280), RA8875_WHITE);
	tft.drawFastHLine(0, oilTempPos[yPos] + oilTempHeight + 30, 280, RA8875_WHITE);
	tft.drawFastHLine(0, waterTempPos[yPos] + waterTempHeight + 30, 280, RA8875_WHITE);
	tft.drawFastHLine(0, ecuTempPos[yPos] + ecuTempHeight + 30, 280, RA8875_WHITE);
	tft.drawFastHLine(0, voltagePos[yPos] + batteryHeight + 30, 280, RA8875_WHITE);
}

void printInt(const uint16_t& x,
	const uint16_t& y,
	const uint16_t& value,
	uint16_t& prevValue,
	char charValue[],
	const uint8_t len,
	const uint8_t& fontSize,
	bool warning) {

	sprintf(charValue, "%*d", len, value);
	prevValue = value;
	printValue(x, y, charValue, len, fontSize, warning);
}

void printFloat(
	const uint16_t& x,
	const uint16_t& y,
	const float& value,
	float& prevValue,
	char charValue[],
	const uint8_t& len,
	const uint8_t& fontSize,
	bool warning) {

	sprintf(charValue, "%*.01f", len, value);
	prevValue = value;
	printValue(x, y, charValue, len, fontSize, warning);
}

void printFloatNoPoint(
	const uint16_t& x,
	const uint16_t& y,
	const float& value,
	float& prevValue,
	char charValue[],
	const uint8_t& len,
	const uint8_t& fontSize,
	bool warning) {

	sprintf(charValue, "%*d", len, (int)value);
	prevValue = value;
	printValue(x, y, charValue, len, fontSize, warning);
}

void printValue(
	const uint16_t& x,
	const uint16_t& y,
	char charValue[],
	const uint8_t& len,
	const uint8_t& fontSize,
	bool warning
	) {
	tft.textMode();
	tft.textSetCursor(x, y);
	tft.textEnlarge(fontSize);
	if (warning) {
		tft.textColor(RA8875_BLACK, RA8875_RED);
	}
	else {
		tft.textColor(RA8875_WHITE, RA8875_BLACK);
	}
	tft.textWrite(charValue, len);
	tft.textColor(RA8875_WHITE, RA8875_BLACK);
}

void printValues() {
	if (canListener.vehicle.voltage != canListener.vehicle.prevVoltage) {
		char charValue[voltageDispLen];
		printFloat(
			voltagePos[xPos],
			voltagePos[yPos],
			canListener.vehicle.voltage,
			canListener.vehicle.prevVoltage,
			charValue,
			voltageDispLen,
			3,
			false
		);
		tft.textWrite("v");
	}
	if (canListener.vehicle.prevOilTemp != canListener.vehicle.oilTemp) {
		char charValue[oilTempDispLen];
		if (canListener.vehicle.oilTemp > 130) {
			printFloatNoPoint(
				oilTempPos[xPos], 
				oilTempPos[yPos], 
				canListener.vehicle.oilTemp,
				canListener.vehicle.prevOilTemp,
				charValue,
				oilTempDispLen,
				3, 
				true
			);
		}
		else {
			printFloatNoPoint(
				oilTempPos[xPos], 
				oilTempPos[yPos], 
				canListener.vehicle.oilTemp,
				canListener.vehicle.prevOilTemp,
				charValue,
				oilTempDispLen,
				3, 
				false
			);
		}
		tft.textEnlarge(2);
		tft.textWrite(celcius);
	}
	if (canListener.vehicle.prevWaterTemp != canListener.vehicle.waterTemp) {
		char charValue[waterTempDispLen];
		if (canListener.vehicle.waterTemp > 98) {
			printFloatNoPoint(
				waterTempPos[xPos], 
				waterTempPos[yPos], 
				canListener.vehicle.waterTemp,
				canListener.vehicle.prevWaterTemp,
				charValue,
				waterTempDispLen,
				3, 
				true
			);
		}
		else {
			printFloatNoPoint(
				waterTempPos[xPos], 
				waterTempPos[yPos], 
				canListener.vehicle.waterTemp,
				canListener.vehicle.prevWaterTemp,
				charValue,
				waterTempDispLen,
				3, 
				false
			);
		}
		tft.textEnlarge(2);
		tft.textWrite(celcius);
	}
	if (canListener.vehicle.prevEcuTemp != canListener.vehicle.ecuTemp) {
		char charValue[ecuTempDispLen];
		if (canListener.vehicle.ecuTemp > 80) {
			char charValue[ecuTempDispLen];
			printFloatNoPoint(
				ecuTempPos[xPos], 
				ecuTempPos[yPos], 
				canListener.vehicle.ecuTemp,
				canListener.vehicle.prevEcuTemp,
				charValue,
				ecuTempDispLen,
				3, 
				true
			);
		}
		else {
			printFloatNoPoint(
				ecuTempPos[xPos], 
				ecuTempPos[yPos], 
				canListener.vehicle.ecuTemp,
				canListener.vehicle.prevEcuTemp,
				charValue,
				ecuTempDispLen,
				3, 
				false
			);
		}
		tft.textEnlarge(2);
		tft.textWrite(celcius);
	}
	if (canListener.vehicle.prevFuelPressure != canListener.vehicle.fuelPressure) {
		char charValue[fuelPressureDispLen];
		printInt(
			fuelPressurePos[xPos],
			fuelPressurePos[yPos],
			canListener.vehicle.fuelPressure,
			canListener.vehicle.prevFuelPressure,
			charValue,
			fuelPressureDispLen,
			3,
			false
		);
		tft.textWrite("mB");
	}
	if (canListener.vehicle.prevRPM != canListener.vehicle.rpm) {
		char charValue[rpmDispLen];
		if (canListener.vehicle.rpm > MAX_RPM - 500) {
			printInt(
				rpmPos[xPos], 
				rpmPos[yPos], 
				canListener.vehicle.rpm,
				canListener.vehicle.prevRPM,
				charValue,
				rpmDispLen,
				3, 
				true
			);
		}
		else {
			printInt(
				rpmPos[xPos], 
				rpmPos[yPos], 
				canListener.vehicle.rpm,
				canListener.vehicle.prevRPM,
				charValue,
				rpmDispLen,
				3, 
				false
			);
		}
	}
	if (canListener.vehicle.prevSpeed != canListener.vehicle.speed) {
		char charValue[speedDispLen];
		if (canListener.vehicle.speed > canListener.vehicle.prevSpeed) {
			for (int i = canListener.vehicle.prevSpeed; i < canListener.vehicle.speed; i++) {

				uint8_t red;
				uint8_t green;

				if (i <= 100) {
					green = 255;
					red = i * 2.55;
				}
				else {
					red = 255;
					green = 255 - ((i - 100) * 2.55);
				}

				Serial.println((uint16_t)((red << 11) | (green << 5)), HEX);
				
				drawSpeedLine(i, (red << 11) | (green << 5));
			}
		}
		else {
			for (int i = canListener.vehicle.prevSpeed; i >= canListener.vehicle.speed; i--) {
				drawSpeedLine(i, RA8875_BLACK);
			}
		}

		printInt(
			speedPos[xPos], 
			speedPos[yPos], 
			canListener.vehicle.speed,
			canListener.vehicle.prevSpeed,
			charValue,
			speedDispLen,
			3, 
			false
		);
		canListener.vehicle.prevSpeed = canListener.vehicle.speed;
	}
	if (canListener.vehicle.prevGear != canListener.vehicle.gear) {
		if (canListener.vehicle.gear == 0) {
			tft.drawChar(gearPos[xPos], gearPos[yPos], 'N', 0xffff, 0x0000, gearSize);
			canListener.vehicle.prevGear = canListener.vehicle.gear;
		}
		else {
			char gearDisp = 48 + canListener.vehicle.gear;
			canListener.vehicle.prevGear = canListener.vehicle.gear;
			tft.drawChar(gearPos[xPos], gearPos[yPos], gearDisp, 0xffff, 0x0000, gearSize);
		}
	}
	if (canListener.vehicle.prevFanOn != canListener.vehicle.fanOn) {
		tft.graphicsMode();
		if (canListener.vehicle.fanOn) {
			tft.fillRect(disabledIconPos[xPos], disabledIconPos[yPos], 100, 100, RA8875_BLACK);
			tft.drawXBitmap(
				fanIconPos[xPos],
				fanIconPos[yPos],
				fanIcon,
				fanWidth,
				fanHeight,
				RA8875_WHITE
			);
		}
		else {
			tft.drawXBitmap(
				disabledIconPos[xPos],
				disabledIconPos[yPos],
				disabledIcon,
				disabledWidth,
				disabledHeight,
				RA8875_RED
			);
		}
		canListener.vehicle.prevFanOn = canListener.vehicle.fanOn;
	}
	tft.textMode();
}

// Draws the line in a circular speedometer
void drawSpeedLine(const uint8_t& value, const uint16_t& color) {
	int speedToDeg = 280 - value;
	int u0 = (speedoOffsetRadius * sin(speedToDeg * (PI / 180))) + cX;
	int v0 = (speedoOffsetRadius * cos(speedToDeg * (PI / 180))) + cY;
	int u1 = ((speedoBarRadius + speedoOffsetRadius) * sin(speedToDeg * (PI / 180))) + cX;
	int v1 = ((speedoBarRadius + speedoOffsetRadius) * cos(speedToDeg * (PI / 180))) + cY;
	tft.drawLine(u0, v0, u1, v1, color);
}

void runShiftRegister() {
	// Close the latch to write into register memory
	digitalWrite(SR_LATCH, LOW);
	// Scale RPM to number of LEDs
	if (canListener.vehicle.rpm < IDLE_RPM) {
		ledBarSetBits = canListener.vehicle.rpm / 1000;
	}
	else {
		ledBarSetBits = ((canListener.vehicle.rpm - IDLE_RPM) / RPM_SCALE) + 3;
	}
	
	// Set warning lights
	uint8_t warning = warningSetBits | WARNING_LIGHT2 | WARNING_LIGHT4 | WARNING_LIGHT6 | WARNING_LIGHT8;
	
	// Shift bits to register
	for (int i = 0; i < SR_WARNINGBITS; i++) {
		
		digitalWrite(SR_DATA_OUT, (warning & 0x1));
		digitalWrite(SR_CLOCK_OUT, LOW);
		digitalWrite(SR_CLOCK_OUT, HIGH);
		warning = warning >> 1;
	}

	for (int i = 0; i < SR_LEDBITS; i++) {	
		if (i < ledBarSetBits) {
			digitalWrite(SR_DATA_OUT, HIGH);
		}
		else {
			digitalWrite(SR_DATA_OUT, LOW);
		}
		// Manually set clock transition
		digitalWrite(SR_CLOCK_OUT, LOW);
		digitalWrite(SR_CLOCK_OUT, HIGH);
	}
	// Open latch and enable register outputs
	digitalWrite(SR_LATCH, HIGH);
}

uint16_t speedCount;
bool reverse;

void demo() {
	if (reverse) {
		if (canListener.vehicle.rpm != 0) {
			canListener.vehicle.rpm -= 25;
		}
		else {
			canListener.vehicle.oilTemp = random(4730, 4830);
			canListener.vehicle.waterTemp = random(4030, 4230);
			canListener.vehicle.ecuTemp = random(2730, 3230);
			canListener.vehicle.voltage = random(11900, 12900);

			if (canListener.vehicle.gear > 0) {
				canListener.vehicle.gear--;
				canListener.vehicle.rpm = 9000;
			}
			else {
				canListener.vehicle.rpm = 0;
				canListener.vehicle.speed = 0;
			}
		}
	}
	else {
		if (canListener.vehicle.rpm != MAX_RPM) {
			canListener.vehicle.rpm += 25;
		}
		else {
			canListener.vehicle.oilTemp = random(4730, 4830);
			canListener.vehicle.waterTemp = random(4030, 4230);
			canListener.vehicle.ecuTemp = random(2730, 3230);
			canListener.vehicle.voltage = random(11900, 12900);

			if (canListener.vehicle.gear < 6) {
				canListener.vehicle.gear++;
				canListener.vehicle.rpm = 2000;
			}
			else {
				canListener.vehicle.rpm = MAX_RPM - 500;
			}
		}
	}

	if (reverse) {
		if (canListener.vehicle.speed > 0) {
			if (speedCount == 15) {
				canListener.vehicle.speed--;
				speedCount = 0;
			}
			else {
				speedCount++;
			}
		}
	}
	else {
		if (canListener.vehicle.speed < 200 && canListener.vehicle.gear != 0) {
			if (speedCount == 15) {
				canListener.vehicle.speed++;
				speedCount = 0;
			}
			else {
				speedCount++;
			}
		}
	}
}

void setup() {
	// Initialize serial console
	Serial.begin(9600);
	
	// Initialise the display
	Serial.println("RA8875 start");
	while (!tft.begin(RA8875_800x480)) {
		Serial.println("RA8875 Not Found!");
		delay(1000);
	}
	digitalWrite(RA8875_CS, LOW);
	Serial.println("Found RA8875");

	tft.displayOn(true);
	tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	tft.PWM1out(0); // Start faded out
	
	//Draw logo and Fade in
	tft.graphicsMode();
	tft.fillScreen(RA8875_BLACK);
	tft.drawXBitmap(logoPos[xPos], logoPos[yPos], logo, logoWidth, logoHeight, RA8875_RED);
	for (uint8_t i = 0; i != 255; i += 5) {
		tft.PWM1out(i);
		delay(10);
	}

	// Enable shiftregister output
	pinMode(SR_CLOCK_OUT, OUTPUT);
	pinMode(SR_DATA_OUT, OUTPUT);
	pinMode(SR_LATCH, OUTPUT);
	pinMode(SR_OUTPUT_ENABLE, OUTPUT);
	digitalWrite(SR_OUTPUT_ENABLE, HIGH);
	warningSetBits = 0;

	// Initialize the CAN bus
	Can0.begin(500000);
	Can0.attachObj(&canListener);
	canListener.attachGeneralHandler();

	// Let the logo linger a bit
	delay(1800);
	
	// Clear sceen
	tft.fillScreen(RA8875_BLACK);
	tft.textMode();
	tft.textColor(RA8875_WHITE, RA8875_BLACK);
	printIcons();
	printLabels();
	printFrames();
	printValues();
}

void loop() {

	//demo();
	printValues();
	runShiftRegister();
}
