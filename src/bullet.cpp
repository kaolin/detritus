#include "bullet.h"
#include "thing.h"
#include "geometry.h"
#include "state.h"
#include "asteroid.h"

bool Bullet::collision(Thing &t) {
	if (inert) return false;
	return Thing::collision(t);
}

//forced collision
bool Bullet::collision() {
	state->remove_bullets.push_back(this);
	inert=true;
	return true;
}

bool EnemyBullet::collision() {
	state->remove_enemybullets.push_back(this);
	inert=true;
	return true;
}

Bullet::Bullet(const XYZ* velocity, const XYZ* position, const float* colorv) { // Thing() constructor called implicitly
	inert=false;
	setPosition(position);
	setVelocity(velocity);
	ticks = getTicks();
	quad = gluNewQuadric();
	gluQuadricNormals(quad,GL_SMOOTH);
	gluQuadricTexture(quad,GL_TRUE);
    rot1 =  180.f*rand()/RAND_MAX;
    rot2 =  180.f*rand()/RAND_MAX;
    color = colorv;
}

Bullet::~Bullet() { 
	gluDeleteQuadric(quad);
}


void Bullet::render() {
    static XYZ bullpos;
    float tickdiff = getTicks() - ticks;
	if (tickdiff > 1500) {
		state->remove_bullets.push_back(this);
		state->remove_enemybullets.push_back(this);
		return;
	}
	
	//scaling is due to "tilt" of ship -- this math makes bullets come out of tilted nose and then down :)
	bullpos = getPosition();
	bullpos.scale(.975 + (.025 * min(tickdiff,500.0f)/500.0));
	glTranslatef(bullpos);
    glRotatef(rot1,0,1,0);
    glRotatef(rot2,1,0,0);
/* // TODO: bullet-shaped bullets?
	if (displayList == 0) {
		displayList = glGenLists(1);
		glNewList(displayList,GL_COMPILE);
			gluSphere(quad,1,5,5);
		glEndList();
	}
*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
//	glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_SRC_COLOR);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
		if (color) {
			glColor3fv(color);
		} else {
			glColor3f(1,1,1);
		}
    	glBindTexture(GL_TEXTURE_2D, State::getBulletTexture());
		//glCallList(displayList);
		gluSphere(quad,.1,6,6);
    glDisable(GL_COLOR_MATERIAL);
   	glDisable(GL_BLEND);
}

//forced collision
bool BlowthroughBullet::collision() {
	return false;
}

bool StarburstBullet::collision() {
	state->remove_bullets.push_back(this);
	inert=true;
	float speed;
	for (int i=0;i<3;i++) {
		XYZ velocity;
		do {
			velocity.setXYZ((6.f*rand()/RAND_MAX)-3.f,(6.f*rand()/RAND_MAX)-3.f,(6.f*rand()/RAND_MAX)-3.f);
		} while ((speed = velocity.getMagnitude()) < .5 && speed > -.5);
		velocity.normalize();
		velocity.scale(5);
		Bullet *b = new Bullet(&velocity,&getPosition());//,getColor());
		state->bullets.insert(state->bullets.begin(),b);
	}
	return true;
}

void SeekerBullet::iterate(float f) {
	XYZ position = getPosition();
	vector<Asteroid*>::iterator j;
	j = state->asteroids.begin();
	Asteroid *best = NULL;
	float bestdistance = 7.5f;
	XYZ bestVector;
	XYZ distanceVector;
	float distance;

	while (j != state->asteroids.end()) {
		Asteroid *a = *j++;
		distanceVector = position;
		distanceVector.translate(&a->getPosition(),-1);
		distance = distanceVector.getMagnitude();
		if (distance < bestdistance) {
			bestdistance = distance;
			best = a;
			bestVector=distanceVector;
		}
	}
	if (best) {
		float currentspeed = getVelocity().getMagnitude();
		bestVector.normalize(-.35*(f/20));
		XYZ velocity = getVelocity();
		velocity.translate(&bestVector);
		velocity.normalize(currentspeed);
		setVelocity(velocity.getX(),velocity.getY(),velocity.getZ());
	}
	Thing::iterate(f);
}
