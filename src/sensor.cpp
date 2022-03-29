#include "sensor.h"

HDC1080 hdc1080;

float temperature;
float humidity;

void sensor_setup(){
 	//vext on
	pinMode(Vext,OUTPUT);
	digitalWrite(Vext,LOW);
	// Default settings: 
	//  - Heater off
	//  - 14 bit Temperature and Humidity Measurement Resolutions
	hdc1080.begin(0x40);
}

void sensor_read(){
	temperature = (float)(hdc1080.readTemperature());
    humidity = (float)(hdc1080.readHumidity());
}
