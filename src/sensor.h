#ifndef SENSOR_H 
#define SENSOR_H

#include <Wire.h>
#include "HDC1080.h"

extern HDC1080 hdc1080;

extern float temperature;
extern float humidity;

void sensor_setup();
void sensor_read();

#endif /* SENSOR_H */