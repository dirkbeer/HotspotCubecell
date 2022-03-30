#ifndef SENSOR_H 
#define SENSOR_H

#include <Wire.h>
#include "HDC1080.h"

void sensor_setup();
void sensor_read();

#endif /* SENSOR_H */