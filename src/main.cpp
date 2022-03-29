/*
 */

#include "Arduino.h"

#include "wanderer.h"
#include "sensor.h"
#include "helium.h"

uint8_t u8state;
unsigned long u32wait;

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

uint8_t wanderer_read(){
  master.poll(); // check incoming messages
  // calculate based on latest in the data array, even if incoming message was not yet received
  battery_voltage = (double) au16data[0] * 0.1; 
  battery_soc = (double) au16data[4]; 
  load_voltage = (double) au16data[8] * 0.1; 
  load_current = (double) au16data[12] * 0.01; 
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
	temperature = (float)(hdc1080.readTemperature());
  humidity = (float)(hdc1080.readHumidity());
}

void setup() {
  boardInitMcu();
  Serial.begin(115200);
#if(AT_SUPPORT)
  enableAt();
#endif
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();

//  Serial.begin( 115200 ); 
  wanderer_setup();
  sensor_setup();
  u32wait = millis() + 1000;
  u8state = 0; 
}

void loop() {
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
#if(AT_SUPPORT)
      getDevParam();
#endif
      printDevParam();
      LoRaWAN.init(loraWanClass,loraWanRegion);
      deviceState = DEVICE_STATE_JOIN;
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      appDataSize = 12;
      // hdc1080 data
      int16_t tempInt = temperature * 100;
      appData[0] = tempInt >> 8;
      appData[1] = tempInt;
      int16_t humidInt = humidity * 100;
      appData[2] = humidInt >> 8;
      appData[3] = humidInt;
      // wanderer data
      appData[4] = (uint8_t)(au16data[0]>>8);
      appData[5] = (uint8_t)au16data[0];
      appData[6] = (uint8_t)(au16data[4]>>8);
      appData[7] = (uint8_t)au16data[4];
      appData[8] = (uint8_t)(au16data[8]>>8);
      appData[9] = (uint8_t)au16data[8];
      appData[10] = (uint8_t)(au16data[12]>>8);
      appData[11] = (uint8_t)au16data[12];
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;
      break;
    }
    case DEVICE_STATE_CYCLE:
    {
      // Schedule next packet transmission
      txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      LoRaWAN.sleep();
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
  switch( u8state ) {
  case 0: // wait state
    if (millis() > u32wait) u8state++; 
    break;
  case 1: // keep initiating stateful things until all done
    wanderer_write();
    u8state++;
    break;
  case 2: // keep checking for completion of the stateful things until all done
    if ( wanderer_read() == COM_IDLE) {  
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

