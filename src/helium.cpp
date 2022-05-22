#include "helium.h"

/*
* set LoraWan_RGB to Active,the RGB active in loraWan
* RGB red means sending;
* RGB purple means joined done;
* RGB blue means RxWindow1;
* RGB yellow means RxWindow2;
* RGB green means received done;
*/

/* OTAA para*/
uint8_t devEui[] = { 0x60, 0x81, 0xF9, 0x4C, 0x82, 0x88, 0x85, 0xEF };
uint8_t appEui[] = { 0x60, 0x81, 0xF9, 0x13, 0x99, 0xA1, 0x96, 0x96 };
uint8_t appKey[] = { 0x15, 0x57, 0xBB, 0x51, 0xD9, 0x2F, 0x8F, 0xE6, 0x31, 0xDB, 0x50, 0x98, 0x8E, 0x39, 0x23, 0xDA };

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
uint8_t confirmedNbTrials = 6;  // was 4, for no adr, LoRaWan_APP.cpp:defaultDrForNoAdr defaults to DR_3, and max payload for DR_1 is 53 bytes, just enough, so setting of 6 here allows fallback to DR_1

extern float temperature;  // defined in sensor.cpp and needed here
extern float humidity;
extern uint16_t au16data[16];  // defined in wanderer.cpp and needed here

void prepareTxFrame(){
    appDataSize = 52;
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
    appData[12] = (uint8_t)(au16data[16]>>8);
    appData[13] = (uint8_t)au16data[16];
    appData[14] = (uint8_t)(au16data[20]>>8);
    appData[15] = (uint8_t)au16data[20];
    appData[16] = (uint8_t)(au16data[24]>>8);
    appData[17] = (uint8_t)au16data[24];
    appData[18] = (uint8_t)(au16data[28]>>8);
    appData[19] = (uint8_t)au16data[28];
    appData[20] = (uint8_t)(au16data[32]>>8);
    appData[21] = (uint8_t)au16data[32];
    appData[22] = (uint8_t)(au16data[36]>>8);
    appData[23] = (uint8_t)au16data[36];
    appData[24] = (uint8_t)(au16data[40]>>8);
    appData[25] = (uint8_t)au16data[40];
    appData[26] = (uint8_t)(au16data[44]>>8);
    appData[27] = (uint8_t)au16data[44];
    appData[28] = (uint8_t)(au16data[48]>>8);
    appData[29] = (uint8_t)au16data[48];
    appData[30] = (uint8_t)(au16data[52]>>8);
    appData[31] = (uint8_t)au16data[52];
    appData[32] = (uint8_t)(au16data[56]>>8);
    appData[33] = (uint8_t)au16data[56];
    appData[34] = (uint8_t)(au16data[60]>>8);
    appData[35] = (uint8_t)au16data[60];
    appData[36] = (uint8_t)(au16data[64]>>8);
    appData[37] = (uint8_t)au16data[64];
    appData[38] = (uint8_t)(au16data[68]>>8);
    appData[39] = (uint8_t)au16data[68];
    appData[40] = (uint8_t)(au16data[72]>>8);
    appData[41] = (uint8_t)au16data[72];
    appData[42] = (uint8_t)(au16data[76]>>8);
    appData[43] = (uint8_t)au16data[76];
    appData[44] = (uint8_t)(au16data[80]>>8);
    appData[45] = (uint8_t)au16data[80];
    appData[46] = (uint8_t)(au16data[84]>>8);
    appData[47] = (uint8_t)au16data[84];
    appData[48] = (uint8_t)(au16data[88]>>8);
    appData[49] = (uint8_t)au16data[88];
    appData[50] = (uint8_t)(au16data[92]>>8);
    appData[51] = (uint8_t)au16data[92];
}

void helium_setup(){
    deviceState = DEVICE_STATE_INIT;
}

