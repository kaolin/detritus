#ifndef ERIF_SOUNDLIB
#define ERIF_SOUNDLIB

#include "os.h" // WINDOWS or LINUX or OSX

int soundlib_init(int soundn, const char** sounds);

void soundlib_start(void);  // toggle sound in general on
void soundlib_stop(void);   // toggle sound in general off
void soundlib_toggle(void); // toggle sound in general on or off

void soundlib_updateListener(const float* position, const float* velocity, const float* orientation);
	
void startSound(int i); // toggle sound "i" on
void startSound(int i,const float src[]); // toggle sound "i" on at point src
void moveSound(int i,const float src[]); // move sound "i" to point src
void stopSound(int i);  // toggle sound "i" off
void pauseSound(int i);  // pause sound "i"
void rewindSound(int i);  // rewind sound "i"
void resumeSound(int i); //resume sound "i" ... don't need to worry about where it was

//TODO: more options for sounds; setting positions, moving positions, setting velocities, moving velocities, loop options...

#endif
