#include <SD.h>
#include <SPI.h>

#define SD_CS 10

void initialise_sd()
{
	int statussd = SD.begin(SD_CS);
	while (!statussd) {
		Serial.println("Initialising SD Card: status = ");
		Serial.println(statussd);
		delay(1000);
		statussd = SD.begin(SD_CS);
	}
}

void write_to_file()
{
	// Open the file for writing
	File file = SD.open("/testing.txt", FILE_WRITE);

	// Check if the file opened successfully
	if (file) {
		Serial.println("Writing to file...");
		// Write text to the file
		file.println("This Micro SD Module works!");
		// Close the file
		file.close();
		Serial.println("Done.");
	} else {
		// If the file didn't open, print an error
		Serial.println("Error opening file for writing.");
	}
}

void setup()
{
	// Initialize serial communication
	Serial.begin(115200);
	// Initialize the SD card
	initialise_sd();
	// Write to the file
	write_to_file();
}

void loop()
{
	// No repeating code needed
}
