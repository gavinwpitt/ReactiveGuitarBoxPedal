#include "LightControls.h"

// Global variables for storing current light
float currentMillisFadeTime;
float previousMillisFadeTime;
float currentMillisTime;
float previousMillisTime;
int redValue;
int greenValue;
int blueValue;  
int redTargetValue;
int greenTargetValue;
int blueTargetValue;


RGB colors[12];  // 12 colors, 12 notes

void setupColors(){
  redTargetValue = 0; greenTargetValue = 0; blueTargetValue = 0;
  redValue = 0; greenValue = 0; blueValue = 0;
  currentMillisFadeTime = 0; previousMillisFadeTime = 0;
  currentMillisTime = 0; previousMillisTime = 0;
  colors[0] =  { 
    2, 0, 0       }; 
  colors[1] =  { 
    2, 1, 0       }; 
  colors[2] =  { 
    2, 2, 0       }; 
  colors[3] =  { 
    1, 2, 0       }; 
  colors[4] =  { 
    0, 2, 0       }; 
  colors[5] =  { 
    0, 2, 1       }; 
  colors[6] =  { 
    0, 2, 2       }; 
  colors[7] =  { 
    0, 1, 2       }; 
  colors[8] =  { 
    0, 0, 2       }; 
  colors[9] =  { 
    1, 0, 2       }; 
  colors[10] = { 
    2, 0, 2       }; 
  colors[11] = { 
    2, 0, 1       }; 
}

void dimLights(){
  currentMillisFadeTime = millis();
  if(abs(previousMillisFadeTime - currentMillisFadeTime) > FADETIME){
    previousMillisFadeTime = currentMillisFadeTime;
    if(redValue > 0)
      redValue -= redTargetValue/FADETIME;
    if(greenValue > 0)
      greenValue -= greenTargetValue/FADETIME;
    if(blueValue > 0)
      blueValue -= blueTargetValue/FADETIME;
    
    if(redValue < 0)
      redValue = 0;
    if(greenValue < 0)
      greenValue = 0;
    if(blueValue < 0)
      blueValue = 0;
    writeColor();
  }
}

void writeColor(){
  analogWrite(REDPIN, redValue);
  analogWrite(GREENPIN, greenValue);
  analogWrite(BLUEPIN, blueValue);
} 

void changeTargetValuesDiscrete(int note, int volume){
  redTargetValue = colors[note].red * volume;
  greenTargetValue = colors[note].green * volume;
  blueTargetValue = colors[note].blue * volume;
  currentMillisTime = millis();
  if(abs(previousMillisTime - currentMillisTime) > FADEINRATE){
    previousMillisTime = currentMillisTime;
    if(redTargetValue > redValue){
      redValue++;
    }else if(redTargetValue < redValue){
      redValue--;
    }else{}
    
    if(greenTargetValue > greenValue){
      greenValue++;
    }else if(greenTargetValue < greenValue){
      greenValue--;
    }else{}
    
    if(blueTargetValue > blueValue){
      blueValue++;
    }else if(blueTargetValue < blueValue){
      blueValue--;
    }else{}
    
    
  }
  writeColor();
}

void changeTargetValues(int red, int green, int blue){
  redTargetValue = red;
  greenTargetValue = green;
  blueTargetValue = blue;
  changeColorValues(red, green, blue);
  writeColor();
}

void changeColorValues(int red, int green, int blue){
  currentMillisTime = millis();
  if(abs(previousMillisTime - currentMillisTime) > FADEINRATE ){
    previousMillisTime = currentMillisTime;
    if(redTargetValue > redValue){
      redValue++;
    }else if(redTargetValue < redValue){
      redValue--;
    }else{}
    
    if(greenTargetValue > greenValue){
      greenValue++;
    }else if(greenTargetValue < greenValue){
      greenValue--;
    }else{}
    
    if(blueTargetValue > blueValue){
      blueValue++;
    }else if(blueTargetValue < blueValue){
      blueValue--;
    }else{}
    
    
  }
  writeColor();
}

