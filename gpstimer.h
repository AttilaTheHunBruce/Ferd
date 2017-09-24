#ifndef GPS_TIMER_H
#define GPS_TIMER_H


//************************************************************//
// handler for TIMER1 - T1, pin 5
ISR( TIMER1_COMPA_vect ) {
  if ( ++timer1Count >= 500 ) {           // set timer to 1/2 sec interval
    timer1Count = 0;
    msFlag = true;
  }

  if ( ++msClock >= msLimit )
    limitFlag = true;
}


//************************************************************//
void setTwoMinuteMark() {
  limitFlag = false;
  msClock = msClock % TWO_MINUTES_MS;
  if ( msClock > TWO_MINUTES_MS )
    msClock -= TWO_MINUTES_MS;
  msLimit = TWO_MINUTES_MS;
}


//************************************************************//
// Synchronize internal clock with GPS time
void syncTime() {
  // wait for a full set of GPS messages ($GPGGA and $GPRMC)
  bool done = false;
  while ( !done ) {
    // get current GPS data
    if ( mySerial.available() ) {
      char c = mySerial.read();
      done = readGps( c );
    }
  }
}


//************************************************************//
void updateGps() {
  bool flag = false;
  while ( !flag ) {
    if ( mySerial.available() ) {
      char c = mySerial.read();
      flag = readGps( c );
    }
  }
}

#endif         // GPS_TIMER_H
