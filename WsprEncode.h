#ifndef  WSPR_ENCODE_H
#define  WSPR_ENCODE_H


// Encode call, locator, and dBm into WSPR codeblock.
void wsprEncode( const char* call, const char* l_pre, const char* dbm, unsigned char* symbols )
{
  // pack prefix in nadd, call in n1, grid, dbm in n2
  char* c, buf[16];
  strncpy( buf, call, 16 );
  c = buf;
  for ( int i = 0; i < strlen( c ); i++ )
    c[ i ] = toupper( c[ i ] );
  unsigned long ng, nadd = 0;

  if ( strchr( c, '/' ) ) { //prefix-suffix
    nadd = 2;
    int i = strchr( c, '/' ) - c; //stroke position
    int n = strlen( c ) - i - 1; //suffix len, prefix-call len
    c[i] = '\0';
    if ( n == 1 ) ng = 60000 - 32768 + ( c[ i + 1 ] >= '0' && c[ i + 1 ] <= '9' ? c[ i + 1 ] - '0'
                                           : c[ i + 1 ] == ' ' ? 38 : c[ i + 1 ] - 'A' + 10 ) ; // suffix /A to /Z, /0 to /9
    if ( n == 2 ) ng = 60000 + 26 + 10 * ( c[ i + 1 ] - '0' ) + ( c[ i + 2 ] - '0' ); // suffix /10 to /99
    if ( n > 2 ) { // prefix EA8/, right align
      ng = ( i < 3 ? 36 : c[ i - 3 ] >= '0' && c[ i - 3 ] <= '9' ? c[ i - 3 ] - '0' : c[ i - 3 ] - 'A' + 10 );
      ng = 37 * ng + ( i < 2 ? 36 : c[ i - 2 ] >= '0' && c[ i - 2 ] <= '9' ? c[ i - 2 ] - '0' : c[ i - 2 ] - 'A' + 10 );
      ng = 37 * ng + ( i < 1 ? 36 : c[ i - 1 ] >= '0' && c[ i - 1 ] <= '9' ? c[ i - 1 ] - '0' : c[ i - 1 ] - 'A' + 10 );
      if ( ng < 32768 ) nadd = 1; else ng = ng - 32768;
      c = c + i + 1;
    }
  }

  int i = ( isdigit( c[2]) ? 2 : isdigit( c[1] ) ? 1 : 0 ); //last prefix digit of de-suffixed/de-prefixed callsign
  int n = strlen( c ) - i - 1; //2nd part of call len
  unsigned long n1;
  n1 = ( i < 2 ? 36 : c[ i - 2 ] >= '0' && c[ i - 2 ] <= '9' ? c[ i - 2 ] - '0' : c[ i - 2 ] - 'A' + 10 );
  n1 = 36 * n1 + ( i < 1 ? 36 : c[ i - 1 ] >= '0' && c[ i - 1 ] <= '9' ? c[ i - 1 ] - '0' : c[ i - 1 ] - 'A' + 10 );
  n1 = 10 * n1 + c[i] - '0';
  n1 = 27 * n1 + ( n < 1 ? 26 : c[i + 1] - 'A' );
  n1 = 27 * n1 + ( n < 2 ? 26 : c[i + 2] - 'A' );
  n1 = 27 * n1 + ( n < 3 ? 26 : c[i + 3] - 'A' );

  //if(rand() % 2) nadd=0;
  if ( !nadd ) {
    // Copy locator locally since it is declared const and we cannot modify
    // its contents in-place.
    char l[ 4 ];
    strncpy( l, l_pre, 4 );
    for ( int i = 0; i < strlen( l ); i++ )
      l[ i ] = toupper( l[ i ] );

    ng = 180 * ( 179 - 10 * ( l[0] - 'A' ) - ( l[2] - '0' ) ) + 10 * ( l[ 1 ] - 'A' ) + (l[ 3 ] - '0' );
  }
  int p = atoi( dbm );    //EIRP in dBm={0,3,7,10,13,17,20,23,27,30,33,37,40,43,47,50,53,57,60}
  int corr[] = { 0, -1, 1, 0, -1, 2, 1, 0, -1, 1 };
  p = p > 60 ? 60 : p < 0 ? 0 : p + corr[ p % 10 ];
  unsigned long n2 = ( ng << 7 ) | ( p + 64 + nadd );

  // pack n1,n2,zero-tail into 50 bits
  char packed[11] = {n1 >> 20, n1 >> 12, n1 >> 4, ((n1 & 0x0f) << 4) | ((n2 >> 18) & 0x0f),
                     n2 >> 10, n2 >> 2, (n2 & 0x03) << 6, 0, 0, 0, 0
                    };

  // convolutional encoding K=32, r=1/2, Layland-Lushbaugh polynomials
  int k = 0;
  int j, s;
  long nstate = 0;
  unsigned char symbol[ 176 ];
  for ( j = 0; j != sizeof( packed ); j++ ) {
    for ( i = 7; i >= 0; i-- ) {
      unsigned long poly[2] = { 0xf2d05351L, 0xe4613c47L };
      nstate = ( nstate << 1) | ((packed[j] >> i) & 1 );
      for ( s = 0; s != 2; s++ ) { //convolve
        unsigned long n = nstate & poly[s];
        int even = 0;  // even := parity(n)
        while ( n ) {
          even = 1 - even;
          n = n & ( n - 1 );
        }
        symbol[ k ] = even;
        k++;
      }
    }
  }


  // interleave symbols
  const unsigned char npr3[ 162 ] = {
    1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0,
    0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0,
    0, 0
  };
  for ( i = 0; i != 162; i++ ) {
    // j0 := bit reversed_values_smaller_than_161[ i ]
    unsigned char j0;
    p = -1;
    for ( k = 0; p != i; k++ ) {
      for ( j = 0; j != 8; j++ ) // j0:=bit_reverse(k)
        j0 = ( ( k >> j ) & 1) | ( j0 << 1 );
      if ( j0 < 162 )
        p++;
    }
    symbols[ j0 ] = npr3[ j0 ] | symbol[ i ] << 1; //interleave and add sync vector
  }
}

#endif        // WSPR_ENCODE_H

