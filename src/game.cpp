/*

	TWO VERY GOOD TUTORIALS ON OPENGL LIGHTING I SHOULD ACTUALLY FOLLOW NEXT
	GAME:::

	http://www.falloutsoftware.com/tutorials/gl/gl8.htm	

	http://www.sjbaker.org/steve/omniv/opengl_lighting.html

	seriously consider a red/blue stereoscopic mode??

*/

#include "game.h"

#include "timer.h"
#include "font.h"
#include "soundlib.h"

#include "enemy.h"
#include "asteroid.h"
#include "ship.h"
#include "shard.h"
#include "skybox.h"
#include "state.h"
#include "bullet.h"
#include "exhaust.h"
#include "explosion.h"
#include "highscores.h"
#include <cctype>
#include "upgrade.h"

class Powerup;

using std::string;
using std::cout;
using std::endl;
using std::vector;

#define VERSION_LEAD 650

/* GAME STATS */
int mode=0;
const int MODE_INTRO = 0;
const int MODE_TITLE = 1;
const int MODE_HIGHSCORES = 2;
const int MODE_NEXTLEVEL = 3;
const int MODE_CREDITS = 12;
const int MODE_PLAYING = 4;
const int MODE_OUTTRO = 5;
const int MODE_NEWHIGHSCORE = 6;
const int MODE_QUITVERIFY = 8;
const int MODE_DEMO = 9;
const int MODE_QUITVERIFY2 = 10;
const int MODE_GAMENEARLYOVER = 11;
const int MODE_GAMEOVER = 12;
Timer modetimer;

char *versiontext = GAME_VERSION;
char new_hs_name[80] = "";

int score=0;
int mostshards=0;
int highscore=0;
int level=0;
int shields=0;
float fps=0;
bool spawningalien=false;
float nextalien=0;
int nextenemy;

float attract_rot_dx=1;
float attract_rot_dy=1;
float attract_rot_x=0;
float attract_rot_y=0;

const int ATTRACT_SNOOZE = 10000;

bool show_playingfield=false;
bool playable=false;
bool attract=false;
bool press_spacebar=true;

char * titlescreen = 
"  Detritus is a fast-paced shoot-em-up\n"
"strongly based on the old classic Asteroids.\n"
" Replenish your shield with power-ups and\n"
"  gain new weapons.  \"Cleaners\" appear \n"
" periodically, so watch out!  The keyboard \n"
"   controls should be self-explanatory.\n"
;

HighScores *scores;
	
/* END GAME STATS */

//The attributes of the screen
int SCREEN_WIDTH; int SCREEN_HEIGHT;
const int resolutions[][2] = {{1280,1024},{1024,768},{800,600},{640,480}};
int resolutions_n = 4;

int SCREEN_WIDTH_NATIVE = -1;
int SCREEN_HEIGHT_NATIVE = -1;
const int SCREEN_ORTHO_WIDTH = 800; const int SCREEN_ORTHO_HEIGHT = 600;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 60;
const char* GAME_CAPTION = GAME_VERSION;
float mouse_x=-128, mouse_y=-128;

bool key_up = 0;
bool key_down = 0;
bool key_left = 0;
bool key_right = 0;
bool key_space = 0;
bool keyboard_cleared = 0;

State *state;

SDL_Surface *screen;
SDL_Event event;

//GLfloat LightAmbient[]= { .7f,.7f,.7f,1.0f };
GLfloat LightAmbient[]= { 0.0f,.0f,.0f,1.0f };
GLfloat LightAmbient1[]= { 0.5f,.5f,.5f,1.0f };
GLfloat LightDiffuse[]= { 1.0f,1.0f,1.0f,1.0f };
GLfloat LightPosition[]= { 0.0f, 0.0f, 0.0f,1.0f };
GLfloat LightLinearAttenuation= .05f;
GLfloat LightLinearAttenuation1= .3f;

char *definition = "\
de-tri-tus: \
\
	\\di-'trI-t&s\\, n. (( dee-TRY-tus ))\
\
	1. geological: substances (esp. rock) broken off from larger pieces by attrition; such as by ice, glacier, or erosion\
	2. biological: organic waste material by way of decomposition\
	3. debris or fragments left by any destructive process\
";


#ifdef DEBUG
Uint32 videoFlags = SDL_OPENGL|SDL_GL_ACCELERATED_VISUAL|SDL_NOFRAME;//|SDL_FULLSCREEN;//|SDL_FULLSCREEN|SDL_OPENGLBLIT|SDL_HWSURFACE;
#else
Uint32 videoFlags = SDL_OPENGL|SDL_GL_ACCELERATED_VISUAL|SDL_NOFRAME|SDL_FULLSCREEN;//|SDL_OPENGLBLIT|SDL_HWSURFACE;
#endif

void Draw3D(SDL_Surface *S);
void Draw2D(SDL_Surface *S);
void ReshapeGL(int width, int height); 
void Selection(void);
void initDemo(void);

void clean_up() {
	//Quit SDL 
	SDL_Quit(); 
#ifdef DEBUG_MYERROR
	((ofstream*)getErrorStream())->close();
#endif
}

//painful hack because I'm leaking memory I'm afraid to reclaim ... which means object counting isn't happening with enemies the way it ought
int getEnemyCount(void) {
	int count=0;
	vector<Asteroid*>::iterator j;
	j = state->asteroids.begin();
	while (j != state->asteroids.end()) {
		Asteroid *a = *j++;
		if (a->isEnemy()) count++;
	}
	return count;
}
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;

bool init() {
	
	//Start SDL 
	if (SDL_Init( SDL_INIT_EVERYTHING ) != 0) { // 0 is success!
		//SDL_INIT_EVERYTHING == TIMER, AUDIO, VIDEO, CDROM, JOYSTICK
		//not NOPARACHUTE, EVENTTHREAD
		cout << "SDL TIMER INIT FAILED" << endl;
		cout << "Failed to initialize SDL error=" << SDL_GetError() << endl;
		//return false;
	}
#ifdef OSX
//oh how I wish I didn't have to do this... issues with SDLMain.m...
CFBundleRef mainBundle = CFBundleGetMainBundle();
CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
char osxpath[PATH_MAX];
if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)osxpath, PATH_MAX)) {
	cerr << "ERROR CHANGING DIR" << endl;
}
CFRelease(resourcesURL);
chdir(osxpath);
#endif
//ofstream outfile("/tmp/debug");
//char foo[200]; getwd(foo); outfile << foo << endl;
//outfile.close();
	SDL_EnableUNICODE(true);
	SDL_putenv("SDL_VIDEO_WINDOW_POS=0,0");
	//Set the window caption
	SDL_WM_SetCaption(GAME_CAPTION,NULL);
	// 16-bit db with 16-bit db minim
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	//Set us up the screen
	
	/* find a screen resolution */
	int i=0;
	screen = NULL;
	//cout << "width: " << SDL_GetVideoInfo()->current_w << endl;
	//cout << "height: " << SDL_GetVideoInfo()->current_h << endl;
	screen = SDL_SetVideoMode(SDL_GetVideoInfo()->current_w,SDL_GetVideoInfo()->current_h,SCREEN_BPP,videoFlags);//SDL_SWSURFACE | SDL_SRCALPHA);// | SDL_FULLSCREEN); // SDL_GL_STEREO, SDL_GL_STENCIL_SIZE, (is CONFIG_REFLECTION -- how?)
	if (false) for (i=0;i<resolutions_n && screen == NULL;i++) {
		screen = SDL_SetVideoMode(resolutions[i][0],resolutions[i][1],SCREEN_BPP,videoFlags);//SDL_SWSURFACE | SDL_SRCALPHA);// | SDL_FULLSCREEN); // SDL_GL_STEREO, SDL_GL_STENCIL_SIZE, (is CONFIG_REFLECTION -- how?)
		//screen = SDL_SetVideoMode(0,0,SCREEN_BPP,videoFlags);//SDL_SWSURFACE | SDL_SRCALPHA);// | SDL_FULLSCREEN); // SDL_GL_STEREO, SDL_GL_STENCIL_SIZE, (is CONFIG_REFLECTION -- how?)

	}
	if (!screen) {
		cout << "UNABLE TO FIND A RESOLUTION TO SATISFY" << endl;
		return false;
	}

	SCREEN_WIDTH = resolutions[i][0];
	SCREEN_HEIGHT = resolutions[i][1];
	
	SCREEN_WIDTH_NATIVE = SDL_GetVideoInfo()->current_w;
	SCREEN_HEIGHT_NATIVE = SDL_GetVideoInfo()->current_h;

	//initialize OpenGL
	glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

/*
	glClearColor(0.0f,0.0f,0.1f,0.0f);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);

	// Really Nice Perspective Calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
*/
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);	
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightDiffuse);	
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,LightLinearAttenuation1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightDiffuse);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,LightLinearAttenuation);


	float global_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f }; // enable once the other light is working right
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,1.0f);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,1.0f);
	
	// Hide the mouse cursor
	SDL_ShowCursor(0);

	//SDL_FillRect(screen,NULL,SDL_MapRGBA(screen->format,0,0,0,0)); // ??

	//Initialize OpenGL?
	glClearColor(0.0f,0.0f,0.0f,0.5f);							// Black Background
	glClearDepth(1.0f);											// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing (Less Or Equal)
	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel(GL_SMOOTH);									// Select Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

	//call reshape gl...
	ReshapeGL(SCREEN_WIDTH_NATIVE,SCREEN_HEIGHT_NATIVE);
//char pwd[200];
//getwd(pwd);
//cerr << pwd << " ... argh?" << endl;

	state = &State::Get();
	state->ship = new Ship();
	state->skybox = new Skybox();

	//upgrade has to happen before HighScores() :)
	upgrade(); // up1 is moving detritus.db for pre 1.0.5 versions

	const char *path = getApplicationDataPath();
	char *datafile = "detritus.db";
	char *patheddatafile = new char[strlen(datafile) + strlen(path) + 1];
	strcpy(patheddatafile,path);
	strcat(patheddatafile,datafile);
	scores = new HighScores(patheddatafile);
	
	initDemo();
	return true;
}

void initDemo(void) {
	for (int i=0;i<20;i++) {
	//	state->asteroids.insert(state->asteroids.end(),new Enemy());
		state->asteroids.insert(state->asteroids.end(),new Asteroid(1.0f/((rand()%2)+1)));
	}

}

void startPlaying() {
	Asteroid::reset();
	attract = false;
	mode = MODE_PLAYING;
	show_playingfield = true;
	playable=true;
	state->thing.unpause();
	state->ship->reset();
	score = 0;
	mostshards = 0;
	state->clear_field();
	key_up = key_down = key_left = key_space = key_right = 0;
	keyboard_cleared = 0;
	level = 0;
	int asteroids = 3;
	state->clear_field(); //clear demo, f'instance

	nextenemy = (rand()%5000)+5000;
	for (int i=0;i<asteroids;i++) {
#ifdef DEBUG_ENEMIES
		state->asteroids.insert(state->asteroids.end(),new Enemy());
#else
		state->asteroids.insert(state->asteroids.end(),new Asteroid());
#endif
	}
	spawningalien=false;
	nextalien=0;
	state->startRelativeSound(SOUND_KLAXON);
}
void stopPlaying() {
	attract = true;
	show_playingfield = false;
	playable=false;
	state->thing.pause();
	pauseSound(SOUND_ENEMY);
	state->ship->pause();
	stopSound(SOUND_KLAXON);
	
	//and delete all those *#$(* state buffers! WHOO!
	state->clear_field();
	initDemo();
}
void pausePlaying(bool pause=true) {
	if (pause) {
		if (getEnemyCount() > 0) {
			pauseSound(SOUND_ENEMY);
		}
		state->ship->pause();
		playable=false;
		state->thing.pause();
	} else {
		if (getEnemyCount() > 0) {
			resumeSound(SOUND_ENEMY);
		}
		state->ship->pause(false);
		playable=true;
		state->thing.unpause();
	}
}

//SDL being lame???
//#if (defined(__WIN32__) || defined(WINDOWS)) && defined(main)
//#undef main
//#endif

int main( int argc, char* args[] ) {

	srand((unsigned)time(NULL));
	//Make sure the program waits for a quit
	bool quit = false;
	Timer spacebartimer;
	Timer endofleveltimer;

	//Initialize
	if (!init()) {
		cout << "FAILED INIT" << endl;
		return EXIT_FAILURE;
	}
	state->thing.pause(); // start off not iterating the game
	
	mode = MODE_INTRO;
	playable = false;
	show_playingfield = false;
	modetimer.setAlarm(5000);

	//While the user hasn't quit
	while (!quit) {
		//While there's an event to handle

		if (attract || mode == MODE_OUTTRO) {
			if (spacebartimer.checkAlarm()) {
				press_spacebar = !press_spacebar;
				spacebartimer.snoozeAlarm(press_spacebar?3000:1000);
			}
		}

		switch (mode) {
			case MODE_INTRO:
				if (modetimer.checkAlarm()) {
					modetimer.snoozeAlarm(ATTRACT_SNOOZE);
					attract = true;
					spacebartimer.setAlarm(3000,1000);
					mode = MODE_TITLE;
					continue;
				}
				break; /* end MODE_INTRO */
			case MODE_TITLE:
				if (modetimer.checkAlarm()) {
					modetimer.snoozeAlarm(ATTRACT_SNOOZE);
					mode = MODE_HIGHSCORES; // was: MODE_DEMO
					continue;
				}
				break; /* end MODE_TITLE */
			case MODE_DEMO:
				if (modetimer.checkAlarm()) {
					modetimer.snoozeAlarm(ATTRACT_SNOOZE);
					mode = MODE_HIGHSCORES;
					continue;
				}
				break; /* end MODE_DEMO */
			case MODE_HIGHSCORES:
				if (modetimer.checkAlarm()) {
					modetimer.snoozeAlarm(ATTRACT_SNOOZE);
					mode = MODE_TITLE;
					continue;
				}
				break; /* end MODE_HIGHSCORES */
			case MODE_PLAYING:
				if (state->asteroids.size() == 0) {
					if (!endofleveltimer.is_started()) {
						endofleveltimer.setAlarm(1500);
					} else if (endofleveltimer.checkAlarm()) {
						endofleveltimer.stop();
						pausePlaying();
						mode = MODE_NEXTLEVEL;
						modetimer.setAlarm(4000);
						state->ship->setThrust(false,false);
						state->startRelativeSound(SOUND_LEVELUP);
					}
				}
				break; /* end MODE_PLAYING */
			case MODE_NEXTLEVEL:
				if (modetimer.checkAlarm()) {
					level++;
					score += mostshards*10;
					mostshards = 0;
					modetimer.clearAlarm();
					pausePlaying(false);
					mode = MODE_PLAYING;
					int asteroids = 3 + level * 2;
					if (asteroids > 13) asteroids = 13;
					spawningalien=false;
					nextalien=0;

					state->clear_field();
					
					for (int i=0;i<asteroids;i++) {
						state->asteroids.insert(state->asteroids.end(),new Asteroid());
					}

					nextenemy = score + (rand()%5000)+5000;
					state->ship->leveled();
					state->startRelativeSound(SOUND_KLAXON);
					continue;
				}
			case MODE_OUTTRO:
				if (modetimer.checkAlarm()) {
					modetimer.clearAlarm();
					quit = true;
					continue;
				}
				break; /* end MODE_OUTTRO */
			case MODE_NEWHIGHSCORE:
				//wait on input
				break; /* end MODE_NEWHIGHSCORE */
			case MODE_GAMENEARLYOVER:
				if (modetimer.checkAlarm()) {
					modetimer.clearAlarm();
					modetimer.setAlarm(2000);
					mode = MODE_GAMEOVER;
					continue;
				}
                break;
			case MODE_GAMEOVER:
				if (modetimer.checkAlarm()) {
					modetimer.clearAlarm();
					if (scores->isHighScore(score)) {
						new_hs_name[0]=0;
						mode = MODE_NEWHIGHSCORE;
					} else {
						mode = MODE_TITLE;
						stopPlaying();
						modetimer.clearAlarm();
						modetimer.setAlarm(ATTRACT_SNOOZE);
					}
					continue;
				}
				break;
		}


		//Start the frame timer
		state->fps.start();

		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || 
#ifdef WINDOWS
				(event.key.keysym.sym == SDLK_F4 && (event.key.keysym.mod & KMOD_LALT) )
#endif
#ifdef OSX
				(event.key.keysym.sym == 'q' && (event.key.keysym.mod & KMOD_META) )
#endif
#ifdef LINUX
				(event.key.keysym.sym == 'q' && (event.key.keysym.mod & KMOD_CTRL) )
#endif				
				) {
				quit = true;
			} else switch (event.type) {
				case SDL_KEYUP: 
					switch (event.key.keysym.sym) {
						case SDLK_UP: if (playable) state->ship->setThrust(false); break;
						case SDLK_LEFT: if (playable) state->ship->setBank(0); break;
						case SDLK_RIGHT: if (playable) state->ship->setBank(0); break;
						case ' ': if (playable) state->ship->fire(false); break;
						default:
							// take this out, and compile with -Wall to see all options here ;)
						break;
					}
					break;
				case SDL_KEYDOWN: 
					if (mode == MODE_NEWHIGHSCORE) {
						char key = event.key.keysym.unicode;
						if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) continue;
						if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL) continue;
						if (event.key.keysym.sym == SDLK_LMETA || event.key.keysym.sym == SDLK_RMETA) continue;
						if (event.key.keysym.sym == SDLK_LALT || event.key.keysym.sym == SDLK_RALT) continue;
						int len = strlen(new_hs_name);
						if (key == 13 || key == 10) {
							if (len > 0) {
								scores->addScore(new_hs_name,score);
								startSound(SOUND_ASTEROID_BOOM);
								mode=MODE_HIGHSCORES;
								stopPlaying();
								modetimer.clearAlarm();
								modetimer.setAlarm(ATTRACT_SNOOZE);
							} else {
								startSound(SOUND_THRUSTFADE);
							}
						} else if (key == 8 || key == 16 || key == 127) {
							if (len == 0) {
								startSound(SOUND_THRUSTFADE);
							} else {
								new_hs_name[len-1] = 0;
								startSound(SOUND_BULLET);
							}
						} else if (len == 32 || !isprint(key)) {
							startSound(SOUND_THRUSTFADE);
						} else {
							new_hs_name[len] = key;
							new_hs_name[len+1] = 0;
							startSound(SOUND_BULLET);
						}
					} else {
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE: case 'q': 
							if (mode == MODE_QUITVERIFY) {
								mode = MODE_PLAYING;
								pausePlaying(false);
							} else if (mode == MODE_QUITVERIFY2) {
								mode = MODE_TITLE;
							} else if (attract) {
								mode = MODE_QUITVERIFY2; 
							} else if (playable) {
								mode = MODE_QUITVERIFY; 
								pausePlaying();
							}
							break;
						case '`': if (mode == MODE_PLAYING) state->ship->collision(); break;
						case 'y':
							if (mode == MODE_QUITVERIFY) {
								mode = MODE_TITLE;
								stopPlaying();
							} else if (mode == MODE_QUITVERIFY2) {
								mode = MODE_OUTTRO;
								modetimer.setAlarm(15000);
								attract = false;
							}
							break;
						case 'n':
							if (mode == MODE_QUITVERIFY) {
								mode = MODE_PLAYING;
								pausePlaying(false);
							} else if (mode == MODE_QUITVERIFY2) {
								mode = MODE_TITLE;
							}
							break;
						case SDLK_UP: 
							if (playable) {
								state->ship->setThrust(true); key_up = 1;
							}
							break;
						case SDLK_DOWN: 
							if (playable) {
								state->ship->flip(); key_down = 1; 
							}
							break;
						case SDLK_LEFT: 
							if (playable) {
								state->ship->setBank(-1); key_left = 1;
							}
							break;
						case SDLK_RIGHT: 
							if (playable) {
								state->ship->setBank(1); key_right = 1; 
							}
							break;
						case ' ': 
							if (playable) {
								state->ship->fire(); key_space = 1; 
							} else if (attract) {
								startPlaying();
							} else if (mode == MODE_GAMEOVER || mode == MODE_INTRO || mode == MODE_OUTTRO) {
								modetimer.setAlarm(1);
							}
							break;
						case '\t': 
							if (playable) {
								state->ship->setViewport(-1);  
							}
							break;
						case 'h': 
							if (playable) {
								state->ship->teleport();
							}
							break;
						default:
							// take this out, and compile with -Wall to see all options here ;)
						break;
					}}
					break; 
/*
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						mouse_x = event.button.x;
						mouse_y = event.button.y;
						Selection();
					}
					break;
				case SDL_MOUSEMOTION:
					mouse_x = event.button.x;
					mouse_y = event.button.y;
					break;
*/
				case SDL_VIDEORESIZE:
					ReshapeGL(event.resize.w,event.resize.h);
					break;
			}
		}

		//Apply the background to the screen -- origin at top left!
		Draw3D(screen);
		Draw2D(screen);
		state->frame++;

		//Cap the frame rate
		while (state->fps.get_ticks() < 1000.0f/FRAMES_PER_SECOND) {
			//wait
			SDL_Delay(10); // let other things happen!
		}
		if (state->update.get_ticks() > 1000) {
			//A temp string
			char caption[64];

			//Calculate the frames per second and create the string
			fps = (float)state->frame/(state->update.get_ticks()/1000.0f);
			sprintf(caption,"%s (%f fps)",GAME_CAPTION,fps);

			//Reset the caption
			SDL_WM_SetCaption(caption,NULL);

			//Restart the update timer
			state->update.start();
			state->frame = 0;
		}

		SDL_GL_SwapBuffers();
	}

	//Free the surface(s) and quit SDL
	clean_up();

	//Return
	return 0; 
}


void Draw3D(SDL_Surface *S)	{
	static float ticks = state->thing.get_ticks();
	static float attract_ticks = state->thing2.get_ticks();
	float stateticks; 
	float tickdiff;
	float attract_stateticks;
	float attract_tickdiff;

	vector<Asteroid*>::iterator j;
	vector<Shard*>::iterator k;
	vector<Exhaust*>::iterator m;
	vector<Bullet*>::iterator l;
	vector<Powerup*>::iterator n;
	vector<Explosion*>::iterator o;

	stateticks = state->thing.get_ticks();
	tickdiff = stateticks - ticks;
	ticks = stateticks;
	
	attract_stateticks = state->thing2.get_ticks();
	attract_tickdiff = attract_stateticks-attract_ticks;
	attract_ticks = attract_stateticks;

	if (playable || show_playingfield) {

if (playable) {
		/* ITERATE SCENE */
		state->ship->iterate(tickdiff);

		j = state->asteroids.begin();
		while (j != state->asteroids.end()) {
			Asteroid *a = *j++;
			a->iterate(tickdiff);
		}

		m = state->exhaust.begin();
		while (m != state->exhaust.end()) {
			Exhaust *a = *m++;
			a->iterate(tickdiff);
		}

		n = state->powerups.begin();
		while (n != state->powerups.end()) {
			Powerup *a = *n++;
			a->iterate(tickdiff);
		}
}
if (state->shards.size() > (uint)mostshards) mostshards = state->shards.size();
		k = state->shards.begin();
		while (k != state->shards.end()) {
			Shard *a = *k++;
			a->iterate(tickdiff);
		}


		o = state->explosions.begin();
		while (o < state->explosions.end()) {
			Explosion *a = *o++;
			a->iterate(tickdiff);
		}
if (playable) {
		l = state->bullets.begin();
		while (l != state->bullets.end()) {
			Bullet *a = *l++;
			a->iterate(tickdiff);
			j = state->asteroids.begin();
			while (j < state->asteroids.end()) {
				Asteroid *b = *j++;
				if (a->collision(*b)) {
					score+=(int)(b->getHits() * 200);
					if (score > nextenemy) {
						if (getEnemyCount() < 1) {
							XYZ position = state->ship->getPosition();
							position.scale(-1);
							Asteroid *a = new Enemy(position);
							state->asteroids.insert(state->asteroids.end(),a);
						}
						spawningalien=false;
						nextenemy = score + (rand()%5000)+5000;
					}
					break;
				}
			}
		}

		l = state->enemybullets.begin();
		while (l != state->enemybullets.end()) {
			Bullet *a = *l++;
			a->iterate(tickdiff);
			if (state->ship->collision(*a)) {
				break;
			}
			j = state->asteroids.begin();
			while (j < state->asteroids.end()) {
				Asteroid *b = *j++;
				if (!b->isEnemy() && a->collision(*b)) {
					break;
				}
			}
		}
			
		n = state->powerups.begin();
		if (mode != MODE_GAMEOVER)
		while (n < state->powerups.end()) {
			Powerup *b = *n++;
			state->ship->collision(*b);
		}

		j = state->asteroids.begin();
		if (mode != MODE_GAMEOVER)
		while (j < state->asteroids.end()) {
			Asteroid *b = *j++;
			if (state->ship->collision(*b)) {
			}
		}
		
		if (state->ship->hasExploded()) {
			if (score > highscore) highscore = score;
			mode = MODE_GAMENEARLYOVER;
			modetimer.setAlarm(2000);
			state->ship->halt();
			playable=false;
			return;
		}
}	
		/* CLEAR DEAD ELEMENTS */
		if (state->remove_asteroids.size() > 0) {
			for (unsigned int i=0;i<state->remove_asteroids.size();i++) {
				for (unsigned int l=0;l<state->asteroids.size();l++) {
					if (state->asteroids[l] == state->remove_asteroids[i]) {
						state->asteroids.erase(state->asteroids.begin()+l);
						break;
					}
				}
			}
			state->remove_asteroids.erase(state->remove_asteroids.begin(),state->remove_asteroids.end()); // because we can't have random destruction of our remove_asteroids items via clear(), because ... this code is alllllllll messed up.
		}
	
		if (state->remove_powerups.size() > 0) {
			for (unsigned int i=0;i<state->remove_powerups.size();i++) {
				for (unsigned int l=0;l<state->powerups.size();l++) {
					if (state->powerups[l] == state->remove_powerups[i]) {
						state->powerups.erase(state->powerups.begin()+l);
						break;
					}
				}
			}
			state->remove_powerups.erase(state->remove_powerups.begin(),state->remove_powerups.end()); // because we can't have random destruction of our remove_powerups items via clear(), because ... this code is alllllllll messed up.
		}
	
		if (state->remove_exhaust.size() > 0) {
			for (unsigned int i=0;i<state->remove_exhaust.size();i++) {
				for (unsigned int l=0;l<state->exhaust.size();l++) {
					if (state->exhaust[l] == state->remove_exhaust[i]) {
						state->exhaust.erase(state->exhaust.begin()+l);
						break;
					}
				}
			}
			state->remove_exhaust.erase(state->remove_exhaust.begin(),state->remove_exhaust.end()); // because we can't have random destruction of our remove_exhaust items via clear(), because ... this code is alllllllll messed up.
		}

		if (state->remove_explosions.size() > 0) {
			for (unsigned int i=0;i<state->remove_explosions.size();i++) {
				for (unsigned int l=0;l<state->explosions.size();l++) {
					if (state->explosions[l] == state->remove_explosions[i]) {
						state->explosions.erase(state->explosions.begin()+l);
						break;
					}
				}
			}
			state->remove_explosions.erase(state->remove_explosions.begin(),state->remove_explosions.end()); // because we can't have random destruction of our remove_explosions items via clear(), because ... this code is alllllllll messed up.
		}
	
		if (state->remove_shards.size() > 0) {
			for (unsigned int i=0;i<state->remove_shards.size();i++) {
				for (unsigned int l=0;l<state->shards.size();l++) {
					if (state->shards[l] == state->remove_shards[i]) {
						state->shards.erase(state->shards.begin()+l);
						break;
					}
				}
			}
			state->remove_shards.erase(state->remove_shards.begin(),state->remove_shards.end()); // because we can't have random destruction of our remove_shards items via clear(), because ... this code is alllllllll messed up.
		}
	
		if (state->remove_bullets.size() > 0) {
			for (unsigned int bi=0;bi<state->remove_bullets.size();bi++) {
				for (unsigned int bl=0;bl<state->bullets.size();bl++) {
					if (state->bullets[bl] == state->remove_bullets[bi]) {
						state->bullets.erase(state->bullets.begin()+bl);
						break;
					}
				}
			}
			state->remove_bullets.erase(state->remove_bullets.begin(),state->remove_bullets.end()); // because we can't have random destruction of our remove_bullets items via clear(), because ... this code is alllllllll messed up.
		}

		if (state->remove_enemybullets.size() > 0) {
			for (unsigned int bi=0;bi<state->remove_enemybullets.size();bi++) {
				for (unsigned int bl=0;bl<state->enemybullets.size();bl++) {
					if (state->enemybullets[bl] == state->remove_enemybullets[bi]) {
						state->enemybullets.erase(state->enemybullets.begin()+bl);
						break;
					}
				}
			}
			state->remove_enemybullets.erase(state->remove_enemybullets.begin(),state->remove_enemybullets.end()); // because we can't have random destruction of our remove_bullets items via clear(), because ... this code is alllllllll messed up.
		}

		
		//enemies!  if there's 2 or less "asteroids", check to see if it's time to spawn an enemy :)  But don't if we just cleared the board.
		//LOGIC: if there's more than 2 asteroids, reset "spawningalien" to 0
		//if !spawningalien && <= 2 asteroids, spawningalien = true and start countdown.
		//if spawningalien && countdown expired, spawn alien! :)
		if (state->asteroids.size() > 2 || state->asteroids.size() == 0) {
			spawningalien = false;
		} else {
			if (getEnemyCount() < 1) {
				if (!spawningalien) { 
					spawningalien = true; 
					nextalien = ((rand() % 10) + 5) * 1000;  // 5 to 15 seconds :) - maybe count number of enemies and not do more than N per level to limit abuse?
				} else { 
					nextalien -= tickdiff; 
					if (nextalien < 0) { 
						XYZ position = state->ship->getPosition();
						position.scale(-1);
						Asteroid *a = new Enemy(position);
						state->asteroids.insert(state->asteroids.end(),a); 
						spawningalien=false;
					} 
				}
			} else {
				spawningalien = false;
			}
		}
		
		
	} // end if playable

	/* DRAW SCENE */
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

//I'm not drawing things with proper chirality, it seems; and this doesn't seem to save much time, either
//glCullFace(GL_BACK);
//glEnable(GL_CULL_FACE);
	glLoadIdentity();  // Reset The Modelview Matrix

	glPushMatrix();

	if (attract || show_playingfield) {

		/* TODO: shouldn't these all be just one list?  I need to think out how everything interacts, more */

		if (attract) { // menu view, figure this out differently???
/* ITERATE DEMO */
		j = state->asteroids.begin();
		while (j != state->asteroids.end()) {
			Asteroid *a = *j++;
			a->iterate(attract_tickdiff);
		}

			glRotatef(attract_rot_x,0,1,0);
			glRotatef(attract_rot_y,0,0,1);
			attract_rot_x+=attract_rot_dx*(attract_tickdiff/200.f);
			attract_rot_y+=attract_rot_dy*(attract_tickdiff/200.f);
		} else {
			state->ship->orientGLViewport();
		}
		state->skybox->render();
	}
	if (show_playingfield || attract) {
		glEnable(GL_LIGHTING);

		if (show_playingfield) state->ship->setLighting();

glPushAttrib(GL_ALL_ATTRIB_BITS);
		j = state->asteroids.begin();
		while (j != state->asteroids.end()) {
			Asteroid *a = *j++;
			glPushMatrix();
				a->render();
			glPopMatrix();
		}
glPopAttrib();
	}
	if (show_playingfield) {
glPushAttrib(GL_ALL_ATTRIB_BITS);
		l = state->bullets.begin();
		while (l != state->bullets.end()) {
			Bullet *a = *l++;
			glPushMatrix();
				a->render();
			glPopMatrix();
		}
		l = state->enemybullets.begin();
		while (l != state->enemybullets.end()) {
			Bullet *a = *l++;
			glPushMatrix();
				a->render();
			glPopMatrix();
		}
glPopAttrib();
glPushAttrib(GL_ALL_ATTRIB_BITS);
		k = state->shards.begin();
		while (k != state->shards.end()) {
			Shard *a = *k++;
			glPushMatrix();
				a->render();
			glPopMatrix();
		}
glPopAttrib();
//glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (mode != MODE_GAMEOVER && mode != MODE_GAMENEARLYOVER && mode != MODE_NEWHIGHSCORE) {
			glPushMatrix();
				state->ship->render();
			glPopMatrix();
		}
//glPopAttrib();
glPushAttrib(GL_ALL_ATTRIB_BITS);
		m = state->exhaust.begin();
		while (m != state->exhaust.end()) {
			Exhaust *a = *m++;
			glPushMatrix();
				a->render();
			glPopMatrix();
		}
glPopAttrib();
glPushAttrib(GL_ALL_ATTRIB_BITS);
		n = state->powerups.begin();
		while (n != state->powerups.end()) {
			Powerup *a = *n++;
			glPushMatrix();
				a->render();
			glPopMatrix();
		}
glPopAttrib();
glPushAttrib(GL_ALL_ATTRIB_BITS);
		o = state->explosions.begin();
		while (o != state->explosions.end()) {
			Explosion *a = *o++;
			glPushMatrix();
				a->render();
			glPopMatrix();
		}
glPopAttrib();

	} // end show_playingfield

	glPopMatrix();

	glFlush();													// Flush The GL Rendering Pipelines

}

void draw2DImage(GLuint tex, GLfloat *texcoords, float x, float y, float width, float height,int clockwise=0) { /* 0, 90, 180, 270 are valid */
	int zero, one, two, three;
	glBindTexture(GL_TEXTURE_2D,tex);
	glPushMatrix();
	glTranslatef(x,y, 0.0);
	switch (clockwise) {
		case 90: zero = 2; one = 3; two = 0; three = 1; break;
		case 180: zero = 2; one = 3; two = 0; three = 1; break;
		case 270: zero = 0; one = 1; two = 2; three = 3; break;
		default: zero = 0; one = 1; two = 2; three = 3; break;
	}
	if (clockwise == 90 || clockwise == 270) {
		glBegin(GL_QUADS);
			glTexCoord2f(texcoords[two],texcoords[one]);
			glVertex2f(0,0);
			glTexCoord2f(texcoords[two],texcoords[three]);
			glVertex2f(width,0);
			glTexCoord2f(texcoords[zero],texcoords[three]);
			glVertex2f(width,height);
			glTexCoord2f(texcoords[zero],texcoords[one]);
			glVertex2f(0,height);
		glEnd();
		glPopMatrix();
	} else {
		glBegin(GL_QUADS);
			glTexCoord2f(texcoords[zero],texcoords[one]);
			glVertex2f(0,0);
			glTexCoord2f(texcoords[two],texcoords[one]);
			glVertex2f(width,0);
			glTexCoord2f(texcoords[two],texcoords[three]);
			glVertex2f(width,height);
			glTexCoord2f(texcoords[zero],texcoords[three]);
			glVertex2f(0,height);
		glEnd();
		glPopMatrix();
	}
}

void Draw2D(SDL_Surface *S) {
	char print_score[80];
	//char print_high[80];
	char print_level[80];
	char print_asteroids[80];
	char print_shield[80];
	//char print_mostshards[80];
	//char print_debug[80];
	
	//..Draw all 2d information
	glClear (GL_DEPTH_BUFFER_BIT);
	
	float attract_ticks = state->thing2.get_ticks();
	
	glDepthMask(GL_FALSE);

	glDisable(GL_LIGHTING);
	static SDL_Rect src1={0,0,0,0};
	SDL_FillRect(S, &src1, SDL_MapRGBA(S->format,0,0,0,0));
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//change the perspective to align with the window inorder to use 
	//more natrual exact window coords.

	glOrtho(0,SCREEN_ORTHO_WIDTH,SCREEN_ORTHO_HEIGHT,0,200,-200);

	//clear Modelview matrix for new 2d drawing commands
	glMatrixMode(GL_MODELVIEW);


	if (show_playingfield) {
		sprintf(print_score,"Score: %d",score);
		if (state->ship->invulnerable) {
			sprintf(print_shield,"Shield: %d%% (%d msecs)",state->ship->getShieldStrength(),(int)state->ship->invulnerable);
		} else {
			sprintf(print_shield,"Shield: %d%%",state->ship->getShieldStrength());
		}
		sprintf(print_level,"Sector: 0x%04x",level);
		//sprintf(print_high,"High score: %d",highscore);
		sprintf(print_asteroids,"Asteroids: %d",(int)state->asteroids.size());
    	//sprintf(print_mostshards,"Most shards: %d",mostshards);
    	
    	//sprintf(print_debug,"spawning: %s, next: %f",spawningalien?"yes":"no",nextalien);

		//TODO: better HUD
		state->smallfont->drawText(print_shield,10,10);
		state->smallfont->drawText(print_level,10,30);
		state->smallfont->drawText(print_score,10,50);
		state->smallfont->drawText(print_asteroids,10,70);
		//state->smallfont->drawText(print_mostshards,10,90);
		//state->smallfont->drawText(print_high,10,110);
		
		//state->smallfont->drawText(print_debug,10,150);		
		
		state->smallfont->drawText(versiontext,VERSION_LEAD,10);
		//state->font->drawText("\n\n'q' or 'ESC' to quit",440,30); // x, y

		/*
		sprintf(state->debug,"fps: %f",fps);
		if (strlen(state->debug) > 0) {
			state->smallfont->drawText(state->debug,10,130); // x, y
		}
		*/

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); // (alt. GL_REPLACE); MODULATE IS DEFAULT

		/** DRAW KEYBOARD **/
		if (!key_space || !key_up || !key_down || !key_left || !key_right) {
			if (key_space) glColor4f(1,1,1,.3f); else glColor4f(1,1,1,.7f);
			draw2DImage(state->keyboard_space_tex,state->keyboard_space_tex_coord,220,558,276,31,0);
			if (key_left) glColor4f(1,1,1,.3f); else glColor4f(1,1,1,.7f);
			draw2DImage(state->keyboard_arrow_tex,state->keyboard_arrow_tex_coord,617,558,31,31,270); // left arrow
			if (key_down) glColor4f(1,1,1,.3f); else glColor4f(1,1,1,.7f);
			draw2DImage(state->keyboard_arrow_tex,state->keyboard_arrow_tex_coord,658,558,31,31,180); // down arrow
			if (key_right) glColor4f(1,1,1,.3f); else glColor4f(1,1,1,.7f);
			draw2DImage(state->keyboard_arrow_tex,state->keyboard_arrow_tex_coord,699,558,31,31,90); // right arrow
			if (key_up) glColor4f(1,1,1,.3f); else glColor4f(1,1,1,.7f);
			draw2DImage(state->keyboard_arrow_tex,state->keyboard_arrow_tex_coord,658,517,31,31); // up arrow
		} else if (!keyboard_cleared) {
			keyboard_cleared = 1;
			score += 1000;
		}
		/** END DRAW KEYBOARD **/

		/** DRAW RADAR BACKGROUND **/
		glDisable(GL_DEPTH_TEST);									// Enable Depth Testing
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
			glDisable(GL_TEXTURE_2D);
			glTranslatef(100,500,0);
			glColor4f(.6,1,.6,.4);
			const float DEG2RAD = 3.14159/180;
			glBegin(GL_TRIANGLE_FAN);
				for (int i=0;i<360;i+=10) {
					float rads = i*DEG2RAD;
					glVertex2f(cos(rads)*80,sin(rads)*80);
				}
			glEnd(); // GL_TRIANGLE_FAN

    		glColor4f(.8,0,0,1);
    		glBegin(GL_TRIANGLE_FAN);
				for (int i=0;i<360;i+=10) {
					float rads = i*DEG2RAD;
					glVertex2f(cos(rads)*2,sin(rads)*2);
				}
			glEnd(); // GL_TRIANGLE_FAN
			glColor4f(.4,0,0,1);
			glLineWidth(1);
			glPointSize(1);
			glBegin(GL_LINE_LOOP);
				for (int i=0;i<360;i+=10) {
					float rads = i*DEG2RAD;
					glVertex2f(cos(rads)*2,sin(rads)*2);
				}
			glEnd(); // GL_POINTS


			glColor4f(.1,.4,.1,.4);
			glLineWidth(20);
			glPointSize(1);
			glBegin(GL_LINE_LOOP);
				for (int i=0;i<360;i+=10) {
					float rads = i*DEG2RAD;
					glVertex2f(cos(rads)*80,sin(rads)*80);
				}
			glEnd(); // GL_POINTS


			glScalef(6,6,6);

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_COLOR_MATERIAL);
			glDisable(GL_LIGHTING);

			glColor4f(0,.3,0,.8);
			//circle
			glPointSize(4);
			glBegin(GL_POINTS);
				for (int i=0;i<360;i+=10) {
					float rads = i*DEG2RAD;
					glVertex2f(cos(rads)*10,sin(rads)*10);
				}
				for (int i=0;i<360;i+=20) {
					float rads = i*DEG2RAD;
					glVertex2f(cos(rads)*7,sin(rads)*7);
				}
				for (int i=0;i<360;i+=40) {
					float rads = i*DEG2RAD;
					glVertex2f(cos(rads)*4,sin(rads)*4);
				}
			glEnd(); // GL_POINTS

			//rotate to put things around the CENTER of the radar!?

			/* OH MY */
			XYZ tempv;
			XYZ p = state->ship->orientation;

			tempv = state->ship->getPosition();
			gluLookAt(
				tempv.r3[0], // eye x
				tempv.r3[1], // eye y
				tempv.r3[2], // eye z
				0, // center x
				0, // center y
				0, // center z
				-p.r3[0], // up x
				-p.r3[1], // up y
				-p.r3[2]); // up z
			/* END OH MY */

			glPointSize(12);

			//circle


   			//ship
   			p = state->ship->getPosition();
   			XYZ distanceVector = p;
   			XYZ diff;
			
			//asteroids
			vector<Asteroid*>::iterator j = state->asteroids.begin();
			while (j != state->asteroids.end()) {

				Asteroid *a = *j++;
				p = a->getPosition();
				diff = distanceVector; // distance goes from 0 to 24, ish
				diff.translate(&p,-1);
				float distance = diff.getMagnitude();
				float color = 1-distance/24;
				if (distance > 24) distance = 24;
				glColor4f(color,color,color,1);
				glPointSize(4+(color*5));
				color = (-.5 * cos(PI * color)) + .5;
    			glBegin(GL_POINTS);
      				glVertex3f(p.r3[0],p.r3[1],p.r3[2]);
	       		glEnd();
			}

			
			//powerups
			vector<Powerup*>::iterator n = state->powerups.begin();
			glColor4f(0,0,1,.8);
			while (n != state->powerups.end()) {
				Powerup *a = *n++;
				p = a->getPosition();
				diff = distanceVector; // distance goes from 0 to 24, ish
				diff.translate(&p,-1);
				float distance = diff.getMagnitude();
				float color = 1-distance/24;
				if (distance > 24) distance = 24;
				glColor4f(0,0,color,1);
				glPointSize(4+(color*5));
				color = (-.5 * cos(PI * color)) + .5;
				glBegin(GL_POINTS);
    				glVertex3f(p.r3[0],p.r3[1],p.r3[2]);
				glEnd(); //GL_POINTS
			}

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
		glPopMatrix();
	glPopAttrib();

	} // end show_playingfield

	if (mode == MODE_INTRO) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); // (alt. GL_REPLACE); MODULATE IS DEFAULT
		glColor4f(1,1,1,1.0f);
		draw2DImage(state->splash_tex,state->splash_tex_coord,0,0,800,600); // splash!
		glPopAttrib();
		state->smallfont->drawText(versiontext,VERSION_LEAD,580);
	} else if (mode == MODE_OUTTRO) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); // (alt. GL_REPLACE); MODULATE IS DEFAULT
		glColor4f(1,1,1,1.0f);
		draw2DImage(state->outtro_tex,state->outtro_tex_coord,0,0,800,600); // splash!
		if (press_spacebar) {
			state->smallfont->drawText("Press [spacebar] to quit",150,555);
		}
		glPopAttrib();
	} else if (mode == MODE_NEWHIGHSCORE) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); // (alt. GL_REPLACE); MODULATE IS DEFAULT

		glColor4f(0,0,0,.5f);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(0,600);
			glVertex2f(800,600);
			glVertex2f(800,0);
		glEnd();
		glColor4f(1,1,1,1.0f);
		state->font->drawText("WHOO! HIGH SCORE!",210,120);
		state->font->drawText("ENTER YOUR NAME",260,200);	
		char tmp[80];
		strcpy(tmp,new_hs_name);
		if ((int)(attract_ticks / 750) % 2) {
			tmp[strlen(tmp)]='_';
			tmp[strlen(new_hs_name)+1]=0;
		}
		state->font->drawText(tmp,270,350);
		glPopAttrib();
	} else if (attract) {
//		glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
//		glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); // (alt. GL_REPLACE); MODULATE IS DEFAULT
		glColor4f(1,1,1,1.0f);
		draw2DImage(state->title_tex,state->title_tex_coord,0,0,800,600); // splash!
		glPopAttrib();
		if (mode == MODE_TITLE) {
			state->font->drawText(titlescreen,60,240);			
		} else if (mode == MODE_HIGHSCORES) {
			state->font->drawText("HIGH SCORES",260,200);			
			const HighScore *hs = scores->getScores();
			char hsdummy[80];
			int x=195, y=280;
			for (int i=0;i<10;i++) {
				sprintf(hsdummy,"%d",i+1);
				state->smallfont->drawText(hsdummy,x,y+i*25);
				if (hs == NULL) {
					state->smallfont->drawText("---",x+60,y+i*25);
					state->smallfont->drawText("---",x+350,y+i*25);
				} else {
					strncpy(hsdummy,hs->getName(),79);
					hsdummy[79]=0;
					state->smallfont->drawText(hsdummy,x+60,y+i*25);
					sprintf(hsdummy,"%d",hs->getScore());
					state->smallfont->drawText(hsdummy,x+350,y+i*25);
					hs = hs->getNext();
				}
			}
		} else if (mode == MODE_DEMO) {
			state->smallfont->drawText("DEMO: coming soon?",60,270);	// eh, probably not. :)		
		}
		
		if (press_spacebar) {
			state->smallfont->drawText("Press [spacebar] to play",283,545);
		}
		state->smallfont->drawText(versiontext,VERSION_LEAD,580);
		state->smallfont->drawText("'q' to quit",10,580);
		
		
	}

	if (mode == MODE_QUITVERIFY || mode == MODE_QUITVERIFY2 || mode == MODE_GAMEOVER || mode == MODE_NEXTLEVEL  || mode == MODE_GAMENEARLYOVER) {
		glDisable(GL_TEXTURE_2D);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0,0,0,.5f);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(0,600);
			glVertex2f(800,600);
			glVertex2f(800,0);
		glEnd();
		glPopAttrib();
		if (mode == MODE_QUITVERIFY) {
			state->font->drawText("End Game? y/n",300,280);
		} else if (mode == MODE_QUITVERIFY2) {
			state->font->drawText("Really Quit? y/n",300,280);
		} else if (mode == MODE_GAMEOVER || mode == MODE_GAMENEARLYOVER) {
			if (scores->isHighScore(score)) {
				state->font->drawText("WHOO! HIGH SCORE!",210,280);
			} else {
				state->font->drawText("GAME OVER, MAN!",300,280);
			}
		} else if (mode == MODE_NEXTLEVEL) {
			sprintf(print_level,"       SECTOR CLEARED!\n\n         Shard BONUS: %d\n\n  Warping to Sector: 0x%04x",mostshards*10,level+1);
			state->font->drawText(print_level,190,230);
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	return;
}

// Reshape The Window When It's Moved Or Resized
void ReshapeGL(int width, int height) {
	glViewport(0,0,(GLsizei)(width),(GLsizei)(height));// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);// Select The Projection Matrix
	glLoadIdentity();// Reset The Projection Matrix */

	gluPerspective(45.0f,(GLfloat)(width)/(GLfloat)(height),1.0f,100.0f);	// Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
	glLoadIdentity();// Reset The Modelview Matrix

	return;
}

/* unused code from a NeHe tutorial or something that I don't want to forget ;) */
void Selection(void) {
	GLuint buffer[512];
	GLint hits;
	GLint viewport[4]; //x, y, width, height (?)
	int choose, depth;
	float mouse_x_ortho, mouse_y_ortho;
	//mouse_x and mouse_y are based on SCREEN_ORTHO_WIDTH and SCREEN_ORTHO_HEIGHT
	mouse_x_ortho = mouse_x*((float)SCREEN_WIDTH/(float)SCREEN_ORTHO_WIDTH);
	mouse_y_ortho = mouse_y*((float)SCREEN_HEIGHT/(float)SCREEN_ORTHO_HEIGHT);
	glGetIntegerv(GL_VIEWPORT,viewport);
	glSelectBuffer(512,buffer);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble)mouse_x_ortho,(GLdouble)(viewport[3]-mouse_y_ortho),1.0f,1.0f,viewport);
	gluPerspective(45.0,(GLfloat)(viewport[2]-viewport[0])/(GLfloat)(viewport[3]-viewport[1]),0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	
	/** DRAWTARGETS **/
	vector<Asteroid*>::iterator j = state->asteroids.begin();
	glTranslatef(0, 0, -10);
	int i=0;
	while (j != state->asteroids.end()) {
		glLoadName(i++);
		Asteroid *a = *j++;
		glPushMatrix();
		a->render();
		glPopMatrix();
	}

	/** END DRAWTARGETS **/
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	hits = glRenderMode(GL_RENDER);
	if (hits) { 
		choose = buffer[3];
		depth  = buffer[1];
		for (int loop=1;loop<hits;loop++) {
			if (buffer[loop*4+1] < GLuint(depth)) {
				choose = buffer[loop*4+3];
				depth = buffer[loop*4+1];
			}
		}
//cout << "CHOOSE: " << choose << endl;
//cout << "DEPTH: " << depth << endl;
		//loop through buffer[loop*4+1]'s depth
		//choose=buffer[loop*4+3]
		//depth=buffer[loop*4+1];
	}
	//object is int, in order?
//cout << "HIT: " << hits << endl;
	if (hits) {
		state->asteroids[choose]->collision();
	}

	
}
