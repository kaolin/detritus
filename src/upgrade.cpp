#include "upgrade.h"
#include "os.h"
#include <iostream>
#include <fstream>
/* may need to include sqlite3 at some future date, or I may drop that upgrade stuff into the high scores file *cough* */

void upgrade(void) {
	/* platform generic way of getting "application data" path */
	const char *path = getApplicationDataPath();

	//FIRST upgrade!  Copying old Detritus.db if it's in the wrong place, if one doesn't exist in the _right_ place
	char *datafile = "/detritus.db";
	char *patheddatafile = new char[strlen(datafile) + strlen(path) + 1];
	strcpy(patheddatafile,path);
	strcat(patheddatafile,datafile);
	ifstream fin(patheddatafile,ifstream::in);
	fin.close();

	//if datafile doesn't exist in the spot we're upgrading it to, see if one exists in the old data dir
	if (fin.fail()) {
		fin.clear();
		fin.open("data/detritus.db",ifstream::in|ifstream::binary);
		//if datafile does exist in the old data dir
		if (!fin.fail()) {
			ofstream fout(patheddatafile,ifstream::out | ifstream::binary);
			fout << fin.rdbuf();

			fout.close();
		}
		fin.close();
	}
	
	delete(patheddatafile);

}
