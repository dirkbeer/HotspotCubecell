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
uint16_t au16data[16];
/**
 * This is an structure which contains a query to an slave device, can't be bigger than au16data
 */
const uint8_t u8queries = 4;
uint8_t u8id = 0x01; // slave id
uint8_t u8fct = 0x03;  // function code 0x03 = read, 0x06 = write
uint16_t u16CoilsNo = 0x01; // number of coils to read
modbus_t telegram[u8queries] = {
    {u8id, u8fct, 0x0101, u16CoilsNo, au16data}, // battery voltage
    {u8id, u8fct, 0x0100, u16CoilsNo, au16data + 4}, // battery soc
    {u8id, u8fct, 0x0104, u16CoilsNo, au16data + 8}, // load voltage
    {u8id, u8fct, 0x0105, u16CoilsNo, au16data + 12}, // load current
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

void wanderer_setup(){
    softwareSerial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
    master.start(); // start the ModBus object.
    master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
}

void wanderer_read() {
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

void wanderer_load_power_cycle() {
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
    // wait 30 seconds
    delay(30*1000);
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
