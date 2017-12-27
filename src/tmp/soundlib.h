//
//  soundlib.h
//  FallingUp
//
//  Created by Kaolin Fire on 12/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <OpenAL/al.h>
#import <OpenAL/alc.h>

@interface MySoundLib : NSObject
{
	@private
	
	ALCdevice* myALDevice;
	ALCcontext* myALContext;



}

// from the .h file:
//@property ALCdevice* myALDevice;
//@property ALCcontext* myALContext;

// from the .m file:
-(id)init;
-(BOOL)loadWave:(CFStringRef)filename toBuffer:(ALuint)bufferID;
-(void)tearDownEngine;

@end