#ifndef __SHIP_H
#define __SHIP_H

#include <vector>
#include <iostream>

#include "timer.h"

#include "os.h"
#include "glsdl.h"
#include "thing.h"
#include "powerup.h"
#include "game.h"

#include "Model_3DS.h"

using namespace std;

// Forward declaration of Explosion class, why not?
class Explosion;
class State;
class Ship: public Thing {

	friend class Explosion; // explosions should be able to remove themselves from the ship

	public:
		friend class State;
		virtual void render();
		Ship();
		virtual ~Ship();
		XYZ orientation;
		virtual bool collision();
		virtual bool collision(Thing&);
		bool collision(Powerup&);
		void setWeapon(WeaponPowerup*);
		void setShield(ShieldPowerup*);
		void clearWeapon();
		void clearShield();
		void orientGLViewport();
		void setLighting();
		void iterate(float);
		void setThrust(bool,bool fade=true);
		void setBank(int); // -1, 1, or 0
		void setViewport(int=-1); // 0, 1, ... -1 == increment
		void flip();
		void pause(bool=true);
		void fire(bool=true);
		void teleport();
		XYZ center; // where we're "really" oriented, for smoothness and stuff
		int getShieldStrength();
		bool hasExploded();
		void reset();
		void leveled();
		void halt();
		const XYZ& getAxis();
		float invulnerable; // msecs
		
	private:
		XYZ getLookAt();
		XYZ axis;
		float firing;
		int shield; // percentage, essentially 0 to 100 :)
		void explode();
		bool exploded;
		float rot_ticks;
		float hits; // would be an int, but we do funky things for merging
		bool special;
		static GLuint displayList;
		bool thrust;
		int banked;
		int viewport;
		static Model_3DS model;
		float thrustfade;
		GLUquadric *shield_quadric;
		bool ignorecollision;
		vector<Explosion*> explosions;
		vector<Explosion*> remove_explosions;
		WeaponPowerup *weaponup;
		ShieldPowerup *shieldup;
		bool testCollisionHack(Thing &);
		GLUquadric *light;
#ifdef DEBUG_SHIP_BOUNDING
		//for debugging collision detection
		GLUquadric* bounding_quads[12];		
#endif // DEBUG SHIP_BOUNDING
};

#endif
