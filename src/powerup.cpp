#include "powerup.h"
#include "thing.h"
#include "geometry.h"
#include "state.h"
#include "game.h"
#include "bullet.h"

Powerup* Powerup::spawnPowerup() {
	Powerup *p = NULL;
	switch ((int) (((double)rand()/((double)RAND_MAX))*6) + 1) {
        case 1: p = new SpreadPowerup(); break;
        case 2: p = new SpreadPowerup(); break; //p = new LaserPowerup(); break;
        case 3: p = new SeekerPowerup(); break;
        case 4: p = new StarburstPowerup(); break;
        case 5: p = new BlowthroughPowerup(); break;
        case 6: p = new PermashieldPowerup(); break;
    }
	return p;
}

bool Powerup::collision(Thing &t) {
	return Thing::collision(t);
}

//forced collision
bool Powerup::collision() {
	state->startRelativeSound(SOUND_POWERUP);
	state->remove_powerups.push_back(this);
	return true;
}

void Powerup::show(const XYZ *position) {
    setPosition(position);
    setVelocity(0,0,0);
}

Powerup::Powerup() { // Thing() constructor called implicitly
	ticks = getTicks();
	quad = gluNewQuadric();
	mesh = new Mesh(1,.6);
}

Powerup::~Powerup() { 
	gluDeleteQuadric(quad);
}

void Powerup::iterate(float t) {
	Thing::iterate(t);
	//XYZ *f = shard.getAverage();
	//f->normalize();
	//shard.translate(f,.015*t);
	//delete(f);
}

void Powerup::render() {
	if (getTicks() - ticks > 30000) {
		state->remove_powerups.push_back(this);
		return;
	}
	glTranslatef(getPosition());
	glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
		glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
		float mehcolor[] =  {1,1,1,.6};
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,mehcolor);
		glColor4fv(getColor4fv());
		gluSphere(quad,.3+(sin((getTicks()-ticks)/400)*.1),5,5);
	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
}

const float* SpreadPowerup::getColor4fv(void) { return color; }
const float* LaserPowerup::getColor4fv(void) { return color; }
const float* SeekerPowerup::getColor4fv(void) { return color; }
const float* StarburstPowerup::getColor4fv(void) { return color; }
const float* BlowthroughPowerup::getColor4fv(void) { return color; }
const float* PermashieldPowerup::getColor4fv(void) { return color; }

const float LaserPowerup::color[4] = {.2,.2,.8,.5};
const float SpreadPowerup::color[4] = {0,1,0,.5};
const float BlowthroughPowerup::color[4] = {0,1,1,.5};
const float StarburstPowerup::color[4] = {.8,0,0,.5};
const float PermashieldPowerup::color[4] = {.3,0,.8,.5};
const float SeekerPowerup::color[4] = {1,1,0,.5};

void SpreadPowerup::fire(XYZ *position, XYZ *velocity, XYZ *orientation) {
	XYZ axis = *position;
	XYZ velorig = *velocity;
	axis.cross(orientation);
	
	Bullet *b;
	/*
	b = new Bullet(velocity,position);
	state->bullets.push_back(b);
	*/
	
	axis.normalize();
	velocity->translate(&axis,1);
	b = new Bullet(velocity,position,color);
	state->bullets.push_back(b);
	
	*velocity = velorig;
	velocity->translate(&axis,-1);
	b = new Bullet(velocity,position,color);
	state->bullets.push_back(b);
}

/* TODO: this is going to take more thought :/ */
void LaserPowerup::fire(XYZ *position, XYZ *velocity, XYZ *orientation) {
}
	
void SeekerPowerup::fire(XYZ *position, XYZ *velocity, XYZ *orientation) {
	state->bullets.push_back(new SeekerBullet(velocity,position,color));
}
	
void StarburstPowerup::fire(XYZ *position, XYZ *velocity, XYZ *orientation) {
	state->bullets.push_back(new StarburstBullet(velocity,position,color));
}
	
void BlowthroughPowerup::fire(XYZ *position, XYZ *velocity, XYZ *orientation) {
	state->bullets.push_back(new BlowthroughBullet(velocity,position,color));
}

