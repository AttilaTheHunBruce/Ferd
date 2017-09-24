#ifndef  BALLOON_H
#define  BALLOON_H


//************************************************************//
#define DBM                     "20"
#define IDENTIFIER              '7'               // 3rd char in telemetry word - make more unique
#define FLASH_FAST              400
#define FLASH_SLOW              1000
#define GPS_POWER               9
#define ID                      56
#define INT0_PIN                2
#define ONE_SECOND_MS           1000
#define PIN_POWER               A3
#define PIN_VOLTAGE             A1
#define PIN_TEMPERATURE         A0
#define TIME_OFFSET             1                 // seconds offset
#define TWO_MINUTES             (2L*60L)
#define TWO_MINUTES_MS          (TWO_MINUTES*1000L)
#define FREQUENCY_BASE          1000000000        // oscillator base for frequency counter
#define WSPR_SYMBOLS            162
#define WSPR_TIME               ((8192000L+6000L)/12000L)

#define CLK_ENABLE_CONTROL      3
#define CLK0_CONTROL            16
#define CLK1_CONTROL            17
#define CLK2_CONTROL            18
#define Si5351A_addr            0x60
#define SYNTH_PLL_A             26
#define SYNTH_PLL_B             34
#define SYNTH_MS_0              42
#define SYNTH_MS_1              50
#define SYNTH_MS_2              58
#define TIMER0_COUNT            2000              // 2000 msec timer interval


//************************************************************//
#include <Arduino.h>
#include <WString.h>
#include <SoftwareSerial.h>
#include <SI5351_WSPR.h>
#include <ctype.h>

//************************************************************//
SoftwareSerial mySerial( 10, 11 );   // RX, TX
SI5351_WSPR wspr = SI5351_WSPR();

// keying rate = 12000 / 8192 = 1.4648 buad
// continuous phase 4-FSK, tone separation = 1.4648 Hz
// occupied bandwidth ~ 6 Hz (5.8594 Hz)
// number of binary channel symbols = (50+K-1)*2 = 162
// duration of transmission = 162 * 8192 / 12000 = 110.6 sec
// so each symbol duration = 8192 / 12000 = 0.6827 sec


//************************************************************//
void readGPS( char );
void timeRequest( long );
void setToTwoMinuteMark();

const int bufferSize = 80;
const float METERS_TO_FEET = 3.280839895;


float altitude;
char buffer[ bufferSize ];
char grid[ 5 ];
char gridEx[ 3 ];
float latitude;
float longitude;
bool measurePowerFlag = false;
int minutes = 0;
int numberOfSatellites;
char secondCall[ 7 ];
char secondDbm[ 3 ];
char secondGrid[ 5 ];
int seconds = 0;
uint8_t secondSymbols[ WSPR_SYMBOLS ];
bool storing = false;
uint8_t symbols[ WSPR_SYMBOLS ];
float temperature;
int timer1Count = 0;
char timeStuff[ 80 ];
int transmitPower = 0;
int transmitPowerDBm = 0;
float velocity;
float voltage;


//************************************************************//
enum States
{ StartGPS, WaitingForSatelliteLock, WaitFor2Minutes, Transmission1Setup,
  Transmission1, Transmission1Post, Transmission2Setup, Transmission2,
  Transmission2Post, Transmission3Setup, Transmission3, Transmission3Post,
  Transmission4Setup, Transmission4, Transmission4Post, None
} state = StartGPS;


//************************************************************//
volatile bool gpsFlag = false;
volatile bool limitFlag = false;
volatile long msClock = 0L;
volatile bool msFlag = false;
volatile long msLimit = 0;
volatile bool startTimeSync = false;
volatile bool syncDone = false;


//************************************************************//
#include "wsprEncode.h"
#include "printFunctions.h"
#include "readGPS.h"
#include "gpsTimer.h"
#include "stateMachine.h"
#include "powerOff.h"

#endif         // BALLOON_H
