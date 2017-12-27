/*
	VERSION INFO:
		000 first public release Jan 12 2004
		001 fix: drops only counted as +1 level, regardless of the number of lines killed
		002 trippy trails
		003 sound, levels spaced out, speed calmed down...
		004 prettied up a bit, fixed a bug in the scorefile, added "end game" option
		004b changed background rendering to textures instead of an image
			newer installer for windows
			icon added for windows
			openal toolkit included for os x
			in-game menus--you don't have to use function keys for everything
			cleaner UI
			base score algo taken from http://en.wikipedia.org/wiki/Tetris -- scaled down by a factor of ten
			ability to start from higher levels
			several tilesets
			preview reduces points by 20 percent
			low-scoring "CHEAT" mode reduces points by 60 percent
			
*/

#import "os.h" //WINDOWS or LINUX or OSX
#import "imagelib.h"
#import "soundlib.h"

#import "fallingup.h"
#import <stdlib.h>
#import <sys/time.h>
#import <stdio.h>
#import <math.h>

//#import <CoreFoundation/CFString.h>

int menuselected, textureselected, startlevel;

#define MENU_START 0
#define MENU_NEXT 1
#define MENU_SOUND 2
#define MENU_LEVEL 3
#define MENU_EXIT 4
#define MENU_N 5

#define TEXTURE_N 5

void colorMenu(int lit) {
	if (lit) {
		glColor4f(0,0,1,1);
	} else {
		glColor4f(1,1,1,.5);
	}
}

void startGame(); 

long getAbsoluteMillis(void) {
	static int first=1;
	static struct timeval start;
	static struct timeval now; // static just to avoid re-creation of variable... not that it matters THAT much
	if (first) { first = 0; gettimeofday(&start,NULL); }
	gettimeofday(&now,NULL);
	return ((now.tv_sec - start.tv_sec)*1000 + ((now.tv_usec - start.tv_usec)/1000.0f));
}

/**
	customizable 'dedication' or 'about' screen?
	customizable colors
	customizable shapes and number of shapes?
	splash screen?

	sounds: block tick, block drop, line clear (2,3,4?), board "lock"; music???

	two modes of play? how to switch/choose?	menu option, changes at 'new game'

	stages (levels?):
		1 (0 lines)	-- normal normal
		2 (1 line)	 -- swings upside-down
		3 (8 lines)	-- swings left-right
		4 (15 lines) -- slow spin left-right
		5 (23 lines) -- slow spin top-bottom
		6 (31 lines) -- slow random spin [[ how to fade?? ]]
		7 (39 lines) -- slow random spin based on key presses [[ how to fade?? ]]
		8 (47 lines) -- medium *

*/

#define INFO 0
#define RUNNING 1
#define PAUSED 2
#define OVER 3
#define HIGHSCORE 4

#define PIECES 7

#define TOPN 10

#define XOFF -6.0
#define YOFF -8
#define ZOFF -16.0

#define MAXFLAME 120.0f

#define ROT_NONE 0
#define ROT_BOUNCE 1
#define ROT_CONTINUE 2
#define ROT_STOP 3
#define ROT_ONCE 4

#define MAXFPS 30

/* AL/ALUT stuff... */
//#define NULL			0
#define NUM_BUFFERS		13
#define NUM_SOURCES		NUM_BUFFERS
ALuint buffers[NUM_BUFFERS];
ALuint sources[NUM_SOURCES];
ALfloat listenerPos[]={0.0,0.0,4.0};
ALfloat listenerVel[]={0.0,0.0,0.0};
ALfloat listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};
ALfloat source0Pos[]={ -2.0, 0.0, 0.0};
ALfloat source0Vel[]={ 0.0, 0.0, 0.0};
char *sounds[NUM_BUFFERS] = {
	"move", 
	"bert", 
	"ready", 
	"water", 
	"beat", 
	"thunk", 
	"drop", 
	"line", 
	"gameover", 
	"highscore", 
	"keystroke", 
	"beat", 
	"welcome"
};

#define SOUND_MOVE 0
#define SOUND_BAD 1
#define SOUND_READY 2
#define SOUND_WATER 3
#define SOUND_BEAT1 4
#define SOUND_THUNK 5
#define SOUND_DROP 6
#define SOUND_LINE 7
#define SOUND_GAMEOVER 8
#define SOUND_HIGHSCORE 9
#define SOUND_KEYSTROKE 10
#define SOUND_BEAT2 11
#define SOUND_WELCOME 12

int beat=0;
int quit=0;
int prevscore = -1;
int prevlevel = -1;

/* GL/GLUT stuff... */
GLuint mainWindowId;
int replacescore=-1, replacechar=0;

GLuint textures[TEXTURE_N];

const short CHANGEY=0, CHANGEX=1, CHANGEROT=2;
const int BASE_SPEED=1200;
const short PC_EL=0, PC_ELB=1, PC_S=2, PC_Z=3, PC_BLOCK=4, PC_BAR=5, PC_TEE=6; //PC:Piece#
const short data[][4][16]= {
	{ { 0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1		} ,{ 0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,0		} ,{ 0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0		} ,{ 0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,0		} } , //PC_EL
	{ { 0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0		} ,{ 0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1		} ,{ 0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,0		} ,{ 0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,0		} } , //PC_ELB
	{ { 0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0		} ,{ 0,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0		} ,{ 0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0		} ,{ 0,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0		} } , //PC_S
	{ { 0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1		} ,{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0		} ,{ 0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1		} ,{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0		} } , //PC_Z
	{ { 0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0		} ,{ 0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0		} ,{ 0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0		} ,{ 0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0		} } , //PC_BLOCK
	{ { 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0		} ,{ 0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0		} ,{ 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0		} ,{ 0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0		} } , //PC_BAR
	{ { 0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0		} ,{ 0,0,0,0,0,0,1,0,0,0,1,1,0,0,1,0		} ,{ 0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0		} ,{ 0,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0		} }	//PC_TEE

};

float colors[][3] = {
	{ 0.4,0.4,0.8	} , { 0.8,0.4,0.4	} , { 0.6,0.4,0.6	} , { 0.4,0.8,0.4	} , { 1.0,0.4,0.2	} , { 0.8,0.8,0.8	} , { 0.3,0.3,0.3	}
};

const int BLOCKTIMER=0;
const int SCALE=128,SCALEWIDTH=256,SCALEHEIGHT=256;
int HEIGHT=480, WIDTH=320, BLOCKW=200, BLOCKH=400;
char PROGRAM_NAME[80] = "Falling Up";
char title[80], scoreString[80], lineString[80], levelString[80];

#define MAXLEVEL_NAMES 14
char *levels[MAXLEVEL_NAMES] = {
	"Deceptive beginnings...", //0 == 0
	"Flipped!", //1 == 1
	"Flip^2", //2 == 5
	"A Hint Of Things To Come", //3 == 9
	"Double gasp", //4 == 15
	"Begin To Know Terror!", //5 == 21
	"Lightly Whirled Peas", //6 == 23
	"A Pause Before The Storm", //7 == 29
	"The Light Rain", //8 == 35
	"Plain Jane Up", //9 == 38
	"Up For Another Go", //10 == 45
	"Bouncy Bouncy Bouncy!", //11 == 47
	"Breather...", //12 == 57
	"Loopy!", //13 == 67
};

char *topnames[TOPN];
long topscores[TOPN];
int	toplevels[TOPN];

int linelevels[MAXLEVEL_NAMES] = { 1, 5, 9, 15, 21, 23, 29, 35, 38, 45, 47, 57, 67, 69 };

char *levelstring;

//info for keeping track of the time and frames per second
int timeOfRender=0;
int timeAtLastFrame=0;
int timeAtLastSecond=0;
int timeAtLastRender=0;
int timeOfBlockDrop=0;
int timeAtLastTexture=0;
int timeAtLastFlameCount= 0;
int timeAtLastRotCount = 0;

int frame=0;
int fps=0;
int falling=0, capture=0;
int displayFPS = false, displayNext = false, displayTrails = false, displaySound = false, displayCheat = false;


int paused, gameOn;
int flameCount=1, flameDirection=1, rotCount=0, rotDirection=1, rotted=0;
int mode;

int lines, speed, level, nextlevel, fakelines;
long score;

int rotMsec, rotMax, rotSpeed, rotStyle, rotDegrees;

short current, nextpiece;
short orientation;
short posx, posy; //position based on *center* of 3x3 grid. bar goes outside to the left/top
short grid[10][24]; //4 added to the top in the case of the bar being flipped up
short grid_rot[10][24]; //4 added to the top in the case of the bar being flipped up


unsigned char *image;

char* makeString(int i) {
	 char* mystring = (char*)malloc(sizeof(char)*(i+1));
	 for (;i>=0;i--) mystring[i]=0;
	 return mystring;
}

void startSound(int i) {
		if (displaySound)
		alSourcePlay(sources[i]);
}

void stopSound(int i) {
		alSourceStop(sources[i]);
}

void displayOpenALError(char* s, ALenum e) {
	switch (e) {
		case AL_INVALID_NAME:
			printf("%s Invalid Name\n", s);
			break;
		case AL_INVALID_ENUM:
			printf("%s Invalid Enum\n", s);
			break;
		case AL_INVALID_VALUE:
			printf("%s Invalid Value\n", s);
			break;
		case AL_INVALID_OPERATION:
			printf("%s Invalid Operation\n", s);
			break;
		case AL_OUT_OF_MEMORY:
			printf("%s Out Of Memory\n", s);
			break;
		default:
			printf("*** ERROR *** Unknown error case (%d) in displayOpenALError\n",e);
			break;
	};
}

// http://steinsoft.net/index.php?site=Programming/Code%20Snippets/OpenGL/gluperspective
void mygluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
	GLfloat xmin, xmax, ymin, ymax;
	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	
	glFrustumf(xmin,xmax,ymin,ymax,zNear,zFar);
}
			
/*
int LoadAndAssignWAV(char* file, ALuint buf) {
	int			error;
	ALenum		format; 
	ALsizei		size;
	ALsizei		freq; 
	ALvoid*		data;

	
	// Load in the WAV file from disk
#ifdef WINDOWS
	alutLoadWAVFile(file, &format, &data, &size, &freq, &loop);	// windows
#endif
#ifdef OSX
	//alutLoadWAVFile(file, &format, &data, &size, &freq);	// os x
			loadWave(file,
#endif
#ifdef LINUX
	alutLoadWAVFile((Albyte*)file, &format, &data, &size, &freq, &loop);	// linux
#endif
	if ((error = alGetError()) != AL_NO_ERROR) { 
		displayOpenALError("alutLoadWAVFile : ", error);	
		return 0; 
	}

	// Copy the new WAV data into the buffer
	alBufferData(buf,format,data,size,freq); 
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
*/

void writeScores(void) {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	for (int i=0;i<TOPN;i++) {
		if (topnames[i] != 0 && topnames[i][0] != 0) {
			[defaults setObject:[NSString stringWithFormat:@"%s",topnames[i]] forKey:[NSString stringWithFormat:@"highscores name %d",i]];
			[defaults setInteger:topscores[i] forKey:[NSString stringWithFormat:@"highscores score %d",i]];
			[defaults setInteger:toplevels[i] forKey:[NSString stringWithFormat:@"highscores level %d",i]];
		}
	}

	[defaults synchronize];

}

void readScores(void) {
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	NSString *hs_name;
	const char *tmp;
	char *tmp2;
	int hs_score, hs_level;
	for (int i=0;i<TOPN;i++) {
		hs_name = [defaults stringForKey:[NSString stringWithFormat:@"highscores name %d",i]];
		hs_score = [defaults integerForKey:[NSString stringWithFormat:@"highscores score %d",i]];
		hs_level = [defaults integerForKey:[NSString stringWithFormat:@"highscores level %d",i]];

		tmp = [hs_name UTF8String];
		
		if (tmp) {
			tmp2 = (char*) malloc(sizeof(char)* (strlen(tmp)+1));
			strcpy(tmp2,tmp);
		} else {
			tmp2 = (char*) malloc(sizeof(char));
			tmp2[0] = 0;
		}
		topnames[i] = tmp2;
		topscores[i] = hs_score;
		toplevels[i] = hs_level;
	}
}

void endGame(void) {
	int i,j;
	char *name;
	for (i=0, replacescore = -1;i<TOPN && replacescore == -1;i++) {
		if (score > topscores[i]) {
			replacescore = i;
			//replacechar=0;
			mode = HIGHSCORE;
			name = topnames[replacescore];
			if (topnames[TOPN-1]) free (topnames[TOPN-1]);
			for (j=TOPN-2;j>=replacescore;j--) {
				topscores[j+1]=topscores[j];
				topnames[j+1]=topnames[j];
				toplevels[j+1]=toplevels[j];
			}
			//topnames[replacescore]=makeString(20);
			//topnames[replacescore][replacechar]='_';
			topscores[replacescore]=score;
			toplevels[replacescore]=level;
		}
	}
	if (mode == HIGHSCORE) {
		startSound(SOUND_HIGHSCORE);
	} else if (quit) {
		mode=INFO;
		startSound(SOUND_GAMEOVER);
	} else {
		mode=OVER;
		startSound(SOUND_GAMEOVER);
	}
	paused=false;
	gameOn=false;
	stopSound(SOUND_WATER);
}

void quitGame(void) {
	quit = true;
	endGame();
}

int isHit(int myposx, int myposy, const short* tempblock) {
	int x,y;
	for (x=0;x<4;x++) for (y=0;y<4;y++) {
		if (tempblock[x+y*4] != 0) {
			//if this space exists in the grid, or is outside the 
			//left/right/bottom bounds... bip
			if (myposx+x < 0 || myposx+x > 9 || myposy-y < 0) return 1;
			if (grid[myposx+x][myposy-y] != -1) return 1;
		}
	}
	return 0;
}

void setBlock(int myposx, int myposy, int myorientation,int mycurrent) {
	int x,y,clear,numclear,suby, temp, n, points;
	for (x=0;x<4;x++) for (y=0;y<4;y++) {
		if(data[mycurrent][myorientation][x+y*4] != 0) {
			grid[myposx+x][myposy-y] = current;
			grid_rot[myposx+x][myposy-y] = myorientation;
		}
	}
	current=nextpiece;
	nextpiece=rand()%PIECES;
	posx=3;
	posy=22;
	orientation=0;
	if (isHit(posx,posy,data[current][orientation])) {
		endGame();
		return;
	}
	/*test to see if we can clear some lines :)*/
	numclear=0;
	n = nextlevel;
	for (y=0;y<21;y++) {
		clear=1;
		for (x=0;x<10;x++) if (grid[x][y] == -1) clear=0;
		if (clear) {
			numclear++;
			lines+=1;
			//go up a level, depending
			temp = lines+fakelines;
			while (temp > linelevels[MAXLEVEL_NAMES-1]) {
				temp -= linelevels[MAXLEVEL_NAMES-1];
			}
			if (temp >= linelevels[nextlevel%MAXLEVEL_NAMES]) {
				nextlevel++;
			}
			for (suby=y+1;suby<21;suby++) for (x=0;x<10;x++) {
				grid[x][suby-1]=grid[x][suby];
				grid_rot[x][suby-1]=grid_rot[x][suby];
			}
			y--;
		}
		//speed is msecs to wait for a block drop
		if ((lines+fakelines) < 16) {
			speed=BASE_SPEED - (lines+fakelines) * 35;
		} else if ((lines+fakelines) < 65) {
			speed=BASE_SPEED - 16 * 35 - ((lines+fakelines) - 16) * 7;
		} else {
			speed=BASE_SPEED - 16 * 35 - 59 * 7 - ((lines+fakelines) - 65) * 3;
		}
		if (speed < 100) speed=100;

		if (numclear > 0) {
			if (numclear == 1) {
				points = (n * 4 + 4);
			} else if (numclear == 2) {
				points = (n * 10 + 10);
			} else if (numclear == 3) {
				points = (n * 30 + 30);
			} else { //TETRIS =)
				points = (n * 120 + 120);
			}
 			if (displayCheat) {
				points *= .4;
			} else if (displayNext) {
				points *= .8;
			}
			score+=points;
			startSound(SOUND_LINE);
		}
	}
	clear=1;
	for (x=0;x<10;x++) if (grid[x][21] != -1) clear=0;
	if (clear!=1) endGame();
	/*test to see if the game is over*/
}

int testBounds(short changeType, int amount) {
	//if we would hit something, and it's due to ydif, stick us there.
	//hitting something INCLUDES the bottom of the screen, BTW, just in case 
	//you were wondering, this is where that check happens. Neener neener. :)
	//if we're rotating or xdiffing or ydiffing and not hitting anything, 
	//make it so.
	int i,mycurrent,myposx,myposy,myorientation, stuck;
	short tempblock[16];
	mycurrent=current;
	myposx=posx;
	myposy=posy;
	myorientation=orientation;
	stuck = false;
	if (changeType == CHANGEY) {
		for (i=0;i<16;i++) tempblock[i]=data[mycurrent][myorientation][i];
		if (isHit(myposx,myposy-amount,tempblock) == 1) {
			//stick us there
			if (falling) {
				falling=false;
				switch (capture) {
					case MY_KEY_LEFT:	testBounds(CHANGEX,1); break;
					case MY_KEY_RIGHT: testBounds(CHANGEX,-1); break;
					case MY_KEY_UP:		testBounds(CHANGEROT,1); break;
					default: break;
				}
				stuck = testBounds(CHANGEY,1);
				if (stuck) { startSound(SOUND_THUNK); }
				return stuck;
			} else {
				falling=false;
				setBlock(myposx,myposy,myorientation,mycurrent);
				stuck = true;
			}
		} else { posy-=amount; }
	} else if (changeType == CHANGEX) {
		for (i=0;i<16;i++) tempblock[i]=data[current][orientation][i];
		if (isHit(myposx-amount,myposy,tempblock) != 1) {
			posx-=amount;
		} else {
			stuck = true;
		}
	} else { //changeType == CHANGEROT
		for (i=0;i<16;i++) tempblock[i]=data[current][(orientation+1)%4][i];
		if (isHit(myposx,myposy,tempblock) != 1) {
			orientation=(orientation+1)%4;
		} else { stuck = true; }
	}
	return stuck;
}

typedef struct TexCoord { float tu, tv; } TexCoord;
typedef struct VertCoord {float x, y, z; } VertCoord;

void drawCube(float r) {
	static TexCoord texCoords[] = {
		0,1 , 1,1 , 0,0 , 1,0
	};
	VertCoord vertCoords[] = {
		-r,r,0 , r,r,0 , -r,-r,0 , r,-r,0
	};

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	glVertexPointer(3,GL_FLOAT,0,vertCoords);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);	
}

void mycycletexture(bool left=false) {
	if (left) {
		if (textureselected == 0) textureselected = TEXTURE_N-1;
		else textureselected--;
	} else {
		if (textureselected == TEXTURE_N-1) textureselected = 0;
		else textureselected++;
	}	
}

void fillBox(float x1, float y1, float x2, float y2, bool disable=true,float z=0.0, TexCoord *coords=NULL) {
	static TexCoord texCoords[] = {
		 0,0 , 1,0 , 0,1 , 1,1
	};
	glColor4f(1,1,1,1);
	if (disable) glDisable(GL_TEXTURE_2D);
	else {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
		if (coords == NULL) coords = texCoords;		
	}
	
	VertCoord vertCoords[] = {
		 x1,y1,z , x2,y1,z , x1,y2,z , x2,y2,z
	};
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glVertexPointer(3,GL_FLOAT,0,vertCoords);
	
	if (!disable) glTexCoordPointer(2, GL_FLOAT,0, coords);
	
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	
	glDisableClientState(GL_VERTEX_ARRAY);	
	if (disable) {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	glEnable(GL_TEXTURE_2D);
}

void drawBox(float x1, float y1, float x2, float y2) {

	glDisable(GL_TEXTURE_2D);

	VertCoord vertCoords[] = {
		x1,y1 , x2,y1 , x1,y2 , x2,y2
	};
	
	VertCoord vertCoords2[] = {
		x1,y1 , x2,y1 , x2,y2 , x1,y2 
	};
	
	glEnableClientState(GL_VERTEX_ARRAY);

	glColor4f(1.0,1.0,1.0,0.2);
	glVertexPointer(2,GL_FLOAT,0,vertCoords);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glDisable(GL_BLEND);
	
	glPushMatrix();
		glColor4f(1.0,1.0,1.0,1);
		//glTranslatef(0,0,.2);
		glVertexPointer(2,GL_FLOAT,0,vertCoords2);
		glDrawArrays(GL_LINE_LOOP,0,4);
	glPopMatrix();

	glEnable(GL_BLEND);

	glDisableClientState(GL_VERTEX_ARRAY);	
	
	glEnable(GL_TEXTURE_2D);
}

void drawBlock(float x, float y, float z, float red, float green, float blue, short rotation, int color) {
	// save the current position and move to the new one

	glPushMatrix();
		x-=4.5;
		y-=7;
		glTranslatef(x, y, z);

		// begin to draw a block in this position

		glEnable(GL_TEXTURE_2D);
		// we set the color to what was passed, but we also make our own shades
		glColor4f(red, green, blue,0.9);
 		glBindTexture(GL_TEXTURE_2D,textures[textureselected]);
		glRotatef((float)rotation*90.0,0,0,1);
		drawCube(.5);
		glDisable(GL_TEXTURE_2D);

	// before we finish, restore the matrix
	glPopMatrix();
}

void renderBitmapString(float x, float y, void *myfont,char *string)
{
/*
	char *c;
	// set position to start drawing fonts
	glDisable(GL_TEXTURE_2D);
	glRasterPos2f(x, y);
	// loop all the characters in the string
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter((void*)myfont, (char)*c);
	}
 */
}


void setOrthographicProjection(void) {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();
		// reset matrix
	
		glLoadIdentity();
		// set a 2D orthographic projection
		glOrthof(0.0f,WIDTH,HEIGHT,0.0,-1.0,1.0);
		// move the origin from the bottom left corner
		// to the upper left corner
		glScalef(1, -1, 1);
		glTranslatef(0, -HEIGHT, 0);
		glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection(void) {
		// set the current matrix to GL_PROJECTION
		glMatrixMode(GL_PROJECTION);
		// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
}

void renderBlocks(void) {
	int x, y;
	float XLEFT, XRIGHT, YTOP, YBOTTOM;
	XLEFT = -5;
	XRIGHT= XLEFT+10.0;
	YTOP = -7.5;
	YBOTTOM = YTOP-1;
	//char lineitem[80];
	int cheat[4];

	for (x=0;x<4;x++) { cheat[x] = 0; }
//	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glTranslatef(-1, -1,2*ZOFF);
	glTranslatef(XOFF/2,YOFF/2,ZOFF*1.4);
	//glTranslatef(1, 4,-5);
	//glTranslatef(0,.15,0);
	//glScalef(.5,.5,1);
	
	//increment 'display level'
	if (level != nextlevel) {
		if (rotStyle != ROT_STOP && rotCount > 0) rotStyle = ROT_STOP;
		if (rotCount == 0 || rotted) {
			rotted=false;
			rotCount = 0;
			//level = nextlevel;
			level++;
			levelstring=levels[level % MAXLEVEL_NAMES];
			switch (level % MAXLEVEL_NAMES) {
				case 1: rotStyle=ROT_ONCE; rotMax=230; rotMsec=15; rotDirection = 1; break; break;
				case 2: rotStyle=ROT_ONCE; rotMax=230; rotMsec=15; rotDirection = 1; break;
				case 3: rotStyle=ROT_ONCE; rotMax=230; rotMsec=15; rotDirection = 1; break;
				case 4: rotStyle=ROT_ONCE; rotMax=230; rotMsec=15; rotDirection = 1; break;
				case 5: rotStyle=ROT_CONTINUE; rotMax=230; rotMsec=15; rotDirection = 1; break;
				case 6: rotStyle=ROT_ONCE; rotMax=230; rotMsec=15; rotDirection = 1; break;
				case 7: rotStyle=ROT_ONCE; rotMax=230; rotMsec=15; rotDirection = 1; break;
				case 8: rotStyle=ROT_CONTINUE; rotMax=230; rotMsec=20; rotDirection = 1; break;
				case 9: rotStyle=ROT_ONCE; rotMax=230; rotMsec=20; rotDirection = 1; break;
				case 10: rotStyle=ROT_ONCE; rotMax=230; rotMsec=20; rotDirection = 1; break;
				case 11: rotStyle=ROT_BOUNCE; rotMax=230; rotMsec=20; rotDirection = 1; break;
				case 12: rotStyle=ROT_ONCE; rotMax=230; rotMsec=20; rotDirection = 1; break;
				case 13: rotStyle=ROT_CONTINUE; rotMax=230; rotMsec=20; rotDirection = 1; break;
				case 14: rotStyle=ROT_ONCE; rotMax=230; rotMsec=15; rotDirection = 1; break;
				default: break; //already set up?  level 0
			}
		}
	}

	switch (level % MAXLEVEL_NAMES) {
		case 0: rotCount = 0; break;
		case 1:
			glRotatef(180.0f*((float)rotCount/(float)rotMax),5,0,0);
			glTranslatef(0.0f, -5.0*((float)rotCount/(float)rotMax), 0.0f);
		break;
		case 2:
			glRotatef(180.0f,5,0,0);
			glTranslatef(0.0f, -5.0, 0.0f);
			glRotatef(180.0f*((float)rotCount/(float)rotMax),0,5,0);
		break;
		case 3:
			glRotatef(180.0f*((float)(rotMax-rotCount)/(float)rotMax),5,0,0);
			glRotatef(180.0f*((float)(rotMax-rotCount)/(float)rotMax),0,5,0);
			glTranslatef(0.0f, -5.0*((float)(rotMax-rotCount)/(float)rotMax), 0.0f);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),-10,0,0);
		break;
		case 4:
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,0,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),5,0,0);
		break;
		case 5:
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,5,0);
		break;
		case 6:
			glRotatef(180.0f*((float)rotCount/(float)rotMax),0,5,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,1,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),1,0,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,0,1);
		break;
		case 7:
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,0,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),5,0,0);
		break;
		case 8:
			glRotatef(360.0f*((float)rotCount/(float)rotMax),1,0,0);
		break;
		case 9:
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,1,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),1,0,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,0,1);
		break;
		case 10:
			glRotatef(180.0f*((float)rotCount/(float)rotMax),5,0,0);
			glTranslatef(0.0f, -5.0*((float)rotCount/(float)rotMax), 0.0f);
			glRotatef(180.0f*((float)rotCount/(float)rotMax),0,5,0);
		break;
		case 11:
			glRotatef(180.0f,5,0,0);
			glTranslatef(0.0f, -5.0, 0.0f);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,5,0);
		break;
		case 12:
			glRotatef(180.0f,5,0,0);
			glTranslatef(0.0f, -5.0, 0.0f);
		break;
		case 13:
			glRotatef(180.0f,5,0,0);
			glTranslatef(0.0f, -5.0, 0.0f);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,0,0);
			glRotatef(360.0f*((float)rotCount/(float)rotMax),5,0,0);
		break;
		case 14:
			glRotatef(180.0f,5,0,0);
			glTranslatef(0.0f, -5.0, 0.0f);
		break;
		default: // ???
			glRotatef(360.0f*((float)rotCount/(float)rotMax),0,5,0);
	}

	switch(mode) {
	case HIGHSCORE: break;
	case OVER:
	case PAUSED: 
	case RUNNING:
		glColor4f(1,1,1,1);
		drawBox(XLEFT+.02,-7.52,XRIGHT-.02,13.48);
		//	for (int j=-50;j<50;j++) for (int k=-50;k<50;k++) {
		//		drawBox(j,k,j-1,k-1);
		//	}
		if (displayCheat) {
			for (x=0;x<4;x++) for (y=0;y<4;y++) {
				if (data[current][orientation][y*4+x] != 0) {
					cheat[x] = 1;
				}
			}
		}
		for (x=0;x<4;x++) {
			if (cheat[x]) {
				drawBox(XLEFT+.02+x+posx,-7.52,XLEFT+.98+x+posx,13.48);
			}
		}
		//for each x/y of the grid, paint what's there... then paint our block in its position?
		for (x=0;x<10;x++) for (y=0;y<20;y++) {
			if (grid[x][y] > -1) {
				drawBlock(x,y,0,colors[grid[x][y]][0],colors[grid[x][y]][1],colors[grid[x][y]][2],grid_rot[x][y],grid[x][y]);
			}
		}
		if (current != -1) {
			for (x=0;x<4;x++) for (y=0;y<4;y++) {
				if (data[current][orientation][y*4+x] != 0)
					drawBlock(posx+x,posy-y,0,colors[current][0],colors[current][1],colors[current][2],(short)orientation,current);
			}
		}

		// draw the UNDERLINE

		glColor4f(1.0, 1.0, 0.0,1.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,textures[textureselected]);
		for (x=0;x<10;x++) {
			fillBox(XLEFT+x, YBOTTOM, XLEFT+x+1, YTOP,false);
		}
		glDisable(GL_TEXTURE_2D);

		break;
	default:
		break;
	}
	glPopMatrix();
}

void renderScores(void) {
	
	//char lineitem[81];
	//int i;
	glDisable(GL_TEXTURE_2D);
	if (mode == HIGHSCORE) {
		glColor4f(1,1,0,.4);
		fillBox(20,246-replacescore*20,400,244-(replacescore-1)*20);
	}
	drawBox(20,50,400,275);
	glColor4f(.2,0,.4,1);/*
	for (i=0;i<TOPN;i++) {
		sprintf(lineitem,"%-20s    %9d  %3d",topnames[i],topscores[i],toplevels[i]);
		//renderBitmapString(30,250-i*20,(void *)font,lineitem);
	}
	 */
}

void drawNext(void) {
	int x, y;
	if (displayNext) {
		if (nextpiece != -1) {
			glPushMatrix();
			//glTranslatef(9.5, 1.7, ZOFF);
			    glTranslatef(7, -.8, ZOFF);
				for (x=0;x<4;x++) for (y=0;y<4;y++) {
					if (data[nextpiece][0][y*4+x] != 0)
						drawBlock(x,4-y,0,colors[nextpiece][0],colors[nextpiece][1],colors[nextpiece][2],0,nextpiece);
				}
			glPopMatrix();
		}
	}
}

void drawStats(void) {
//		char lineString[80];
//	char scoreString[80];


	setOrthographicProjection();
	//glDisable(GL_BLEND);
	glColor4f(1,1,1,1);
		drawBox(11,HEIGHT-35,177,HEIGHT-56); //for level name
		//drawBox(7,HEIGHT-15,235,HEIGHT-38); //for level name
	//glEnable(GL_BLEND);
/*	
		sprintf(scoreString,"%d",score);
		sprintf(lineString,"%d",lines);
		glColor4f(.2,0,.4,1.0);

		//renderBitmapString(13+(4*(24-strlen(levelstring))),HEIGHT-31,(void *)font,levelstring);

		if (displayNext) {
			drawBox(280,HEIGHT-320,415,HEIGHT-570); //for scorebox
		} else {
			drawBox(280,HEIGHT-345,415,HEIGHT-570); //for scorebox
		}
		glColor4f(0.2,0.4,0.6,1.0);
		if (!displayNext) {
			renderBitmapString(288,HEIGHT-376,(void *)font,"Next:");
			glColor4f(0.0,0.0,0.0,1);
			renderBitmapString(298,HEIGHT-391,(void *)font,"N/A");
		} else {
			renderBitmapString(288,HEIGHT-351,(void *)font,"Next:");
		}
		glColor3f(0.2,0.4,0.6);
		renderBitmapString(288,HEIGHT-426,(void *)font,"Score:");
		glColor3f(0,0,0);
		renderBitmapString(298,HEIGHT-441,(void *)font,scoreString);
		glColor3f(0.2,0.4,0.6);
		renderBitmapString(288,HEIGHT-476,(void *)font,"Lines:");
		glColor3f(0,0,0);
		renderBitmapString(298,HEIGHT-491,(void *)font,lineString);
		glColor3f(0.2,0.4,0.6);
		renderBitmapString(288,HEIGHT-526,(void *)font,"Level:");
		glColor3f(0,0,0);
		sprintf(lineString,"%d",level);
		renderBitmapString(298,HEIGHT-541,(void *)font,lineString);
 */
	resetPerspectiveProjection();
}

void renderScene(void) {
	//char lineitem[80];//, linestring[80], scorestring[80];
	int i, temp, temp2;// x, y, temp, temp2;
	unsigned char* imagetemp;
	//int toptext=450, lefttext=50;
	//int lefttext=50;
	//int pluser = -20;
	//int menupluser = -30;
	//timeval t;
	//gettimeofday(&t,NULL);
	//double start = t.tv_sec * 1000000+t.tv_usec;


	//glutSetWindow(mainWindowId); // not really needed? -- only one window
	timeAtLastRender = timeOfRender;
	timeOfRender = getAbsoluteMillis();
	
	if (1000.0/MAXFPS - (timeOfRender - timeAtLastRender) > 0) {
		usleep((int)(1000.0/MAXFPS - (timeOfRender - timeAtLastRender)) * 1000);
	}

	frame++;

	glViewport(0,0,WIDTH,HEIGHT);

	// was windowResize
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//mygluPerspective(80,(float)WIDTH/(float)HEIGHT,1.0,5000.0);
	mygluPerspective(70,(float)WIDTH/(float)HEIGHT,1.0,5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// end was windowResize
	
	//glLoadIdentity(); // necessary!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (timeOfRender - timeAtLastFlameCount > 100) {
		flameCount+=flameDirection;
		if (flameCount > MAXFLAME || flameCount < 1) flameDirection *= -1;
		timeAtLastFlameCount = timeOfRender;
		temp = WIDTH*HEIGHT*3;
		imagetemp=image;
		if (displayTrails && mode != HIGHSCORE && mode != INFO) {
			if (level % 5	== 1) {
				for (i=0;i<temp;i++,imagetemp++) {
					if (image[i] >= 240) image[i] = 255;
					else image[i]+=15;
				}
			} else if (level % 5 == 2) {
				for (i=0;i<temp;i++,imagetemp++) {
					if (image[i] >= 45 && image[i] <= 75) {
						image[i]=60;
					} else if (image[i] < 45) {
						image[i]+=15;
					} else {
						image[i]-=15;
					}
				}
			} else if (level %5 == 3) {
				for (i=0;i<temp;i++,imagetemp++) {
					if (image[i] < 15) image[i] = 0;
					else image[i]-=15;
				}
			} else if (level %5 == 4) {
				temp2 = getAbsoluteMillis() %256;
				for (i=0;i<temp;i++,imagetemp++) {
					image[i]=temp2;
				}
			} else {
				temp2 = 155;
				for (i=0;i<temp;i++,imagetemp++) {
					image[i]=temp2;
				}
			}
		}
	}

	if (timeOfRender - timeAtLastRotCount > rotMsec) {
		if (!rotted && rotStyle != ROT_NONE) {
			rotCount +=rotDirection;
			if (rotCount < 0) {
				if (rotStyle == ROT_ONCE || rotStyle == ROT_STOP) { rotted = true; rotCount = 0;}
				else if (rotStyle == ROT_BOUNCE) { rotDirection *=-1; rotCount = rotDirection; }
				else if (rotStyle == ROT_CONTINUE) { rotCount += rotMax; }
			} else {
				if (rotCount > rotMax) {
					if (rotStyle == ROT_ONCE || rotStyle == ROT_STOP) { rotted = true; rotCount = rotMax; }
					else if (rotStyle == ROT_BOUNCE) { rotDirection *=-1; rotCount = rotMax+rotDirection; }
					else if (rotStyle == ROT_CONTINUE) { rotCount -= rotMax; }
				}
			}
			timeAtLastRotCount = timeOfRender;
		}
	}

	if (timeOfRender - timeAtLastSecond > 1000) {
		fps = frame*1000.0/(timeOfRender - timeAtLastSecond);
		timeAtLastSecond = timeOfRender;
		frame = 0;
		sprintf(title,"%s [%3d FPS], %d, %d, %d, %d",PROGRAM_NAME,fps,flameCount,rotCount, level, nextlevel);
		//if (displayFPS) glutSetWindowTitle(title); //TODO: FPS
	}

	switch (mode) {
	case INFO:
			/*
		setOrthographicProjection();
			lefttext=105;
			colorMenu(menuselected==MENU_START);
			//renderBitmapString(lefttext,toptext,(void *)menufont,"START");
			colorMenu(menuselected==MENU_NEXT);
			//renderBitmapString(lefttext,toptext+=menupluser,(void *)menufont,"NEXT PIECE");
			colorMenu(false);
			if (displayCheat) {
				glColor4f(1,1,0,1);
				strcpy(lineitem,"CHEAT");
			} else if (displayNext) {
				strcpy(lineitem,"visible");
			} else {
				strcpy(lineitem,"hidden");
			}
			//renderBitmapString(lefttext+140,toptext,(void *)menufont,lineitem);

			colorMenu(menuselected==MENU_SOUND);
			//renderBitmapString(lefttext,toptext+=menupluser,(void *)menufont,"SOUND");
			colorMenu(false);
			//renderBitmapString(lefttext+140,toptext,(void *)menufont,displaySound?"on":"off");

			colorMenu(menuselected==MENU_LEVEL);
			//renderBitmapString(lefttext,toptext+=menupluser,(void *)menufont,"LEVEL");
			colorMenu(false);
			sprintf(lineitem,"%d",startlevel);
			//renderBitmapString(lefttext+140,toptext,(void *)menufont,lineitem);

			colorMenu(menuselected==MENU_EXIT);
			//renderBitmapString(lefttext,toptext+=menupluser,(void *)menufont,"EXIT");

			glColor4f(1.0,1.0,1.0,1.0);
			//renderBitmapString(20,toptext+=menupluser-15,(void *)menufont,"Top scores");

			renderScores();

			glColor4f(1,1,1,1);
			glEnable(GL_TEXTURE_2D);
 			glBindTexture(GL_TEXTURE_2D,textures[textureselected]);
			fillBox(lefttext+140, 450-menupluser, lefttext+170, 450, false,.5);
			glDisable(GL_TEXTURE_2D);

		resetPerspectiveProjection();*/
		break;

	case RUNNING:
		drawStats();
		if (falling) {
			testBounds(CHANGEY,1);
		} else if (timeOfRender > timeOfBlockDrop + speed) {
			if (testBounds(CHANGEY,1)) {
				startSound(SOUND_THUNK);
			} else {
				//beat -- but it's on two channels so as to not cut itself off
				startSound(beat?SOUND_BEAT1:SOUND_BEAT2);
				beat = !beat;
			}
			timeOfBlockDrop = timeOfRender;
		}
		renderBlocks();
		drawNext();
		break;
	case OVER:
		drawStats();
		renderBlocks();
			/*
		setOrthographicProjection();
			glPushMatrix();
				glTranslatef(225,440,0);
				glRotatef(timeOfRender/10,0,0,1);
				glColor4f(0.0,0.0,0.0,1.0);
				drawBox(-925,-15,1500,15);
				drawBox(-925,-15,1500,15);
				drawBox(-925,-15,1500,15);
				//renderBitmapString(-90,-7,(void *)menufont,"-- GAME OVER --");
			glPopMatrix();
		resetPerspectiveProjection();
			 */
		break;
	case PAUSED:
		drawStats();
		renderBlocks();
		drawNext();
		timeOfBlockDrop += (timeOfRender - timeAtLastRender); // extend block drop
		break;
	case HIGHSCORE:
			/*
		setOrthographicProjection();
			//renderBitmapString(114,435,(void *)menufont,"!!! HIGH SCORE !!!");
			topnames[replacescore][replacechar]=(((int)(timeOfRender/500.0))%2)?'_':' ';
			renderScores();
			drawBox(30,320,400,415);
			glColor4f(0.2,0.4,0.6,1.0);
			//renderBitmapString(45,385,(void *)font,"Congratulations!  Please enter your");
			//renderBitmapString(45,360,(void *)font,"name below for posterity.  You know,");
			//renderBitmapString(45,335,(void *)font,"or don't.  Whatever.");
		resetPerspectiveProjection();
			 */
		break;
 

	default:
		break;
	}

//	mycycletexture();
	
}


void startGame(void) {
	int x, y;
	quit = false;
	mode=RUNNING;
	gameOn=true;
	falling=false;
	//srand(time(NULL));
	lines=0;
	level=startlevel;
	nextlevel=startlevel;
	fakelines=linelevels[startlevel];
	score=0;
	levelstring=levels[startlevel];
	speed=BASE_SPEED;
	for (x=0;x<10;x++) for (y=0;y<24;y++) {
		grid[x][y]=-1;
		grid_rot[x][y]=0;
	}
	for (x=0;x<10;x++) for (y=0;y<startlevel;y++) {
		grid[x][y]=(short)((rand()%(PIECES+1))-1);
	}
	for (x=0;x<startlevel;x++) {
		grid[rand()%10][x] = -1;
	}
	current=(short)(rand()%PIECES);
	nextpiece=(short)(rand()%PIECES);
	posx=(short)3;
	posy=(short)22;
	orientation=(short)0;
	timeOfBlockDrop=0;
	rotCount=0;
	rotStyle=ROT_NONE;
	rotMax=180;
	
	prevscore = -1;
	prevlevel = -1;

	startSound(SOUND_READY);
	startSound(SOUND_WATER);
	setHandled(false);	
}

void bosskey(void) {
		if (mode == RUNNING) {
			mode=PAUSED;
			stopSound(SOUND_WATER);
		}
		//glutIconifyWindow();
}

void pauseGame(void) {
	bosskey();
}

void processNormalKeys(unsigned char key) { //, int x, int y) {
	if (key == 27) {
		if (mode == INFO) {
			exit(0);
		} else if (mode == PAUSED) {
			quit = true; endGame();
		} else if (mode == RUNNING) {
			stopSound(SOUND_WATER);
			mode=PAUSED;
		} else if (mode == OVER) {
			mode = INFO;
			startSound(SOUND_LINE);
		}
	} else if (mode == PAUSED) {
		startSound(SOUND_WATER);
		mode = RUNNING;
	} else if (mode == RUNNING) {
		if (key == ' ' && !falling) {
			falling=true; capture=0;
			startSound(SOUND_DROP);
		}
	} else if (mode == OVER) {
		mode = INFO;
		startSound(SOUND_LINE);
	} else if (mode == HIGHSCORE) {
		if (key == 8 || key == 14 || key == 127 || key == '_') {
			startSound(SOUND_KEYSTROKE);
			if (replacechar > 0) {
			topnames[replacescore][replacechar]=0;
			replacechar--;
			topnames[replacescore][replacechar]='_';
			}
		} else if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {
			startSound(SOUND_KEYSTROKE);
			if (replacechar < 20 - 1) {
				topnames[replacescore][replacechar]=key;
				replacechar++;
				topnames[replacescore][replacechar]='_';
			}
		} else if (key == '\n' || key == 13 || key == 10) {
			startSound(SOUND_LINE);
			topnames[replacescore][replacechar]=0;
			writeScores();
			mode = INFO;
		}
	} else if (mode == INFO) { /*
		switch (key) {
			case ' ': 
			case 13: 
				switch (menuselected) {
					case MENU_START: startGame(); break;
					case MENU_SOUND: displaySound = !displaySound; break;
					case MENU_NEXT:
						if (displayNext) {							
							if (false) { //glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
								displayCheat = true;
							} else {
								displayCheat = displayNext = false;
							}
						} else {
							displayNext = true; displayCheat = false;
						}
					break;
					case MENU_EXIT: exit(0); break;
					case MENU_LEVEL: if (startlevel > 9) startlevel = 0; else startlevel++;
					default: break;
				}
				startSound(SOUND_KEYSTROKE);
			break;
			default: break;
		}*/
	}
}

void processSpecialKeys(int key) {//, int x, int y) {
	//if (key == MY_KEY_F4 && glutGetModifiers() & GLUT_ACTIVE_ALT) exit(0);
	if (mode == INFO) {/*
		switch (key) {
			case MY_KEY_F5: if (!displayCheat || mode == INFO) { displayNext = !displayNext; } break;
			case MY_KEY_F7: displaySound = !displaySound; break;
			case MY_KEY_F1: startSound(SOUND_KEYSTROKE); startGame(); break;
			case MY_KEY_DOWN: 
				startSound(SOUND_KEYSTROKE);
				if (menuselected == MENU_N-1) menuselected = 0;
				else menuselected++;
			break;
			case MY_KEY_UP: 
				startSound(SOUND_KEYSTROKE);
				if (menuselected == 0) menuselected = MENU_N-1;
				else menuselected--;
				break;
			case MY_KEY_LEFT: 
			case MY_KEY_RIGHT: 
			case MY_KEY_F8:
				if (menuselected == MENU_START || key == MY_KEY_F8) {
					//mycycletexture(key==MY_KEY_LEFT);
				} else
				switch (menuselected) {
					case MENU_SOUND: displaySound = !displaySound; break;
					case MENU_NEXT:
						if (displayNext) {
							if (false) {//glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
								displayCheat = true;
							} else {
								displayCheat = displayNext = false;
							}
						} else {
							displayNext = true; displayCheat = false;
						}
					break;
					case MENU_LEVEL:
						if (key == MY_KEY_LEFT) {
							if (startlevel == 0) startlevel = 10; else startlevel--;
						} else {
							if (startlevel == 10) startlevel = 0; else startlevel++;
						}
					break;
					default: break;
				}
				if (menuselected != MENU_EXIT) startSound(SOUND_KEYSTROKE);
			break;
			default: break;
		}
	*/} else
	switch (key) {
	case MY_KEY_DOWN:
		if (mode == RUNNING) {
			if (testBounds(CHANGEY,1)) {
				startSound(SOUND_THUNK);
			} else {
				startSound(SOUND_MOVE);
			}
		}
		break;
	case MY_KEY_LEFT:
		if (mode == RUNNING) {
			if (falling) { capture = MY_KEY_LEFT; }
			else if (testBounds(CHANGEX,1)) {
				startSound(SOUND_BAD);
			} else {
				startSound(SOUND_MOVE);
			}
		}
		break;
	case MY_KEY_RIGHT:
		if (mode == RUNNING) {
			if (falling) { capture = MY_KEY_RIGHT; }
			else if (testBounds(CHANGEX,-1)) {
				startSound(SOUND_BAD);
			} else {
				startSound(SOUND_MOVE);
			}
		}
		break;
	case MY_KEY_UP:
		if (mode == RUNNING) {
			if (falling) { capture = MY_KEY_UP; }
			else if (testBounds(CHANGEROT,1)) {
				startSound(SOUND_BAD);
			} else {
				startSound(SOUND_MOVE);
			}
		}
		break;
	case MY_KEY_F3:
		//displayFPS = !displayFPS;
		break;
	case MY_KEY_F5:
		//displayNext = !displayNext;
		break;
	case MY_KEY_F6:
		//displayTrails = !displayTrails;
		break;
	case MY_KEY_F7:
		displaySound = !displaySound;
		if (displaySound && mode == RUNNING) {
			startSound(SOUND_WATER);
		} else {
			stopSound(SOUND_WATER);
		}
		break;
	case MY_KEY_F8:
		//if (textureselected == TEXTURE_N-1) textureselected = 0;
		//else textureselected++;
		break;
	case MY_KEY_F1:
	//	if (mode != RUNNING) {
	//		startGame();
	//	}
		break;
	case MY_KEY_F4:
		//if (glutGetModifiers() & GLUT_ACTIVE_ALT) exit(0);
		if (mode == RUNNING || mode == PAUSED) { quit = true; endGame(); }
		break;
	case MY_KEY_F2:
		if (mode == RUNNING) {
			mode=PAUSED;
			stopSound(SOUND_WATER);
		} else if (mode == PAUSED) {
			mode=RUNNING;
			startSound(SOUND_WATER);
		}
		break;
	//case MY_KEY_F12:	bosskey(); break;
	default: break;
	}
}


void fallingupInit(void) {
	int i;
	image = (unsigned char*) malloc((HEIGHT*WIDTH)*3*sizeof(unsigned char));
	mode=INFO;

	glEnable(GL_LINE_SMOOTH);
//	glEnable(GL_POLYGON_SMOOTH);

	for (i=0;i<TOPN;i++) {
		topnames[i]=NULL;
		topscores[i]=0;
		toplevels[i]=0;
	}
	startSound(SOUND_WELCOME);
	menuselected = MENU_START;
	textureselected = 0;
	startlevel = 0;

}
/*
void windowResize(int w, int h) {
	glutSetWindow(mainWindowId);
	glViewport(0,0,w,h);
	if (w != WIDTH || h != HEIGHT) {
		glutReshapeWindow(WIDTH,HEIGHT);
	}
	else {
		//glutSetWindow(blockWindowId);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(80,(float)WIDTH/(float)HEIGHT,1.0,5000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}
*/

int initFallingup(void){
	
	int i,error;
	//TODO: see "Handling Initialization Failure" in developer.apple.com documentation
	id sound = [[MySoundLib alloc] init];
	alGetError(); // clear error code
	alGenBuffers(NUM_BUFFERS,buffers); // create buffers
	if ((error = alGetError()) != AL_NO_ERROR) { 
		debug(@"couldn't generate sound buffers -  %i:", error);
		return 1;
	}

	alGenSources(NUM_SOURCES,sources); // create sources
	if ((error = alGetError()) != AL_NO_ERROR) { 
		debug(@"couldn't generate sound sources - %i", error);
		return 1;
	}
	
	// Load in the WAV and store it in a buffer, then make the source and link 'em
	for (i=0;i<NUM_BUFFERS;i++) {
		
		CFStringRef s = CFStringCreateWithCString(NULL,sounds[i],NULL);
		if (![sound loadWave:s toBuffer:buffers[i]]) {
			// Error loading in the WAV so quit
			alDeleteBuffers(NUM_BUFFERS, buffers);
			return 1;
		}
		alSourcef(sources[i], AL_PITCH, 1.0f);
		alSourcef(sources[i], AL_GAIN, 1.0f);
		alSourcefv(sources[i], AL_POSITION, source0Pos);
		alSourcefv(sources[i], AL_VELOCITY, source0Vel);
		alSourcei(sources[i], AL_BUFFER,buffers[i]);
		alSourcei(sources[i], AL_LOOPING, AL_FALSE);
	}
	//override above defaults
	alSourcei(sources[3],AL_LOOPING,AL_TRUE); // water loops

	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowPosition(100,100); //TODO: CENTER?
	//glutInitWindowSize(WIDTH,HEIGHT);
//	mainWindowId = glutCreateWindow(PROGRAM_NAME);

	return 0;
}

int initFallingupPostGL() {
	int i;

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	i=0;
	textures[i++]=LoadTexture("pencil.png");
	textures[i++]=LoadTexture("gem.png");
	textures[i++]=LoadTexture("glass.png");
	textures[i++]=LoadTexture("swiss.png");
	textures[i++]=LoadTexture("stripes.png");
	
	fallingupInit();
	
	
	return 0;

}

int isOver() { return mode == OVER; }
int isHighscore() { return mode == HIGHSCORE; }
char* getLevelText() { return levelstring; }
int getLevel() { return level; }
int getScore() { return score; }
void showNextpiece(int show) { displayNext = show; }
void setSound(int show) { displaySound = show; }
void setTileset(int foo) { textureselected=foo; }
int isPlaying() { return mode != OVER && mode != INFO && mode != HIGHSCORE; }
int isShowingNext(){ return displayNext; }

int levelChanged() { if (prevlevel != level) { prevlevel = level; return true; } return false; }
int scoreChanged() { if (prevscore != score) { prevscore = score; return true; } return false; }

int getTileset() { return textureselected; };
int getSound() { return displaySound; }

int getTopScore(int i) { return topscores[i]; }
char* getTopName(int i) { return topnames[i]; }
int getTopLevel(int i) { return toplevels[i]; }
int getTopN() { return TOPN; }

// check to see if this is a high score ... if so, return place 
int checkScore(int s) {
	for (int i=0;i<TOPN;i++) {
		if (s > topscores[i]) return i+1;
	}
	return 0;
}

int handled=0;
int isHandled() { return handled; }
void setHandled(int i) { handled = i; }
void setHSName(char *name) { 
	topnames[replacescore] = (char*) malloc(sizeof(char) * (strlen(name)+1));
	strcpy(topnames[replacescore],name); 
}