#ifndef NoteHandling.h
#define NoteHandling.h

void setupNoteHandling();

void changeLightsByNote(int period);

void changeLightsByNoise(int noteDown);

//Compare the frequency input to the correct 
//frequency and light up the appropriate LEDS
void updateNote(float frequency);

void changeLightsByNoteSpectrum(int period);

void updateNoteSpectrum(int percentage);

void adjustSpectrumBounds(int lowerBoundInput, int upperBoundInput);

#endif
