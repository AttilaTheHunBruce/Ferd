#ifndef POWER_OFF_h
#define POWER_OFF_h


//************************************************************//
void flashLED( int cycles ) {
  digitalWrite( LED_BUILTIN, HIGH );
  delay( cycles );
  digitalWrite( LED_BUILTIN, LOW );
  delay( cycles );
}


//************************************************************//
void gpsHotStart() {
  uint8_t CFG_RST[] =
  { 0xB5, 0x62, 0x06, 0x04, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x0E, 0x64
    //0x13, 0x87
  };
  mySerial.write( CFG_RST, sizeof( CFG_RST ) / sizeof( uint8_t ) );
}


//************************************************************//
void gpsWarmStart() {
  uint8_t CFG_RST[] =
  { 0xB5, 0x62, 0x06, 0x04, 0x04, 0x00,
    0x01, 0x00, 0x00, 0x00,
    0x0F, 0x68
  };
  mySerial.write( CFG_RST, sizeof( CFG_RST ) / sizeof( uint8_t ) );
}


//************************************************************//
void gpsPowerUp() {
  uint8_t GPSon[] =
  { 0xB5, 0x62, 0x06, 0x11, 0x02, 0x00,
    0x00, 0x00,
    0x19, 0x81
  };
  mySerial.write( GPSon, sizeof( GPSon ) / sizeof( uint8_t ) );
  gpsHotStart();
  //gpsWarmStart();
}


//************************************************************//
void gpsPowerDown() {
  // Switch the whole GPS to backup mode - minimal current draw
  // <5mA, loses all settings, wakes on command via serial or after
  // set time. Set GPS to backup mode (sets it to never wake up
  // on its own)
  uint8_t GPSoff[] =
  { 0xB5, 0x62, 0x02, 0x41, 0x08, 0x00,
    0x08, 0x0B, 0x0B, 0x00,
    0x02, 0x00, 0x00, 0x00,
    0x6B, 0x0A
  };
  mySerial.write( GPSoff, sizeof( GPSoff ) / sizeof( uint8_t ) );
}


//************************************************************//
void Si5351Write( uint8_t addr, uint8_t data )
{
  Wire.beginTransmission( Si5351A_addr );
  Wire.write( addr );
  Wire.write( data );
  Wire.endTransmission();
}//************************************************************//


//************************************************************//
void disableChannelA() {
  Si5351Write( CLK_ENABLE_CONTROL, 0b00000001 );
}


//************************************************************//
void disableChannelB() {
  Si5351Write( CLK_ENABLE_CONTROL, 0b00000010 );
}


uint8_t Si5351Read( uint8_t addr )
{
  Wire.requestFrom( Si5351A_addr, 1 );
  return Wire.read();
}

#endif              // POWER_OFF_h

