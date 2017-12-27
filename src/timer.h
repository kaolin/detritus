#ifndef KIF_SDL_TIMER
#define KIF_SDL_TIMER

//The timer

class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;
    
    //The ticks stored when the timer was paused
    int pausedTicks;
    
    //The timer status
    bool paused;
    bool started;
    
		//alarm
		unsigned int alarm;
		int snooze;

    public:
    //Initializes variables
    Timer();
    
    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();
    
    //Get the number of ticks since the timer started
    //or gets the number of ticks when the timer was paused
    int get_ticks();
    
    //Checks the status of the timer
    bool is_started();
    bool is_paused();    

		//alarm functions
	void setAlarm(int seconds);
	void setAlarm(int seconds, int snooze);
	bool checkAlarm();
	void snoozeAlarm();
	void snoozeAlarm(int);
	void clearAlarm();

};

#endif
