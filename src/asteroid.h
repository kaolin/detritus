#ifndef __ASTEROID_H
#define __ASTEROID_H

#include <vector>
#include <iostream>

#include "timer.h"

#include "os.h"
#include "glsdl.h"
#include "thing.h"
#include "powerup.h"
#include "game.h"
#include "explosion.h"

extern int level;

using namespace std;

class Asteroid: public Thing {
	public:
		virtual bool isEnemy() { return false; }
		virtual void render();
		virtual bool collision();
		virtual bool collision(Thing &);
		virtual void iterate(float t);
		Asteroid(float size=1,bool perturb=true);
		virtual ~Asteroid();
		float getHits();
		static void reset();
	private:
		static int kills_since_powerup;
		double lifetime;
		void explode();
		int tickoffset;
		float size;
		float hits; // would be an int, but we do funky things for merging
		bool special;
		GLuint displayList;
//		int color;
		Powerup* powerup;
#ifdef DEBUG_ASTEROID_BOUNDING
		GLUquadric *sphere;
#endif
};

#endif
