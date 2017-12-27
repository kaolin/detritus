#ifndef __THING_H
#define __THING_H

#include <vector>
#include <iostream>

#include "geometry.h"
#include "timer.h"

#include "os.h"
#include "glsdl.h"

using namespace std;

class State;
class Thing {

	public:
		virtual void render()=0;
		virtual void iterate(float t);
		//virtual bool collision() const=0;
		virtual bool collision();
		virtual bool collision(Thing &);
		Thing();
		virtual ~Thing();
		const XYZ& getPosition();
		const XYZ& getVelocity();
		const XYZ& getAxis();
		const Mesh *getMesh();

	protected:
		void setVelocity(const XYZ &x);
		void setVelocity(float x, float y, float z);
		void setPosition(const XYZ &x);
		void setPosition(float x, float y, float z);
		float getPositionX();
		float getPositionY();
		float getPositionZ();
		float getVelocityX();
		float getVelocityY();
		float getVelocityZ();
		float getTicks();
		State *state;
		Mesh *mesh;
		
	private:
		XYZ position, velocity, axis;
};

#endif
