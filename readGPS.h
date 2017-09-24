#ifndef READ_GPS_h
#define READ_GPS_h


//************************************************************//
/*
  Longitude

  Add 180 to the longitude, and take the integer value /20, and
  add one. Then figure out which letter of the alphabet that
  corresponds to, usually written in upper case. The example will
  be 147.877/20=7. Adding one will give the 8th letter of the alphabet,
  or H. Note 7.877 is remaining.

  Take the remainder of what is left,
  and divide by 2, rounding down. This is the number, no conversion
  required. The example will give a value of 3. Note 1.877 is remaining.
  Take the remainder that is left, and multiply by 12, and add one.
  Round down to the nearest integer.. This is the letter of the alphabet,
  usually written in lower case. The example gives a value of 22+1=23.
  This will be the letter w.

  Latitude

  Add 90 to the longitude, and take the integer value /10, and add one.
  Then figure out which letter of the alphabet that corresponds to, usually
  written in upper case. The example will be 104.321/10=10. Adding one will
  give the 11th letter of the alphabet, or K. Note 4.321 is remaining.

  Take the remainder of what is left, and round down. This is the number,
  no conversion required. The example will give a value of 4. Note 0.321 is remaining.
  Take the remainder that is left, and multiply by 24, and add one. Round down to
  the nearest integer.. This is the letter of the alphabet, usually written in
  lower case. The example gives a value of 7+1=8. This will be the letter h.
  Putting them together by pairs, and alternating first longitude then latitude,
  gives the grid square for 32.123 W, 14.321 N to be HK34wh.
*/

//************************************************************//
void gridSq( float latitude, float longitude ) {
  longitude = longitude + 180;
  int lon = longitude / 20;
  grid[ 0 ] = upperCaseFromNumber( lon );
  float remainder = longitude - 20 * lon;
  int x = remainder / 2;
  grid[ 2 ] = numberFromNumber( x );
  remainder -= 2 * x;
  gridEx[ 0 ] = lowerCaseFromNumber( 12 * remainder );

  latitude = latitude + 90;
  int lat = latitude / 10;
  grid[ 1 ] = upperCaseFromNumber( lat );
  remainder = latitude - 10 * lat;
  int y = remainder;
  grid[ 3 ] = numberFromNumber( y );
  remainder = remainder - y;
  gridEx[ 1 ] = lowerCaseFromNumber( 24 * remainder );

  grid[ 4 ] = '\0';
  gridEx[ 2 ] = '\0';
}


//************************************************************//
void secondWSPR( float altitude, float temperature, float voltage, float velocity,
                 int numSat ) {

  int altitude1 = altitude / 1750;
  int altitude2 = ( altitude - 1750 * altitude1 ) / 50;
  int temperature1 = ( temperature + 30 ) / 5;
  int volt = ( voltage - 3 ) * 4;
  int velocity1 = velocity / 10;
  int velocity2 = velocity - 10 * velocity1;

  secondCall[ 0 ] = 'Q';
  secondCall[ 1 ] = upperCaseFromNumber( altitude2 );
  secondCall[ 2 ] = IDENTIFIER;
  secondCall[ 3 ] = upperCaseFromNumber( temperature1 );
  secondCall[ 4 ] = gridEx[ 0 ];
  secondCall[ 5 ] = gridEx[ 1 ];
  secondCall[ 6 ] = 0;

  secondGrid[ 0 ] = upperCaseFromNumber( altitude1 );
  secondGrid[ 1 ] = upperCaseFromNumber( volt );
  secondGrid[ 2 ] = numberFromNumber( velocity1 );
  secondGrid[ 3 ] = numberFromNumber( velocity2 );
  secondGrid[ 4 ] = 0;

  secondDbm[ 0 ] = numberFromNumber( transmitPower / 10 );
  secondDbm[ 1 ] = numberFromNumber( transmitPower % 10 );
  secondDbm[ 2 ] = 0;
}


//************************************************************//
// returns updated clock in gpsClock (number of seconds)
bool readGps( char c ) {
  static byte pos = 0;
  static bool gpgga = false, gprmc = false;
  char *temp;

  if ( c == '$' ) {
    pos = 0;
    storing = true;
  }
  if ( pos > bufferSize - 2 )
    pos = 0;

  if ( storing ) {
    buffer[ pos++ ] = c;
    if ( c == '\n' ) {
      storing = false;
      if ( strncmp(  buffer, "$GPGGA", 6 ) == 0 ) {
        strcpy( timeStuff, buffer );

        // extract GPS data
        strtok( buffer, "," );

        long hours = atol( strtok( NULL, "," ) ) % 100000L;  // hours
        minutes = (int)( hours / 100L ) % 2;          // only care about 2 min increments
        seconds = (int)( hours % 100 );
        msClock = 1000L * ( ( 60 * minutes + seconds - TIME_OFFSET ) % TWO_MINUTES_MS );
        timer1Count = 0;

        latitude = atof( strtok( NULL, "," ) );
        float degs = floor( latitude / 100.0 );
        float mins = ( latitude - 100.0 * degs ) / 60.0;
        latitude = degs + mins;

        temp = strtok( NULL, "," );
        if ( strcmp( "S", temp ) == 0 )
          latitude = -latitude;

        longitude = atof( strtok( NULL, "," ) );
        degs = floor( longitude / 100 );
        mins = ( longitude - 100 * degs ) / 60.0;
        longitude = degs + mins;

        temp = strtok( NULL, "," );
        if ( strcmp( "W", temp ) == 0 )
          longitude = -longitude;

        strtok( NULL, "," );                  // Fix Quality
        numberOfSatellites = atoi( strtok( NULL, "," ) );
        strtok( NULL, "," );                  // Horizontal Dilution of Precision

        altitude = atof( strtok( NULL, "," ) ) * METERS_TO_FEET ;
        gpgga = true;
      }

      else if ( strncmp(  buffer, "$GPRMC", 6 ) == 0 ) {
        // extract GPS data
        strtok( buffer, "," );
        strtok( NULL, "," );
        strtok( NULL, "," );
        strtok( NULL, "," );
        strtok( NULL, "," );
        strtok( NULL, "," );
        velocity = atof( strtok( NULL, "," ) );
        gprmc = true;
      }
    }
  }
  return gpgga & gprmc;
}
#endif    // READ_GPS_h
