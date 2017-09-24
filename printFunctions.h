#ifndef PRINT_FUNCTIONS_h
#define PRINT_FUNCTIONS_h


//************************************************************//
void spaces();
void lf();
void prnt( char * );


//************************************************************//
void lf() {
  Serial.print( F( "\n" ) );
}


//************************************************************//
char lowerCaseFromNumber( int n ) {
  return (char)( n + 'a' );
}


//************************************************************//
char numberFromNumber( int n ) {
  return (char)( n + '0' );
}


//************************************************************//
void prnt( char *s ) {
  Serial.print( s );
}


//************************************************************//
void printEqualSign() {
  Serial.print( F( " = " ) );
}


//************************************************************//
void printF( const char *s, float f ) {
  prnt( s );
  printEqualSign();
  Serial.print( f );
  spaces();
}


//************************************************************//
void printFN( const char *s, float f ) {
  printF( s, f );
  lf();
}


//************************************************************//
void printI( const char *s, int i ) {
  prnt( s );
  printEqualSign();
  Serial.print( i );
  spaces();
}


//************************************************************//
void printIN( const char *s, int i ) {
  printI( s, i );
  lf();
}


//************************************************************//
void printL( const char *s, long l ) {
  prnt( s );
  printEqualSign();
  Serial.print( l );
  spaces();
}


//************************************************************//
void printLN( const char *s, long l ) {
  printL( s, l );
  lf();
}


//************************************************************//
void printS( const char *s, char *t ) {
  prnt( s );
  printEqualSign();
  prnt( t );
  spaces();
}


//************************************************************//
void printSN( const char *s, char *t ) {
  printS( s, t );
  lf();
}


//************************************************************//
void printSymbols( uint8_t *table ) {
  static byte count = 0;
  for ( int k = 0; k < WSPR_SYMBOLS; k++ ) {
    Serial.print( table[ k ] );
    Serial.print( " " );
    if ( ++count >= 18 ) {
      count = 0;
      lf();
    }
  }
  lf();
}


//************************************************************//
void printU( const char *s, uint32_t u ) {
  prnt( s );
  printEqualSign();
  Serial.print( u );
  spaces();
}


//************************************************************//
void printULn( const char *s, uint32_t u ) {
  printL( s, u );
  lf();
}


//************************************************************//
void printUpdate() {
  lf();
  lf();
  Serial.println( F( "//************************************//" ) );
  Serial.println( F( "WSPR-Balloon Control Program" ) );
  Serial.println( F( "copyright (c) Bruce Raymond 2017" ) );
  Serial.print( F( "software version      " ) );
  Serial.println( Version );
  Serial.print( F( "clock:                " ) );
  Serial.print( msClock );
  Serial.println( F( " msec" ) );
  Serial.println( F( "----------------------------------------" ) );
  Serial.print( F( "call sign:            " ) );
  Serial.println( CALL );
  Serial.print( F( "grid square:          " ) );
  Serial.print( grid );
  Serial.print( secondCall[ 4 ] );
  Serial.println( secondCall[ 5 ] );
  Serial.print( F( "dBm:                  " ) );
  Serial.println( DBM );
  Serial.println( F( "----------------------------------------" ) );
  Serial.print( F( "second call sign:     " ) );
  Serial.println( secondCall );
  Serial.print( F( "second grid square:   " ) );
  Serial.println( secondGrid );
  Serial.print( F( "second dBm:           " ) );
  Serial.println( secondDbm );
  Serial.println( F( "----------------------------------------" ) );
  Serial.print( F( "latitude:             " ) );
  Serial.print( latitude );
  Serial.println( " deg" );
  Serial.print( F( "longitude:            " ) );
  Serial.print( longitude );
  Serial.println( " deg" );
  Serial.print( F( "altitude:             " ) );
  Serial.print( altitude );
  Serial.println( " ft" );
  Serial.print( F( "velocity:             " ) );
  Serial.print( velocity );
  Serial.println( " meters/sec" );
  Serial.print( F( "voltage:              " ) );
  Serial.print( voltage );
  Serial.println( " volts" );
  Serial.print( F( "temperature:          " ) );
  Serial.print( temperature );
  Serial.println( " degC" );
  Serial.print( F( "transmit power:       " ) );
  Serial.print( transmitPowerDBm );
  Serial.println( " dBm (0=startup)" );
  Serial.print( F( "number of sattelites: " ) );
  Serial.println( numberOfSatellites );
  Serial.println( F( "//************************************//" ) );
  lf();
}


//************************************************************//
void spaces() {
  Serial.print( F( "   " ) );
}


//************************************************************//
char upperCaseFromNumber( int n ) {
  return (char)( n + 'A' );
}

#endif            // PRINT_FUNCTIONS_h
