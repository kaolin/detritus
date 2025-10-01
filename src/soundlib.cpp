#include "soundlib.h"
#include <iostream>
using namespace std;

#define MY_AL_SCALE 10.0f

static ALuint buffers[NUM_BUFFERS];
static ALuint sources[NUM_SOURCES];
static ALuint explosions[NUM_EXPLOSIONS];
ALfloat source0Pos[]={ -2.0, 0.0, 0.0};
ALfloat source0Vel[]={ 0.0, 0.0, 0.0};

boolean disabled = true;

int soundlib_sound = false;

void soundlib_updateListener(const float* p, const float* v, const float* o) {
	ALfloat position[3], velocity[3], orientation[6];
	for (int i=0;i<3;i++) {
		position[i]=p[i]/MY_AL_SCALE;
		velocity[i]=v[i]/MY_AL_SCALE;
	}
	for (int i=0;i<6;i++) {
		orientation[i]=o[i]/MY_AL_SCALE;
	}
   	alListenerfv(AL_POSITION,position);
   	alListenerfv(AL_VELOCITY,velocity);
   	alListenerfv(AL_ORIENTATION,orientation);
}

void soundlib_start(void) { if (!disabled) soundlib_sound = true; }
void soundlib_stop(void) { soundlib_sound = false; } //TODO: keep track of what's already playing, stop those?
void soundlib_toggle(void) { if (!disabled) soundlib_sound = !soundlib_sound; }

void resumeSound(int i) {
	if (soundlib_sound) {
		alSourcePlay(sources[i]);
	}
}

void startSound(int i,const float src[]) { 
	static int nextexplosion=0;
	ALfloat srcAL[3];
	srcAL[0] = src[0]/MY_AL_SCALE;
	srcAL[1] = src[1]/MY_AL_SCALE;
	srcAL[2] = src[2]/MY_AL_SCALE;
	if (soundlib_sound) {
		if (i==0) {
			//explosions we multi-source! :)
			alSourceStop(explosions[i]);
	    	alSourcefv(explosions[nextexplosion], AL_POSITION, srcAL);
			alSourcePlay(explosions[nextexplosion]);
			nextexplosion++;
			if (nextexplosion >= NUM_EXPLOSIONS) nextexplosion=0;
		} else {
	    	alSourcefv(sources[i], AL_POSITION, srcAL);
			alSourcePlay(sources[i]); 
		}
	}
}
void moveSound(int i,const float src[]) {
	ALfloat srcAL[3];
	srcAL[0] = src[0]/MY_AL_SCALE;
	srcAL[1] = src[1]/MY_AL_SCALE;
	srcAL[2] = src[2]/MY_AL_SCALE;
	alSourcefv(sources[i],AL_POSITION,srcAL);
}
void startSound(int i) { if (soundlib_sound) alSourcePlay(sources[i]); }
void stopSound(int i) { if (!disabled) alSourceStop(sources[i]); }
void pauseSound(int i) { if (soundlib_sound) alSourcePause(sources[i]); }
void rewindSound(int i) { if (soundlib_sound) alSourceRewind(sources[i]); }

void displayOpenALError(const char* s, ALenum e) {
  switch (e) {
    case AL_INVALID_NAME:
      cout << "AL Invalid Name: " << s << endl;
      break;
    case AL_INVALID_ENUM:
      cout << "AL Invalid Enum: " << s << endl;
      break;
    case AL_INVALID_VALUE:
      cout << "AL Invalid Value: " << s << endl;
      break;
    case AL_INVALID_OPERATION:
      cout << "AL Invalid Operation: " << s << endl;
      break;
    case AL_OUT_OF_MEMORY:
      cout << "AL Out Of Memory: " << s << endl;
      break;
    default:
      cout << "*** ERROR *** Unknown error case (" << e << ") in displayOpenALError" << endl;
      break;
  };
}

int loadWav(const char* file, int buf) {
  int      error;
  ALenum    format; 
  ALsizei    size;
  ALsizei    freq; 
	#ifndef OSX
  ALboolean  loop;
	#endif
  ALvoid*    data;

  // Load in the WAV file from disk
  #ifdef OSX
  alutLoadWAVFile((ALbyte*)file,&format,&data,&size,&freq); // os x
  #endif
  #ifdef WINDOWS
  alutLoadWAVFile(file, &format, &data, &size, &freq, &loop);  // windows
  #endif
  #ifdef LINUX
  alutLoadWAVFile((ALbyte*)file, &format, &data, &size, &freq, &loop);  // linux
  #endif
  if ((error = alGetError()) != AL_NO_ERROR) { 
    displayOpenALError("alutLoadWAVFile : ", error);  
    return 0; 
  }

  // Copy the new WAV data into the buffer
  alBufferData((ALuint)buf,format,data,size,freq); 
  if ((error = alGetError()) != AL_NO_ERROR) { 
    displayOpenALError("alBufferData :", error); 
    return 0; 
  }

  // Unload the WAV file
  alutUnloadWAV(format,data,size,freq); 
  if ((error = alGetError()) != AL_NO_ERROR) { 
    displayOpenALError("alutUnloadWAV :", error);
    return 0;
  }

  return 1;
}

int soundlib_init([[maybe_unused]] int soundn, const char** sounds) {
  int error, i;
  alutInit(0,0); // init
  alGetError(); // clear error code
  alGenBuffers(NUM_BUFFERS,buffers); // create buffers
  if ((error = alGetError()) != AL_NO_ERROR) { 
    displayOpenALError("alutUnloadWAV :", error);
    return 0;
  }
  alGenSources(NUM_SOURCES,sources); // create sources
  alGenSources(NUM_EXPLOSIONS,explosions); // more sources :) "28-32 buffers general limit?" what about sources?
  if ((error = alGetError()) != AL_NO_ERROR) { 
    displayOpenALError("alutUnloadWAV :", error);
    return 0;
  }
  
  // Load in the WAV and store it in a buffer, then make the source and link 'em
  for (i=0;i<NUM_BUFFERS;i++) {
    if (!loadWav(sounds[i], buffers[i])) {
      // Error loading in the WAV so quit
      cout << "Unable to find file: '" << sounds[i] << "'" << endl;
      alDeleteBuffers(NUM_BUFFERS, buffers); 
      return 0;
    }
    alSourcef(sources[i], AL_PITCH, 1.0f);
    alSourcef(sources[i], AL_GAIN, 1.0f);
    alSourcefv(sources[i], AL_POSITION, source0Pos);
    alSourcefv(sources[i], AL_VELOCITY, source0Vel);
    alSourcei(sources[i], AL_BUFFER,buffers[i]);
    alSourcei(sources[i], AL_LOOPING, AL_FALSE);
    //alSourcef(sources[i],AL_ROLLOFF_FACTOR,2.0f);
  }

  for (i=0;i<NUM_EXPLOSIONS;i++) {
    alSourcef(explosions[i], AL_PITCH, 1.0f);
    //alSourcef(explosions[i], AL_GAIN, .3f);
    alSourcefv(explosions[i], AL_POSITION, source0Pos);
    alSourcefv(explosions[i], AL_VELOCITY, source0Vel);
    alSourcei(explosions[i], AL_BUFFER,buffers[0]);
    alSourcei(explosions[i], AL_LOOPING, AL_FALSE);
  }
	
  //override above defaults
  alSourcei(sources[6],AL_LOOPING,AL_TRUE); // enemy loops
  alSourcei(sources[7],AL_LOOPING,AL_TRUE); // thrust loops
  //alSourcef(sources[1], AL_GAIN, .5f); // quieter bullet
  disabled = false;
	return 1; // dunno, but sounded good... didn't have anything else to do?
}
