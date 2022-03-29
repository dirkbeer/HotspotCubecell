/*
 */

#include "Arduino.h"

#include "wanderer.h"
#include "sensor.h"
#include "helium.h"

uint8_t u8state;
unsigned long u32wait;

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

