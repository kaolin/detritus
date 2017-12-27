#ifndef __EXPLOSION_H
#define __EXPLOSION_H

#include <vector>
#include <iostream>

#include "timer.h"
#include "geometry.h"

#include "os.h"
#include "glsdl.h"
#include "thing.h"
#include "exhaust.h"

using namespace std;

class Explosion: public Exhaust {
	public:
		Explosion(Thing *,const XYZ* velocity, const XYZ* position, float size=1.0f, float shortenby=0.0f);
		virtual ~Explosion();
		Explosion operator=(const Explosion &);
		void iterate(float t);		
		void render();
		static void explode(int count, float size, Thing *t,float shortenby);

	protected:
		Thing *thing;
		int level;
		float size;
};

#endif
