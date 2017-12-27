#include "asteroid.h"
#include "shard.h"
#include "thing.h"
#include "geometry.h"
#include "state.h"
#include "powerup.h"
#include "explosion.h"

//forced collision
bool Asteroid::collision() {
	//special = !special;
	explode();
	return true;
}

bool Asteroid::collision(Thing &t) {
	if (Thing::collision(t)) {
		//collision();
	}
	return false;
}

void Asteroid::iterate(float t) {
	this->Thing::iterate(t);
	lifetime+=t;
}

int Asteroid::kills_since_powerup;

void Asteroid::reset() {
	kills_since_powerup = 0;
}

float Asteroid::getHits() { return hits; }

void Asteroid::explode() {
	kills_since_powerup++;
	// get each triangle
	vector<Triangle*> triangles = mesh->getTriangles();
	int count = triangles.size();
	Shard* s;
	
	const XYZ position = getPosition();
	const XYZ velocity = getVelocity();
	// turn each triangle into a shard
	// TODO: need to take into account asteroid's rotation, etc, to apply
	// all that to the triangle (asteroid-centric) coordinates
	Triangle t;
	for (int i=0;i<count;i++) {
		t = triangles[i];
		s = new Shard(&t,&velocity,&position);
		state->shards.push_back(s);

	}

	if (hits < 2) {
		hits++;
		// let's make some more!
		for (int i=0;i<2;i++) {
			Asteroid *a = new Asteroid(2/(hits+2));
			a->setPosition(getPosition());
			a->hits = hits;
			state->asteroids.push_back(a);
		}
	}

	if (powerup) {
        powerup->show(&getPosition());
		state->powerups.push_back(powerup);
		powerup = NULL;
	}
	// clean up the asteroid!
	state->remove_asteroids.push_back(this);
	state->startRelativeSound(SOUND_ASTEROID_BOOM,position);
	Explosion::explode(50,.4f,this,4000.f);
}

Asteroid::Asteroid(float size,bool perturb) { // Thing() constructor called implicitly
	this->size = size;
	lifetime = 0;
	mesh = new Mesh(1,((size+1)*(size+1)*(size+1)-1)/5);
	lifetime = 0;

	if (perturb) {
		mesh->perturb(size*.4);
	
		mesh->subdivide(1,false);
		mesh->perturb(size*.3);
	}
	
	
	//mesh->subdivide(1,false);
	//mesh->perturb(.3);
	XYZ *p = new XYZ((40.f*rand()/RAND_MAX)-20.f,(40.f*rand()/RAND_MAX)-20.f,(40.f*rand()/RAND_MAX)-20.f);
	float speed;
	do {
		setVelocity((6.f*rand()/RAND_MAX)-3.f,(6.f*rand()/RAND_MAX)-3.f,(6.f*rand()/RAND_MAX)-3.f);
	} while ((speed = getVelocity().getMagnitude()) < .5 && speed > -.5); // we want all rocks to be moving!
	XYZ vel = getVelocity();
	vel.scale(.5f/size);
	setVelocity(vel);
	displayList = 0;
	//special = rand() < RAND_MAX / 6;
	special = 0;
	p->normalize(12);
	setPosition(p);
	hits = 0;
	delete(p);
	powerup = NULL;
//	powerup = Powerup::spawnPowerup(); // for debugging
	if (powerup == NULL && size > 0.51f && (double)rand()/((double)RAND_MAX)*1.5 > size) {
		if ((double)rand()/((double)RAND_MAX)*((float)kills_since_powerup) > (20.0 * ((float)level+1.0f) / 1.5) ) {
			kills_since_powerup=0;
            powerup = Powerup::spawnPowerup();
		}
	}
	#ifdef DEBUG_ASTEROID_BOUNDING
		sphere = gluNewQuadric();
	#endif
}

Asteroid::~Asteroid() {
	glDeleteLists(displayList,1);
	if (powerup) delete powerup;
	#ifdef DEBUG_ASTEROID_BOUNDING
		gluDeleteQuadric(sphere);
	#endif
}

void Asteroid::render() {
	glTranslatef(getPosition());

/*
	glPushMatrix();
		// draw velocity for debugging
		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
			glLineWidth(2);
			glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(getVelocity().r3[0],getVelocity().r3[1],getVelocity().r3[2]);
			glEnd();
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();
*/

	if (displayList == 0) {
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1.0);
		displayList = glGenLists(1);
		//glNewList(displayList,GL_COMPILE_AND_EXECUTE);
		glNewList(displayList,GL_COMPILE);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, State::getAsteroidTexture());

			vector<Triangle*> triangles = mesh->getTriangles();
			vector<Triangle*>::iterator i = triangles.begin();

			glTexGenf(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
			float ones[] = {1.0f,1.0f,1.0f,1.0f};
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ones);
			glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);
#ifndef DEBUG_ASTEROID_BOUNDING
			glBegin(GL_TRIANGLES);
				while (i != triangles.end()) {
					Triangle *t = *i++;

					t->calcSphericalUV();
					for (int it = 0; it < 3; it++) {
						if (special) {
							glColor3f(1,0,0);
						}
						glTexCoord2f(t->p[it]->uv[0],t->p[it]->uv[1]);
						XYZ normal = t->p[it];
						normal.normalize();
	    				glNormal3f(normal.r3[0],normal.r3[1],normal.r3[2]);
	    				glVertex3f(t->p[it]->r3[0], t->p[it]->r3[1], t->p[it]->r3[2]);
					}
                
				}
			glEnd();
			

			GLfloat white[4] = {1.,1.,1.,1.};
//			glPushAttrib(GL_ALL_ATTRIB_BITS);
			
			glColor3f(0.,0.,0.);
			glDepthFunc(GL_LEQUAL);
//			glEnable(GL_BLEND);
			glLightfv(GL_LIGHT0,GL_SPECULAR,white); // set that already I think
			glBindTexture(GL_TEXTURE_2D,State::getSpecularTexture());

			glBegin(GL_TRIANGLES);
				while (i!= triangles.end()) {
					Triangle *t = *i++;
					for (int it = 0; it < 3; it++) {
						XYZ normal = t->p[it];
						normal.normalize();
	    				glNormal3f(normal.r3[0],normal.r3[1],normal.r3[2]);
	    				glVertex3f(t->p[it]->r3[0], t->p[it]->r3[1], t->p[it]->r3[2]);
					}
				}
			glEnd(); // GL_TRIANGLES
			

#endif
#ifdef DEBUG_ASTEROID_BOUNDING
glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	gluSphere(sphere,size,20,20);
#endif
//exit(1);
			if (special) {
				glPopAttrib();
			}
			glDisable(GL_TEXTURE_GEN_S);
		glEndList();
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	   glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	   if (powerup) {
            glColor3fv(powerup->getColor4fv());
	       	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,powerup->getColor4fv());
        } else {
			float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specReflection);
			glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 96);
            glColor3f(1,1,1);
        }
       glCallList(displayList);
    glColor4f(1,1,1,1);
	glDisable(GL_COLOR_MATERIAL);
}
