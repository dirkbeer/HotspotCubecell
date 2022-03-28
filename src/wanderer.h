#ifndef WANDERER_H 
#define WANDERER_H

#include <softSerial.h>
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

#endif /* WANDERER_H */

