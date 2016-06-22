/*
Name: HackerEvolutionDualityDefines.cpp
Desc: General engine defines (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#include "../generic - inc/HackerEvolutionDualityDefines.h"

//=======================================================================================================================================
// Global SDL variables
SDL_Renderer *sdlRenderer;
SDL_Window   *sdlWindow;

//=======================================================================================================================================
// Global game settings
int g_FullScreen         = 1;
int g_LowQualityGraphics = 0;
int g_IsTabletPC         = 0;   // Are we running on a tablet pc (no keyboard)
int g_OnScreenKeyboard   = 0;   // Render/Use on-screen keyboard for tablets
int g_TabletPCPrecision  = 20;  // Tablet PC tap precision. Allows easier matching of the in-game sliders
int g_DebugLogOn         = 1;   // Debug log on (extended debug info is being logged)
int g_DetailedServerInfo = 0;   // 1 if the display resolution is large enough to accomodate more detailed server info
int g_MaxTextureSize;           // Maximum texture size

//=======================================================================================================================================
// Global variables
long g_SoundVolume   = -1500;
long g_SoundVolumeFX = -1500;

int  g_DisplayWidth  = 0;
int  g_DisplayHeight = 0;

float g_FrameTime = 1.0f / 60.0f;

float g_LevelTime;

// Global AI settings
int g_AISetting_AIAttackTarget;
int g_AISetting_MoneyDest; 
int g_AISetting_MoneySpend;
int g_AISetting_Deathmatch;

// AI variables
int AI_LastAttackedServer = -1;              // Which was the last server attacked? (This is to avoid everyone attacking the same server)

//=======================================================================================================================================
// UPDATE 1 stuff
//=======================================================================================================================================
// Per MOD global DNS
int          hedDNSCount = 0;
hedDNSEntry  hedDNS[HED_MAX_DNS_ENTRIES];