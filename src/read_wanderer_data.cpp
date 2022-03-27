void read_wanderer(){  
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    telegram.u8id = 0x01; // slave address
    telegram.u8fct = 0x03; // function code (this one is registers read)
    telegram.u16RegAdd = 0x0101; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
    u8state++;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 2000; 
        Serial.println(au16data[0]);//Or do something else!
    }
    break;
  }
}