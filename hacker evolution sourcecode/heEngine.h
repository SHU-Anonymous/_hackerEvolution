// heEngine.h
// 
// Game engine interface
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _heEngine_h_
#define _heEngine_h_

void loadGameOptions();

//===============================================================================================
// Initialize game engine
void initGameEngine();

//===============================================================================================
// Update game engine (run a game frame)
void updateGameEngine(long frame_time);

//===============================================================================================
// Close game engine
void closeGameEngine();

//===============================================================================================
// Execute command
void execCommand(char* command);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Initialize game windows
void init_NewGame(int pResetPlayerInfo = 1);

//==============================================================================================
// Game intro
int runIntro();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Set help tooltip
void setHelpToolTip(char* msg);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Set current game mod
void setGameMod(char* pModFilePath);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Save current game
void saveGame();

void displayEngineInfo(); // * display engine info and performance

int   bouncedLink_IsHost(int pHostIndex);   // * checks if a host is the bounced link
int   bouncedLink_GetTraceMultiplier();     // * Returns the tracetime multiplier for the current bounced link setup
void  bouncedLink_UpdateHosts();            // * Updates the hosts in the bounced links

#endif