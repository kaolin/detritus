#include <sqlite3.h>
#include "highscores.h"
#include <iostream>

using namespace std;

static std::ostream *myerror;

sqlite3* getDB(const char *filename) {
#ifdef DEBUG_MYERROR
(*myerror) << "OPENED DB:" << filename << endl;
#endif
	sqlite3 *db;
	int rc=0;
	rc = sqlite3_open(filename,&db);
	if (rc) {
#ifdef DEBUG_MYERROR
		*myerror << "Can't open database: " << sqlite3_errmsg(db) << endl;
#endif
		sqlite3_close(db);
		return NULL;
	}
	return db;
}

void closeDB(sqlite3* db) {
	sqlite3_close(db);
}

void errorDB([[maybe_unused]] sqlite3* db) {
#ifdef DEBUG_MYERROR
	*myerror << "DB Error: " << sqlite3_errmsg(db) << endl;
#endif
	exit(EXIT_FAILURE);
}

HighScores::HighScores(const char *filename) {
	myerror = getErrorStream();
        failed = false;
	hs = NULL;
	dbname=new char[strlen(filename)+1];
	strcpy(dbname,filename);
	sqlite3* db = getDB(dbname);
	sqlite3_stmt *stmt;
	if (db != NULL) {
		//SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'highscores' .. if not exists, create!
		if (sqlite3_prepare(db,"CREATE TABLE IF NOT EXISTS highscores(name varchar(32),score int, stamp datetime);",-1,&stmt,0)) errorDB(db);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		closeDB(db);
	} else {
#ifdef DEBUG_MYERROR
		*myerror << "UNABLE TO LOAD DB: " << dbname << endl;
#endif
	}
	reloadScores();
}

bool HighScores::hasScores(void) { if (failed) return false; return hs != NULL; }

const HighScore* HighScores::getScores() {
	if (failed) return NULL;
	return hs;
}

HighScores::~HighScores() { if (hs) delete(hs); if (dbname) delete dbname; }

void HighScores::reloadScores(void) {
	sqlite3 *db = getDB(dbname);
	sqlite3_stmt *stmt;
	int rc;
	HighScore *next=NULL, *tmp;
	char s[256];

	if (hs) { delete(hs); hs = NULL; }
	if (db == NULL) {
		failed = true;
#ifdef DEBUG_MYERROR
		(*myerror) << "unable to get db to load high scores" << endl;
#endif
	} else {
		if (sqlite3_prepare(db,"SELECT name, score, stamp FROM highscores ORDER BY score DESC, stamp ASC LIMIT 10;",-1,&stmt,0)) errorDB(db);
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		strncpy(s,(char*)sqlite3_column_text(stmt,0),255);
		s[255]=0;
		tmp = new HighScore(s,sqlite3_column_int(stmt,1));
			if (hs == NULL) {
				next = hs = tmp;
			} else {
				next->next = tmp;
				next = tmp;
			}
		}
		sqlite3_finalize(stmt);
		closeDB(db);
	}
}

void HighScores::addScore(const char* name, int score) {
	if (failed) return;
	sqlite3_stmt* stmt;
	sqlite3* db = getDB(dbname);
	if (sqlite3_prepare(db,"INSERT INTO highscores(name,score,stamp) VALUES(?,?,datetime('now'));",-1,&stmt,0)) errorDB(db);
	if (sqlite3_bind_text(stmt,1,name,strlen(name),SQLITE_STATIC)) errorDB(db);
	if (sqlite3_bind_int(stmt,2,score)) errorDB(db);
	sqlite3_step(stmt); // returns rs, not ERROR
	sqlite3_finalize(stmt);
	closeDB(db);
	reloadScores();
}

/* check to see if this is a new high score out of N */
bool HighScores::isHighScore(int score, int n) {
	if (failed) return false;
	if (score < 1) return false;
	HighScore *next = hs;
	while (n > 0 && next != NULL) {
		if (score > next->score) return true;
		next = next->next;
		n--;
	}
	if (next == NULL && n >0) return true;
	return false;
};

const char* HighScore::getName() const { return name; }

int HighScore::getScore() const { return score; }

bool HighScore::hasNext() const { return next != NULL; }

HighScore * HighScore::getNext() const { return next; }

HighScore::HighScore(const char *name, int score) {
	this->score = score;
	this->name = new char[strlen(name)+1];
	strcpy(this->name,name);
	this->next = NULL;
}

HighScore::~HighScore() {
	if (next) delete(next);
	delete(name);
}
