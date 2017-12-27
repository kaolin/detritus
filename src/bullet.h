#ifndef __BULLET_H
#define __BULLET_H

#include <vector>
#include <iostream>

#include "timer.h"
#include "geometry.h"

#include "os.h"
#include "glsdl.h"
#include "thing.h"

using namespace std;

class Bullet: public Thing {
	public:
		virtual void render();
		virtual bool collision();
		virtual bool collision(Thing &);
		Bullet(const XYZ* velocity, const XYZ* position, const float* colorin=NULL);
		~Bullet();
	protected:
		const float * getColor() { return color; }
		bool inert;
	private:
		const float *color;
		float ticks;
		float rot1, rot2;
		GLUquadric* quad;
		GLuint displayList;
};

class EnemyBullet: public Bullet {
	public:
		EnemyBullet(const XYZ* velocity, const XYZ* position, const float* colorin=NULL):Bullet(velocity,position,colorin){}
		virtual bool collision();
};

class SeekerBullet: public Bullet {
	public:
		SeekerBullet(const XYZ* velocity, const XYZ* position, const float* colorin=NULL):Bullet(velocity,position,colorin){}
		virtual void iterate(float t);
};

class StarburstBullet: public Bullet {
	public:
		StarburstBullet(const XYZ* velocity, const XYZ* position, const float* colorin=NULL):Bullet(velocity,position,colorin){}
		virtual bool collision();
};

class BlowthroughBullet: public Bullet {
	public:
		BlowthroughBullet(const XYZ* velocity, const XYZ* position, const float* colorin=NULL):Bullet(velocity,position,colorin){}
		virtual bool collision();
};

#endif
