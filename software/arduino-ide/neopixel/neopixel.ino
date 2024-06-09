// For NEOPIXEL
#include <Adafruit_NeoPixel.h>

#define PIN_NEOPIXEL 3

Adafruit_NeoPixel pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void light_off()
{
	pixel.setBrightness(0);
	pixel.fill(0x000000);
	pixel.show();
}

void light_green_on()
{
	pixel.setBrightness(5);
	pixel.fill(0x4CBB17);
	pixel.show();
}

void light_red_on()
{
	pixel.setBrightness(5);
	pixel.fill(0xFF0000);
	pixel.show();
}

void light_blue_on()
{
	pixel.setBrightness(5);
	pixel.fill(0x0000FF);
	pixel.show();
}

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(115200);
	pixel.begin();
}

void loop()
{
	// put your main code here, to run repeatedly:
	Serial.println("Hello world!");
	delay(1000);
	light_red_on();
	delay(500);
	light_green_on();
	delay(500);
	light_blue_on();
	delay(500);
	light_off();
}
