/*
 */

#include "wanderer.h"
#include "sensor.h"

uint8_t u8state;
unsigned long u32wait;

void wanderer_setup();
void wanderer_write();
uint8_t wanderer_read();

void sensor_setup();
void sensor_read();

void setup() {
  Serial.begin( 115200 ); 
  wanderer_setup();
  sensor_setup();
  u32wait = millis() + 1000;
  u8state = 0; 
}

void loop() {
  switch( u8state ) {
  case 0: // wait state
    if (millis() > u32wait) u8state++; 
    break;
  case 1: // initiate things
    wanderer_write();
    sensor_read();
    u8state++;
    break;
  case 2: // complete things
    if ( wanderer_read() == COM_IDLE) {  // currently printing everything on every wanderer reading returned - how to disentangle to print only once everything is available?
      u8state = 0;
      u32wait = millis() + 2000;
      Serial.println( battery_voltage );
      Serial.println( load_current );
      Serial.println( temperature );
      Serial.println( humidity );
      Serial.println( "-----" );
    }
    break;
  }
}

void wanderer_setup(){
  telegram[0].u8id = 0x01; // slave address
  telegram[0].u8fct = 0x03; // function code (this one is registers read)
  telegram[0].u16RegAdd = 0x0101; // start address in slave
  telegram[0].u16CoilsNo = 0x1; // number of elements (coils or registers) to read
  telegram[0].au16reg = au16data; // pointer to a memory array in the Arduino

  telegram[1].u8id = 0x01; // slave address
  telegram[1].u8fct = 0x03; // function code (this one is registers read)
  telegram[1].u16RegAdd = 0x0105; // start address in slave
  telegram[1].u16CoilsNo = 0x1; // number of elements (coils or registers) to read
  telegram[1].au16reg = au16data + 4; // pointer to a memory array in the Arduino

  softwareSerial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
  master.start(); // start the ModBus object.
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u8query = 0;
}

void wanderer_write(){
  master.query( telegram[u8query] ); // send query (only once)
  u8query++;
	if (u8query > num_queries) u8query = 0;
}

uint8_t wanderer_read(){
  master.poll(); // check incoming messages
  // calculate based on latest in the data array, even if incoming message was not yet received
  battery_voltage = (double) au16data[0] * 0.1; 
  load_current = (double) au16data[4] * 0.01; 
  return master.getState();
}

void sensor_setup(){
 	//vext on
	pinMode(Vext,OUTPUT);
	digitalWrite(Vext,LOW);
	// Default settings: 
	//  - Heater off
	//  - 14 bit Temperature and Humidity Measurement Resolutions
	hdc1080.begin(0x40);
}

void sensor_read(){
	temperature = hdc1080.readTemperature();
  humidity = hdc1080.readHumidity();
}