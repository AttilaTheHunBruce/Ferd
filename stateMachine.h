#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

//************************************************************//
void readGps() {
  while ( !gpsFlag ) {
    if ( mySerial.available() ) {
      char c = mySerial.read();
      gpsFlag = readGps( c );
    }
  }
}


//************************************************************//
float getPower() {
  const float v0 = 3.3;           // power supply voltage
  const float n  = 1023;          // A/D range
  const float p0 = 2.3;           // voltage for power cutoff
  const char px[] = { 0, 3, 7, 10, 13, 17, 20, 23, 27, 30, 33, 37,
                      40, 43, 47, 50, 53, 57, 60
                    };
  const char dBm[] = { 0, 20, 20, 20, 20, 21, 21, 21, 21, 22,
                       22, 22, 22, 22, 23, 23, 23, 23
                     };
  const int size = sizeof( px ) / sizeof( int );
  int pwr = 10 * ( analogRead( PIN_POWER ) * v0 / n - p0 );
  pwr = pwr < 0 ? 0 : pwr;
  pwr = pwr > size ? size : pwr;
  transmitPowerDBm = dBm[ pwr ];
  return px[ pwr ];
}


//************************************************************//
float ln( float x ) {
  return -0.00804 * x * x + 0.25533 * x + 0.5515;
}


//************************************************************//
void getVoltageAndTemperature() {
  const float zz = 11.04;         // zz = ln( 1 /R0 * exp( -B0 / T0 ) = 10K * exp( -3977 / 298 ) )
  const float rx = 8.2 ;          // Kohms
  const float b0 = 3977;          // B0 ohms
  const float t0 = 273.15;        // absolute zero - Kelvin
  const float v0 = 3.3;           // power supply voltage
  const float n  = 1023;          // A/D range
  //  const float p0 = 2.3;           // voltage for power cutoff
  //  const float slope = 1.0;        // slope for power equation
  const float fudge = 4.0;        // dummy calibration constant - a guess
  voltage = analogRead( PIN_VOLTAGE ) * 6.4516E-3 * 1.041; // 2 * 3.3 volts / 1023 counts
  float volts = analogRead( PIN_TEMPERATURE ) * v0 / n;
  float resistance = rx * volts / ( v0 - volts );
  temperature = b0 / ( zz + ln( resistance ) ) - t0 - fudge;
}


//************************************************************//
void encodeWsprData( float latitude, float longitude ) {
  getVoltageAndTemperature();
  gridSq( latitude, longitude );
  wsprEncode( CALL, grid, DBM, symbols );
  secondWSPR( altitude, temperature, voltage, velocity, numberOfSatellites );
  wsprEncode( secondCall, secondGrid, atoi( secondDbm ), secondSymbols );
}


//************************************************************//
void sendSymbols( uint8_t symbols[], States nextState ) {
  static int wsprCount = 0;
  const int offsetFrequencies[] = { 4395, 2390, 1465, 0 };    // symbol frequencies (mHz)

  if ( limitFlag ) {
    limitFlag = false;

    if ( wsprCount >= WSPR_SYMBOLS ) {
      wsprCount = 0;
      state = nextState;
    }

    else {
      byte symbol = symbols[ wsprCount++ ];
      wspr.setFreqA( offsetFrequencies[ symbol ] );
      msLimit += WSPR_TIME;
      limitFlag = false;
    }

    if ( measurePowerFlag && wsprCount == WSPR_SYMBOLS / 4 ) {
      transmitPower = getPower();
      measurePowerFlag = false;
    }
  }
}


#endif      // STATE_MACHINE_H
