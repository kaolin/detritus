//
//  soundlib.m
//  FallingUp
//
//  Created by Kaolin Fire on 12/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "soundlib.h"
#import <AudioToolbox/AudioFile.h>
@implementation MySoundLib

// from the .m file:
-(id)init
{
	if (self = [super init]) {
		myALDevice = NULL;
		myALDevice = alcOpenDevice(NULL);
	
		myALContext = NULL;
		myALContext = alcCreateContext(myALDevice, NULL);
	
		alcMakeContextCurrent(myALContext);
	
		//alGenBuffers(1, &testALBuffer);
		//alGenSources(1, &testALSource);
	
		//[self loadWave:CFSTR("bass_loop_a") toBuffer:testALBuffer];
		//alSourcei(testALSource, AL_BUFFER, testALBuffer); 
		//alSourcePlay(testALSource);
	}
	return self;
}

- (BOOL)loadWave:(CFStringRef)filename toBuffer:(ALuint)bufferID
{
	static CFStringRef wav = CFSTR("wav");
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	CFURLRef fileURL =
	CFBundleCopyResourceURL(CFBundleGetMainBundle(), filename,wav, NULL);
	
	AudioFileID	audioFileID;
	AudioFileOpenURL (	
					  fileURL,
					  0x01, //fsRdPerm,						// read only
					  kAudioFileWAVEType,
					  &audioFileID
					  );
	
	UInt32 nPropertySize = 0;
	UInt32 nPropertyWritable = 0;
	AudioFileGetPropertyInfo (audioFileID,
							  kAudioFilePropertyAudioDataPacketCount,
							  &nPropertySize,
							  &nPropertyWritable);
	
	UInt64 sampleLength;
	if (sizeof(sampleLength) == nPropertySize)	{
		AudioFileGetProperty(audioFileID, kAudioFilePropertyAudioDataPacketCount, &nPropertySize, &sampleLength);
	}
	
	AudioStreamBasicDescription desc;
	UInt32 nPropSize = sizeof(desc);
	AudioFileGetProperty(audioFileID, kAudioFilePropertyDataFormat, &nPropSize, &desc);
	
	UInt32 nOutNumBytes, nOutNumPackets;
	nOutNumPackets = sampleLength;
	SInt16 sampleData[sampleLength];
	
	AudioFileReadPackets (
						  audioFileID,
						  false,
						  &nOutNumBytes,
						  NULL,
						  0,
						  &nOutNumPackets,
						  sampleData
						  );
	
	
	ALsizei nSoundSizeInBytes = sizeof(sampleData);
	ALsizei nSampleRate = desc.mSampleRate;
	
	alBufferData(bufferID, AL_FORMAT_MONO16, sampleData, nSoundSizeInBytes, nSampleRate);
	[pool release];
    return YES;
}

-(void)tearDownEngine
{
	alcMakeContextCurrent(NULL); 
	alcDestroyContext(myALContext); 
	alcCloseDevice(myALDevice); 
}

@end
