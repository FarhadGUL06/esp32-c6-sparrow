#include <SD.h>
#include <SPI.h>

#define SD_CS_PIN        10
#define SD_CLK_PIN       6
#define SD_MOSI_PIN      7
#define SD_MISO_PIN       2



void initialise_sd()
{
	int statussd = SD.begin(SD_CS_PIN);
	while (!statussd) {
		Serial.println("Initialising SD Card: status = ");
		Serial.println(statussd);
		delay(1000);
		statussd = SD.begin(SD_CS_PIN);
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

    SPIClass spiSD = SPIClass(HSPI);
  spiSD.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN); 

	// Initialize the SD card
	initialise_sd();
	// Write to the file
	write_to_file();
}

void loop()
{
	// No repeating code needed
}
