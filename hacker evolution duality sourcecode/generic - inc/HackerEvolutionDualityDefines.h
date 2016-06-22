/*
Name: HackerEvolutionDualityDefines.h
Desc: General engine defines (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _HackerEvolutionDualityDefines_h_
#define _HackerEvolutionDualityDefines_h_

// Game and platform specific defines

#define GAME_HACKEREVOLUTIONDUALITY
//#define GAME_HACKERCHALLENGES

//#define DEMO

#define PLATFORM_WINDOWS                                // Build the game for Win32
//#define PLATFORM_MACOS                                  // Build the game for MacOS
//#define PLATFORM_LINUX                                  // Build the game for Linux
//#define PLATFORM_IOS                                    // Build the game for iOS (iPad only)

//#define BUILD_EACHILLINGO 

#define BUILD_STEAM
#define GAME_TARGET "STEAM-"

//#define TARGET_MACAPPSTORE

#ifdef DEMO
    #define GAME_VERSION  "exosyphen.4.hed.5.12-DEMO"
#else
    #define GAME_VERSION  "exosyphen.4.hed.5.12"
#endif

#ifdef PLATFORM_WINDOWS
#define GAME_PLATFORM GAME_TARGET##"Win32 ("## __DATE__ ## ")"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#endif

#ifdef PLATFORM_MACOS
#define GAME_PLATFORM "MacOS"
#define stricmp strcasecmp

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#endif

#ifdef PLATFORM_LINUX
#define GAME_PLATFORM "Linux"
#define stricmp strcasecmp

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"

#endif

#ifdef PLATFORM_IOS
#define GAME_PLATFORM "iOS"
#define stricmp strcasecmp
#endif

// SDL global stuff

extern SDL_Renderer *sdlRenderer;
extern SDL_Window   *sdlWindow;


#define HED_PI 3.14159265f

// Data types
typedef char hedString[1024];

// RGB color
class hedRGB
{
public:
    hedRGB()
    {
    	r = 1.0f; g = 1.0f; b = 1.0f;
    }
    hedRGB(float _r, float _g, float _b)
    {
        r = _r; g = _g; b = _b;
    }

    float r,g,b;
};

// When this is defined, the game soundtrack is killed, so that I can keep playing whatever I am playing
// loud on the wall mounted 5.1 sound system, and be able to annoy neighbours at least +- 3 floors. 
//#define MODE_PARTYDEV   

//=======================================================================================================================================
// Global game settings
extern int g_FullScreen;
extern int g_LowQualityGraphics;
extern int g_MaxTextureSize;

#ifdef GAME_HACKEREVOLUTIONDUALITY
extern int g_IsTabletPC;
extern int g_OnScreenKeyboard;              // Render/Use on-screen keyboard for tablets

extern int g_TabletPCPrecision;
extern int g_DebugLogOn;
extern int g_DetailedServerInfo;            // 1 if the display resolution is large enough to accomodate more detailed server info
#endif

//=======================================================================================================================================
// Global variables
extern long g_SoundVolume;
extern long g_SoundVolumeFX;

extern int  g_DisplayWidth;
extern int  g_DisplayHeight;

extern float g_FrameTime;
extern float g_LevelTime;

// Global AI settings
extern int g_AISetting_AIAttackTarget;
extern int g_AISetting_MoneyDest; 
extern int g_AISetting_MoneySpend;
extern int g_AISetting_Deathmatch;


//=======================================================================================================================================
// Gameplay constants
// Windowed mode sizes
#define HED_WINDOWED_MODE_COUNT 12
static int g_WindowedWidth[HED_WINDOWED_MODE_COUNT]  = {1024,1024,1280,1280,1280,1280,1360,1400,1440,1680,1920,1920};
static int g_WindowedHeight[HED_WINDOWED_MODE_COUNT] = { 600, 768, 720, 768, 800,1024, 768,1050, 900,1050,1080,1200};

// Game modes
#define GAMEMODE_IDLE            0     // Game modes

#define GAMEMODE_ATTACK_FIREWALL 1
#define GAMEMODE_ATTACK_DOS      2
#define GAMEMODE_ATTACK_EMP      3
//#define GAMEMODE_ATTACK_HACK     4
#define GAMEMODE_ATTACK_VPTOOL   5     // Attack mode: Voice print password tool
#define GAMEMODE_ATTACK_RETINA   6
#define GAMEMODE_ATTACK_KEYCRACK 7

#define GAMEMODE_HARDWARE        8
#define GAMEMODE_CONSOLE         9
#define GAMEMODE_OBJECTIVES     10
#define GAMEMODE_INTERFACE      11
#define GAMEMODE_MESSAGES       12

#define GAMEMODE_CONFIRM_FAILED  13     // Failed level confirmation
#define GAMEMODE_CONFIRM_MSG     14
#define GAMEMODE_CONFIRM_EXIT    15     // Message box to confirm exit-to-menu
#define GAMEMODE_CONFIRM_EXIT_2  16     // Message box to confirm exit-to-menu or advance to the next level

#define GAMEMODE_LEVEL_FAILED    17
#define GAMEMODE_LEVEL_COMPLETED 18

#define GAMEMODE_PAUSED          19

const static char* g_GameModeNames[] = 
{
"_IDLE",
"_ATTACK_FIREWALL",
"_ATTACK_DOS",
"_ATTACK_EMP",
"_ATTACK_HACK",
"_ATTACK_VPTOOL",
"_ATTACK_RETINA",
"_ATTACK_KEYCRACK",
"_HARDWARE",
"_CONSOLE",
"_OBJECTIVES",
"_INTERFACE",
"_MESSAGES",
"_SM_CONFIRM_FAILED",
"_SM_CONFIRM_MSG",
"_SM_EXIT",
"_SM_EXIT_2",
"_FAILED",
"_COMPLETED"
};


//=======================================================================================================================================
// Graphics constants
//#define TARGETMAP_WIDTH_1          3072.0f                       //2560.0f;
//#define TARGETMAP_ASPECT_RATIO     (TARGETMAP_WIDTH_1 / 1024.0f) // 2.5f

// Global stuff
#define DISPLAY_TOP_OFFSET         50  // Size of the top area

// Main menu 
#define GFX_BACKGROUND_WIDTH      1440 // Background image width and height
#define GFX_BACKGROUND_HEIGHT      900
#define GFX_BACKGROUND_EXT_SIZE     11 // Background image left/right extension image width 
#define GFX_BACKGROUND_FILL_DX       2 // Background fill image offset (horizontal)

#define GFX_MAINMENU_X             495 // Main menu list background x
#define GFX_MAINMENU_Y             300 // Main menu list background y
#define GFX_MAINMENU_OFFSET          5 // Main menu list background offset

#define GFX_MAINMENU_SLIDER_X     208  // Main menu slider (in the background)
#define GFX_MAINMENU_SLIDER_Y     150
#define GFX_MAINMENU_SLIDER_DIST  580
#define GFX_MAINMENU_SLIDER_SPEED 100

#define GFX_MAINMENU_WMAP_X       448  // Main menu sliding worldmap (in the background)
#define GFX_MAINMENU_WMAP_Y       193
#define GFX_MAINMENU_WMAP_DIST    896
#define GFX_MAINMENU_WMAP_SPEED   100

#define GFX_MAINMENU_SLOGO_X       10 - 6  // Main menu spinning logo (left) coordinates
#define GFX_MAINMENU_SLOGO_Y      200 - 6

// Main menu texts and values
#define MENU_MAIN_COUNT                       7 // Number of items in the main menu
#define MENU_MAIN_LIST_X                    560 // Main menu texts: x, y and vertical spacing
#define MENU_MAIN_LIST_Y                    350
#define MENU_MAIN_LIST_SPACE                 30
#define MENU_MAIN_CURSOR_X                  540 // Main menu cursor coordinates
#define MENU_MAIN_CURSOR_Y                  347

#ifdef BUILD_STEAM
const static char* g_MainMenuNames[] = 
{"Play: single player","Play: open world","Load profile","Options","Get more levels to play!","Hacker Evolution NEWS","Exit"};
#else
const static char* g_MainMenuNames[] = 
{"Play: single player","Play: open world","Load profile","Options","Online leaderboards","Credits","Exit"};
#endif
const static char* g_MainMenuDifficulty[] = {"Easy","Medium","Hard"};

//=======================================================================================================================================
// Gameplay constants
#define GAME_MAXLEVELS     100        // Maximum number of levels in a mod
#define GAME_MAXSERVERS    50         // Maximum number of servers in the game
#define GAME_MAXOBJECTIVES 20         // Maximum objectives per level
#define GAME_MAXMESSAGES   21         // Maximum messages per level
#define GAME_MAXFILES      10         // Maximum number of files on a server

#define HED_TRACETIME_UNIT         5  // Base trace time unit (seconds)
#define HED_TRACE_PER_SEC        1.0f // The amount by which the player trace level increases per second (when being traced)

#define SERVER_CHARGETIME_EMP (120.0f + 10.0f)  // seconds needed for the EMP tool to charge
#define SERVER_CHARGETIME_DOS (60.0f + 10.0f)   // seconds needed for the DOS tool to charge
#define SERVER_KEYREGEN_TIME  360     // 495, seconds until the server key is being regenerated

// ** AI constants
#define AI_ATTACK_COUNTDOWN                 180 //240 // Maximum time in seconds, the AI will wait until it attacks
#define AI_HOSTILE_DECREASE_FIREWALL       5.0f // Amount by which the hostility level is decreased after an attack
#define AI_HOSTILE_DECREASE_DOS            5.0f // Amount by which the hostility level is decreased after an attack
#define AI_HOSTILE_DECREASE_EMP            5.0f // Amount by which the hostility level is decreased after an attack
#define AI_ATTACK_DURATION                10.0f // Duration of the AI attack animation
#define AI_WAIT_SPENDMONEY                20.0f // Time between an AI gets the money and spends it
#define AI_WAIT_RESPAWN                   10.0f // Time until an AI respawns in deatmatch

#define AI_HOSTILE_INCREASE_FIREWALL      10.0f // AI hostility increase after a firewall attack
#define AI_HOSTILE_INCREASE_DOS           15.0f // AI hostility increase after DOS attack
#define AI_HOSTILE_INCREASE_EMP           20.0f // AI hostility increase after an EMP attack
#define AI_HOSTILE_INCREASE_TRACE          0.1f // AI hostility increase while being traced
#define AI_HOSTILE_INCREASE_BOUNCE         0.1f // AI hostility increase while being traced

#define AI_HOSTILE_INCREASE_MONEY          500  // AI hostility will increase/decrease 1% for every <AI_HOSTILE_INCREASE_MONEY> money taken from him

#define AI_ROGUE_CHANCE                 100000  // 1 <AI_ROGUE_CHANCE> in Chance that a non-hostile AI goes rogue and "develops" a sudden hostility
#define AI_ROGUE_VALUE                       5  // Rogue hostility value

#define AI_OPENWORLD_HOSTILITY              15  // Initial hostility range of the AI servers in the open world game

#define AI_ATTACK_FIREWALL                  1
#define AI_ATTACK_DOS                       2
#define AI_ATTACK_EMP                       3

// These constants are for adjusting the game based on the difficulty setting
#define pda_AI_ATTACK_COUNTDOWN            30  // AI_ATTACK_COUNTDOWN is shorter by 30 seconds for medium and 60 seconds for hard)
#define pda_AI_HOSTILE_INCREASE             5  // Extra hostility increase when attacked
#define pda_HED_TRACE_PER_SEC              0.5 // HED_TRACE_PER_SEC = pda_HED_TRACE_PER_SEC + (g_PlayerDifficulty * HED_TRACE_PER_SEC) 

// AI taunt messages
#define AI_TAUNT_MSG_COUNT 3
const static char* g_AITauntMessage_EMP[]       = 
{"EMP blast away...","Gotta love satellites!", "Let's make some DAMAGE."};
const static char* g_AITauntMessage_DOS[]       = 
{"Floodgates are open!","I love denial of service attacks.","Packets dispatched."};
const static char* g_AITauntMessage_FIREWALL[]  = 
{"Let's put some fire on that firewall!","I want your cash.","Is your firewall strong enough?"};
const static char* g_AITauntMessage_RECV[]      = 
{"That hurt!","Revenge is coming!","You shouldn't have done that."};
const static char* g_AITauntMessage_MONEY[]     = 
{"Thanks for the cash!","Is it payday already?","Cash is king!"};
const static char* g_AITauntMessage_SPEND[]     = 
{"Spending money was always easy.","I just bought some new toys.","There goes my money."};

// AI variables
extern int AI_LastAttackedServer;              // Which was the last server attacked? (This is to avoid everyone attacking the same server)

// ** Score
#define SCORE_FIREWALL                    100
#define SCORE_DOS                         200
#define SCORE_EMP                         250
#define SCORE_VOICEPRINT                  300
#define SCORE_LEVELFAILED                5000

// ** Hardware constants
#define UPGRADE_PRICE_FIREWALL          5000
#define UPGRADE_PRICE_CPU               5000
#define UPGRADE_PRICE_INTEGRITY        10000
#define UPGRADE_TRACE_DECREASE            10

//=======================================================================================================================================
// Sound constants
#define SND_MAX_CHANNELS      128                // Maximum number of sound fx channels
#define SND_MAX_PLAYLIST_SIZE 128                // Maximum number of songs in a playlist

//=======================================================================================================================================
// UPDATE 1 stuff
//=======================================================================================================================================
// DNS entry
// Global DNS list. One per each mod.
// This is loaded when the MOD is SET.
#define HED_MAX_DNS_ENTRIES 4096

typedef struct
{
	char hostName[256];
	char hostIP[256];
}hedDNSEntry;

extern int          hedDNSCount;
extern hedDNSEntry  hedDNS[HED_MAX_DNS_ENTRIES];

#endif
