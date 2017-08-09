#ifndef LightControls.h
#define LightControls.h

// Constants for Lights
#define INTENSITY 127
#define DECAYRATE 20
#define FADEINRATE 2
#define REDPIN 5
#define GREENPIN 3
#define BLUEPIN 6

#include "Arduino.h"


struct RGB{ // data structure for colors
  int red;
  int green;
  int blue;
};

void setupColors();

void dimLights();

void writeColor();

void changeTargetValuesDiscrete(int note, int octave);

void changeTargetValues(int red, int green, int blue);

void changeColorValues(int red, int green, int blue);

#endif
