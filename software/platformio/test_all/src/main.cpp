#include "main.h"

void test_sd() {
    Serial.println("Testing SD Card");
    int statussd = SD.begin(CS_SD);
    while (!statussd) {
        Serial.println("Initialising SD Card: status = ");
        Serial.println(statussd);
        delay(1000);
        statussd = SD.begin(CS_SD);
    }
    // Open the file for writing
    File file = SD.open("/test.txt", FILE_WRITE);

    // Check if the file opened successfully
    if (file) {
        Serial.println("Writing to file...");
        // Write text to the file
        file.println("Acest ESP32 Mergee!");
        // Close the file
        file.close();
        Serial.println("Done.");
    } else {
        // If the file didn't open, print an error
        Serial.println("Error opening file for writing.");
    }
    Serial.println("");
}

void test_neopixel() {
    Serial.println("Testing NeoPixel");
    light_green_on();
    delay(1000);
    light_red_on();
    delay(1000);
    light_blue_on();
    delay(1000);
    light_off();
    delay(1000);
    Serial.println("");
}

void test_ltr308() {
    Serial.println("Testing LTR308");
    while (!ltr308.begin()) {
        Serial.println("Could not find a valid LTR308 sensor, check wiring!");
        delay(500);
    }
    setup_ltr308();
    for (uint8_t i = 0; i < 5; i++) {
        unsigned long rawData;

        if (ltr308.getData(rawData)) {
            Serial.print("Raw Data: ");
            Serial.println(rawData);
            double lux;
            boolean good;
            good = ltr308.getLux(gain, integrationTime, rawData, lux);
            Serial.print("Lux: ");
            Serial.print(lux);
            if (good)
                Serial.println(" (valid data)");
            else
                Serial.println(" (BAD)");
        } else {
            byte error = ltr308.getError();
            Serial.print("Error: ");
            Serial.println(error);
        }
        delay(1000);
    }
    Serial.println("");
}

void test_ltr303() {
    Serial.println("Testing LTR303");
    while (!ltr303.begin()) {
        Serial.println("Could not find a valid LTR303 sensor, check wiring!");
        delay(500);
    }
    ltr303.setGain(LTR3XX_GAIN_1);
    ltr303.setIntegrationTime(LTR3XX_INTEGTIME_50);
    ltr303.setMeasurementRate(LTR3XX_MEASRATE_50);

    for (uint8_t i = 0; i < 15; i++) {
        bool valid;
        uint16_t visible_plus_ir, infrared;

        if (ltr303.newDataAvailable()) {
            valid = ltr303.readBothChannels(visible_plus_ir, infrared);
            if (valid) {
                Serial.print("CH0 Visible + IR: ");
                Serial.print(visible_plus_ir);
                Serial.print("\t\tCH1 Infrared: ");
                Serial.println(infrared);
            }
        }

        delay(1000);
    }

    Serial.println("");
}

void test_ssd1306() {
    Serial.println("Testing SSD1306");

    while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        delay(2000);
    }
    display.display();
    delay(2000);
    display.clearDisplay();

    // Write "It works"
    display.setTextSize(2.4);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("It works!");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.display();
    Serial.println("");
}

void test_max17048(uint8_t show_display) {
    Serial.println("Testing MAX17048");
    while (!lipo.begin()) {
        Serial.println("MAX17048 not detected. Please check wiring!");
        delay(500);
    }
    lipo.quickStart();
    lipo.setThreshold(20);
    for (uint8_t i = 0; i < 10; i++) {
        voltage = lipo.getVoltage();
        soc = lipo.getSOC();
        alert = lipo.getAlert();

        // Print the variables:
        Serial.print("Voltage: ");
        Serial.print(voltage);
        Serial.println(" V");

        Serial.print("Percentage: ");
        Serial.print(soc);
        Serial.println(" %");

        Serial.print("Alert: ");
        Serial.println(alert);
        Serial.println();

        if (show_display) {
            while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
                Serial.println(F("SSD1306 allocation failed"));
                delay(2000);
            }
            display.clearDisplay();
            display.setTextSize(3);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.print(voltage);
            display.println("V");
            display.print(soc);
            display.println("%");
            display.display();
            if (show_display == 2) {
                i = 0;
            }
        }
        delay(1000);
    }
    // Show values on display
    Serial.println("");
}

void test_bmi085() {
    Serial.println("Testing BMI085");
    payload_entry = (char *)malloc(size_of_entry * sizeof(char));
    while (accel.begin() != 1 || gyro.begin() != 1) {
        Serial.println("Initialising...");
        delay(500);
    }

    for (uint8_t i = 0; i < 10; i++) {
        accel.readSensor();
        gyro.readSensor();

        float accelX = accel.getAccelX_mss();
        float accelY = accel.getAccelY_mss();
        float accelZ = accel.getAccelZ_mss();

        float gyroX = gyro.getGyroX_rads();
        float gyroY = gyro.getGyroY_rads();
        float gyroZ = gyro.getGyroZ_rads();
        float temp = accel.getTemperature_C();

        int current_time = 0;
        memset(payload_entry, 0, size_of_entry);
        sprintf(payload_entry, "%d;%d;%f;%f;%f;%f;%f;%f;%d\n", current_time,
                index, accelX, accelY, accelZ, gyroX, gyroY, gyroZ, (int)temp);
        Serial.print("Payload: ");
        Serial.println(payload_entry);
        delay(1000);
    }
    Serial.println("");
}

void test_lsm6dsl() {
    Serial.println("Testing LSM6DSL");
    LSM6DSLSensor AccGyr(&Wire, LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW);
    delay(500);
    AccGyr.begin();
    AccGyr.Enable_X();
    AccGyr.Enable_G();

    for (uint8_t i = 0; i < 10; i++) {
        // Read accelerometer and gyroscope.
        int32_t accelerometer[3];
        int32_t gyroscope[3];
        AccGyr.Get_X_Axes(accelerometer);
        AccGyr.Get_G_Axes(gyroscope);

        // Output data.
        Serial.print("Acc[mg]: ");
        Serial.print(accelerometer[0]);
        Serial.print(" ");
        Serial.print(accelerometer[1]);
        Serial.print(" ");
        Serial.print(accelerometer[2]);
        Serial.print(" | Gyr[mdps]: ");
        Serial.print(gyroscope[0]);
        Serial.print(" ");
        Serial.print(gyroscope[1]);
        Serial.print(" ");
        Serial.println(gyroscope[2]);
        delay(1000);
    }

    Serial.println("");
}

void test_bme680() {
    Serial.println("Testing BME680");
    while (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME680 sensor, check wiring!");
        delay(500);
    }

    for (uint8_t i = 0; i < 10; i++) {
        in_temperature = bme.readTemperature();
        in_pressure = bme.readPressure() / 100.0F;
        in_humidity = bme.readHumidity();
        Serial.println("Temperature: " + String(in_temperature) + " *C");
        Serial.println("Pressure: " + String(in_pressure) + " hPa");
        Serial.println("Humidity: " + String(in_humidity) + " %");
        delay(1000);
    }

    Serial.println("");
}

void test_ics43434() {
    Serial.println("Testing ICS43434");
    setup_ics43434();
    for (uint8_t i = 0; i < 50; i++) {
        size_t bytesIn = 0;
        esp_err_t result = i2s_read(I2S_PORT, sBuffer, sizeof(sBuffer),
                                    &bytesIn, portMAX_DELAY);

        if (result == ESP_OK) {
            Serial.printf("Bytes read: %d\n", bytesIn);

            if (bytesIn > 0) {
                for (int i = 0; i < bytesIn / 2; i++) {
                    Serial.print(sBuffer[i]);
                    Serial.print(" ");
                }
                Serial.println();
            } else {
                Serial.println("No data received.");
            }
        } else {
            Serial.printf("i2s_read failed: %d\n", result);
        }
        delay(100);
    }
    i2s_zero_dma_buffer(I2S_PORT);
}

void test_w25q512jv() {
    Serial.println("Testing W25Q512JV");

    while (!myFlash.begin(PIN_FLASH_CS)) {
        Serial.println("W25Q512JV not detected. Please check wiring!");
        delay(500);
    }
    setup_w25q512jv();

    for (uint8_t i = 0; i < 5; i++) {
        Serial.println();
        Serial.println("r)ead HEX values, 1k bytes");
        Serial.println("v)iew ASCII values, 1k bytes");
        Serial.println("d)ump raw values, 1k bytes");
        Serial.println("w)rite test values, 1k bytes");
        Serial.println("e)rase entire chip");
        Serial.println();

        while (Serial.available()) Serial.read();
        while (Serial.available() == 0);

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
                myFlash.writeBlock(x, myVal, 4);
            }
        } else {
            Serial.print("Unknown choice: ");
            Serial.write(choice);
            Serial.println();
        }
        delay(1000);
    }

    Serial.println("");
}

void setup() {
    /*
        Board versions with components:
        1. ESP32 Sparrow Rev 3.0 -
        SD, Neopixel, LTR308, SSD1306, BMI085, BME680, ICS43434, W25Q256JV
        2. ESP32 Sparrow Rev 3.1 -
        SD, Neopixel, LTR308, SSD1306, LSM6DSL, BME688, ICS43434, W25Q512JV,
       MAX17048
        2. ESP32 Sparrow Rev 3.2 -
        SD, Neopixel, LTR303, SSD1306, LSM6DSL, BME688, ICS43434, W25Q512JV,
       MAX17048
    */
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.begin(115200);
    delay(6000);
    // Test SD Card
    test_sd();
    // Test NeoPixel
    test_neopixel();
    // Test LTR308 or LTR303
    // test_ltr308();
    // test_ltr303();
    // Test SSD1306
    test_ssd1306();
    // Test BMI085
    // test_bmi085();
    // Test LSM6DSL
    test_lsm6dsl();
    // Test BME680
    test_bme680();
    // Test ICS43434
    test_ics43434();
    // Test W25Q512JV
    test_w25q512jv();
    // Test MAX17048
    test_max17048(1);
}

void loop() {}