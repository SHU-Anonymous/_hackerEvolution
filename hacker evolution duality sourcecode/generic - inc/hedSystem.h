/*
Name: hedSystem.h
Desc: Operating system specific code

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _hedSystem_h_
#define _hedSystem_h_

#include "HackerEvolutionDualityDefines.h"

#define DEBUG_PRINT(name,value) sysLogPrintDebug("variable: %s = %d",name,value);

#if defined(PLATFORM_IOS) 

// TODO IPAD : keyboard ?
// Key codes
#define HEDKEY_ENTER       99
#define HEDKEY_BACKSPACE   98
#define HEDKEY_ESCAPE      27
#define HEDKEY_DEL         99
#define HEDKEY_TAB         99
#define HEDKEY_PAUSE       97

#endif

#if defined(PLATFORM_WINDOWS) 

// Key codes
#define HEDKEY_ENTER       -3
#define HEDKEY_BACKSPACE   -1
#define HEDKEY_ESCAPE      -9
#define HEDKEY_DEL         -6
#define HEDKEY_TAB         -2
#define HEDKEY_PAUSE      -22

#endif

#if defined(PLATFORM_MACOS)

// Key codes
#define HEDKEY_ENTER      13
#define HEDKEY_BACKSPACE 127
#define HEDKEY_ESCAPE     27
#define HEDKEY_DEL      -999 // This key doesn't exist on the Mac
#define HEDKEY_TAB         9
#define HEDKEY_PAUSE       8
#endif

#if defined(PLATFORM_LINUX)

// Key codes
#define HEDKEY_ENTER      13
#define HEDKEY_BACKSPACE   8
#define HEDKEY_ESCAPE     27
#define HEDKEY_DEL       127
#define HEDKEY_TAB         9
#define HEDKEY_PAUSE       -1

#endif

#define HEDKEY_LEFT       -4
#define HEDKEY_RIGHT      -5
#define HEDKEY_HOME       -7
#define HEDKEY_END        -8
#define HEDKEY_F1         -10
#define HEDKEY_F2         -11

#define HEDKEY_F8         -12
#define HEDKEY_F9         -13
#define HEDKEY_F10        -16
#define HEDKEY_F11        -14
#define HEDKEY_F12        -15

// UPDATE 1
#define HEDKEY_UP         -16
#define HEDKEY_DOWN       -17

void sysInit();                              // Misc. system initializations
void sysShutdown();                          // 

int  sysRandomNumber(int range);                        // Returns a random number in a given range
long sysTimeMilliseconds();                             // Returns the system time in milliseconds
void sysLogPrint(char *format,...);                     // Print text to a log file
void sysLogPrintDebug(char *format,...);                // Print text to a log file (extended debug)
void sysLogPrintAI(char *format,...);                   // Print text to the AI log
void sysActivityLogPrint(char *format,...);             // Print text to a log file
void sysOpenCommunityURL();                             // Opens the game community URL in the default browser
int  sysSubmitScore(char* nickname, char* level, int score, int speed, int attacks, int time); // Submit Open World score to online leaderboards

extern int       hedModCount;
extern hedString hedModList[1024];
extern hedString hedModListDesc[1024];
extern hedString hedModActive;
void             sysPopulateModList();                  // Create a list of all available game mods in the game folder

// System code for creating the path where application data is saved.
// Some OS-es doesn't allow us to save various game files in the game folder (such as Linux, Vista, etc).

extern char sysAppDataPath[1024];
extern char sysAppDataPathLast[1024];

char* sysGetAppDataPath(char* fileName);

//================================================================================================================================================
// Global stuff (used in HE and HEU)
int input_getLastKey();
long sysTimeSeconds();
int mouseLBClick();

extern int mouseX;
extern int mouseY;

extern int _mouseRBDown;
extern int _mouseLBDown;
extern int _mouseClick;


#endif
