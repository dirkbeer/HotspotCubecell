#include "wanderer.h"
#include <ModbusRtu.h>

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
 * The data array for modbus query results
 */
uint16_t au16data[96];
/**
 * This is an structure which contains a query to an slave device, can't be bigger than au16data
 */
const uint8_t u8queries = 24;
uint8_t u8id = 0x01; // slave id
uint8_t u8fct = 0x03;  // function code 0x03 = read, 0x06 = write
uint16_t u16CoilsNo = 0x01; // number of coils to read
modbus_t telegram[u8queries] = {
    {u8id, u8fct, 0x0100, u16CoilsNo, au16data}, // battery soc
    {u8id, u8fct, 0x0101, u16CoilsNo, au16data + 4}, // battery voltage
    {u8id, u8fct, 0x0102, u16CoilsNo, au16data + 8}, // battery current, charging
    {u8id, u8fct, 0x0103, u16CoilsNo, au16data + 12}, // controller temperature
    {u8id, u8fct, 0x0104, u16CoilsNo, au16data + 16}, // load voltage
    {u8id, u8fct, 0x0105, u16CoilsNo, au16data + 20}, // load current
    {u8id, u8fct, 0x0106, u16CoilsNo, au16data + 24}, // load power
    {u8id, u8fct, 0x0107, u16CoilsNo, au16data + 28}, // solar voltage
    {u8id, u8fct, 0x0108, u16CoilsNo, au16data + 32}, // solar current
    {u8id, u8fct, 0x0109, u16CoilsNo, au16data + 36}, // solar power
    {u8id, u8fct, 0x010B, u16CoilsNo, au16data + 40}, // battery voltage, day max
    {u8id, u8fct, 0x010C, u16CoilsNo, au16data + 44}, // battery voltage, day min
    {u8id, u8fct, 0x010D, u16CoilsNo, au16data + 48}, // battery current, day max charging
    {u8id, u8fct, 0x010E, u16CoilsNo, au16data + 52}, // battery current, day max discharging
    {u8id, u8fct, 0x010F, u16CoilsNo, au16data + 56}, // battery power, day max charging
    {u8id, u8fct, 0x0110, u16CoilsNo, au16data + 60}, // battery power, day max discharging
    {u8id, u8fct, 0x0111, u16CoilsNo, au16data + 64}, // battery AH, day charging
    {u8id, u8fct, 0x0112, u16CoilsNo, au16data + 68}, // battery AH, day discharging
    {u8id, u8fct, 0x0113, u16CoilsNo, au16data + 72}, // energy generation, day
    {u8id, u8fct, 0x0114, u16CoilsNo, au16data + 76}, // energy consumption, day
    {u8id, u8fct, 0x0115, u16CoilsNo, au16data + 80}, // total operating days
    {u8id, u8fct, 0x0116, u16CoilsNo, au16data + 84}, // total battery over-discharges 
    {u8id, u8fct, 0x0117, u16CoilsNo, au16data + 88}, // total battery full-charges
    {u8id, u8fct, 0x0120, u16CoilsNo, au16data + 92}, // load status & charging state
};

/**
 * The data array for modbus query results
 */
uint16_t au16data2[16];
/**
 * This is an structure which contains a query to an slave device, can't be bigger than au16data
 */
const uint8_t u8queries2 = 1;
uint8_t u8id2 = 0x01; // slave id
uint8_t u8fct2 = 0x06;  // function code 0x03 = read, 0x06 = write
uint16_t u16CoilsNo2 = 0x01; // number of coils to read
modbus_t telegram2[u8queries] = {
    {u8id2, u8fct2, 0x010a, u16CoilsNo2, au16data2}, // load on / off
};

/**
 * The data array for modbus query results
 */
uint16_t au16data3[16];
/**
 * This is an structure which contains a query to an slave device, can't be bigger than au16data
 */
const uint8_t u8queries3 = 1;
uint8_t u8id3 = 0x01; // slave id
uint8_t u8fct3 = 0x06;  // function code 0x03 = read, 0x06 = write
uint16_t u16CoilsNo3 = 0x01; // number of coils to read
modbus_t telegram3[u8queries] = {
    {u8id3, u8fct3, 0x0120, u16CoilsNo3, au16data3}, // load state
};

extern eState_Wanderer wandererState;
extern bool power_cycle_requested;
extern u_long load_back_on_time;
extern u_long power_cycle_duration;

void wanderer_setup(){
    softwareSerial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
    master.start(); // start the ModBus object.
    master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
    wandererState = wanderer_get_loadstate(); // TODO: Need better handeling of load state and how to power cycle
    power_cycle_requested = false;  
}

eState_Wanderer wanderer_get_loadstate() {
    uint8_t u8query = 0;
    eState_Wanderer load_state;
    while (u8query < u8queries3){
        master.query( telegram3[u8query] ); 
        do {
            master.poll();
        } while ( master.getState() == COM_WAITING );
        Serial.println( au16data3[u8query*4] );
        u8query++;
    }
    if ( (bool)( au16data3[0]>>7 ) ){
        load_state = WANDERER_STATE_LOAD_ON;
        Serial.println( "WANDERER_STATE_LOAD_ON" );
    } else {
        load_state = WANDERER_STATE_LOAD_OFF;
        Serial.println( "WANDERER_STATE_LOAD_OFF" );
    }
    return load_state;
}

void wanderer_read_all() {
    uint8_t u8query = 0;
    while (u8query < u8queries){
        master.query( telegram[u8query] ); 
        do {
            master.poll();
        } while ( master.getState() == COM_WAITING );
        Serial.println( au16data[u8query*4] );
        u8query++;
    }
}

void wanderer_load_off() {
    // turn load off
    au16data2[0] = 0x0; 
    uint8_t u8query = 0;
    while (u8query < u8queries2){
        master.query( telegram2[u8query] ); 
        do {
            master.poll();
        } while ( master.getState() == COM_WAITING );
        Serial.println( au16data2[u8query*4] );
        u8query++;
    }
}

void wanderer_load_on() {
    // turn load on
    au16data2[0] = 0x1;
    uint8_t u8query2 = 0;
    while (u8query2 < u8queries2){
        master.query( telegram2[u8query2] ); 
        do {
            master.poll();
        } while ( master.getState() == COM_WAITING );
        Serial.println( au16data2[u8query2*4] );
        u8query2++;
    }
}

void wanderer_load_toggle(eState_Wanderer wandererState, u_long power_cycle_duration){ // TODO: this sometimes causes the cubecell to send two uplinks in rapid succession - why? Does not skip uplink, instead causes an extra one
    delay(1000);  // TODO: wait for other things (what?) to finish before printing - this does fix the incomplete printing issue, but why the issue? (one Serial.delayByte(); is not enough)
    switch(wandererState){
        case(WANDERER_STATE_LOAD_ON):
        {
            load_back_on_time = millis() + power_cycle_duration;
            wanderer_load_off();
            wandererState = WANDERER_STATE_LOAD_OFF;
            Serial.println(F("load OFF ..."));
            break;
        }
        case(WANDERER_STATE_LOAD_OFF):
        {
            if(millis()>load_back_on_time){
            wanderer_load_on();
            wandererState = WANDERER_STATE_LOAD_ON;
            power_cycle_requested = false;
            Serial.println(F("load ON ..."));
            }
            break;
        }
        default:
        {
            break;
        }
    }
}