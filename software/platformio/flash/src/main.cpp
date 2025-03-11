#include <Arduino.h>
#include <SPI.h>
#include <SparkFun_SPI_SerialFlash.h>

const byte PIN_FLASH_CS = 23;
SFE_SPI_FLASH myFlash;

// Buffer for reading flash contents
const uint32_t BUFFER_SIZE = 4096;  // 4KB buffer (matches sector size)
uint8_t readBuffer[BUFFER_SIZE];

void setup() {
    Serial.begin(115200);
    delay(3000);
    while (!Serial);  // Wait for user to open terminal
    Serial.println(F("W25Q512JV Example"));
    // Begin the flash using the chosen CS pin. Default to:
    // spiPortSpeed=2000000, spiPort=SPI and spiMode=SPI_MODE0
    if (myFlash.begin(PIN_FLASH_CS) == false) {
        Serial.println(
            F("SPI Flash not detected. Check wiring. Maybe you need to pull up "
              "WP/IO2 and HOLD/IO3? Freezing..."));
        while (1);
    }

    Serial.println(F("SPI Flash detected"));
    sfe_flash_manufacturer_e mfgID = myFlash.getManufacturerID();
    if (mfgID != SFE_FLASH_MFG_UNKNOWN) {
        Serial.print(F("Manufacturer: "));
        Serial.println(myFlash.manufacturerIDString(mfgID));
    } else {
        uint8_t unknownID =
            myFlash.getRawManufacturerID();  // Read the raw manufacturer ID
        Serial.print(F("Unknown manufacturer ID: 0x"));
        if (unknownID < 0x10) Serial.print(F("0"));  // Pad the zero
        Serial.println(unknownID, HEX);
    }

    // Get the JEDEC ID (32-bit value)
    uint32_t jedecID = myFlash.getJEDEC();
    Serial.print("JEDEC ID: 0x");
    Serial.println(jedecID, HEX);

    // Extract manufacturer, memory type, and capacity
    uint8_t manufacturerID = (jedecID >> 16) & 0xFF;  // First byte
    uint8_t memoryType = (jedecID >> 8) & 0xFF;       // Second byte
    uint8_t capacityCode = jedecID & 0xFF;            // Third byte

    // Calculate flash size based on capacity code
    uint32_t flashSizeBytes = 0;
    Serial.println("Capacity code: 0x" + String(capacityCode, HEX));
    // Example for Winbond (W25Q series)
    if (manufacturerID == SFE_FLASH_MFG_WINBOND) {
        switch (capacityCode) {
            case 0x20:  // W25Q512JV (512M-bit = 64MB)
                flashSizeBytes = 64 * 1024 * 1024;
                break;
            case 0x19:  // W25Q256JV (256M-bit = 32MB)
                flashSizeBytes = 32 * 1024 * 1024;
                break;
            case 0x18:  // W25Q128JV (128M-bit = 16MB)
                flashSizeBytes = 16 * 1024 * 1024;
                break;
            case 0x17:  // W25Q64JV (64M-bit = 8MB)
                flashSizeBytes = 8 * 1024 * 1024;
                break;
            case 0x16:  // W25Q32JV (32M-bit = 4MB)
                flashSizeBytes = 4 * 1024 * 1024;
                break;
            default:
                Serial.println("Unknown Winbond capacity code!");
                break;
        }
    }
    Serial.print("Flash size: ");
    Serial.print(flashSizeBytes);
    Serial.println(" bytes");

    // Start integrity check
    Serial.println("\nStarting flash integrity check...");
    Serial.println("This may take several minutes...");

    uint32_t address = 0;
    uint32_t errors = 0;
    uint32_t checkedBytes = 0;
    unsigned long startTime = millis();

    while (address < flashSizeBytes) {
        // Read block of data
        sfe_flash_read_write_result_e result =
            myFlash.readBlock(address, readBuffer, BUFFER_SIZE);

        if (result != SFE_FLASH_READ_WRITE_SUCCESS) {
            Serial.print("Read error at address 0x");
            Serial.println(address, HEX);
            errors += BUFFER_SIZE;  // Count entire block as errors
            address += BUFFER_SIZE;
            continue;
        }

        // Verify buffer contents (should be 0xFF if erased)
        for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
            if (readBuffer[i] != 0xFF) {
                errors++;
            }
        }

        // Progress reporting
        checkedBytes += BUFFER_SIZE;
        if (checkedBytes % (1024 * 1024) == 0) {  // Report every 1MB
            float progress = (address * 100.0) / flashSizeBytes;
            Serial.print("Checked ");
            Serial.print(progress, 1);
            Serial.println("%");
        }

        address += BUFFER_SIZE;
    }

    // Calculate statistics
    unsigned long elapsedTime = (millis() - startTime) / 1000;
    float errorPercent = (errors * 100.0) / flashSizeBytes;

    Serial.println("\nIntegrity check complete!");
    Serial.print("Total time: ");
    Serial.print(elapsedTime);
    Serial.println(" seconds");
    Serial.print("Checked bytes: ");
    Serial.print(flashSizeBytes);
    Serial.println(" bytes");
    Serial.print("Errors found: ");
    Serial.print(errors);
    Serial.print(" (");
    Serial.print(errorPercent, 3);
    Serial.println("%)");

    if (errors == 0) {
        Serial.println("Flash integrity: PERFECT");
    } else if (errorPercent < 0.1) {
        Serial.println("Flash integrity: GOOD");
    } else {
        Serial.println("Flash integrity: POOR");
    }
}

void loop() {
    Serial.println();
    Serial.println("r)ead HEX values, 1k bytes");
    Serial.println("v)iew ASCII values, 1k bytes");
    Serial.println("d)ump raw values, 1k bytes");
    Serial.println("w)rite test values, 1k bytes");
    Serial.println("e)rase entire chip");
    Serial.println();

    while (Serial.available()) Serial.read();  // Clear the RX buffer
    while (Serial.available() == 0);           // Wait for a character

    byte choice = Serial.read();

    if (choice == 'r') {
        Serial.println("Read raw values for 1024 bytes");

        for (int x = 0; x < 0x0400; x++) {
            if (x % 16 == 0) {
                Serial.println();
                Serial.print("0x");
                if (x < 0x100) Serial.print("0");
                if (x < 0x10) Serial.print("0");
                Serial.print(x, HEX);
                Serial.print(": ");
            }

            byte val = myFlash.readByte(x);
            if (val < 0x10) Serial.print("0");
            Serial.print(val, HEX);
            Serial.print(" ");
        }

        Serial.println();

    } else if (choice == 'v') {
        Serial.println("View ASCII for 1024 bytes");

        for (int x = 0; x < 0x0400; x++) {
            if (x % 16 == 0) Serial.println();

            byte val = myFlash.readByte(x);
            if (isAlphaNumeric(val)) Serial.write(val);
        }
    } else if (choice == 'd') {
        for (int x = 0; x < 0x0400; x++) {
            byte val = myFlash.readByte(x);
            Serial.write(val);
        }
    } else if (choice == 'e') {
        Serial.println("Erasing entire chip");
        myFlash.erase();
    } else if (choice == 'w') {
        Serial.println("Writing test HEX values to first 1024 bytes");
        uint8_t myVal[4] = {0xDE, 0xAD, 0xBE, 0xEF};
        for (int x = 0; x < 0x0400; x += 4) {
            myFlash.writeBlock(x, myVal, 4);  // Address, pointer, size
        }
    } else {
        Serial.print("Unknown choice: ");
        Serial.write(choice);
        Serial.println();
    }
}