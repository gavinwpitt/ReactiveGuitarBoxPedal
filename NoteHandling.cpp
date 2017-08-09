#include "NoteHandling.h"
#include "LightControls.h"

int selectedNote;
int upperBound;
int lowerBound;

void setupNoteHandling(){
  selectedNote = 0;
}

void changeLightsByNote(int period){
  // 'Note' Lights
  //calculate frequency timer rate/period, update frequency only if it's between bounds of E1 and E4 (for bass)
  if(38462/float(period) > 39 && 38462/float(period) < 6000){
    float frequency = 38462/float(period);
    updateNote(frequency);
  }
}

void changeLightsByNoteSpectrum(int period){
  // 'Note' Lights
  //calculate frequency timer rate/period, update frequency only if it's between bounds of E1 and E4 (for bass)
  if(38462/float(period) > 39 && 38462/float(period) < 6000){
    float frequency = 38462/float(period);
    Serial.println("Frequency");
    Serial.print(frequency);
    Serial.println("");
    float spectrumPercentFloat = 762 * ( (frequency - (float)lowerBound ) / (float)(upperBound - lowerBound) );
    updateNoteSpectrum((int)spectrumPercentFloat);
  } 
}

void updateNoteSpectrum(int spectrumPercent){
  int red, green, blue;
  
  if(spectrumPercent > 762){
    spectrumPercent = 762;
  }
  if(spectrumPercent < 0){
    spectrumPercent = 0;
  }
  
  Serial.println("");
  Serial.print("Spectrum Percent: ");
  Serial.println(spectrumPercent);
  if(spectrumPercent < 254 && spectrumPercent >= 0){
     red = 254 - spectrumPercent;
     green = 0;
     blue = spectrumPercent;
   }else if(spectrumPercent >= 254 && spectrumPercent < 508){
     red = 0;
     green = spectrumPercent % 254;
     blue = 254 - (spectrumPercent % 254);
   }else if(spectrumPercent >= 508 && spectrumPercent < 635){
     red = (spectrumPercent % 254);
     green = 254 - (spectrumPercent % 254);
     blue = (spectrumPercent % 254)/2;
   }else if(spectrumPercent >= 635 && spectrumPercent < 763){
     red = (spectrumPercent % 254);
     green = (spectrumPercent % 254);
     blue = (spectrumPercent % 254); 
   }else{
   }
   
   changeTargetValues(red, green, blue);
}

void adjustSpectrumBounds(int lowerBoundInput, int upperBoundInput){
   upperBound = upperBoundInput;
   lowerBound = lowerBoundInput;
}

void changeLightsByNoise(int noteDown){
  // If no note is down (noteDown == 1), change selectedNote and do nothing?
  if(noteDown == 0){
    if(selectedNote >= 11){
      selectedNote = 0;
    }else{
      selectedNote ++;
    }
  }else{
    changeTargetValuesDiscrete(selectedNote, 1);
  }
}

//Compare the frequency input to the correct 
//frequency and light up the appropriate LEDS
void updateNote(float frequency){
  int octave = 1;
  int note;
  int reducingFreq = frequency;
  while(reducingFreq > 30){
    // C - 32.70
    if(reducingFreq >= 31.78 && reducingFreq < 33.67){
      note = 0;
      break;
    }
    // C# - 34.64
    if(reducingFreq >= 33.67 && reducingFreq < 35.67){
      note = 1;
      break;
    }
    // D - 36.70
    if(reducingFreq >= 35.67 && reducingFreq < 37.79){
      note = 2;
      break;
    }
    // D#
    if(reducingFreq >= 37.79 && reducingFreq < 40.04){
      note = 3;
      break;
    }
    // E
    if(reducingFreq >= 40.04 && reducingFreq < 42.42){
      note = 4;
      break;
    }
    // F
    if(reducingFreq >= 42.42 && reducingFreq < 44.95){
      note = 5;
      break;
    }
    // F#
    if(reducingFreq >= 44.95 && reducingFreq < 47.62){
      note = 6;
      break;
    }
    // G
    if(reducingFreq >= 47.62 && reducingFreq < 50.45){
      note = 7;
      break;
    }
    
    if(reducingFreq >= 50.45 && reducingFreq < 53.45){
      note = 8;
      break;
    }
    if(reducingFreq >= 53.45 && reducingFreq < 56.63){
      note = 9;
      break;
    }
    if(reducingFreq >= 56.63 && reducingFreq < 60){
      note = 10;
      break;
    }
    if(reducingFreq >= 60 && reducingFreq < 63.36){
      note = 11;
      break;
    }
    reducingFreq = reducingFreq / 2.0;
    octave++;
  }
  changeTargetValuesDiscrete(note, 1);
}
