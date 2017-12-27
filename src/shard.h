#ifndef __SHARD_H
#define __SHARD_H

#include <vector>
#include <iostream>

#include "timer.h"
#include "geometry.h"

#include "os.h"
#include "glsdl.h"
#include "thing.h"

using namespace std;

class Shard: public Thing {
	public:
		virtual void render();
		virtual bool collision();
		virtual bool collision(Thing&);
		Shard(Triangle* t, const XYZ* velocity, const XYZ* position);
		~Shard();

		Shard operator=(const Shard &);
		Shard operator=(const Triangle &);
		void iterate(float t);

	private:
		Triangle shard; // three 3d points define the shard, uv define texture
		int textureid; // just in case, should be set by constructor
		XYZ spin; // 3 rotational axes for the shard
		float ticks;
};

#endif
