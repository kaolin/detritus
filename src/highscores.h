#ifndef __HIGHSCORES_H
#define __HIGHSCORES_H

#include <string>
#include "os.h"

class HighScore;

class HighScores {
	public:
		HighScores(const char *filename);
		virtual const HighScore* getScores();
		virtual ~HighScores();
		virtual bool hasScores(void);
		virtual void reloadScores(void);
		virtual void addScore(const char *name, int score);
		virtual bool isHighScore(int score, int n=10);
		virtual bool isFailed() { return failed; }
	private:
		HighScore *hs;	
		char *dbname;
		bool failed;
};

class HighScore {
	friend class HighScores;
	public:
		virtual const char* getName() const;
		virtual int getScore() const;
		virtual bool hasNext() const;
		virtual HighScore* getNext() const;
		HighScore(const char *name, int score);
		virtual ~HighScore();
	private:
		char* name;
		int score;
		int date; // TODO?
		HighScore *next;
};
#endif
