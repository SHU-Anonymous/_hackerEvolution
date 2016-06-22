// HackerEvolutionDefines.h
// 
// Global game defines
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _HackerEvolutionDefines_h_
#define _HackerEvolutionDefines_h_

#define ENABLE_CHEATS
#define DEVELOPER


#define PLATFORM_WINDOWS
//#define PLATFORM_MACOS
//#define PLATFORM_LINUX

#ifdef PLATFORM_WINDOWS
#define ENGINE_VERSION "he.engine-3.19-2014.win [Build : "## __DATE__ ## "/" ## __TIME__ ##"]"
#endif

#ifdef PLATFORM_MACOS
#define ENGINE_VERSION "he.engine-3.19-2014.macos"
#endif

#ifdef PLATFORM_LINUX
#define ENGINE_VERSION "he.engine-3.19-2014.linux"
#endif

#ifdef PLATFORM_LINUX
#define nullptr NULL
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Language pack selection
//#define HE_LANG_RUSSIAN
//#define HE_LANG_POLISH
#include "language-packs/he_language_russian.h" 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Global data types
typedef char hestring[1024];

extern hestring   gSerialNumber;
extern int        gFullScreen;
extern int        gDinamicDifficulty;
extern int        gDemo;

extern int gFocusLost;
extern int g_screen_width;
extern int g_screen_height;

extern int quit;

// Global data types
// Files on the player's host
typedef struct
{
	char fileName[256];
	char filePath[1024];
	int  size;
}gameFile;

// * Maximum serial number length
#define HE_MAX_SERIALNUMBER_LENGTH 20

// Window text 
#define FONT_SIZE            14

// Console
#define MAX_COMMANDLINE_CHARS 65

// Misc
#define WINDOW_X_OFFS 5
#define WINDOW_Y_OFFS 25

#define TARGETMAPIMAGE_WIDTH  690
#define TARGETMAPIMAGE_HEIGHT 320

#define TOOLTIP_WIDTH  160
#define TOOLTIP_HEIGHT 100

// Server specific stuff
#define MAX_SERVER_FILES 100

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Player score and info constants
#define HE_SCORE_SPECIAL 500  // * Score points for special achievements

#define HE_SCORE_CRACK    10
#define HE_TRACE_CRACK    15
#define HE_SCORE_DECRYPT  10
#define HE_TRACE_DECRYPT  5

#define HE_TRACE_TRANSFER 3

#define HE_SCORE_TRANSFER       5
#define HE_SCORE_TRANSFER_RATIO 100

#define HE_SCORE_EXEC  5
#define HE_TRACE_EXEC  5

#define HE_TRACE_TRACED 50   // Global tracer increases by this amount, when player has been traced
#define HE_TRACE_LOGIN  1    // Global tracer increases by this amount, when player uses the "login" command

#define HE_KILLTRACE_COST 500 // The cost to reduce the global trace by 10%

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc gameplay constants
#define HE_INTRO_WAITTIME        120 // number of seconds to wait, after the intro is completed, to jump to the game menu

#define HE_GLOBALTRACE_BLINK     70  // Above this value, the global trace message will blink
#define HE_TOOLTIP_TIMEOUT       30  // Number of seconds while the tooltip window is displayed
#define HE_NICKNAME_MINLENGTH     2  // Minimum length of player nickname

#define HE_LEVELDONE_WAITTIME    15  // Number of seconds to wait, after a level is completed, to jump to the next one

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Sound effects
#define SFX_NEW_MESSAGE "he-soundfx/hesfx-newmessage.wav"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Main menu constants
//
#define MM_MENU_X     365
#define MM_MENU_Y     200      
#define MM_COLOR_1    171,128,15
#define MM_COLOR_2    0,0,0

// * Misc constants
#define WINDOW_HOFFS   5   // horizontal offset of inner window area
#define WINDOW_VOFFS  30   // vertical offset of inner window area

#define HE_WINDOW_IHOFFS  15   // horizontal offset of inner window area
#define HE_WINDOW_IVOFFS  35   // vertical offset of inner window area


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Player hardware
#define HE_MAX_CPU      4
#define HE_MAX_MODEM    4
#define HE_MAX_FIREWALL 4
#define HE_MAX_MEM      4

// * Hardware icons
static char* HE_ICON_NAME_CPU[]      = {"icon-cpu1.png","icon-cpu2.png","icon-cpu3.png","icon-cpu4.png"};
static char* HE_ICON_NAME_MODEM[]    = {"icon-modem1.png","icon-modem2.png","icon-modem3.png","icon-modem4.png"};
static char* HE_ICON_NAME_FIREWALL[] = {"icon-firewall1.png","icon-firewall2.png","icon-firewall3.png","icon-firewall4.png"};
static char* HE_ICON_NAME_MEM[]      = {"icon-mem1.png","icon-mem2.png","icon-mem3.png","icon-mem4.png"};

// * Hardware prices
static int HE_CPU_PRICE[HE_MAX_CPU]      = {2000,4000,8000,16000};
static int HE_MODEM_PRICE[HE_MAX_MODEM]    = {1000,2000,3000,4000};
static int HE_FIREWALL_PRICE[HE_MAX_FIREWALL] = {1000,2000,3000,5000};
static int HE_MEM_PRICE[HE_MAX_MEM]      = {2000,3000,4000,5000};


// * Command console constants
#define HE_MAX_COMMANDHISTORY 10
#define HE_MAX_DICTIONARY     100

// * Window constants
#define HE_WINDOW_SCROLLSIZE  5
#define HE_WINDOW_TEXTLINES   4096

// * Engine constants
#define HE_MAX_BOUNCEDLINK    256
#define HE_MAX_BOUNCECOUNT    3

// * Global variables
extern double gFrameTime;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Default speeds
#define HE_SPEED_CRACK    2900
#define HE_SPEED_DECRYPT  11900
#define HE_SPEED_TRACE    12000
#define HE_SPEED_DOWNLOAD 20
#define HE_SPEED_TRANSFER 100


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Interface constants
#define HE_TARGETMAP_WIDTH  690
#define HE_TARGETMAP_HEIGHT 320

#define HE_HOSTTOOLTIP_WIDTH 260

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Current mod info
#define HE_MAX_MOD_LEVELS 100 // maximum number of levels in a mod


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc
#define HE_MAX_PROFILES 6     // Max. number of save profiles

extern hestring gModPath;
extern hestring gModFolder;
extern hestring gModName;
extern hestring gModDescription;
extern hestring gModLevelList[HE_MAX_MOD_LEVELS];
extern int      gModLevelCount;
extern int      gModLevelCurrent;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Current skin info
typedef struct
{
	int r,g,b;
}gSkinColor;

#define SKINCOLOR(x) x.r,x.g,x.b

extern hestring   gSkinName;
extern gSkinColor gSkinColorHostHacked;
extern gSkinColor gSkinColorHost;
extern gSkinColor gSkinColorTargetMap;
extern gSkinColor gSkinColorProgress;
extern gSkinColor gSkinColorBouncedLink;
extern gSkinColor gSkinColorCommandPrompt;
extern gSkinColor gSkinColorWindowText;

void loadSkinColors();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc utility functions
void pump_messages();
void stripNewLine(char* pString);
void stripSpaces(char* pDest, char *pSrc);
int  wordCount(char* pSrc);
void createModPath(char* pPath, char* pFileName);
int  PointInRect(int pX, int pY, int pLeft, int pTop, int pWidth, int pHeight);
int  power_of(int pBase, int pPower);
int  _nearestPowOf2(int pNumber);

// * Mod utility functions
char* createModFilePath(char* pFileName);
char* createModLevelFilePath(char* pFileName);

// * Skin utility functions
char* createSkinPath(char* pFileName);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc variables
extern  int gDirectXLost;    // * This is set to one, when DirectX lost focus
extern long gSoundVolume;      // * Global volume level
extern long gSoundFXVolume;    // * Global volume level

int heCheckFile(char* fileName, int size);  // * Check if a file exists, and has the specified size

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * password encryption and decryption
void encryptPassword(char* password);
void decryptPassword(char* password);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * File and folder checksum calculation
unsigned long fileCheckSum(char* filename);
int           FolderCheckSum(char* csPath);
unsigned long fileSizeBytes(char* filename);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Checks if the game is activated (correctly)
int _he();

#endif
