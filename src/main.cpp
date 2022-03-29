/*
 */

#include "Arduino.h"
#include "wanderer.h"
#include "sensor.h"
#include "helium.h"

uint8_t u8state;
unsigned long u32wait;

void setup() {
  Serial.begin(115200);
  wanderer_setup();
  sensor_setup();
  helium_setup();
  // variables for the data aquisistion part of the loop
  u32wait = millis() + 1000;
  u8state = 0; 
}

void loop() {
  // LoRaWAN transmission part of the loop
  helium_run();
  // data aquisition part of the loop
  switch( u8state ) {
  case 0: // wait state
    if (millis() > u32wait) u8state++; 
    break;
  case 1: // keep initiating stateful things until all done
    wanderer_write();
    u8state++;
    break;
  case 2: // keep checking for completion of the stateful things until all done
    if ( !wanderer_polling() ) {  
      num_read++;
      if ( num_read == num_queries ){
        num_read = 0;
        u8state++;
      }
    }
    break;
  case 3: // do these things after the stateful things are done
    Serial.println( battery_voltage );
    Serial.println( battery_soc );
    Serial.println( load_voltage );
    Serial.println( load_current );
    sensor_read();
    Serial.println( temperature );
    Serial.println( humidity );
    Serial.println( "-----" );
    u8state = 0;
    u32wait = millis() + 2000;
    break;
  }
}

