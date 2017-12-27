#include "thing.h"
#include "game.h"
#include "state.h"

Thing::Thing() {
	state = &State::Get();
	mesh = NULL; // just to be safe!
	//cout << "new thing: " << this << endl;
}

Thing::~Thing() {
	//if (mesh) delete(mesh);
	mesh = NULL;
	state = NULL;
	//cout << "--- THING DESTROYED: " << this << " ------" << endl;
}

bool Thing::collision() {
	return false;
}

const Mesh * Thing::getMesh() { return mesh; }

bool Thing::collision(Thing &t) {
	float r1, r2, distance;
	XYZ distanceVector;

//cout << "TESTING: " << &t << " VERSUS " << this << endl;

//cout << "IN COLLISION" << endl;
	//if (&t == 0 || (&t < (void*)0xa0000)) {
	//if (&t == 0 || ((&t < (void*)0x7000000))) {
/*
	if (&t == 0 || ((&t < (void*)0x7000000) || (&t > (void*) 0x10000000))) {
//cout << "SKIPPING BAD DATA: " << &t << endl;
		return false;
	}
*/
	//if (&t == 0) return false;

//cout << "BEFORE t.mesh" << endl;
//cout << &(t.mesh) << endl;
	if (&t.mesh) {
//cout << "TMESH, GOING IN" << endl;
//cout << "address of: " << &t.mesh << endl;
//cout << "ref#: " << t.mesh << endl;
//t.mesh->getMaximum();
//cout << "FUNCTION WAS SAFE TO CALL" << endl;
//cout << t.mesh->getMaximum() << " (cout result) " << endl;
//cout << "NOW CALLING WITH ASSIGNMENT" << endl;
r1 = t.mesh->getMaximum();
		//r1 = t.mesh->getMaximum();
//cout << t.mesh->getMaximum() << endl;
	} else {
//cout << "DEFAULT r1" << endl;
		r1 = 0;
	}

//cout << "BEFORE mesh" << endl;
	if (mesh) {
		r2 = mesh->getMaximum();
	} else {
//cout << "DEFAULT r2" << endl;
		r2 = 0;
	}

	distanceVector = position;
//cout << "POSITION: " << position << endl;

	distanceVector.translate(&t.position,-1);
	distance = distanceVector.getMagnitude();


	if (distance < (r1 + r2)) {
//cout << " COLLISION ! " << endl;
		collision();
		t.collision();
		return true;
	}
	return false;

}

void Thing::setVelocity(const XYZ &x) { 
	velocity = x; // implicit memberwise copy
}

void Thing::setPosition(const XYZ &x) { 
	position = x; // implicit memberwise copy
}
void Thing::setVelocity(float x, float y, float z) { velocity.setXYZ(x,y,z); }
void Thing::setPosition(float x, float y, float z) { position.setXYZ(x,y,z); }
const XYZ& Thing::getPosition() { return position; }
const XYZ& Thing::getVelocity() { return velocity; }
const XYZ& Thing::getAxis() { return axis; }
float Thing::getTicks() { return state->thing.get_ticks(); }

void Thing::iterate(float diff) {
	double tempf, speed, scale;
	diff/=100; // scale for happier being

	XYZ v1, v2, ac, rc, v3;
	axis = getPosition(); // so we've got this prepped for figuring out our axis

	// update position by rotating it by Theta, the amount called for by
	// the velocity vector.  The below definition assumes radian
	// measure for the angles; Since ||p|| =1, the expression for theta 
	// is simplified.

	speed=getVelocity().getMagnitude();
	scale=getPosition().getMagnitude();

	v3 = getPosition();
	v3.scale(1/scale); // 0,0,0 IS NOT ALLOWED, OKAY? :)
	if (speed > .00001) { // else too slow to care...? and destroys our calcs!
		v1 = v3;
		tempf=.1*speed/(2.0f*PI)*diff;
		v1.scale(cos(tempf));
		v2 = getVelocity();
		v2.scale(sin(tempf)/speed);
		v1 += v2;
		v1.normalize(scale);
		setPosition(v1);
		
		//ob->speed = magnitude(ob->velocity);
		//if (speedlimit > 0) {
			//if (ob->speed > speedlimit) ob->speed = speedlimit;
		//}
	}
	
	// rotate velocity vector to maintain orthogonality to position vector
	axis.cross(getVelocity()); // old position crossed old velocity gives old axis
	v1 = axis;
	v1.cross(getPosition()); // cross with new position to get new velocity
	v1.normalize(speed); // keep this constant despite all the number fudging
	setVelocity(v1); // set new velocity

}
