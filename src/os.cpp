#include "os.h"
#include "game.h" // for version and path and such

#ifdef WINDOWS
#include <shlwapi.h> // wuf!
#else
#include <pwd.h> // password manip stuff unix-happy (OS X happy too!)
#endif
/*

http://msdn.microsoft.com/en-us/library/bb762494.aspx

Note  In Windows Vista, these values have been replaced by KNOWNFOLDERID values. See that topic for a list of the new constants and their corresponding CSIDL values. For convenience, corresponding KNOWNFOLDERID values are also noted here for each CSIDL value.

The CSIDL system is supported under Windows Vista for compatibility reasons. However, new development should use KNOWNFOLDERID values rather than CSIDL values.

http://support.microsoft.com/kb/310294/en-us

To locate the correct Application Data folder, pass the appropriate CSIDL value, based on the category of your application data.
•	For each user (roaming) data, use the CSIDL_APPDATA value. This defaults to the following path:
\Documents and Settings\<User Name>\Application Data
•	For each user (non-roaming) data, use the CSIDL_LOCAL_APPDATA value. This defaults to the following path:
\Documents and Settings\<User Name>\Local Settings\Application Data
•	For each computer (non-user specific and non-roaming) data, use the CSIDL_COMMON_APPDATA value. This defaults to the following path:
\Documents and Settings\All Users\Application Data 

Do not store application data directly in this folder. Instead, use the PathAppend function to append a subfolder to the path that SHGetFolderPath returns. Make sure that you use the following convention:
Company Name\Product Name\Product Version 

For example, the resultant full path may appear as follows:
\Documents and Settings\All Users\Application Data\My Company\My Product\1.0 

*/

boolean is_intel_byte_order() {
	static long test = 0x12345678;
	return (*(unsigned char*)(&test) == 0x78) ? true : false;
}

ostream* getErrorStream() {
	static ostream *retval = NULL;
	#ifdef DEBUG_MYERROR
		retval = new ofstream("/tmp/detritus_err.txt");
	#else
		retval = (ostream*)&cerr;
	#endif
	return retval;
}

const char* getApplicationDataPath(void) {
	char* path=NULL;
	if (path != NULL) return path;
	
	#ifdef WINDOWS
	TCHAR winpath[MAX_PATH+1];
	SHGetFolderPath(NULL,CSIDL_APPDATA,NULL,0,winpath); //can wrap call with SUCCEEDED, else DWORD err = GetLastError()

	PathAppend(winpath,WINPATH_COMPANY);
	CreateDirectory(winpath,NULL); // NULL = we don't want to worry about permissions, sheesh!
	PathAppend(winpath,WINPATH_GAME);
	CreateDirectory(winpath,NULL);
	PathAppend(winpath,WINPATH_VERSION);
	CreateDirectory(winpath,NULL);
	
	path = new char[strlen(winpath)+2];
	strcpy(path,winpath);
    strcat(path,"/");
	
	/*SHCreateDirectoryEx requires win2k/winme+
	PathAppend(winpath,_T("\\Erif\\Detritus\\1.0\\") );
	SHCreateDirectoryEx(NULL,winpath,NULL); // ERROR_FILE_EXISTS/ERROR_ALREADY_EXISTS or ERROR_BAD_PATHNAME or ERROR_SUCCESS ...
	*/
	
	#else 
            //good for OS X, good for linux
            struct passwd *foo = getpwuid(geteuid());
            path = new char[strlen(foo->pw_dir)+3];
            strcpy(path,foo->pw_dir);
            strcat(path,"/.");
            (*getErrorStream()) << path << endl;
	#endif
	
	/* my catchall that should never be reached */	
	if (path == NULL) path = "data";
	return path;
}
