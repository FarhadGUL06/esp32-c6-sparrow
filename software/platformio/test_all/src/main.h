#include <Adafruit_BME680.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LTR329_LTR303.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <BMI085.h>
#include <LSM6DSLSensor.h>
#include <LTR308.h>
#include <SD.h>
#include <SPI.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <SparkFun_SPI_SerialFlash.h>
#include <Wire.h>

#include "driver/i2s.h"

// I2C
const int I2C_SDA = 21;
const int I2C_SCL = 22;

// SD Card
const int CS_SD = 10;
File myFile;

// NEOPIXEL
const int NEOPIXEL_PIN = 3;
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void light_off() {
    pixel.setBrightness(0);
    pixel.fill(0x000000);
    pixel.show();
}

void light_green_on() {
    pixel.setBrightness(5);
    pixel.fill(0x4CBB17);
    pixel.show();
}

void light_red_on() {
    pixel.setBrightness(5);
    pixel.fill(0xFF0000);
    pixel.show();
}

void light_blue_on() {
    pixel.setBrightness(5);
    pixel.fill(0x0000FF);
    pixel.show();
}

// LTR308
LTR308 ltr308;
unsigned char gain = 0;
unsigned char integrationTime = 0;
unsigned char measurementRate = 3;
unsigned char ID;
unsigned char control;
void setup_ltr308() {
    if (ltr308.getPartID(ID)) {
        Serial.print("Got Sensor Part ID: 0X");
        Serial.print(ID, HEX);
        Serial.println();
    } else {
        byte error = ltr308.getError();
    }

    if (ltr308.setPowerUp()) {
        Serial.print("Powering up...");
        Serial.println();
    } else {
        byte error = ltr308.getError();
    }

    if (ltr308.getPower(control)) {
        Serial.print("Control byte is: 0X");
        Serial.print(control, HEX);
        Serial.println();
    } else {
        byte error = ltr308.getError();
    }

    Serial.println("Setting Gain...");

    if (ltr308.setGain(gain)) {
        ltr308.getGain(gain);

        Serial.print("Gain Set to 0X");
        Serial.print(gain, HEX);
        Serial.println();
    } else {
        byte error = ltr308.getError();
    }

    Serial.println("Set timing...");
    if (ltr308.setMeasurementRate(integrationTime, measurementRate)) {
        ltr308.getMeasurementRate(integrationTime, measurementRate);

        Serial.print("Timing Set to ");
        Serial.print(integrationTime, HEX);
        Serial.println();

        Serial.print("Meas Rate Set to ");
        Serial.print(measurementRate, HEX);
        Serial.println();
    } else {
        byte error = ltr308.getError();
    }
}

// LTR303
Adafruit_LTR329 ltr303 = Adafruit_LTR303();

// NAX17048
SFE_MAX1704X lipo;
double voltage = 0;
double soc = 0;
bool alert;

// SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// BMI085
BMI085Gyro gyro(Wire, 0x68);
BMI085Accel accel(Wire, 0x18);
char *payload_entry;
int size_of_entry = 80;

// BME680
Adafruit_BME680 bme;
float in_temperature;
float in_pressure;
float in_humidity;

// ICS-43434
const i2s_port_t I2S_PORT = I2S_NUM_0;
int16_t sBuffer[64];

void setup_ics43434() {
    esp_err_t err;
    // I2S Configuration
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 64};

    // I2S Pin Configuration
    const i2s_pin_config_t pin_config = {
        .bck_io_num = 18,                   // MIC_SCK (Clock)
        .ws_io_num = 20,                    // MIC_WS (Word Select)
        .data_out_num = I2S_PIN_NO_CHANGE,  // Not used for input
        .data_in_num = 19                   // MIC_SD (Data)
    };

    // Install I2S driver
    err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("Failed installing driver: %d\n", err);
        while (true);
    } else {
        Serial.println("I2S driver installed.");
    }

    // Set I2S pin configuration
    err = i2s_set_pin(I2S_PORT, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("Failed setting pin: %d\n", err);
        while (true);
    } else {
        Serial.println("I2S pin configuration successful.");
    }

    // Start with zeroing the input buffer
    i2s_zero_dma_buffer(I2S_PORT);
}

// W25Q512JV
const byte PIN_FLASH_CS = 23;
SFE_SPI_FLASH myFlash;
const uint32_t BUFFER_SIZE = 4096;
uint8_t readBuffer[BUFFER_SIZE];

void setup_w25q512jv() {
    Serial.println(F("SPI Flash detected"));
    sfe_flash_manufacturer_e mfgID = myFlash.getManufacturerID();
    if (mfgID != SFE_FLASH_MFG_UNKNOWN) {
        Serial.print(F("Manufacturer: "));
        Serial.println(myFlash.manufacturerIDString(mfgID));
    } else {
        uint8_t unknownID = myFlash.getRawManufacturerID();
        Serial.print(F("Unknown manufacturer ID: 0x"));
        if (unknownID < 0x10) Serial.print(F("0"));
        Serial.println(unknownID, HEX);
    }

    // Get the JEDEC ID (32-bit value)
    uint32_t jedecID = myFlash.getJEDEC();
    Serial.print("JEDEC ID: 0x");
    Serial.println(jedecID, HEX);

    // Extract manufacturer, memory type, and capacity
    uint8_t manufacturerID = (jedecID >> 16) & 0xFF;
    uint8_t memoryType = (jedecID >> 8) & 0xFF;
    uint8_t capacityCode = jedecID & 0xFF;

    uint32_t flashSizeBytes = 0;
    Serial.println("Capacity code: 0x" + String(capacityCode, HEX));

    if (manufacturerID == SFE_FLASH_MFG_WINBOND) {
        switch (capacityCode) {
            case 0x20:
                flashSizeBytes = 64 * 1024 * 1024;
                break;
            case 0x19:
                flashSizeBytes = 32 * 1024 * 1024;
                break;
            case 0x18:
                flashSizeBytes = 16 * 1024 * 1024;
                break;
            case 0x17:
                flashSizeBytes = 8 * 1024 * 1024;
                break;
            case 0x16:
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
            errors += BUFFER_SIZE;
            address += BUFFER_SIZE;
            continue;
        }

        for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
            if (readBuffer[i] != 0xFF) {
                errors++;
            }
        }

        // Progress reporting
        checkedBytes += BUFFER_SIZE;
        if (checkedBytes % (1024 * 1024) == 0) {
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
