#ifndef HELIUM_H 
#define HELIUM_H

#include "LoRaWan_APP.h"

extern float temperature;
extern float humidity;
extern uint16_t au16data[16];

void prepareTxFrame();
void helium_setup();
void helium_run();

#endif /* HELIUM_H */
