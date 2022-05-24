#ifndef WANDERER_H 
#define WANDERER_H

#include <softSerial.h>

enum eState_Wanderer
{
    WANDERER_STATE_LOAD_OFF,
    WANDERER_STATE_LOAD_ON,
};

void wanderer_setup();
void wanderer_read_all();
void wanderer_load_on();
void wanderer_load_off();
void wanderer_load_cycle();

#endif /* WANDERER_H */

