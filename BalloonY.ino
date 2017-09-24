// WSPR/Balloon program.
//
// This program drives a WSPR transmitter. Position, time, velocity,
// and altitude data are taken from a GPS module.
//
// Author: Bruce Raymond (nd8i@arrl.net)
#define Version  "0.1  05 Sep 2017"
//

//************************************************************//
// Your callsign is the only parameter that you'll need to set.
//------------------------------------------------------------//
#define CALL                   "ND8I"
//------------------------------------------------------------//
//************************************************************//
#include "Balloon.h"

/*
   Balloon Program Flow

   Initialization

   1. recall data from EEPROM
        (latitude, longitude, altitude, clock cal, and frequency cal)
   2. start wall clock
   3. wait for GPS to com on line
   4. sync wall clock to GPS

   Main Loop

   1. 0 min     - turn on GPS module
   2. n*2 min   - sync uC clock to GPS, then sync up with Si5351A
   3. n*2+2 min - regular WSPR transmission
   4. n*2=4 min - extended WSPR transmission
   5. n*2+6 min - regular WSPR transmission (repeat of #3)
   6. n*2=8 min - extended WSPR transmission (repeat of #4)

   Each symbol has a frequency of n * 12000 / 8192, n=0..3.
   Of course, this is in addition to the carrier frequency.
   Each symbol has a duration of 8192 / 12000 (0.683 sec).
*/


//************************************************************//
void setup() {
  Serial.begin( 57600 );                   // regular serial port  (PC serial port 115200 baud)
  // The actual serial port runs at 9600 baud. However, there is no option
  // in the programming environment for a 3.3 volt/16 MHz oscillator,
  // so the 4800 baud option was chosen to compensate.
  mySerial.begin( 4800 );                 // software serial port connected to GPS
  pinMode( LED_BUILTIN, OUTPUT );
  pinMode( 5, INPUT );
  pinMode( A0, INPUT );
  pinMode( A1, INPUT );
  pinMode( A3, INPUT );
  pinMode( GPS_POWER, INPUT );
  memset( symbols, 0, WSPR_SYMBOLS );
  Serial.println( F( "Ferd is starting up." ) );

  if ( wspr.begin() < 0 ) {
    Serial.println( F( "Uh-oh. Can't open up the Si5351A." ) );
    while ( 1 )
      Serial.println( F( "no Si5351A" ) );
  }

  for ( int k = 0; k < 4; k++ )
    flashLED( FLASH_FAST );

  gpsPowerUp();                           // hopefully start from a known point

  cli();                                  // turn off all interrupts

  // Timer1 - read frequency input
  TCCR1A = _BV( COM1A1 ) | _BV( COM1A0 );
  TCCR1B = _BV( WGM12 ) | _BV( CS12 ) | _BV( CS11 ) | _BV( CS10 );
  TCCR1C = B00000000;
  OCR1AH = 0x03;
  OCR1AL = 0xE7;                          // this works out for 1000 cycles to trip the interrupt
  TIMSK1 = _BV( OCIE1A );

  sei();                                  // turn interrupts back on

  state = StartGPS;
  msClock = 0L;

  wspr.setFrequencyB( FREQUENCY_BASE );
  disableChannelA();
  disableChannelB();
  Serial.println( F( "Startup is finished." ) );
}


//************************************************************//
void TransmissionSetup( States tState, int n ) {
  static byte ledStatus = 0;
  if ( msFlag ) {
    msFlag = false;
    ledStatus = ledStatus ? 0 : 0xFF;
    digitalWrite( LED_BUILTIN, ledStatus );
  }

  if ( limitFlag ) {
    limitFlag = false;
    wspr.enableOutputs();
    digitalWrite( LED_BUILTIN, HIGH );
    Serial.print( "segment " );
    Serial.println( n );
    state = tState;
  }
}


//************************************************************//
void TransmissionPost( States tState ) {
  setTwoMinuteMark();
  disableChannelA();
  digitalWrite( LED_BUILTIN, LOW );
  state = tState;
}


//************************************************************//
void loop() {
  switch ( state ) {
    case StartGPS:
      lf();
      Serial.println( F( "Enabling GPS." ) );
      wspr.disableOutputs();
      delay( 100 );

      gpsPowerUp();
      delay( 100 );

      numberOfSatellites = 0;
      state = WaitingForSatelliteLock;
      break;

    case WaitingForSatelliteLock:
      updateGps();

      if ( numberOfSatellites >= 4 ) {
        syncTime();
        delay( 100 );
        wspr.enableOutputs();
        disableChannelA();
        gpsPowerDown();
        Serial.println( F( "Putting GPS to sleep." ) );

        encodeWsprData( latitude, longitude );
        printUpdate();
        Serial.println( F( "Symbols:" ) );
        printSymbols( symbols );

        state = Transmission1Setup;
        setTwoMinuteMark();
      }
      break;

    case Transmission1Setup:
      measurePowerFlag = true;
      TransmissionSetup( Transmission1, 1 );
      break;

    case Transmission1:   // transmit primary WSPR data stream
      sendSymbols( symbols, Transmission1Post );
      break;

    case Transmission1Post:
      TransmissionPost( Transmission2Setup );
      break;

    case Transmission2Setup:
      TransmissionSetup( Transmission2, 2 );
      break;

    case Transmission2:   // transmit other WSPR data stream
      sendSymbols( secondSymbols, Transmission2Post );
      break;

    case Transmission2Post:
      TransmissionPost( Transmission3Setup );
      break;

    case Transmission3Setup:
      TransmissionSetup( Transmission3, 3 );
      break;

    case Transmission3:   // transmit primary WSPR data stream
      sendSymbols( symbols, Transmission3Post );
      break;

    case Transmission3Post:
      TransmissionPost( Transmission4Setup );
      break;

    case Transmission4Setup:
      TransmissionSetup( Transmission4, 4 );
      break;

    case Transmission4:   // transmit other WSPR data stream
      sendSymbols( secondSymbols, Transmission4Post );
      break;

    case Transmission4Post:
      digitalWrite( LED_BUILTIN, LOW );
      state = StartGPS;
      break;

    case None:
      break;

    default:
      state = StartGPS;
      break;
  }
}

