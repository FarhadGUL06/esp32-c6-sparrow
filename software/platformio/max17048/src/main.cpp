#include <Arduino.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <Wire.h>

SFE_MAX1704X lipo;

double voltage = 0;  // Variable to keep track of LiPo voltage
double soc = 0;      // Variable to keep track of LiPo state-of-charge (SOC)
bool alert;  // Variable to keep track of whether alert has been triggered

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Wait for user to open terminal
    Serial.println(F("MAX17043 Example"));
    Wire.begin();
    lipo.enableDebugging();
    if (lipo.begin() == false) {
        Serial.println(
            F("MAX17043 not detected. Please check wiring. Freezing."));
        while (1);
    }

    lipo.quickStart();
    lipo.setThreshold(20);
}

void loop() {
    // lipo.getVoltage() returns a voltage value (e.g. 3.93)
    voltage = lipo.getVoltage();
    // lipo.getSOC() returns the estimated state of charge (e.g. 79%)
    soc = lipo.getSOC();
    // lipo.getAlert() returns a 0 or 1 (0=alert not triggered)
    alert = lipo.getAlert();

    // Print the variables:
    Serial.print("Voltage: ");
    Serial.print(voltage);  // Print the battery voltage
    Serial.println(" V");

    Serial.print("Percentage: ");
    Serial.print(soc);  // Print the battery state of charge
    Serial.println(" %");

    Serial.print("Alert: ");
    Serial.println(alert);
    Serial.println();

    delay(500);
}