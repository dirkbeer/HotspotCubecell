#include "sensor.h"

HDC1080 hdc1080;

float temperature;
float humidity;

void sensor_setup(){
 	//vext on
	pinMode(Vext,OUTPUT);
	digitalWrite(Vext,LOW);
	hdc1080.begin(0x40);
	//digitalWrite(Vext,HIGH); // turning sensor power off causes erroneous readings, but maybe delay would fix? how to correctly implement delay in loop?
}

void sensor_read(){
	//digitalWrite(Vext,LOW);
	temperature = (float)(hdc1080.readTemperature());
    humidity = (float)(hdc1080.readHumidity());
	//digitalWrite(Vext,HIGH); // turning sensor power off causes erroneous readings, but maybe delay would fix? how to correctly implement delay in loop?
}
