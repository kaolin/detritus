#include "exhaust.h"
#include "thing.h"
#include "geometry.h"
#include "state.h"
#include "ship.h"

bool Exhaust::collision(Thing &t) {
	return false; // TODO: COLLISIONS?
}

//forced collision
bool Exhaust::collision() {
	return false; // TODO: COLLISIONS?
}

Exhaust::Exhaust(const XYZ* velocity, const XYZ* position, float fade) { // Thing() constructor called implicitly
	setPosition(position);
	setVelocity(velocity);
	ticks = getTicks()-fade;
}

Exhaust::~Exhaust() { }

void Exhaust::iterate(float t) {
	Thing::iterate(t);

	//XYZ velocity = getVelocity();
	//exhaust.translate(velocity,.015*t);
}

void Exhaust::render() {
	if (getTicks() - ticks > 700) {
		state->remove_exhaust.push_back(this);
		return;
	}

	glTranslatef(getPosition());

	billboardCheatSphericalBegin(false);
	
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glBindTexture(GL_TEXTURE_2D,State::getExhaustTexture());

 		float scale = (getTicks() - ticks) / 500.f;
		float alpha = 1.25-scale;
		alpha = .2; // hardcoding because something was screwing up the radar if it wasn't ... ??? !!! ???
		glColor4f(1,1,1,alpha);
		scale+=.75;
		glScalef(.2,.2,.2);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); 
			glVertex2f(-1*scale, -1*scale); 
			glTexCoord2f(0,1); 
			glVertex2f(-1*scale,1*scale); 
			glTexCoord2f(1,1); 
			glVertex2f(1*scale,1*scale); 
			glTexCoord2f(1,0); 
			glVertex2f(1*scale,-1*scale); 
		glEnd();

		glEnable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	billboardEnd(false);

}

Exhaust Exhaust::operator=(const Exhaust &p) {
	this->textureid = p.textureid;
	this->spin = p.spin;
	return *this;
}
