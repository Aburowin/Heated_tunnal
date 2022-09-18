#include <PWMaa.h> //download https://code.google.com/archive/p/arduino-pwm-frequency-library/downloads
// edit ATimerDefs.h and add '(0,0,0,0,0), // TIMER1C'  after Line 107
// if applied to Arduino Mega 2560

#include <PZEM004Tv30.h> // Power meter library
#include <math.h>

//PZEM004Tv30 pzemOT1(68, 69); // Heater 2-8
//PZEM004Tv30 pzemOT8(66, 67); // Heater 2-7
//PZEM004Tv30 pzemOT13(64, 65); // Heater 2-4
//PZEM004Tv30 pzemOT14(&Serial3);   // Heater 5 S-1
//PZEM004Tv30 pzemOT15(63, 62);     // Heater 6 S-1
//PZEM004Tv30 pzemOT16(&Serial1);   // Heater 4 S
//PZEM004Tv30 pzemOT17(&Serial2);   // Heater 3 S
//PZEM004Tv30 pzemOT18(11, 13);    //  Heater 2-1
//PZEM004Tv30 pzemOT19(51, 50);     // Heater 1

//float power1;
float voltage1;
float current1;
float factor1;

//float powerx;  


// initializ the SSRs
//use pin 11 on the Mega instead, otherwise there is a frequency cap at 31 Hz
//  ledTimer0  Pins 13, 4  might influence the internal clock
int ledTimer1 = 11;  // Pins 11, 12 16-bits timer
//  ledTimer2  Pins 10, 9 8-bits timer
int8_t ledTimer3 = 5;    // Pins 5, 2, 3 16-bits timer
int8_t ledTimer4 = 6;    // Pins 6, 7, 8 16-bits timer
int8_t ledTimer5 = 44;    // Pins 44, 45, 46 16-bits timer

int32_t frequency = 1;     //PWM frequency (in Hz)
int mark = 0;
// String comdata = "";       // initialize input string
int numdata[2] = {0};
float DutyCircle;
const byte numChars = 32;
char comdata[numChars];

boolean newData = false;

void setup()
{
  //initialize all timers except for 0, to save time keeping functions
  InitTimersSafe();
  //sets the frequency for the specified pin
  bool success1 = SetPinFrequencySafe(ledTimer1, frequency);
  bool success3 = SetPinFrequencySafe(ledTimer3, frequency);
  bool success4 = SetPinFrequencySafe(ledTimer4, frequency);
  bool success5 = SetPinFrequencySafe(ledTimer5, frequency);
  //if the pin frequency was set successfully, turn Relay on
  
  // achieve setting PWM frequnecy as well using following code
  //Timer1_Initialize();
  //Timer1_SetFrequency(frequency);

  //Timer3_Initialize();
  //Timer3_SetFrequency(frequency);

  //Timer4_Initialize();
  //Timer4_SetFrequency(frequency);

  //Timer5_Initialize();
  //Timer5_SetFrequency(frequency);
  Serial.begin(115200); // set up communication

}

void loop()
{
  
  int j = 0;
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
 recvWithStartEndMarkers();
 if (newData == true){
  if (comdata[0] == 'F')
  {
    pwmWrite(6, 0); // change the duty circle
    delay(10);
    pwmWrite(5, 0); // change the duty circle
    delay(10);
    pwmWrite(2, 0); // change the duty circle
    delay(10);
    pwmWrite(7, 0); // change the duty circle
    delay(10);
    pwmWrite(8, 0); // change the duty circle
    delay(10);
    pwmWrite(44, 0); // change the duty circle
    delay(10);
    pwmWrite(46, 0); // change the duty circle
    delay(10);
    pwmWrite(12, 0); // change the duty circle
    mark = 0;
   // comdata = String("");
  }


    int condit = strlen(comdata);//.length() - 1;
    // convert input characters
    for (int i = 0; i < condit ; i++)
    {
      if (comdata[i] == ',')
      {
        j++;
      }
      else
      {
        numdata[j] = numdata[j] * 10 + (int(comdata[i]) - '0');
      }
    }
    pwmWrite(numdata[0], numdata[1]); // change the duty circle
    DutyCircle = float(numdata[1]) / 255 * 100;
    //Serial.println(DutyCircle);
    //Serial.println(numdata[1]);
    for (int i = 0; i < 2; i++)
    {
      // Serial.println(numdata[i], DEC);
      numdata[i] = 0;
    }
    // Serial.println(DutyCircle,DEC);
//     comdata = String("");
    mark = 0;
  }

     newData = false;
    }

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        delay(2);

        if (recvInProgress == true) {
            if (rc != endMarker) {
                comdata[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                comdata[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
     mark = 1;
}

//
