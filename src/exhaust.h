#ifndef __EXHAUST_H
#define __EXHAUST_H

#include <vector>
#include <iostream>

#include "timer.h"
#include "geometry.h"

#include "os.h"
#include "glsdl.h"
#include "thing.h"

using namespace std;

class Exhaust: public Thing {
	public:
		virtual void render();
		virtual bool collision();
		virtual bool collision(Thing&);
		Exhaust(const XYZ* velocity, const XYZ* position, float fade=0.0f);
		virtual ~Exhaust();

		Exhaust operator=(const Exhaust &);
		void iterate(float t);

	protected:
		int textureid; // just in case, should be set by constructor
		XYZ spin; // 3 rotational axes for the exhaust
		float ticks;
};

#endif
