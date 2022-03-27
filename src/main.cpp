/**
 */

#include <ModbusRtu.h>
#include <softSerial.h>
#include "HDC1080.h"

HDC1080 hdc1080;

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8state;

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
modbus_t telegram;

unsigned long u32wait;

void read_HDC1080();
void read_wanderer();
modbus_t write_telegram(uint16_t regadd, uint16_t outdata[16]);

void setup() {
  // start hard and soft serial ports
  Serial.begin( 115200 ); 
  softwareSerial.begin(9600);
	// turn on voltage to the sensor and start it
	//pinMode(Vext,OUTPUT);
	//digitalWrite(Vext,LOW);
  //hdc1080.begin(0x40);
  // start modbus
  master.start(); // start the ModBus object.
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  delay(1000);
}

void loop() {
  read_wanderer();
  //read_HDC1080();
  delay(1000);
}

void read_HDC1080(){
	Serial.print("T=");
	Serial.print(hdc1080.readTemperature());
	Serial.print("C, RH=");
	Serial.print(hdc1080.readHumidity());
	Serial.println("%");
}

void read_wanderer(){
  uint16_t voltage[16];
  master.query( write_telegram(0x0101, voltage) ); // send query (only once)
  master.poll(); // check incoming messages
  while ( master.getState() == COM_WAITING ); // wait until com state is idle
  Serial.println(voltage[0]);
/*
  delay(500);
  master.query( write_telegram(0x0101) ); // send query (only once)
  delay(500);
  master.poll(); // check incoming messages
  delay(500);
  Serial.println(au16data[0]);

  delay(500);
  master.query( write_telegram(0x0104) ); // send query (only once)
  delay(500);
  master.poll(); // check incoming messages
  delay(500);
  Serial.println(au16data[0]);

  delay(500);
  master.query( write_telegram(0x0105) ); // send query (only once)
  delay(500);
  master.poll(); // check incoming messages
  delay(500);
  Serial.println(au16data[0]);
  */
}

modbus_t write_telegram(uint16_t regadd, uint16_t outdata[16]){
  telegram.u8id = 0x01; // slave address
  telegram.u8fct = 0x03; // function code (this one is registers read)
  telegram.u16RegAdd = regadd; // start address in slave
  telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
  telegram.au16reg = outdata; // pointer to a memory array in the Arduino
  return telegram;
}