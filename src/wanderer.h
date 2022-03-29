#ifndef WANDERER_H 
#define WANDERER_H

#include <softSerial.h>

extern uint16_t au16data[16];

extern const int num_queries;
extern int num_read;

extern double battery_voltage;
extern double battery_soc;
extern double load_voltage;
extern double load_current;

void wanderer_setup();
void wanderer_write();
bool wanderer_polling();

#endif /* WANDERER_H */

