#include <BMI085.h>
#include <Wire.h>

// FOR BMI085
BMI085Gyro gyro(Wire, 0x68);
BMI085Accel accel(Wire, 0x18);
char *payload_entry;
int size_of_entry = 80;

void initialise_bmi()
{
	while (accel.begin() != 1 || gyro.begin() != 1) {
		Serial.println("Initialising...");
		delay(100);
	}
}

void get_entry_info(long index)
{
	// Get BMI085 data and timestamp
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
	sprintf(payload_entry, "%d;%d;%f;%f;%f;%f;%f;%f;%d\n", current_time, index,
			accelX, accelY, accelZ, gyroX, gyroY, gyroZ, (int)temp);
}

void setup()
{
	Serial.begin(115200);
	initialise_bmi();
	payload_entry = (char *)malloc(size_of_entry * sizeof(char));
}

void loop()
{
	// Main code loop
	int index = 0;
	get_entry_info(index++);
	Serial.print(payload_entry);
	delay(1000);
}
