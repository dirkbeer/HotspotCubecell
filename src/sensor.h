#ifndef SENSOR_H 
#define SENSORR_H

#include <Wire.h>
#include "HDC1080.h"

HDC1080 hdc1080;

float temperature;
float humidity;

#endif /* SENSOR_H */