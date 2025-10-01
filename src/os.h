#ifndef __OS_H
#define __OS_H
#include <iostream>
#include <fstream>

//#define DEBUG_MYERROR
std::ostream * getErrorStream();

#define GAME_GLTEXPARAM GL_LINEAR

#ifndef WINDOWS
#ifndef OSX
#ifndef LINUX
#error ERROR - #define either WINDOWS, OSX, or LINUX
#endif
#endif
#endif

#include <math.h>

#ifndef PI
#define PI 3.14159
#endif

#ifdef OSX
// #define DATAPREFIX "Contents/Resources/"
#include "CoreFoundation/CoreFoundation.h"
#include <ApplicationServices/ApplicationServices.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#endif

#ifdef LINUX

#endif

// SDL includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#ifdef OSX
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <OpenGL/glu.h>
#include <al.h>
#include <alc.h>
//#include <MacOSX_OALExtensions.h>
#include <oalMacOSX_OALExtensions.h>
//#include "../osx/altypes.h"
#include "../osx/alut.h"
#endif

#ifdef WINDOWS
#include <SDL/SDL_main.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <AL/alut.h>
#endif
/* end WINDOWS */

#ifdef LINUX
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <AL/alut.h>

#ifndef AL_INVALID_ENUM
#define AL_INVALID_ENUM AL_ILLEGAL_ENUM
#endif

#ifndef AL_INVALID_OPERATION
#define AL_INVALID_OPERATION AL_ILLEGAL_COMMAND
#endif

#endif
/* end LINUX */

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef boolean
#define boolean int
#endif

//#ifndef uint
//#define uint unsigned int
//#endif
typedef unsigned int uint;

#include "glsdl.h"

//TODO: found sample code that gives this in SDL
boolean is_intel_byte_order();
const char* getApplicationDataPath(void);

// simplecode thanks to <lj user="stay"> http://livejournal.com/users/stay
// Use these to return swapped bytes as result of expression
#define SWAP_BYTES_16(val) (unsigned short)((((val) & 0xFF) << 8) | (((val) & 0xFF00) >> 8))
#define SWAP_BYTES_32(val) ((SWAP_BYTES_16((val) & 0xFFFF) << 16) | (SWAP_BYTES_16(((val) & 0xFFFF0000) >> 16)))
// use these to swap a value in place, as if it were a function call:
// SWAP_BYTES_16P(&myShortValue);
#define SWAP_BYTES_16_Ptr(ptr) *(ptr) = SWAP_BYTES_16(*(ptr))
//#define SWAP_BYTES_32_Ptr(ptr) *(ptr) = SWAP_BYTES_32(*(ptr))

// Or, if you prefer, swap in place as if it's a function call with a call by ref parameter#define SWAP_BYTES_16_Val(name) (name) = SWAP_BYTES_16(name)
#define SWAP_BYTES_32_Val(name) (name) = SWAP_BYTES_32(name)

#define SWAP_BYTES_32_Ptr(ptr) *(unsigned long *)(ptr) = SWAP_BYTES_32(*(unsigned long *)(ptr))


#define NUM_BUFFERS 10
#define NUM_SOURCES NUM_BUFFERS
#define NUM_EXPLOSIONS 10

#ifndef DATAPREFIX
#define DATAPREFIX ""
#endif

#endif
