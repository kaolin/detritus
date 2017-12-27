#ifndef __SKYBOX_H
#define __SKYBOX_H

#include <vector>
#include <iostream>

#include "timer.h"

#include "os.h"
#include "glsdl.h"
#include "geometry.h"
#include "state.h"

using namespace std;

class State;

class Skybox {
	public:
		Skybox();
		~Skybox();
		void render();
	protected:
		XYZ position;
		XYZ size;
		GLuint displayList;
		vector<GLuint> skybox_tex;
		State *state;
};

#endif
