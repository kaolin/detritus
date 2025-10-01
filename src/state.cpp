#include "state.h"
#include "glsdl.h"
#include "skybox.h"
#include "asteroid.h"
#include "ship.h"
#include "bullet.h"
#include "powerup.h"
#include "enemy.h"

using namespace std;

State& State::Get() {
	static State s;
	return s;
}

void State::setOrientationMatrix(XYZ *right, XYZ *up, XYZ *in) {
	/*

	 M = | A B C D | -> X-axis = [ A E I ] [[ X is right ]]
	     | E F G H |    Y-axis = [ B F J ] [[ Y is up ]]
	     | I J K L |    Z-axis = [ C G K ] [[ Z is in ]]
	     | M N O P |

   OpenGL: A=0, E=1, I=2, M=3, B=4, F=5, J=6, N=7, C=8, G=9, K=10, ...

	 essentially:

	 M = | newright.i newup.i newin.i 0 |
    	 | newright.j newup.j newin.j 0 |
    	 | newright.k newup.k newin.k 0 |
    	 | 0          0       0       1 |
	*/

	orientationMatrix[0] = right->r3[0];
	orientationMatrix[1] = right->r3[1];
	orientationMatrix[2] = right->r3[2];
	orientationMatrix[4] = up->r3[0];
	orientationMatrix[5] = up->r3[1];
	orientationMatrix[6] = up->r3[2];
	orientationMatrix[8] = in->r3[0];
	orientationMatrix[9] = in->r3[1];
	orientationMatrix[10] = in->r3[2];
	orientationMatrix[3] = orientationMatrix[7] = orientationMatrix[11] =
	orientationMatrix[12] = orientationMatrix[13] = orientationMatrix[14] = 0;
	orientationMatrix[15] = 1;
}

State::State() {
	//Load the files
	if (load_files() == false) {
		cout << "FAILED load_files" << endl;
		return; // TODO: display / return errors
	}

	//Start the update timer
	update.start();

	//Start the thing timer
	thing.start();
	
	//Start the thing2 timer
	thing2.start();

	//skybox = new Skybox();
	/* TO PREVENT LAAAAAAAAAAAAAAAMITUDE crashing */
	asteroids.reserve(1000);
	bullets.reserve(1000);
	shards.reserve(1000);
	exhaust.reserve(1000);
	powerups.reserve(1000);
	remove_asteroids.reserve(1000);
	remove_bullets.reserve(1000);
	remove_shards.reserve(1000);
	remove_exhaust.reserve(1000);
	remove_powerups.reserve(1000);

}

State::~State() {
	//free surfaces??
	if (skybox) delete(skybox);
	//delete vectors of info 
}

bool State::load_files() {
	SDL_Surface *surf;

	surf = load_image(DATAPREFIX"data/images/kbd_arrow.png");
	if (!surf)  { cout << "NO ARROW" << endl; flush(cout); return false; }
	keyboard_arrow_tex = SDL_GL_LoadTexture(surf,keyboard_arrow_tex_coord);
	SDL_FreeSurface(surf);

	surf = load_image(DATAPREFIX"data/images/kbd_space.png");
	if (!surf)  { cout << "NO SPACEBAR" << endl; flush(cout); return false; }
	keyboard_space_tex = SDL_GL_LoadTexture(surf,keyboard_space_tex_coord);
	SDL_FreeSurface(surf);

	surf = load_image(DATAPREFIX"data/images/splash.png");
	if (!surf)  { cout << "NO SPLASH" << endl; flush(cout); return false; }
	splash_tex = SDL_GL_LoadTexture(surf,splash_tex_coord);
	SDL_FreeSurface(surf);

	surf = load_image(DATAPREFIX"data/images/outtro-noGUD.png");
	if (!surf)  { cout << "NO OUTTRO" << endl; flush(cout); return false; }
	outtro_tex = SDL_GL_LoadTexture(surf,outtro_tex_coord);
	SDL_FreeSurface(surf);

	surf = load_image(DATAPREFIX"data/images/title.png");
	if (!surf)  { cout << "NO OUTTRO" << endl; flush(cout); return false; }
	title_tex = SDL_GL_LoadTexture(surf,title_tex_coord);
	SDL_FreeSurface(surf);
	
	//specular texture is a 2x2 white texture map for second pass on textured objects that desire specularity
	glBindTexture(GL_TEXTURE_2D,specular_tex);
	unsigned char pixels[2*2*4];
	memset(pixels,0xff,2*2*4);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
/*
	surf = load_image(DATAPREFIX"data/cursor.png");
	if (!surf)  {
		cout << "NO CURSOR" << endl;
		flush(cout);
		return false;
	}
	cursor_tex = SDL_GL_LoadTexture(surf);
	SDL_FreeSurface(surf);
*/

	/* TODO: drop this in skybox?? */
	if ((surf = load_image(DATAPREFIX"data/images/skybox_BK.png"))) {
		skybox_tex.push_back(SDL_GL_LoadTexture(surf));
		SDL_FreeSurface(surf);
	} else { cout << "MISSING SKYBOX TEXTURE: BK" << endl; return false; }

	if ((surf = load_image(DATAPREFIX"data/images/skybox_DN.png"))) {
		skybox_tex.push_back(SDL_GL_LoadTexture(surf));
		SDL_FreeSurface(surf);
	} else { cout << "MISSING SKYBOX TEXTURE: DN" << endl; return false; }

	if ((surf = load_image(DATAPREFIX"data/images/skybox_FR.png"))) {
		skybox_tex.push_back(SDL_GL_LoadTexture(surf));
		SDL_FreeSurface(surf);
	} else { cout << "MISSING SKYBOX TEXTURE: FR" << endl; return false; }

	if ((surf = load_image(DATAPREFIX"data/images/skybox_LF.png"))) {
		skybox_tex.push_back(SDL_GL_LoadTexture(surf));
		SDL_FreeSurface(surf);
	} else { cout << "MISSING SKYBOX TEXTURE: LF" << endl; return false; }

	if ((surf = load_image(DATAPREFIX"data/images/skybox_RT.png"))) {
		skybox_tex.push_back(SDL_GL_LoadTexture(surf));
		SDL_FreeSurface(surf);
	} else { cout << "MISSING SKYBOX TEXTURE: RT" << endl; return false; }

	if ((surf = load_image(DATAPREFIX"data/images/skybox_UP.png"))) {
		skybox_tex.push_back(SDL_GL_LoadTexture(surf));
		SDL_FreeSurface(surf);
	} else { cout << "MISSING SKYBOX TEXTURE: UP" << endl; return false; }
	/* END TODO: drop this in skybox */

	font = new Font(FONT_NAME,28,
		1,1,1 //red, green, blue fg
	);

	smallfont = new Font(FONT_NAME,14,
		1,1,1 //red, green, blue fg
	);

	if ((surf = load_image(DATAPREFIX"data/images/asteroids_base_sampletexture_venus.png"))) {
		asteroid_tex = SDL_GL_Build2DMipmaps(surf,512,256);
		SDL_FreeSurface(surf);
	} else {
		cout << "NO ASTEROIDS TEXTURE" << endl;
		return false;
	}

	if ((surf = load_image(DATAPREFIX"data/images/bullet.png"))) {
		bullet_tex = SDL_GL_Build2DMipmaps(surf,128,64);
		SDL_FreeSurface(surf);
	} else {
		cout << "NO BULLET TEXTURE" << endl;
		return false;
	}

	if ((surf = load_image(DATAPREFIX"data/images/smoke2.png"))) {
		exhaust_tex = SDL_GL_LoadTexture(surf); // no mipmaps ... umm ...
		SDL_FreeSurface(surf);
	} else {
		cout << "NO EXHAUST TEXTURE" << endl;
		return false;
	}

	/* Sound */
	char* SOUNDS[NUM_BUFFERS] = {
			DATAPREFIX"data/sounds/asteroid-explosion.wav",
			DATAPREFIX"data/sounds/bullet.wav",
			DATAPREFIX"data/sounds/shield.wav",
			DATAPREFIX"data/sounds/ship-explosion.wav",
			DATAPREFIX"data/sounds/powerup.wav",
			DATAPREFIX"data/sounds/klaxon.wav",
			DATAPREFIX"data/sounds/saucer.wav",
			DATAPREFIX"data/sounds/thrust.wav",
			DATAPREFIX"data/sounds/thrust-off.wav",
			DATAPREFIX"data/sounds/level-up.wav"
	};
	
	glPushMatrix();
		Enemy::model.Load(DATAPREFIX"data/models/saucer001.3ds");
		Enemy::model.scale = .12f;
		Enemy::displayList = glGenLists(1);
		glNewList(Enemy::displayList,GL_COMPILE);
		glScalef(3,3,3);
		Enemy::model.Draw();
		glEndList();
	glPopMatrix();
	
	glPushMatrix();
		Ship::model.Load(DATAPREFIX"data/models/spaceship_checkers.3ds");
//		Ship::model.rot.x = 90.0f;
//		Ship::model.rot.y = 90.0f;
//		Ship::model.rot.z = 90.0f;
		Ship::model.scale = .12f;
		Ship::displayList = glGenLists(1);
		glNewList(Ship::displayList,GL_COMPILE);
//        		Ship::model.shownormals = true;
				Ship::model.Draw();
		glEndList();
	glPopMatrix();


	soundlib_init(NUM_BUFFERS,SOUNDS);
	soundlib_start();
	return true;
}

void State::startRelativeSound(int i) {
	float pos[3] = {0,0,0};
	startSound(i,pos);
}

void State::startRelativeSound(int i,const XYZ &position) {
	//XYZ shipposition = ship->getPosition();
	XYZ soundposition = position;
	//shipposition.scale(-1);
	//soundposition.translate(&shipposition);
	//soundposition.scale(.0075);
	startSound(i,soundposition.r3);
}

void State::moveRelativeSound(int i, const XYZ &position) {
	XYZ soundposition = position;
	moveSound(i,soundposition.r3);
}

void State::stopSound(int i) {
	stopSound(i); //this go crashy crashy selfloop?
}


const char* State::FONT_NAME = DATAPREFIX"data/fonts/max_1.0.1.ttf";

void State::clear_field(void) {
	bullets.erase(bullets.begin(),bullets.end());
	enemybullets.erase(enemybullets.begin(),enemybullets.end());
	asteroids.erase(asteroids.begin(),asteroids.end());
	shards.erase(shards.begin(),shards.end());
	exhaust.erase(exhaust.begin(),exhaust.end());
	powerups.erase(powerups.begin(),powerups.end());
}

bool State::set_icon(SDL_Window* W) {
	SDL_Surface *icon = load_image(DATAPREFIX"data/images/icon2.png");
	if (!icon) return false;
	SDL_SetWindowIcon(W, icon);
	return true;
}

