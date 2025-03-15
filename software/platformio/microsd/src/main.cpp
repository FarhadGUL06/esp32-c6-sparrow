#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

const int CS = 10;
File myFile;

void setup()
{
	Serial.begin(9600);
	delay(6000);
	while (!Serial)
		; // Wait for user to open terminal
	Serial.println(F("MicroSD Example"));

	Serial.print("Initializing SD card...");

	if (!SD.begin()) {
		Serial.println("initialization failed!");
		return;
	}
	Serial.println("initialization done.");

	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	myFile = SD.open("test.txt", FILE_WRITE);

	myFile.println("testing 1, 2, 3.");
	// close the file:
	myFile.close();

	// re-open the file for reading:
	myFile = SD.open("test.txt");
	if (myFile) {
		Serial.println("test.txt:");

		// read from the file until there's nothing else in it:
		while (myFile.available()) {
			Serial.write(myFile.read());
		}
		// close the file:
		myFile.close();
	} else {
		// if the file didn't open, print an error:
		Serial.println("error opening test.txt");
	}
}

void loop()
{
}