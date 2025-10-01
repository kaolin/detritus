/*
 * 	•	Endianess Handling: The loadWav function assumes the WAV file is in little-endian format. If you’re working with big-endian systems, you’ll need to implement byte-swapping for multi-byte values.
	•	File Format Assumptions: This implementation assumes the WAV files are uncompressed (PCM) and have either 8 or 16 bits per sample. If your WAV files use compression or have different formats, additional parsing logic will be required.

*/
#include <iostream>
#include "soundlib.h"
#include <vector>
#include <cstring>
using namespace std;

#define MY_AL_SCALE 10.0f

static ALuint buffers[NUM_BUFFERS];
static ALuint sources[NUM_SOURCES];
static ALuint explosions[NUM_EXPLOSIONS];
ALfloat source0Pos[] = {-2.0, 0.0, 0.0};
ALfloat source0Vel[] = {0.0, 0.0, 0.0};

boolean disabled = true;

int soundlib_sound = false;

const char* alErrorString(ALenum err) {
    switch (err) {
    case AL_NO_ERROR: return "AL_NO_ERROR";
    case AL_INVALID_NAME: return "AL_INVALID_NAME";
    case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
    default: return "Unknown AL error";
    }
}

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
	int error;
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
    if ((error = alGetError()) != AL_NO_ERROR) {
        std::cerr << "error playing explosion" << std::endl;
		}

                        nextexplosion++;
                        if (nextexplosion >= NUM_EXPLOSIONS) nextexplosion=0;
                } else {
    if ((error = alGetError()) != AL_NO_ERROR) {
        std::cerr << "error playing " << i << std::endl;
		}
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


bool loadWav(const char* filename, ALuint buffer) {
    // Open the WAV file
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Read the WAV header
    char header[44];
    file.read(header, 44);
    if (file.gcount() < 44) {
        std::cerr << "Invalid WAV file: " << filename << std::endl;
        return false;
    }

    // Extract format information
    ALenum format;
    ALsizei freq;
    short channels = *reinterpret_cast<short*>(&header[22]);
    short bitsPerSample = *reinterpret_cast<short*>(&header[34]);
    if (channels == 1) {
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else if (channels == 2) {
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    } else {
        std::cerr << "Unsupported channel count: " << channels << std::endl;
        return false;
    }
    freq = *reinterpret_cast<int*>(&header[24]);

    // Read the audio data
    file.seekg(44, std::ios::beg);
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (data.empty()) {
        std::cerr << "No audio data found in file: " << filename << std::endl;
        return false;
    }

    // Load data into OpenAL buffer
    alBufferData(buffer, format, data.data(), static_cast<ALsizei>(data.size()), freq);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "Failed to load WAV data into buffer: " << filename << std::endl;
        return false;
    }

    return true;
}

int soundlib_init([[maybe_unused]] int soundn, const char** sounds) {
    int error, i;

    // Open default device
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device." << std::endl;
        return 0;
    }

    // Create context
    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create audio context." << std::endl;
        alcCloseDevice(device);
        return 0;
    }

    // Make context current
    alcMakeContextCurrent(context);

    // Generate buffers
    alGenBuffers(NUM_BUFFERS, buffers);
    if ((error = alGetError()) != AL_NO_ERROR) {
        std::cerr << "Failed to generate buffers." << std::endl;
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 0;
    }

    // Generate sources
    alGenSources(NUM_SOURCES, sources);
    alGenSources(NUM_EXPLOSIONS, explosions);
    if ((error = alGetError()) != AL_NO_ERROR) {
        std::cerr << "Failed to generate sources." << std::endl;
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 0;
    }

    // Load WAV files into buffers
    for (i = 0; i < NUM_BUFFERS; ++i) {
        if (!loadWav(sounds[i], buffers[i])) {
            std::cerr << "Error loading WAV file: " << sounds[i] << std::endl;
            alcDestroyContext(context);
            alcCloseDevice(device);
            return 0;
        }
        alSourcef(sources[i], AL_PITCH, 1.0f);
        alSourcef(sources[i], AL_GAIN, 1.0f);
        alSourcefv(sources[i], AL_POSITION, source0Pos);
        alSourcefv(sources[i], AL_VELOCITY, source0Vel);
        alSourcei(sources[i], AL_BUFFER, buffers[i]);
        alSourcei(sources[i], AL_LOOPING, AL_FALSE);
    }

    // Additional setup for explosions
    for (i = 0; i < NUM_EXPLOSIONS; ++i) {
        alSourcef(explosions[i], AL_PITCH, 1.0f);
        alSourcefv(explosions[i], AL_POSITION, source0Pos);
        alSourcefv(explosions[i], AL_VELOCITY, source0Vel);
        alSourcei(explosions[i], AL_BUFFER, buffers[0]);
        alSourcei(explosions[i], AL_LOOPING, AL_FALSE);
    }

    // Override defaults
    alSourcei(sources[6], AL_LOOPING, AL_TRUE); // enemy loops
    alSourcei(sources[7], AL_LOOPING, AL_TRUE); // thrust loops

		// WTF WTF WTF!?
    //alcDestroyContext(context);
    //alcCloseDevice(device);

		disabled = false;

		cout << "INIT SUCCEEDED" << endl;

    return 1;
}
