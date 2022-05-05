/*
 *  HotspotCubecell
 *
 */
#include "Arduino.h"
#include "helium.h"
#include "sensor.h"
#include "wanderer.h"

bool power_cycle_requested;
enum eState_Wanderer
{
    WANDERER_STATE_LOAD_OFF,
    WANDERER_STATE_LOAD_ON,
};
eState_Wanderer wandererState;
u_long load_back_on_time;
u_long power_cycle_duration;

//downlink data handle function
void downLinkDataHandle(McpsIndication_t *mcpsIndication) // Note that the downlink must contain data in order to be trigger this; also, make sure no other integrations are sending downlinks - dots in console should be blue.
{
  Serial.printf("downlink received on FPort %d ...\r\n", mcpsIndication->Port);
  if (mcpsIndication->Port == 7) {  
    power_cycle_requested = true;
    Serial.println("power cycle requested (FPort 7)...");
  }
}

void setup() {
  Serial.begin(115200);
  sensor_setup();
  wanderer_setup();
  power_cycle_duration = 30 * 1000;  // milliseconds, durations less than appTxDutyCycle will result in appTxDutyCycle duration 
  wanderer_load_on(); // TEST ONLY, TAKE OUT FOR SAFETY
  wandererState = WANDERER_STATE_LOAD_ON;
  power_cycle_requested = false;  
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
      wanderer_read();
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
      if(power_cycle_requested){ // this causes the cubecell to send two uplinks in rapid succession - why? Does not skip uplink, instead causes an extra one
        delay(1000);  // wait for other things (what?) to finish before printing - this does fix the incomplete printing issue (one Serial.delayByte(); is not enough)
        switch(wandererState){
            case(WANDERER_STATE_LOAD_ON):
            {
              load_back_on_time = millis() + power_cycle_duration;
              wanderer_load_off();
              wandererState = WANDERER_STATE_LOAD_OFF;
              Serial.println("load OFF ...");
              break;
            }
            case(WANDERER_STATE_LOAD_OFF):
            {
              if(millis()>load_back_on_time){
                wanderer_load_on();
                wandererState = WANDERER_STATE_LOAD_ON;
                power_cycle_requested = false;
                Serial.println("load ON ...");
              }
              break;
            }
            default:
            {
              break;
            }
          }
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

