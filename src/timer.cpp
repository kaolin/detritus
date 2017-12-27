#include "timer.h"

#include "SDL/SDL.h"

Timer::Timer()
{
    //Initialize the variables
		alarm = 0;
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;    
}

void Timer::start()
{
    //Start the timer
    started = true;
    
    //Unpause the timer
    paused = false;
    
    //Get the current clock time
    startTicks = SDL_GetTicks();    
}

void Timer::setAlarm(int seconds) {
	setAlarm(seconds,seconds);
}

void Timer::setAlarm(int seconds, int snooze) {
	alarm = seconds+SDL_GetTicks();
	this->snooze = snooze;
	start();
}

bool Timer::checkAlarm() {
	if (alarm > 0) {
		if (alarm <= SDL_GetTicks()) {
			return true;
		}
	}
	return false;
}

void Timer::snoozeAlarm() {
	if (alarm > 0) {
		alarm = SDL_GetTicks()+snooze;
	}
}

void Timer::snoozeAlarm(int ticks) {
	if (alarm > 0) {
		alarm = SDL_GetTicks()+ticks;
	}
}

void Timer::clearAlarm() {
	alarm = 0;
}

void Timer::stop()
{
    //Stop the timer
    started = false;
    
    //Unpause the timer
    paused = false;    
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;
    
        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;
    
        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;
        
        //Reset the paused ticks
        pausedTicks = 0;

				if (alarm > 0) alarm -= pausedTicks;
    }
}

int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }    
    }
    
    //If the timer isn't running
    return 0;    
}

bool Timer::is_started()
{
    return started;    
}

bool Timer::is_paused()
{
    return paused;    
}
