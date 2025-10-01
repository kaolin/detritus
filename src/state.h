#ifndef __STATE_H
#define __STATE_H

#include "os.h"

#include "timer.h"
#include "glsdl.h"
#include "font.h"
#include "game.h"
#include "soundlib.h"
#include "geometry.h"

/* 
	NOTE: THIS IS NOT A THREAD-SAFE SINGLETON PATTERN!!!!

		http://www.research.ibm.com/designpatterns/pubs/ph-jun96.txt

	State::Get().asteroids // works fine
	State &state = State::Get(); // works fine
	State another = state; // FAILS
*/


class Asteroid;
class Shard;
class Skybox;
class Ship;
class Bullet;
class Exhaust;
class Powerup;
class Explosion;

class State {
	public:
		static State& Get();

		Ship* ship;

		vector<Asteroid*> asteroids;
		vector<Asteroid*> remove_asteroids;

		vector<Shard*> shards;
		vector<Shard*> remove_shards;

		vector<Bullet*> bullets;
		vector<Bullet*> remove_bullets;
		
		vector<Bullet*> enemybullets;
		vector<Bullet*> remove_enemybullets;

		vector<Exhaust*> exhaust;
		vector<Exhaust*> remove_exhaust;

		vector<Explosion*> explosions;
		vector<Explosion*> remove_explosions;

		vector<Powerup*> powerups;
		vector<Powerup*> remove_powerups;

		Skybox* skybox;
		Timer fps; // used to calculate the frames per second
		Timer update; // used to update the caption
		Timer thing; // used to update the rock rotation
		Timer thing2; // used to update attract loop stuffs
		int frame; // keep track of frame count
		static GLuint getAsteroidTexture() { return (Get()).asteroid_tex; }
		static GLuint getSpecularTexture() { return (Get()).specular_tex; }
		static GLuint getExhaustTexture() { return (Get()).exhaust_tex; }
		static GLuint getBulletTexture() { return (Get()).bullet_tex; }
		GLuint asteroid_tex, exhaust_tex, bullet_tex;
		vector<GLuint> skybox_tex;
		GLuint keyboard_space_tex; GLfloat keyboard_space_tex_coord[4];
		GLuint keyboard_arrow_tex; GLfloat keyboard_arrow_tex_coord[4];
		GLuint splash_tex; GLfloat splash_tex_coord[4];
		GLuint outtro_tex; GLfloat outtro_tex_coord[4];
		GLuint title_tex; GLfloat title_tex_coord[4];
		GLuint specular_tex;
		Font *font; 
		Font *smallfont;
		char debug[400];
		void setOrientationMatrix(XYZ *right, XYZ *up, XYZ *in);
		float orientationMatrix[16];
		void startRelativeSound(int i); // toggle sound "i" on ship
		void startRelativeSound(int i,const XYZ &position); // toggle sound "i" on at point src
		void moveRelativeSound(int i,const XYZ &position); // move sound "i" to point src
		void stopSound(int i); // KILL sound KILL!!!
		void clear_field(); // reset all the vectors
		bool set_icon(SDL_Window *W); // seticonwheeee

	private:
		State(); // ctor is hidden
		State(State const&); // copy ctor is hidden
		State& operator=(State const&); // assign op is hidden
		~State(); // dtor hidden

		#define SURFACE_C 8
		SDL_Surface *surfaces[SURFACE_C];


		const static char* FONT_NAME;

		bool load_files(); // load image files, prep textures (called from init)

};

#endif
