// FOR ARDUINO DUE 
// ChanghyeonKim, Seoul National University
// initial release    : 2017-06-20
// final modification : 2017-07-02
// Email : rlackd93@snu.ac.kr

#include "fsrSound.h"
#include <Wire.h>

// initial settings

// 1. Stimulus Mode Selection - SOUND / LED / SOUND + LED
const int signal_LED    = 1; // if 1 , LED on 
const int signal_SOUND  = 1; // if 1 , Sound on. If you set both values with 1, SOUND and LED burst simultaneously .  
const int whiteNoise_on = 1; // if 1 , white noise on
const int fsrOrButton   = 0; // if 1 , fsr on / 0 : button on 

// 2. trials and signal settings
#define expArrSz 24 // The number of stimulations in one trial. Every trial should have the same number of stimulations.
#define expNum 4 // The number of trials. 
// You can write stimulus up to 700 in one trial.
const int sign_Freq[][expArrSz]     = {{1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000},
                                       {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000},
                                       {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000},
                                       {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000}};// unit : [ms] 
                                   
const int sign_Duration[]           = {5,10,15,20}; // unit : [ms], 
                                   
const int sign_Interval[][expArrSz] = {{400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400},
                                       {600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600},
                                       {400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400},
                                       {800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800}};// unit : [ms]  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Main body of the code ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////// You NEED NOT change from here ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pin declaration
#define audioPin  5  // audio out pin     (digitalWrite pin)
#define ledPin    4   // LED out pin       (digitalWrite pin)
#define noisePin  7  // white noise pin   (digitalWrite pin)
#define switchPin 2 // for trial change. (digitalInput pin)
#define buttonPin 10

int k=0;
unsigned long t_now;
unsigned long t_ref ; 
float t_gap;
int seq_num = -1;
int buttonOn=0;
int doLoop = 0;

fsrSound *myfsrsound; // create a class object

// TC1 & channel 0 

void setup() {
  Serial.begin(460800); // serial communication with PC by 500000 baudrate
  myfsrsound = new fsrSound(); // fsr with pulldown resistor 10k Ohm
  analogReadResolution(12);
  pinMode(audioPin,OUTPUT); // for AUDIO output   
  pinMode(switchPin,INPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(noisePin,OUTPUT);
  pinMode(buttonPin,INPUT);
  t_ref=micros(); // initialize the reference time
}

void loop() {
  if(doLoop==1){
    for(int k = 0;k<expArrSz;k++){
      int steps=0;
      int toneOn = 0;
      while(steps==0){ // signal on
        whiteNoise(noisePin,whiteNoise_on);
        digitalWrite( noisePin, random(2) ); // random white noise generation
        t_now = micros(); // get current time in microseconds
        t_gap = (float)(t_now - t_ref)/1000.0; // time gap from the reference time 
        
        if(t_gap < sign_Duration[seq_num] && myfsrsound->signOn == 1){
          if(signal_LED==1) digitalWrite(ledPin,HIGH);
          if(toneOn == 0 && signal_SOUND==1){
             digitalWrite(audioPin,HIGH); 
             dueTone(audioPin, sign_Freq[seq_num][k],sign_Duration[seq_num]);
             toneOn = 1; 
          }
        }
        else if(t_gap >= sign_Duration[seq_num] && myfsrsound->signOn==1){
          whiteNoise(noisePin,whiteNoise_on);
          myfsrsound->signOn=0;
          t_ref = t_now; 
          toneOn=0;
          steps=1;
        }
        myfsrsound->getFsr(fsrOrButton);
        myfsrsound->getTime();
        myfsrsound->getFreq(sign_Freq[seq_num][k]);
        myfsrsound->sendData();
      }
      while(steps == 1){ // signal off 
        whiteNoise(noisePin,whiteNoise_on);
        t_now = micros();// get current time in microseconds
        t_gap = (float)(t_now - t_ref)/1000.0;// time gap from the reference time 
        
        if(t_gap <sign_Interval[seq_num][k]-sign_Duration[seq_num] && myfsrsound->signOn == 0){
          if(signal_LED==1) digitalWrite(ledPin,LOW); // signal off 
        }
        else if(t_gap>=sign_Interval[seq_num][k]-sign_Duration[seq_num] && myfsrsound->signOn == 0){
          myfsrsound->signOn=1;
          t_ref = t_now;
          steps=0;
        }
        myfsrsound->getFsr(fsrOrButton);
        myfsrsound->getTime();
        myfsrsound->getFreq(sign_Freq[seq_num][k]);
        myfsrsound->sendData();
      }
    }
    // finish alarm. 
    delay(1500);
    dueTone(audioPin,523,100);// C, 1000 [ms]
    delay(100);
    dueTone(audioPin,587,100);// D, 1000 [ms]
    delay(100);
    dueTone(audioPin,659,100);// E, 1000 [ms]
    delay(100);
    dueTone(audioPin,698,100);// F, 1000 [ms]
    delay(100);
    dueTone(audioPin,783,100);// G, 1000 [ms]

    doLoop=0; // A trial is done. click the "BUTTON" if you want to do next trial.
  }
  
  if(doLoop==0){ // below part is only for the Serial communication with PC.
    buttonOn=digitalRead(switchPin); // Trial change signal
    Serial.print('N');
    if(buttonOn==1){
      for(int k=0;k<150;k++){
        Serial.print('Y');
      }
      delay(2000);

      doLoop=1;
      seq_num++;
      if(seq_num>expNum-1){
        while(1){
            Serial.print("UUU"); // All experiments is ended ! ( send 'U' to the PC.)
            delay(100);
          }
        }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// for tone (Arduino due) /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


#define TONE_TIMER TC1
#define TONE_CHNL 0
#define TONE_IRQ TC3_IRQn

// TIMER_CLOCK4   84MHz/128 with 16 bit counter give 10 Hz to 656KHz

// sound from 27Hz to 4KHz

static uint8_t pinEnabled[PINS_COUNT];
static uint8_t TCChanEnabled = 0;
static boolean pin_state = true ;
static Tc *chTC = TC1;
static uint32_t chNo = 0;

volatile static int32_t tick_count;
static uint32_t tone_pin;

void whiteNoise(uint32_t _pin, int onOff){
  if(onOff == 1) digitalWrite(_pin,random(2));
}

void dueTone(uint32_t _pin, uint32_t freq, int32_t duration) {
    const uint32_t rc = VARIANT_MCK / 128 / freq; 
    tone_pin = _pin;
    tick_count = 0;  // strange wipe out previous duration
    
    if (duration > 0 ) tick_count = 2 * freq * duration / 1000 ;

    else tick_count = -1;
    
    pmc_set_writeprotect(false); // enable writting 
    pmc_enable_periph_clk( (uint32_t) TONE_IRQ); // 
    TC_Configure(chTC, chNo, TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC ); // 84MHz/128 (clock 4). if the counter reaches the RC, initialize the counter.  
    
    TC_SetRA(chTC, chNo, rc/2); // 50% duty cycle
    TC_SetRC(chTC, chNo, rc);   // 
    TC_Start(chTC, chNo);
    
    chTC->TC_CHANNEL[chNo].TC_IER = TC_IER_CPCS|TC_IER_CPAS;  // RC compare interrupt
    chTC->TC_CHANNEL[chNo].TC_IDR = ~(TC_IER_CPCS|TC_IER_CPAS);
    NVIC_EnableIRQ(TONE_IRQ);
    
}

void noTone(uint32_t _pin){
  TC_Stop(chTC, chNo);  // stop timer
  digitalWrite(_pin, LOW);  // no signal on pin
}

void TC3_Handler ( void ){// timer ISR  TC 1 ch 0
  TC_GetStatus(TC1, 0);
  if (tick_count != 0){
    digitalWrite(audioPin, pin_state = !pin_state);
    if (tick_count > 0) tick_count--;
  } else {
    noTone(audioPin);
  }
}



