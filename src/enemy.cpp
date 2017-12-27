#include "enemy.h"
#include "shard.h"
#include "thing.h"
#include "geometry.h"
#include "state.h"
#include "powerup.h"
#include "ship.h"


int Enemy::enemies = 0;
GLuint Enemy::displayList = 0;
Model_3DS Enemy::model;

//forced collision
bool Enemy::collision() {
	explode();
	return true;
}

int Enemy::getEnemiesCount(void) {
    return enemies;
}
        
bool Enemy::collision(Thing &t) {
	if (Thing::collision(t)) {
		//collision();
	}
	return false;
}

void Enemy::iterate(float t) {
	Asteroid::iterate(t);
	state->moveRelativeSound(SOUND_ENEMY,getPosition());
	nextbullet -= t;
	if (nextbullet < 0) {
		fire();
		nextbullet = ((rand()%3)+2) * 1000;
	}
}


void Enemy::explode() {
	// TODO
	vector<Triangle*> triangles = mesh->getTriangles();
	int count = triangles.size();
	Shard* s;

	const XYZ position = getPosition();
	const XYZ velocity = getVelocity();

	// turn each triangle into a shard
	Triangle t;
	for (int i=0;i<count;i++) {
		t = triangles[i];  // implicit memberwise blah blah blah???  man, I need to worry about memory so much more than this.
		s = new Shard(&t,&velocity,&position);
		state->shards.push_back(s);

	}
	// clean up the enemy!
	state->remove_asteroids.push_back(this);
	//state->stopSound(SOUND_ENEMY);
	pauseSound(SOUND_ENEMY);
	rewindSound(SOUND_ENEMY);
//	alSourceStop(SOUND_ENEMY);
	state->startRelativeSound(SOUND_ASTEROID_BOOM,getPosition());
	Explosion::explode(50,.7f,this,3000.f);
}

void Enemy::init(void) {
    enemies++;
    mesh->subdivide(1,false);
/*    if (!displayList) {
		model.Load(DATAPREFIX"data/models/saucer001.3ds");
		model.scale = .12f;
		displayList = glGenLists(1);
		glNewList(displayList,GL_COMPILE);
		   	glScalef(3,3,3);
			model.Draw();
		glEndList();
	}
*/
	state->startRelativeSound(SOUND_ENEMY,getPosition());
	nextbullet = ((rand()%3)+2) * 1000;
}

Enemy::Enemy():Asteroid(1,false) {
	init();
}

Enemy::Enemy(const XYZ& position):Asteroid(1,false) {
	init();
	setPosition(position);
}

Enemy::~Enemy() {
	//glDeleteLists(displayList,1);
	enemies--;
}

void Enemy::render() {
	glPushMatrix();

	XYZ right, up, velocity;
	up = getPosition();
	right = getPosition();
	velocity = getVelocity();
	velocity.normalize();
	right.cross(velocity);
	right.normalize(-1);
	up.normalize(-1);
	state->setOrientationMatrix(&right,&up,&velocity);

	glTranslatef(getPosition());
	
    glMultMatrixf(state->orientationMatrix);
	glColor4f(1,1,1,1);

   	glRotatef(getTicks()/10,0,1,0);

	glCallList(displayList);

    glPopMatrix();    
}

void Enemy::fire() {
	XYZ velocity = getVelocity();
	XYZ position = getPosition();
	XYZ tmp, effect;
	float speed;

	//try to fire at the nearest thing
	vector<Asteroid*>::iterator j;
	j = state->asteroids.begin();
	Thing *best = NULL;
	float bestdistance=10000;
	XYZ bestVector;
	XYZ distanceVector;
	float distance;

	while (j != state->asteroids.end()) {
		Asteroid *a = *j++;
		distanceVector = position;
		distanceVector.translate(&a->getPosition(),-1);
		distance = distanceVector.getMagnitude();
		if (a != this && distance < 9 && (best == NULL || distance < bestdistance)) {
			bestdistance = distance;
			best = a;
			bestVector=distanceVector;
		}
	}

	//oh yeah, consider the player as a target ;)	
	distanceVector = position;
	distanceVector.translate(&state->ship->getPosition(),-1);
	distance = distanceVector.getMagnitude();
	if (distance < 9 && distance < bestdistance) {
		best = state->ship;
		bestVector = distanceVector;
	}

	if (best) {
		effect = bestVector;
		effect.scale(-1);
		effect.translate(&best->getVelocity()); // why not let them lead, huh? :)
	} else {
		//pick a random direction orthogonal to our position
		do {
			tmp.r3[0] = rand()/(float)RAND_MAX;
			tmp.r3[1] = rand()/(float)RAND_MAX;
			tmp.r3[2] = rand()/(float)RAND_MAX;
			effect = position;
			effect.cross(tmp);
		} while ((speed = effect.getMagnitude()) < .1 && speed > -.1); // get some real direction
	}
	
	effect.normalize(5);


	Bullet *b;
	b = new EnemyBullet(&effect,&position);
	state->enemybullets.push_back(b);
	state->startRelativeSound(SOUND_BULLET,getPosition());

}
