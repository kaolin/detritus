#include "thing.h"
#include "geometry.h"
#include "state.h"
#include "ship.h"
#include "bullet.h"
#include "shard.h"
#include "exhaust.h"
#include "explosion.h"

GLuint Ship::displayList = 0;
Model_3DS Ship::model;

const XYZ& Ship::getAxis() { return axis; }

void Ship::fire(bool start) {
	if (!start) { firing = 0; return; }
	XYZ velocity = orientation;
	XYZ position = getPosition();
	position.translate(&orientation,2);
	velocity.normalize();
	velocity.scale(5);
	velocity.translate(&getVelocity());
	
	if (weaponup) {
		weaponup->fire(&position, &velocity, &orientation);
	} else {
		Bullet *b;
		b = new Bullet(&velocity,&position);
		state->bullets.push_back(b);
	}
	firing=400;
	state->startRelativeSound(SOUND_BULLET);
}

int Ship::getShieldStrength() { return shield; }

void Ship::halt() {
	setVelocity(0,0,0);
	setThrust(false,false);
	banked = 0;
	thrustfade = 0;
}

void Ship::setViewport(int i) { 
	if (i == -1) viewport++;
	else viewport = i; 
	if (viewport > 1) viewport = 0;
}

bool Ship::hasExploded() {
	return exploded;
}

XYZ Ship::getLookAt() {
	XYZ at = getVelocity();
	at.translate(&orientation,.25);
	return at;
}

void Ship::orientGLViewport() {
	XYZ ship, eye, tempv5;
	XYZ tempv, tempv2, tempv3, tempv4;
	ship = getPosition();

	switch(viewport) {
		case 1:
			glTranslatef(0,0,-30);
			center = orientation;
			break;
		case 0:
		default:
			tempv = getPosition();
			center = getLookAt();
			tempv2 = center;
			tempv2.normalize(-1*tempv.getMagnitude());
			tempv4 = getPosition();
			tempv4.scale(.70); // magic number...?
			tempv5= getPosition();
			tempv5.normalize();
			gluLookAt(
				tempv2.r3[0]+tempv4.r3[0], // eye x
				tempv2.r3[1]+tempv4.r3[1], // eye y
				tempv2.r3[2]+tempv4.r3[2], // eye z
				0+tempv4.r3[0], // center x
				0+tempv4.r3[1], // center y
				0+tempv4.r3[2], // center z
				-tempv5.r3[0], // up x
				-tempv5.r3[1], // up y
				-tempv5.r3[2]); // up z
		break;
	}
	axis = center;
}

void Ship::reset() {
	ignorecollision = false;
	exploded = false;
	shield = 100;
	XYZ *p = new XYZ(-1,0,0);
	setVelocity(0,0,0);
	orientation.setXYZ(0,1,0);
	special = rand() < RAND_MAX / 6.f;
	p->normalize(12);
	setPosition(p);
	hits = 0;
	setThrust(false,false);
	banked = 0;
	rot_ticks = 0;
	viewport = 0;
	thrustfade = 0;
	invulnerable = 3000;
	firing = 0;
	if (weaponup) { delete weaponup; weaponup = NULL; }
	if (shieldup) {	delete shieldup; shieldup = NULL; }
}

void Ship::leveled() {
	if (invulnerable < 3000) invulnerable = 3000;	
	if (shieldup) {	delete shieldup; shieldup = NULL; }
	firing = 0;
	halt();
}

Ship::Ship() { // Thing() constructor called implicitly
	weaponup = NULL;
	shieldup = NULL;
	reset();
	mesh = new Mesh(1,.5); // unneccessary except things crash without for now on collision test or something
	shield_quadric = gluNewQuadric();
	light = gluNewQuadric();
#ifdef DEBUG_SHIP_BOUNDING
	for (int i=0;i<12;i++) {	
		bounding_quads[i] = gluNewQuadric();
	}
#endif // DEBUG_SHIP_BOUNDING
}

void Ship::teleport() {
	//TODO -- orient properly!
	setVelocity(0,0,0);
	XYZ *p = new XYZ((40.f*rand()/RAND_MAX)-20.f,(40.f*rand()/RAND_MAX)-20.f,(40.f*rand()/RAND_MAX)-20.f);
	p->normalize(12);
	setPosition(p);
	delete(p);
}

void Ship::flip() { orientation.scale(-1); }

Ship::~Ship() {
	if (mesh) { delete mesh; mesh = NULL; }
	clearWeapon();
	clearShield();
	setThrust(false,false);
	gluDeleteQuadric(shield_quadric);
	gluDeleteQuadric(light);
#ifdef DEBUG_SHIP_BOUNDING
	for (int i=0;i<12;i++) {
		gluDeleteQuadric(bounding_quads[i]);
	}
#endif // DEBUG_SHIP_BOUNDING
}

//forced collision
bool Ship::collision() {
	if (ignorecollision) {
		return false;
	}
	if (!invulnerable) {
		shield -= 30;
		if (shield < 0) {
			shield = 0;
			XYZ a(0,0,0);
			orientation = getLookAt();
			orientation.normalize();
			setVelocity(a);
			explode();
		} else {
			state->startRelativeSound(SOUND_SHIELD);
			invulnerable = 1000;
		}
		if (shield < 100) clearWeapon();
	}
	return true;
}

bool Ship::testCollisionHack(Thing &t) {
	float r1, r2, distance;
	XYZ boundingcenter,boundingorientation,distanceVector;

	if (t.getMesh()) {
		r1 = t.getMesh()->getMaximum();
	} else {
		r1 = 0;
	}

	//primary sphere bound
	r2 = 1.7;

	boundingcenter = getPosition();
	boundingorientation = orientation;
	boundingorientation.scale(.7);
	boundingcenter.translate(&boundingorientation);

	distanceVector = boundingcenter;
	distanceVector.translate(&t.getPosition(),-1);
	distance = distanceVector.getMagnitude();

	if (distance < (r1 + r2)) {
		//then we can testing the inside bounds!
		//secondary sphere bounds, WHEE
		boundingcenter = getPosition();
		for (int i=0;i<12;i++) {
			if (i==0) {
				r2 = 1.1;
				boundingorientation.scale(1.5);
			} else {
				r2 = .3;
				if (i==1) {
					boundingorientation=orientation;
					boundingorientation.scale(r2*0.625f);
				}
				boundingcenter.translate(&boundingorientation);
			}
			distanceVector = boundingcenter;
			distanceVector.translate(&t.getPosition(),-1);
			distance = distanceVector.getMagnitude();
			if (distance < (r1 + r2)) {
				collision();
				t.collision();
				return true;
			}
		}
	}
	return false;
}

bool Ship::collision(Powerup &t) {
	bool b = false, c = ignorecollision;
	ignorecollision = true;
	b = testCollisionHack(t);
	ignorecollision = c;
	if (b) {
		if (t.isWeapon()) {
			setWeapon((WeaponPowerup*)&t);
		} else if (t.isShield()) {
			setShield((ShieldPowerup*)&t);
			invulnerable=30000;
			if (shield < 100) shield = 80;
//			if (invulnerable > 60000) invulnerable = 60000;
		}
		//shield up regardless, because we're NICE :)
		shield += 20; if (shield > 200) shield = 200;
	}
	return b;
}

void Ship::clearWeapon() {
	if (weaponup) {
		delete weaponup;
		weaponup = NULL;
	}
}

void Ship::setWeapon(WeaponPowerup *t) {
	//TODO: if same weaponup, upgrade further
	clearWeapon();
	weaponup = t;
}


void Ship::clearShield() {
	if (shieldup) {
		delete shieldup;
		shieldup = NULL;
	}
}

void Ship::setShield(ShieldPowerup *t) {
	clearShield();
	shieldup = t;
}

bool Ship::collision(Thing &t) {
	bool b = testCollisionHack(t);
	return b;
}

void Ship::explode() {
	if (exploded) return; // just once, yo!
	exploded = true;
	state->startRelativeSound(SOUND_SHIP_BOOM);
	Explosion::explode(150,1.0f,this,2000.f);
}

void Ship::setBank(int i) { banked = i; }

void Ship::setThrust(bool b,bool fade) { 
	thrust = b; 
	if (thrust) {
		rewindSound(SOUND_THRUST);
		state->startRelativeSound(SOUND_THRUST,getPosition());
	}
	else {
		stopSound(SOUND_THRUST);
		if (fade) state->startRelativeSound(SOUND_THRUSTFADE,getPosition());
	}
}

void Ship::pause(bool pause) {
	if (pause) {
		if (thrust) pauseSound(SOUND_THRUST);
	} else {
		if (thrust) resumeSound(SOUND_THRUST);
	}
}

void Ship::iterate(float diff) {
	if (firing > 0) {
		firing -= diff;
		if (firing <= 0) {
			fire();
		}
	}
	XYZ position = getPosition();
	XYZ axis = position;
	XYZ tempv, oaxis;

	axis.cross(orientation);

	Thing::iterate(diff);

	if (invulnerable > 0) {
		invulnerable -= diff;
		if (invulnerable <= 0) {
			invulnerable = 0;
		}
	}

	XYZ velocity = getVelocity();
	float speed = velocity.getMagnitude();
	
	float orientationr6[6];
	for (int i=0;i<3;i++) {
		orientationr6[i] = orientation.r3[i];
		orientationr6[i+3] = -position.r3[i];
	}
	
	soundlib_updateListener(position.r3,velocity.r3,orientationr6); // by orientation
	//soundlib_updateListener(position.r3,velocity.r3,velocity.r3); // by screen-forwardness // buggier
	
	if (thrust) {
		thrustfade = 1.0;
		XYZ exhaust_velocity; exhaust_velocity = orientation; //exhaust_velocity.setXYZ(0,0,0);// = orientation;
		exhaust_velocity.scale(-1);
		XYZ exhaust_position = getPosition();
		exhaust_position.translate(&orientation,-.1);
		// now jiggle position by ... perpendicular to orientation.  so by position, positive or negative, and by axis, positive or negative.
		XYZ jiggle = getPosition();
		jiggle.normalize(1);
		float scale = ((double)rand()/((double)RAND_MAX)+(double)(1))*.4 -.5 ;
		jiggle.scale(scale);
		exhaust_position.translate(&jiggle);

		jiggle = axis;
		jiggle.normalize(1);
		scale = ((double)rand()/((double)RAND_MAX)+(double)(1))*.4 -.5 ;
		jiggle.scale(scale);
		exhaust_position.translate(&jiggle);

		Exhaust *e = new Exhaust(&exhaust_velocity,&exhaust_position);
		state->exhaust.push_back(e);
		if (speed < .0001) {
			velocity = orientation;
			orientation.normalize(.0001);
		} 
		velocity.translate(&orientation,diff*.008);
		speed = velocity.getMagnitude();
		if (speed > 6) velocity.normalize(6);
	} else {
		if (speed > 0) {
			speed -= .0004 * diff;
			if (speed < 0) speed = 0;
			velocity.normalize(speed);
		}
		thrustfade -= .002 * diff;
		if (thrustfade < 0) thrustfade = 0;
	}

	setVelocity(velocity);

	/* UPDATE ORIENTATION */
	// separate orientation vector into axis-parallel and axis-normal 
	// parts.  rotate part of orientation vector which is orthogonal to 
	// axis, then add  back in the part of the orientation vector which 
	// is in the same direction as the axis. Simplified since ||a||=1; 
	// note &tempv is still unit vector of velocity, so third statement 
	// could be cut.

	if (speed > 0) {

		XYZ parallel,normal,v1,v2;
	
		parallel = axis;
		parallel.scale(axis.dot(orientation));

		normal = orientation;
		normal.translate(&parallel,-1);

		v1 = axis;
		v1.cross(getPosition());

		v2 = getVelocity();
		//float tempf = v2.dot(orientation);///(getVelocity().getMagnitude()); // * orientation.getMagnitude(), but that's always 1!

		//v1.scale( (tempf <= 0)?-1:1 * normal.getMagnitude() );

		normal = v1;

		orientation = parallel;
		orientation.translate(&normal);

		orientation.normalize();
	}
		rot_ticks+=diff * (speed * .1 + .025);
	
	if (banked != 0) {
		tempv = orientation;
		tempv.cross(axis);
		oaxis = getPosition();
		oaxis.cross(orientation);
		if (speed > 0) {
			oaxis.scale(banked*.0003*diff);
		} else {
			oaxis.scale(banked*.0001*diff);
		}
		orientation.translate(&oaxis);
		orientation.normalize();
	}
	
	moveSound(SOUND_THRUST,getPosition().r3);
	moveSound(SOUND_THRUSTFADE,getPosition().r3);

}

void Ship::setLighting() {
glPushMatrix();
	XYZ right, up;
	up = getPosition();
	right = getPosition();
	right.cross(orientation);
	right.normalize(-1);
	up.normalize(-1);
	XYZ lookat = orientation;
	lookat.scale(2.8);
	lookat.translate(&getPosition());
//	lookat.translate(&up,.55);
	glTranslatef(lookat);
	lookat.scale(0);
#ifdef DEBUG_LIGHTING
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glColor4f(1,1,1,1);
	gluSphere(light,.25,5,5);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
#endif

	//glLightfv(GL_LIGHT0,GL_POSITION,getPosition().r3);
	glLightfv(GL_LIGHT0,GL_POSITION,lookat.r3);
glPopMatrix();	
	//glLightfv(GL_LIGHT0,GL_POSITION,getPosition().r3);
	glLightfv(GL_LIGHT1,GL_POSITION,lookat.r3);

//	glLightfv(GL_LIGHT0,GL_POSITION,lookat.r3);
}

void Ship::render() {

XYZ axis;
glPushMatrix();
	glTranslatef(getPosition());

	// FOR LIGHTING UP ORIENTATION AND VELOCITY FOR DEBUGGING
/*
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHTING);
	// draw orientation for debugging
	glDisable(GL_TEXTURE_2D);
glPushMatrix();
glLineWidth(2);
	glColor4f(1,0,0,1);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(orientation.r3[0],orientation.r3[1],orientation.r3[2]);
	glEnd();
glPopMatrix();


glPushMatrix();
	// draw velocity for debugging
	glDisable(GL_TEXTURE_2D);
glPushMatrix();
glLineWidth(2);
	glColor4f(0,0,1,1);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(getVelocity().r3[0],getVelocity().r3[1],getVelocity().r3[2]);
	glEnd();
glPopMatrix();
glPopMatrix();
	glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
*/

	XYZ sign;
	//light 0 is "headlamp"
	//light 1 is "sun"
  glDisable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

	// reorient for pitch
	XYZ right, up;
	up = getPosition();
	right = getPosition();
	right.cross(orientation);
	right.normalize(-1);
	up.normalize(-1);
	state->setOrientationMatrix(&right,&up,&orientation);


	//size the ship a little :)
	glScalef(4,4,4);

	glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);

	// set the ship the right direction
	glPushMatrix();
		glMultMatrixf(state->orientationMatrix);

		glPushMatrix();

            glRotatef(80,1,0,0);
			glRotatef(rot_ticks,0,1,0);

			if (weaponup) {
				glEnable(GL_COLOR_MATERIAL);
				glColor3fv(weaponup->getColor4fv());
		       	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,weaponup->getColor4fv());
		       	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,weaponup->getColor4fv());
			}

			glCallList(displayList);

		    glEnable(GL_BLEND);
			glPushAttrib(GL_ALL_ATTRIB_BITS);
				GLfloat special_ambient[] = {1,1,1,.5};
				glEnable(GL_TEXTURE_GEN_S); // PRETTY =)

				glMaterialfv(GL_FRONT,GL_DIFFUSE,special_ambient);
				glMaterialfv(GL_FRONT,GL_AMBIENT,special_ambient);
				glCallList(displayList);
				glDisable(GL_TEXTURE_GEN_S);
			glPopAttrib();

			float ones[] = {1.f,1.f,1.f,1.f};
			float zeroes[] = {0.f,0.f,0.f,0.f};
	       	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,zeroes);
	       	
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	glScalef(.25,.25,.25);
    glScalef(1.7,1.7,1.7);
		glPushMatrix();
			glEnable(GL_COLOR_MATERIAL);
			glDisable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA,GL_ONE);

			int color = 0;
			float thrust_transparency = .02;
			glRotatef(-10,1,0,0);
			glRotatef(rot_ticks/7,0,0,1);
//			glTranslatef(0,0,-.02);
			for (float f = 0; f < .6; f+=.04) {
				if (f > 0) {
					glEnable(GL_BLEND);
					glBlendFunc(GL_ONE,GL_ONE);
				}
				switch (color++) {
					case 0: glColor4f(.3+(.3*thrustfade),0,0,thrust_transparency); break; // to .6, 0, 0
					case 1: glColor4f(.4+(.2*thrustfade),.3+(.1*thrustfade),.2-(.2*thrustfade),thrust_transparency); break; // to .6, .4, 0
					case 2: glColor4f(.4,.4,.3,thrust_transparency); break; // to .5, .5, .0
					default: glColor4f(.4+(.2*thrustfade),.4-(.2*thrustfade),.4-(.4*thrustfade),thrust_transparency); color=0; break; // to .6, .2, 0

				}
				glTranslatef(0,0,-.007);
				glBegin(GL_TRIANGLE_FAN);
					glVertex2d(0,0);
					double radius = (.6-f)*.2+.03;
					for (double a=0.0;a<=2*PI;a+=.2) {
						glVertex2d(cos(a) * radius,sin(a)*radius);
					}
					glVertex2d(radius,0);
				glEnd();
			}
			glDisable(GL_COLOR_MATERIAL);
		glPopMatrix();
		glDisable(GL_BLEND);

		if (invulnerable) {
	       	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,ones);
			glDisable(GL_DEPTH_TEST);
			glRotatef(80,1,0,0);
			glRotatef(rot_ticks,0,1,0);
			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);

				glTranslatef(0,.4,0);
				glColor4f(1.0f,1.0f,1.0f,min(0.3f,invulnerable/3000.0f));
				glPushMatrix();
					gluSphere(shield_quadric,1,16,16);
				glPopMatrix();
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_BLEND);
			glDisable(GL_COLOR_MATERIAL);
			glEnable(GL_DEPTH_TEST);
	       	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,zeroes);
		}

		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,ones);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glColor4f(1.f,1.f,1.f,1.f);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,zeroes);

	glPopMatrix();
glPopMatrix();
  glDisable(GL_LIGHT1);
/* debugging bounding spheres for ship */
#ifdef DEBUG_SHIP_BOUNDING
glPushMatrix();
	glColor4f(1,1,1,.25);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
	XYZ boundingcenter = getPosition();
	XYZ boundingorientation = orientation;
	glTranslatef(boundingcenter);
	float boundingsize;
	for (int i=0;i<12;i++) {
		if (i==0) {
			boundingsize = 1.1;
			boundingorientation.scale(1.5);
		} else {
			boundingsize = .3;
			if (i==1) {
				boundingorientation = orientation;
				boundingorientation.scale(boundingsize*.625f);
			}
			glTranslatef(boundingorientation);


		}
		gluSphere(bounding_quads[i],boundingsize,12,12);
	}
glPopMatrix();
glPushMatrix();
	glColor4f(0,0,0,.25);
	boundingcenter = getPosition();
	boundingorientation = orientation;
	boundingorientation.scale(.7);
	boundingcenter.translate(&boundingorientation);
	glTranslatef(boundingcenter);
	gluSphere(shield_quadric,1.6,12,12);
glPopMatrix();
glDisable(GL_BLEND);
#endif // DEBUG_SHIP_BOUNDING


//	glLightfv(GL_LIGHT0,GL_POSITION,zeroes);
  glEnable(GL_LIGHT0);

}
