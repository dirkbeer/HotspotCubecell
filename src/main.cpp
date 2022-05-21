/*
 *  HotspotCubecell
 *
 */
#include "Arduino.h"
#include "helium.h"
#include "sensor.h"
#include "wanderer.h"

eState_Wanderer wandererState;
bool power_cycle_requested;
u_long load_back_on_time;
u_long power_cycle_duration = 30 * 1000;  // ms

//downlink data handle function
void downLinkDataHandle(McpsIndication_t *mcpsIndication) // Note that the downlink must contain data in order to be trigger this; also, make sure no other integrations are sending downlinks - dots in console should be blue.
{
  Serial.printf("downlink received on FPort %d ...\r\n", mcpsIndication->Port);
  if (mcpsIndication->Port == 7) {  
    power_cycle_requested = true; // this triggers the asynchronous load toggling in loop()  // TODO: Need better handeling of load state and how to power cycle
    Serial.println(F("power cycle requested (FPort 7)..."));
  }
}

void setup() {
  Serial.begin(115200);
  sensor_setup();
  wanderer_setup();
  //delay(1000); // TODO: Why does none of the setup Serial.println work?
  helium_setup();
}

void loop(){
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
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
      if(power_cycle_requested){ 
        wanderer_load_toggle(wandererState, power_cycle_duration); // TODO: stop using the wandererState global variable? effect on the variable is hidden here but should be explicit
      }
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

