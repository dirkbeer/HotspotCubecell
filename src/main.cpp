/**
 *  Modbus master example 2:
 *  The purpose of this example is to query an array of data
 *  from an external Modbus slave device.
 *  This example is similar to "simple_master", but this example
 *  allows you to use software serial instead of hardware serial
 *  in case that you want to use D1 & D2 for other purposes.
 *  The link media can be USB or RS232.
 
  The circuit:
 * software serial rx(D3) connect to tx pin of another device
 * software serial tx(D4) connect to rx pin of another device
 
 * In this example, we will use two important methods so that we can use
 * software serial.
 *
 * 1. Modbus::Modbus(uint8_t u8id)
 * This is a constructor for a Master/Slave through USB/RS232C via software serial
 * This constructor only specifies u8id (node address) and should be only
 * used if you want to use software serial instead of hardware serial.
 * This method is called if you create a ModBus object with only on parameter "u8id"
 * u8id is the node address of the arduino that will be programmed on,
 * 0 for master and 1..247 for slave
 * for example: Modbus master(0); 
 * If you use this constructor you have to begin ModBus object by
 * using "void Modbus::begin(SoftwareSerial *softPort, long u32speed)".
 * 
 * 2. void Modbus::begin(SoftwareSerial *sPort, long u32speed)
 * Initialize class object.
 * This is the method you have to use if you construct the ModBus object by using 
 * Modbus::Modbus(uint8_t u8id) in order to use software serial and to avoid problems.
 * You have to create a SoftwareSerial object on your own, as shown in the example.
 * sPort is a pointer to your SoftwareSerial object, u32speed is the baud rate, in 
 * standard increments (300..115200)

 created long time ago
 by smarmengol
 modified 29 July 2016
 by Helium6072

 This example code is in the public domain.
 */

#include <ModbusRtu.h>
#include <softSerial.h>
#include "HDC1080.h"

HDC1080 hdc1080;

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8state;

softSerial mySerial(GPIO1 /*TX pin*/, GPIO2 /*RX pin*/);//Create a SoftwareSerial object so that we can use software serial. Search "software serial" on Arduino.cc to find out more details.

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  port : serial port
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus master(0, mySerial); // this is master and RS-232 or USB-FTDI via software serial

/**
 * This is an structe which contains a query to an slave device
 */
modbus_t telegram;

unsigned long u32wait;

void read_HDC1080();
void read_wanderer();

void setup() {
  // start hard and soft serial ports
  Serial.begin( 115200 ); 
  mySerial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
	// turn on voltage to the sensor and start it
	pinMode(Vext,OUTPUT);
	digitalWrite(Vext,LOW);
  hdc1080.begin(0x40);
  // start modbus
  master.start(); // start the ModBus object.
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0; 
}

void loop() {
  read_wanderer();
  //read_HDC1080();
  //delay(3000);
}

void read_HDC1080(){
	Serial.print("T=");
	Serial.print(hdc1080.readTemperature());
	Serial.print("C, RH=");
	Serial.print(hdc1080.readHumidity());
	Serial.println("%");
}

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