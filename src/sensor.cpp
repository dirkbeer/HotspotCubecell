#include "sensor.h"

HDC1080 hdc1080;
float temperature;
float humidity;

void sensor_setup(){
    // nothing needed here unless want to keep sensor on all the time
}

void sensor_read(){
    pinMode(Vext,OUTPUT);
    digitalWrite(Vext,LOW);
    hdc1080.begin(0x40);
    temperature = (float)(hdc1080.readTemperature());
    humidity = (float)(hdc1080.readHumidity());
    hdc1080.end();
    digitalWrite(Vext,HIGH);
}
