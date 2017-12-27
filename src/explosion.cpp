#include "explosion.h"
#include "exhaust.h"
#include "thing.h"
#include "geometry.h"
#include "state.h"
#include "ship.h"

Explosion::Explosion(Thing *t,const XYZ* velocity, const XYZ* position,float size,float shortenby):Exhaust(velocity,position,shortenby) {
	this->thing = t;
	this->size = size;
}

Explosion::~Explosion() { }

void Explosion::iterate(float t) {
	Thing::iterate(t);
}

void Explosion::render() {
	if (getTicks() - ticks > 5000) {
		state->remove_explosions.push_back(this);
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

 		float scale = (max(1500.0f,getTicks() - ticks)) / 500;
		glColor4f(1,1,1,1-(getTicks()-ticks)/5000);
		scale+=.75;
		glScalef(.2*size,.2*size,.2*size);
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

Explosion Explosion::operator=(const Explosion &p) {
	Exhaust::operator=(p);
	this->thing = p.thing;
	this->level = p.level;
	return *this;
}

void Explosion::explode(int count, float size, Thing *t, float shortenby) {
	XYZ position = t->getPosition();
	XYZ axis = t->getAxis();
   	XYZ explosion_velocity;
   	XYZ explosion_position;
   	XYZ jiggle = position;

	for (int i=0;i<count;i++) {
		explosion_position = position;
		jiggle = position;
    	jiggle.normalize(1);
    	float scale = ((double)rand()/((double)RAND_MAX)+(double)(1))*.4 -.5 ;
    	jiggle.scale(scale);
    	explosion_position.translate(&jiggle);

    	jiggle = axis;
    	jiggle.normalize(1);
    	scale = ((double)rand()/((double)RAND_MAX)+(double)(1))*.4 -.5 ;
    	jiggle.scale(scale);
    	explosion_position.translate(&jiggle);

    	explosion_velocity.setXYZ(
			((double)rand()/((double)RAND_MAX))-0.5f,
			((double)rand()/((double)RAND_MAX))-0.5f,
			((double)rand()/((double)RAND_MAX))-0.5f
		);
        jiggle=t->getPosition();

        explosion_velocity.scale(1.5);
		jiggle.normalize(1);
        if ( ((double)rand()/((double)RAND_MAX)) > .5) {
            jiggle.scale(.5);
        } else {
            jiggle.scale(-.5);
        }
        explosion_velocity.translate(&jiggle);
        //explosion_velocity.scale(size);

    	Explosion *e = new Explosion(t,&explosion_velocity,&explosion_position,size, shortenby);
    	State::Get().explosions.push_back(e);
    }
}
