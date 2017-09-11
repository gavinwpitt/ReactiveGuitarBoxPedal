/*
 * ReactiveGuitarBoxPedal
 * by Gavin Pitt
 *
 * This code interprets an electric signal coming in via guitar/bass guitar.
 * The pedal lights up depending on the noise coming from the guitar signal.
 *
 * There is some code to add fuctionality for pitch control, but I cannot currently
 * get it to work as smoothly as I'd like. Instead it detects only incoming noise.
 *
 * Based upon:
 * Arduino Frequency Detection
 * created October 7, 2012
 * http://www.instructables.com/id/Arduino-Frequency-Detection/
 * by Amanda Ghassaei
 *
*/

#include "LightControls.h"
#include "NoteHandling.h"

#define NOTEMODE 8
#define NOISEMODE 9
#define SPECTRUMNOTEMODE 10
#define SPECTRUMNOISEMODE 11

//data storage variables
int switchMode;
byte newData = 0;
byte prevData = 0;
int clipping;
unsigned int time = 0;//keeps time and sends vales to store in timer[] occasionally
int timer[10];//storage for timing of events
int slope[10];//storage for slope of events
unsigned int totalTimer;//used to calculate period
unsigned int period;//storage for period of wave
byte index = 0;//current storage index
int maxSlope = 0;//used to calculate max slope as trigger point
int newSlope;//storage for incoming slope data
int sustainCheck;
int noteDown;
int sustainTime;
int volumeMax;
int volumeCheckRate;
int volumeCheckCount;
int repeatFlat;

int crawlingTestNumber;

//variables for deciding whether you have a match
byte noMatch = 0;//counts how many non-matches you've received to reset variables if it's been too long
byte slopeTol = 3;//slope tolerance- adjust this if you need
int timerTol = 8;//timer tolerance- adjust this if you need

//variables for amp detection
unsigned int ampTimer = 0;
byte maxAmp = 0;
byte checkMaxAmp;
byte ampThreshold = 35;//raise if you have a very noisy signal

int value;

//variables for tuning
int correctFrequency;//the correct frequency for the string being played

void setup(){
  Serial.begin(9600);
  
  //LED pins
  pinMode(REDPIN,OUTPUT);
  pinMode(GREENPIN,OUTPUT);
  pinMode(BLUEPIN,OUTPUT);
  pinMode(13, OUTPUT);
  // Mode Reading Pin
  pinMode(NOTEMODE, INPUT);
  pinMode(NOISEMODE, INPUT);
  pinMode(SPECTRUMNOTEMODE, INPUT);
  pinMode(SPECTRUMNOISEMODE, INPUT);
  
  pinMode(A1, INPUT);
  
  //Beginning LED sequence
  digitalWrite(REDPIN, 255);
  delay(500);
  digitalWrite(GREENPIN, 255);
  delay(500);
  digitalWrite(BLUEPIN, 255);
  delay(500);
  digitalWrite(REDPIN, 0);
  digitalWrite(GREENPIN, 0);
  digitalWrite(BLUEPIN, 0);
  setupColors();
  clipping = 0;
  switchMode = 1;
  repeatFlat = 0;
  noteDown = 0;
  sustainTime = 600;
  volumeCheckRate = 300;
  volumeCheckCount = 0;
  adjustSpectrumBounds(80, 600);
  cli();//disable interrupts
  
  //set up continuous sampling of analog pin 0 at 38.5kHz
 
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //e 
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts
}
int newData1;

ISR(ADC_vect) {//when new ADC value ready
  PORTB &= B11101111;//set pin 12 low
  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (newData == 0 || newData == 255){//if clipping
    PORTB |= B00100000;//set pin 13 high- turn on clipping indicator led
    clipping = 1;
  }else{
    clipping = 0;
  }
  //printmessage();
  //Serial.println("INTERUPT");

  
  if(switchMode == 0 || switchMode == 2){
    if (prevData < 127 && newData >=127){//if increasing and crossing midpoint
      newSlope = newData - prevData;//calculate slope
      if (abs(newSlope-maxSlope)<slopeTol){//if slopes are ==
        //record new data and reset time
        slope[index] = newSlope;
        timer[index] = time;
        time = 0;
        if (index == 0){//new max slope just reset
          PORTB |= B00010000;//set pin 12 high
          noMatch = 0;
          index++;//increment index
        }
        else if (abs(timer[0]-timer[index])<timerTol && abs(slope[0]-newSlope)<slopeTol){//if timer duration and slopes match
          //sum timer values
          totalTimer = 0;
          for (byte i=0;i<index;i++){
            totalTimer+=timer[i];
          }
          period = totalTimer;//set period
          //reset new zero index values to compare with
          timer[0] = timer[index];
          slope[0] = slope[index];
          index = 1;//set index to 1
          PORTB |= B00010000;//set pin 12 high
          noMatch = 0;
        }
        else{//crossing midpoint but not match
          index++;//increment index
          if (index > 9){
            reset();
          }
        }
      }
      else if (newSlope>maxSlope){//if new slope is much larger than max slope
        maxSlope = newSlope;
        time = 0;//reset clock
        noMatch = 0;
        index = 0;//reset index
      }
      else{//slope not steep enough
        noMatch++;//increment no match counter
        if (noMatch>9){
          reset();
        }
      }
    }    
    time++;//increment timer at rate of 38.5kHz
    ampTimer++;//increment amplitude timer
    if(abs(127-ADCH)>maxAmp){
      maxAmp = abs(127-ADCH);
    }
    if(ampTimer==1000){
      ampTimer = 0;
      checkMaxAmp = maxAmp;
      maxAmp = 0;
    }

  }else if(switchMode == 1){
    // check if note is down, but this is handled at top of interupt function
    if(abs(127-ADCH)>ampThreshold){
      if(volumeCheckCount > 0){
        if(abs(127-newData) > volumeMax){
          volumeMax = abs(127-newData);
        }
        volumeCheckCount--;
      }else{
        changeLightsByNoise(noteDown, volumeMax);
        volumeMax = 0;
        volumeCheckCount = volumeCheckRate;
      }
      sustainCheck = sustainTime;
      noteDown = 1;
    }else{
      if(sustainCheck > 0){
        sustainCheck--;
      }else{
        noteDown = 0;
        volumeMax = 0;
      }
      changeLightsByNoise(noteDown, volumeMax);
    }
    
  }else{
  
  }

}

void reset(){//clean out some variables
  index = 0;//reset index
  noMatch = 0;//reset match couner
  maxSlope = 0;//reset slope
}

void checkClipping(){
  if(clipping == 1){
    digitalWrite(13, HIGH);
  }else{
    digitalWrite(13, LOW);
  }
}

void checkMode(){
  if( digitalRead(NOTEMODE) ){
    switchMode = 0;
  }else if( digitalRead(NOISEMODE)){
    switchMode = 1; 
  }else if(digitalRead(SPECTRUMNOTEMODE)){
    switchMode = 2;
  }else if(digitalRead(SPECTRUMNOISEMODE)){
    switchMode = 3;
  }else{
    
  }
}

/* RCtime
 *   Duplicates the functionality of the Basic Stamp's RCtime
 *   Allows digital pins to be used to read resistive analog sensors
 *   One advantage of this technique is that is can be used to read very wide ranging inputs.
 *   (The equivalent of 16 or 18 bit A/D)
 */

int sensorPin = 7;              // 220 or 1k resistor connected to this pin
long result = 0;

long RCtime(int sensPin){
   long result = 0;
   pinMode(sensPin, OUTPUT);       // make pin OUTPUT
   digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
   delay(1);                       // wait a  ms to make sure cap is discharged

   pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
   digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
   while(digitalRead(sensPin)){    // wait for pin to go low
      result++;
   }

   return result;                   // report results   
}

void loop(){
  checkClipping();
  checkMode();
  //sustainTime = RCtime(sensorPin);
  Serial.println( ADCH );
  //value = analogRead(A1);
  if(maxAmp > ampThreshold || noteDown){
    if(switchMode == 0){
      changeLightsByNote(period);
    }else if(switchMode == 1){
      //changeLightsByNoise(noteDown, volume);
    }else if(switchMode == 2){
      changeLightsByNoteSpectrum(period);
    }else{
    
    }
  }else{
    dimLights();
  }
}
