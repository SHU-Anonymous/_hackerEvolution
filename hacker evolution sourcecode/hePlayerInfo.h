// hePlayerInfo.h
// 
// Player information management class
// This holds the variables that are tied to the player
//
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _hePlayerInfo_h_
#define _hePlayerInfo_h_

#include "HackerEvolutionDefines.h"

class hePlayerInfo
{
public:
	hePlayerInfo();
	~hePlayerInfo();

	void  setHostInfo(char* username, char* hostname);
	char* getCommandPrompt();
	int   getFileIndex(char* name);

	
	void resetPlayerInfo();                // * Resets all player info to default
	void saveGame(char* filename);         // * Saves player profile
	void loadGame(char* filename);         // * Load player profile
	int  validateSaveGame(char* filename); // * Checks a savegame to ensure it hasn't been hacked

	// * Retrieve hardware/software specific info
	int  getCPUMultiplier();
	int  getMemMultiplier();
	int  getModemMultiplier();
	int  getFirewallMultiplier();

    int  getDinamicPerformanceMultiplier()
    {
        int DinamicPerformanceMultiplier;
        
        DinamicPerformanceMultiplier = getCPUMultiplier() * getFirewallMultiplier();
        DinamicPerformanceMultiplier = _nearestPowOf2(DinamicPerformanceMultiplier);
        if(DinamicPerformanceMultiplier > 2)
            DinamicPerformanceMultiplier /= 2;

        // * cap
        if(DinamicPerformanceMultiplier > 5)
            DinamicPerformanceMultiplier = 5;

        return DinamicPerformanceMultiplier;
    }

	int  getMemSize();
	int  getMemSizeUsed();
	int  getMemSizeFree()
	{
		return getMemSize() - getMemSizeUsed();
	}

	// Files on player's computer
	gameFile fileList[1024];
	int      fileCount;

	// * Player hardware
	int cpu1, cpu2;
	int mem1, mem2;
	int modem;
	int firewall;
	int nnadapter;

	// * Player stats, score, etc
	int mScore;       // score
	int mGlobalTrace; // global trace level
	int mMoney;       // player money
	int mHackCount;   // number of succesfull hacks
	int mTraceCount;  // number of times, player was traced

	int mLevelCount;  // Number of levels completed
	int mMinuteCount; // Number of minutes played

	int mLostCount;

	hestring mNickname;  // * player nickname

protected:
	char m_username[256];
	char m_hostname[256];
	char m_commandprompt[512];
};

#endif
