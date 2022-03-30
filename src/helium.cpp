#include "helium.h"
#include "sensor.h"

/*
* set LoraWan_RGB to Active,the RGB active in loraWan
* RGB red means sending;
* RGB purple means joined done;
* RGB blue means RxWindow1;
* RGB yellow means RxWindow2;
* RGB green means received done;
*/

/* OTAA para*/
uint8_t devEui[] = { 0x60, 0x81, 0xF9, 0x9E, 0x84, 0x0C, 0x68, 0x40 };
uint8_t appEui[] = { 0x60, 0x81, 0xF9, 0x13, 0x99, 0xA1, 0x96, 0x96 };
uint8_t appKey[] = { 0x54, 0x39, 0xB2, 0x79, 0xBB, 0x9A, 0x45, 0x49, 0x8E, 0xEA, 0xDB, 0xEA, 0x3F, 0x47, 0x16, 0x97 };

/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask, default channels 0-7*/ 
uint16_t userChannelsMask[6]={ 0xFF00,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15 * 1000; 

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;

void prepareTxFrame(){
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
}

void helium_setup(){
    deviceState = DEVICE_STATE_INIT;
    LoRaWAN.ifskipjoin();
}

