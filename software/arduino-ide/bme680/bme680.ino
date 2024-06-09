#include <Adafruit_BME680.h>

Adafruit_BME680 bme;
float in_temperature;
float in_pressure;
float in_humidity;

void start_bme()
{
	int status = bme.begin(0x76);
	while (!status) {
		Serial.println("Could not find a valid BME680 sensor, check wiring!");
		delay(500);
	}
}

void get_bme()
{
	in_temperature = bme.readTemperature();
	in_pressure = bme.readPressure() / 100.0F;
	in_humidity = bme.readHumidity();
	Serial.println("Temperature: " + String(in_temperature) + " *C");
	Serial.println("Pressure: " + String(in_pressure) + " hPa");
	Serial.println("Humidity: " + String(in_humidity) + " %");
}

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(115200);
	start_bme();
}

void loop()
{
	// put your main code here, to run repeatedly:
	get_bme();
	delay(1000);
}
