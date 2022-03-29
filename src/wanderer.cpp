#include "wanderer.h"
#include <ModbusRtu.h>

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8query; //!< pointer to message query

softSerial softwareSerial(GPIO1 /*TX pin*/, GPIO2 /*RX pin*/);

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  port : serial port
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus master(0, softwareSerial); // this is master and RS-232 or USB-FTDI via software serial

/**
 * This is an structe which contains a query to an slave device
 */
//#define NUM_QUERIES 2
//modbus_t telegram[NUM_QUERIES];
const int num_queries = 4;
modbus_t telegram[num_queries];

int num_read;

double battery_voltage;
double battery_soc;
double load_voltage;
double load_current;

void wanderer_setup(){
  // battery voltage
  telegram[0].u8id = 0x01; // slave address
  telegram[0].u8fct = 0x03; // function code (this one is registers read)
  telegram[0].u16RegAdd = 0x0101; // start address in slave
  telegram[0].u16CoilsNo = 0x1; // number of elements (coils or registers) to read
  telegram[0].au16reg = au16data; // pointer to a memory array in the Arduino
  // battery soc
  telegram[1].u8id = 0x01; // slave address
  telegram[1].u8fct = 0x03; // function code (this one is registers read)
  telegram[1].u16RegAdd = 0x0100; // start address in slave
  telegram[1].u16CoilsNo = 0x1; // number of elements (coils or registers) to read
  telegram[1].au16reg = au16data + 4; // pointer to a memory array in the Arduino
  // load voltage
  telegram[2].u8id = 0x01; // slave address
  telegram[2].u8fct = 0x03; // function code (this one is registers read)
  telegram[2].u16RegAdd = 0x0104; // start address in slave
  telegram[2].u16CoilsNo = 0x1; // number of elements (coils or registers) to read
  telegram[2].au16reg = au16data + 8; // pointer to a memory array in the Arduino
  // load current
  telegram[3].u8id = 0x01; // slave address
  telegram[3].u8fct = 0x03; // function code (this one is registers read)
  telegram[3].u16RegAdd = 0x0105; // start address in slave
  telegram[3].u16CoilsNo = 0x1; // number of elements (coils or registers) to read
  telegram[3].au16reg = au16data + 12; // pointer to a memory array in the Arduino

  softwareSerial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
  master.start(); // start the ModBus object.
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u8query = 0;
  num_read = 0;
}

void wanderer_write(){
  master.query( telegram[u8query] ); // send query (only once)
  u8query++;
	if (u8query > num_queries) u8query = 0;
}

bool wanderer_polling(){
  master.poll(); // check incoming messages
  // calculate based on latest in the data array, even if incoming message was not yet received
  battery_voltage = (double) au16data[0] * 0.1; 
  battery_soc = (double) au16data[4]; 
  load_voltage = (double) au16data[8] * 0.1; 
  load_current = (double) au16data[12] * 0.01; 
  return master.getState() == COM_WAITING;
}