#include <SPI.h>
#include <SPIMemory.h>

#define FLASH_CS 23

SPIFlash flash(FLASH_CS);

void setup()
{
	// Initialize serial communication
	Serial.begin(115200);
	if (flash.error()) {
		Serial.println(flash.error(VERBOSE));
	}
	flash.begin();
	if (getID()) {

		printLine();
		printTab(7);
		Serial.print("Testing library code");
		printLine();
		printTab(3);
		Serial.print("Function");
		printTab(2);
		Serial.print("Test result");
		printTab(3);
		Serial.print("     Runtime");
		printLine();

		powerDownTest();
		Serial.println();
		powerUpTest();
		Serial.println();

		Serial.println();

		eraseChipTest();
		Serial.println();
		eraseSectionTest();
		Serial.println();
		eraseBlock64KTest();
		Serial.println();
		eraseBlock32KTest();
		Serial.println();
		eraseSectorTest();
		// Serial.println();

		/* #if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_ESP8266)
				delay(10);
				powerDownTest();
				powerUpTest();
				Serial.println();
			  #endif */
		printLine();
		printTab(3);
		Serial.print("Data type");
		printTab(2);
		Serial.print("I/O Result");
		printTab(1);
		Serial.print("      Write time");
		printTab(1);
		Serial.print("      Read time");
		printLine();

		byteTest();
		Serial.println();
		charTest();
		Serial.println();
		wordTest();
		Serial.println();
		shortTest();
		Serial.println();
		uLongTest();
		Serial.println();
		/*#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_ESP8266)
				delay(10);
			  #endif */
		longTest();
		Serial.println();
		floatTest();
		Serial.println();
		structTest();
		Serial.println();
		arrayTest();
		Serial.println();
		stringTest();

		/* #if !defined(ARDUINO_ARCH_SAM) || !defined(ARDUINO_ARCH_ESP8266)
				Serial.println();
				powerDownTest();
				powerUpTest();
			  #endif */
		printLine();
		if (!flash.functionRunTime()) {
			Serial.println(F("To see function runtimes ncomment RUNDIAGNOSTIC "
							 "in SPIMemory.h."));
		}
	}
}
void loop()
{
	// No repeating code needed
}
