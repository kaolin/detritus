Changelog

1.0.7 - 
  * library update to rebuild for macos
  * codesigned for macos

1.0.5 - 
  * new asteroid texture
  * new stars texture
  * new "cloud" particle explosions for asteroids and aliens
  * code now compiles and runs on OS X
  * fixed "random" errors on OS X and linux
  * Alt-F4, Cmd-Q, and Ctrl-Q all fast-quit out of the game for their appropriate OS
  * ship and enemy models/displaylists are created once at state initialization (gets rid of the weird one-time-per-runtime hiccough when the enemy first appears); 
  * made orientation "fixed" when you die instead of jumping around; 
  * ship no longer magically reappears during high score entry
  * high scores now save outside program directory for happier Vista, OS X, and linux experiences
  * installer file size reduced through streamlining of libraries


1.0.4 - as per website, la la.
1.0.3 - enemy bullets are now actually removed from collision detection after they "fade away" (they had just been removed at end-of-level--oops!); bug fixed in ship collision detection (had been only detecting "point" of ship, essentially).
1.0.2 - stopped runtimecompressing the executable as that caused it to fail on some machines
1.0.1 - lightened asteroid texture, added light, made lights better
1.0.0 - initial release

GAMESITES:
http://www.all4you.dk/FreewareWorld/Detritus-44494.html
http://www.versiontracker.com/dyn/moreinfo/win/155748

REVIEWSITES:
http://playthisthing.com/suggestion-detritus
http://jayisgames.com/536/gamesubmit.php ((need to submit))

DISCUSSIONS:
http://www.gamedev.net/community/forums/topic.asp?topic_id=490301
http://forums.indiegamer.com/showthread.php?t=13394
http://forums.indiegamer.com/showthread.php?t=13376
http://www.idevgames.com/forum/showthread.php?t=15031
http://www.gather.com/viewArticle.jsp?articleId=281474977320494
http://www.gather.com/viewVideo.jsp?id=11821949021863671

TO POST:
http://forums.tigsource.com/index.php?topic=155.0
http://mac.wareseeker.com/Games/detritus-d.zip/323837
http://mac.softpedia.com/get/Games/Detritus.shtml
http://mac.rbytes.net/cat/mac/games/etritus/
http://www.versiontracker.com/dyn/moreinfo/macosx/
http://freshmeat.net/
http://icewalk.com/


NOTES:
cross-compiling on the mac: http://www.idevgames.com/forum/showthread.php?t=14806
current rules master list: http://www.theinspiracy.com/Current%20Rules%20Master%20List.htm
SDL/OpenGL in Xcode: http://www.meandmark.com/sdlopenglpart1.html
Protonic - 2D sprite animation tool: http://forums.indiegamer.com/showthread.php?t=13324

TODO:
DETRITUS -- OSX installer - http://developer.apple.com/tools/installerpolicy.html - http://qin.laya.com/tech_coding_help/dylib_linking.html  - http://rixstep.com/2/2/20070526,00.shtml <-- cleaning your OS X release
DETRITUS -- linux installer
DETRITUS -- soundtrack - http://www.devmaster.net/articles/openal-tutorials/lesson8.php
DETRITUS -- credits screen (include licenses/attributions/etc here; review licenses? ugh.)
DETRITUS -- better HUD
DETRITUS -- options screen - http://guichan.sourceforge.net/wiki/index.php/Downloads
DETRITUS -- option -- down: flip, brake, ?
DETRITUS -- option -- viewport [velocity ... orientation]
DETRTIUS -- option -- gamma [0 ... 1] ?
DETRITUS -- option -- SFX volume [0 ... 1] ?


CLEANING UP FOR OS X:

Remove the following files.

    * PkgInfo. This file wastes 4,088 bytes of disk space. Get rid of it. It's only needed by non-native (Carbon) applications. The exact same info is already found in Info.plist. Just get rid of it.
    * Anything resembling 'pbdevelopment.plist'. If you find this file it indicates you've not made a proper final build anyway.
    * NIB files such as classes.nib, data.dependency, and info.nib. You must retain objects.nib (or keyedobjects.nib) as this is the 'freeze dried' code your application will use. All other files - aside from graphics files - in a NIB are to be removed. And if you have graphics files inside a NIB - move them up to Resources. The program loader [NSBundle] will have an easier time finding them and such an arrangement is a cleaner and tidier way to go. No file save for objects.nib (or keyedobjects.nib) ever need be in your NIB. Period.
    * Never use keyedobjects.nib if you can get away with objects.nib. The former wastes a lot of disk space.
    * Do you have embedded frameworks? Why? OK - sidestepping that for a second: do not include header files or the 'Headers' directory in your final build. If they're there then remove them.
    * Are those embedded frameworks you shouldn't have anyway coming with localisations your application doesn't support? Then remove them. Make a habit of looking inside the code you're borrowing.
    * Embedding frameworks is a mess anyway: you have to set them inside your projects and in no time you've got redundancy all over your end users' disks. Find a suitable place for these frameworks such as /Library/Frameworks and then just copy the appropriate files into your source for the DMG or zip archive you create. [See below.]
    * .DS_Store. If you're a developer and using Finder for file management then you're not a real developer. Get a better tool. Make sure you have no .DS_Store nonsense inside your application packages - after all, you might get roasted here and can that be any good?

STORING STUFF IN USERDIR:
http://www.gamedev.net/community/forums/mod/journal/journal.asp?jn=267694&reply_id=2660703 <-- WINDOWS
Its in shlobj.h. Most of the msdn posts list the header,library and OS necessary for the method to work. And often they have a handy code sample showing you how to use it.

late? Vista "Game Explorer" -- http://msdn.microsoft.com/en-us/library/ms687240.aspx

OWES:
Sound effects made with, variously, a microphone, sfxr, and audacity
2D graphics made with photoshop
3D graphics made with DAZ|Hexagon and adjusted with DAZ|Carrara
.3ds format imported thanks to Matthew Fairfax ("just let me know you used it")
Graphics rendered with OpenGL
Sound rendered with OpenAL
Game developed with SDL libraries (LGPL) - SDL_TTF, SDL_Image
Font: MaxFont by Max Stockinger + Kaolin Fire
Font routines provided by Bob Pendleton (LGPL)
Source code edited in Dev-C++ (wxDev-C++) and vim
Windows installer created with NSIS
High scores saved with SQLite (PD)

This game is released under Creative Commons Attribution-Noncommercial
<a rel="license" href="http://creativecommons.org/licenses/by-nc/3.0/us/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc/3.0/us/88x31.png"/></a><br/><span xmlns:dc="http://purl.org/dc/elements/1.1/" href="http://purl.org/dc/dcmitype/InteractiveResource" property="dc:title" rel="dc:type">Detritus</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="http://www.erif.org/code/detritus/" property="cc:attributionName" rel="cc:attributionURL">Kaolin Imago Fire</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/3.0/us/">Creative Commons Attribution-Noncommercial 3.0 United States License</a>.

[maybe later] DETRITUS -- glDrawElements/glDrawArrays for faster rendering? http://www.gamedev.net/community/forums/topic.asp?topic_id=474587&PageSize=25&WhichPage=2
[maybe later] DETRITUS -- determine proper z-order of ship, smoke, shield, and asteroids :/
[maybe later] DETRITUS -- accuracy bonus?
[maybe later] DETRITUS -- asteroids bounce?
[maybe later] DETRITUS -- difficulty levels (old speed/accel/decel/asteroid speeds: beginner; new speed/accel/decel/asteroid speeds: normal)
[maybe later] DETRITUS -- (start in a window, make fullscreen an option?)
[maybe later] DETRITUS -- stars parallax?
[maybe later] DETRITUS -- os x available screen modes: http://www.carbondev.com/site/?page=CGDisplayAvailableModes
[maybe later] DETRITUS -- offer other render settings - 800x600, 1024x768, 1280x1024, 1600x1200 (will take effect next restart?)

[check] DETRITUS -- sqlite issue on os x, or is that just me?
[check] DETRITUS -- sqlite issue on vista, or is that just me?
[check] DETRITUS -- debug last os x issue (sqlite)
[check] DETRITUS -- OS X should save scores to user dir as found in insane process
[check] DETRITUS -- orientation should not change from time of ship-die
[check] DETRITUS -- ship and enemy should should pre-load displaylists
[check] DETRITUS -- save high scores (sqlite???)
[check] DETRITUS -- load high scores (sqlite???)
[check] DETRITUS -- game state -- high score input screen
[check] DETRITUS -- different colors of asteroids mean ... what? ...  make asteroids drop goodies -- goodies can be shot, too??? shield goodies are asteroids of a certain metal -- make them pulse white ... maybe pulsing light blue are bullet upgrades (shoot explosively for a ... time duration ((each bullet then creates more secondary, regular bullets)))
[check] DETRITUS -- do powerups, make them shield + weapon
[check] DETRITUS -- warping to sector sound (whoosh? 5 seconds!)
[check] DETRITUS -- radar should be relative to ORIENTATION not VELOCITY
[check] DETRITUS -- game state -- intro (pre-attract loop) ((ALREADY DONE))
[check] DETRITUS -- game state -- 'next level' transition ((ALREADY DONE, BUT EVEN BETTER NOW))
[check] DETRITUS -- game state -- exit SPLASH ((ALREADY DONE))
[check] DETRITUS -- autoshoot ((ALREADY DONE))
[check] DETRITUS -- GAME OVER should pause for a second to two seconds so player stops SHOOTING  ... [check] DETRITUS -- ship blows up should be massive explosion; start with one explosion, rough, centered on ship... and then have secondary sets of explosion offset at a few intervals? (well, check minus secondaries)
[check] DETRITUS -- "game over man" has radar problems (dots mostly don't display, flickr... presume it's the alpha setting of something?) and yet "sector cleared" works fine.  so... then what could it be?  debug, debug, debug!
[check] DETRITUS -- make bullets niftier (nifty enough?)
[check] DETRITUS -- make thrusters fade
[check] DETRITUS -- ship shields/lives -- do something with collision -- shields! different sized asteroids == different amounts of shield damage.  trying to collect shields over 100 percent == INVINCIBILITY MODE (can't shoot--take half damage, don't go under 100%)
[check] DETRITUS -- sounds should be relative to orientation, not a fixed observer position, duh!
[check] DETRITUS -- sound effects
[check] DETRITUS -- improve collision detection with ship
[check] DETRITUS -- enemy aliens????????
[check] DETRITUS -- aliens should appear exactly opposite ship (this hidden in radar)
[check] DETRITUS -- alien ship shots destroy asteroids
[check] DETRITUS -- windows installer
[check] DETRITUS -- game state -- attract loop (does not include PLAYER, just asteroids and the occasional enemy ship.  with asteroids of various sizes roaming around randomly) - CLOSE ENOUGH
[check] DETRITUS -- game state -- title needs a few things added?  or ... ? - CLOSE ENOUGH
[check] DETRITUS -- game state -- game over
[check] DETRITUS -- decide my own license -- creative commons attribution noncommercial

[NAH] DETRITUS -- storyline????????????????????
[NAH] DETRITUS -- make asteroids merge (they just turn into a glob too quickly; the fun in asteroids is all the little guys flying around)
[NAH] DETRITUS -- aliens can drop mines?  or mines can exist on level as extra lameness?  mines subclass from asteroids, can't be destroyed?  or "explode" regular asteroids based on proximity to ship?
[NAH] DETRITUS -- improve asteroid collision detection
[NAH] DETRITUS -- invincibility mode (round?)--fly through asteroids instead of shooting them
[NAH] DETRITUS -- synch to VBL to avoid visual tearing. (opengl contexts don't use vbl sync circa Dec 18, 2002)



http://www.idevgames.com/forum/archive/index.php/t-13190.html

void ChangeToSensibleDirectory(void)
{
#if defined(__APPLE__)
CFBundleRef mainBundle = CFBundleGetMainBundle();
CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
char path[PATH_MAX];
if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
{
fprintf(stderr, "Can't change to Resources direcory; something's seriously wrong\n");
exit(EXIT_FAILURE);
}
CFRelease(resourcesURL);
chdir(path);
#elif defined(linux)
pid_t my_pid = getpid();
char proc_pid_exe_path[PATH_MAX];
sprintf(proc_pid_exe_path, "/proc/%d/exe", (int)my_pid);

char executable_path[PATH_MAX];
if (readlink(proc_pid_exe_path, executable_path, PATH_MAX) == -1)
{
fprintf(stderr, "Couldn't find executable\n");
return EXIT_FAILURE;
}

*(strrchr(executable_path, '/')) = '\0';
chdir(executable_path);

/* assume we're in PREFIX/bin */
chdir("../share");
#elif defined(_WIN32)
/* we're already in a good place */
#else
#error Unknown OS
#endif
}
