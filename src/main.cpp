/*
 *  HotspotCubecell
 *
 */
#include "Arduino.h"
#include "helium.h"
#include "sensor.h"
#include "wanderer.h"

//downlink data handle function
void downLinkDataHandle(McpsIndication_t *mcpsIndication) // Note that the downlink must contain data in order to be trigger this; also, make sure no other integrations are sending downlinks - dots in console should be blue.
{
  Serial.printf("downlink received on FPort %d ...\r\n", mcpsIndication->Port);
  if (mcpsIndication->Port == 7) {  
    wanderer_load_on();
  }
  if (mcpsIndication->Port == 6) {  
    wanderer_load_off();
  }
}

void setup() {
  Serial.begin(115200);
  sensor_setup();
  wanderer_setup();
  helium_setup();
}

void loop(){
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
      LoRaWAN.init(loraWanClass,loraWanRegion);
      deviceState = DEVICE_STATE_JOIN;
      LoRaWAN.setDataRateForNoADR(1);
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      sensor_read();
      wanderer_read_all();
      prepareTxFrame();
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
}

