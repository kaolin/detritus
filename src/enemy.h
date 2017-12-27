#ifndef __ENEMY_H
#define __ENEMY_H

#include <vector>
#include <iostream>

#include "timer.h"

#include "os.h"
#include "asteroid.h"
#include "glsdl.h"
#include "thing.h"
#include "Model_3DS.h"
#include "bullet.h"

using namespace std;

class State;
class Enemy: public Asteroid {
	public:
		friend class State;
        static int getEnemiesCount(void);
		virtual void render();
		virtual bool collision();
		virtual bool collision(Thing &);
		virtual void iterate(float t);
		virtual bool isEnemy() { return true; }
		Enemy();
		Enemy(const XYZ&);
		virtual ~Enemy();
		float getHits();
	private:
        static int enemies;
		void explode();
		int tickoffset;
		int color;
		static Model_3DS model; // one of these
		static GLuint displayList; // or this :)
		void init(void);
		void fire(void);
		float nextbullet;
};

#endif
