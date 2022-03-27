#include "read_wanderer.h"

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8state;

/**
 * This is an structe which contains a query to an slave device
 */
modbus_t telegram;

unsigned long u32wait;

softSerial mySerial(GPIO1 /*TX pin*/, GPIO2 /*RX pin*/);//Create a SoftwareSerial object so that we can use software serial. Search "software serial" on Arduino.cc to find out more details.

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  port : serial port
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus master(0, mySerial); // this is master and RS-232 or USB-FTDI via software serial
    
void read_wanderer(){  
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    telegram.u8id = 0x01; // slave address
    telegram.u8fct = 0x03; // function code (this one is registers read)
    telegram.u16RegAdd = 0x0101; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
    u8state++;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 2000; 
        Serial.println(au16data[0]);//Or do something else!
    }
    break;
  }
}