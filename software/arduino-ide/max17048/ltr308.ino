#include "Arduino.h"
#include <LTR308.h>

#include <Wire.h>

LTR308 light;

const uint8_t PIN_SCL = 22;
const uint8_t PIN_SDA = 21;

unsigned char gain = 0;
unsigned char integrationTime = 0;
unsigned char measurementRate = 3;
unsigned char ID;
unsigned char control;

void initialise_ltr()
{
	while (!light.begin()) {
		Serial.println("Could not find a valid LTR308 sensor, check wiring!");
		delay(500);
	}
}

void read_from_ltr()
{
	unsigned long rawData;

	if (light.getData(rawData)) {

		Serial.print("Raw Data: ");
		Serial.println(rawData);

		// To calculate lux, pass all your settings and readings
		// to the getLux() function.

		// The getLux() function will return 1 if the calculation
		// was successful, or 0 if the sensor was
		// saturated (too much light). If this happens, you can
		// reduce the integration time and/or gain.

		double lux;	  // Resulting lux value
		boolean good; // True if sensor is not saturated

		// Perform lux calculation:

		good = light.getLux(gain, integrationTime, rawData, lux);

		// Print out the results:

		Serial.print("Lux: ");
		Serial.print(lux);
		if (good)
			Serial.println(" (valid data)");
		else
			Serial.println(" (BAD)");
	} else {
		byte error = light.getError();
    Serial.println("Error LTR308");
	}
}

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);
	initialise_ltr();

	if (light.getPartID(ID)) {
		Serial.print("Got Sensor Part ID: 0X");
		Serial.print(ID, HEX);
		Serial.println();
	}
	// Most library commands will return true if communications was successful,
	// and false if there was a problem. You can ignore this returned value,
	// or check whether a command worked correctly and retrieve an error code:
	else {
		byte error = light.getError();
	}

	// To start taking measurements, power up the sensor

	if (light.setPowerUp()) {
		Serial.print("Powering up...");
		Serial.println();
	} else {
		byte error = light.getError();
	}

	if (light.getPower(control)) {
		Serial.print("Control byte is: 0X");
		Serial.print(control, HEX);
		Serial.println();
	} else {
		byte error = light.getError();
	}
	// The light sensor has a default integration time of 100ms,
	// and a default gain of low (3X).

	// If you would like to change either of these, you can
	// do so using the setGain() and setMeasurementRate() command.

	Serial.println("Setting Gain...");

	if (light.setGain(gain)) {
		light.getGain(gain);

		Serial.print("Gain Set to 0X");
		Serial.print(gain, HEX);
		Serial.println();
	} else {
		byte error = light.getError();
	}

	Serial.println("Set timing...");
	if (light.setMeasurementRate(integrationTime, measurementRate)) {
		light.getMeasurementRate(integrationTime, measurementRate);

		Serial.print("Timing Set to ");
		Serial.print(integrationTime, HEX);
		Serial.println();

		Serial.print("Meas Rate Set to ");
		Serial.print(measurementRate, HEX);
		Serial.println();
	} else {
		byte error = light.getError();
	}
}

void loop()
{
	// put your main code here, to run repeatedly:
  Serial.println("Testing LTR308");
	read_from_ltr();
	delay(1000);
}
