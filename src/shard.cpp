#include "shard.h"
#include "thing.h"
#include "geometry.h"
#include "state.h"

bool Shard::collision([[maybe_unused]] Thing &t) {
	return false; // TODO: COLLISIONS?
}

//forced collision
bool Shard::collision() {
	return false; // TODO: COLLISIONS?
}

Shard::Shard(Triangle* t, const XYZ* velocity, const XYZ* position) { // Thing() constructor called implicitly
	shard = t; // implicit memberwise copy constructor via Triangle = *Triangle
	setPosition(position);
	setVelocity(velocity);
	//setPosition(12.f*rand()/RAND_MAX,12.f*rand()/RAND_MAX,12.f*rand()/RAND_MAX);
	//setVelocity(0,0,0);
	//tickoffset = rand();
	//special = rand() < RAND_MAX / 6;
	ticks = getTicks();
}

Shard::~Shard() { }

void Shard::iterate(float t) {
	XYZ *f = shard.getAverage();
	f->normalize();
	shard.translate(f,.015*t);
	delete(f);
}

void Shard::render() {
	if (getTicks() - ticks > 2000) {
		state->remove_shards.push_back(this);
		return;
	}

	glTranslatef(getPosition());

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(.8,.8,.8);
	glBindTexture(GL_TEXTURE_2D,State::getAsteroidTexture());
	glBegin(GL_TRIANGLES);
	shard.calcSphericalUV();
	for (int it = 0; it < 3; it++) {
		glTexCoord2f(shard.p[it]->uv[0],shard.p[it]->uv[1]);
  	 	glNormal3f(shard.p[it]->r3[0], shard.p[it]->r3[1], shard.p[it]->r3[2]);
	   	glVertex3f(shard.p[it]->r3[0], shard.p[it]->r3[1], shard.p[it]->r3[2]);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);

}

Shard Shard::operator=(const Shard &p) {
	this->shard = p.shard;
	this->textureid = p.textureid;
	this->spin = p.spin;
	return *this;
}
