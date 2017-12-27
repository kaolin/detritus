#ifndef __POWERUP_H
#define __POWERUP_H

#include <vector>
#include <iostream>

#include "timer.h"
#include "geometry.h"

#include "os.h"
#include "glsdl.h"
#include "thing.h"

using namespace std;

class Powerup: public Thing {
	public:
		virtual void render(void);
		virtual bool collision(void);
		virtual bool collision(Thing &);
		void show(const XYZ*);
		virtual bool isWeapon() { return false; }
		virtual bool isShield() { return false; }

		static Powerup* spawnPowerup(void);
		
		Powerup();
		~Powerup();
		
		virtual const float* getColor4fv()=0;

		void iterate(float t);

	private:
		float ticks;
		GLUquadric* quad;
		GLuint displayList;
};

class WeaponPowerup: public Powerup { 
    public:
        virtual void fire(XYZ *position, XYZ *velocity, XYZ *orientation)=0;
        virtual bool isWeapon() { return true; }
    protected: // TODO :)
        int level;
};

class ShieldPowerup: public Powerup {
	public:
		virtual bool isShield() { return true; }
};

class SpreadPowerup: public WeaponPowerup { 
    public: 
        virtual void fire(XYZ *position, XYZ *velocity, XYZ *orientation);
        virtual const float* getColor4fv();
    private:
        static const float color[4];
};

class LaserPowerup: public WeaponPowerup { 
    public: 
        virtual void fire(XYZ *position, XYZ *velocity, XYZ *orientation);
        virtual const float* getColor4fv();
    private:
        static const float color[4];
};

class SeekerPowerup: public WeaponPowerup { 
    public: 
        virtual void fire(XYZ *position, XYZ *velocity, XYZ *orientation);
        virtual const float* getColor4fv();
    private:
        static const float color[4];
};

class StarburstPowerup: public WeaponPowerup { 
    public: 
        virtual void fire(XYZ *position, XYZ *velocity, XYZ *orientation); 
        virtual const float* getColor4fv();
    private:
        static const float color[4];
};

class BlowthroughPowerup: public WeaponPowerup { 
    public: 
        virtual void fire(XYZ *position, XYZ *velocity, XYZ *orientation);
        virtual const float* getColor4fv();
    private:
        static const float color[4];
};

class PermashieldPowerup: public ShieldPowerup { 
    public: 
        virtual const float* getColor4fv();
    private:
        static const float color[4];
};


#endif
