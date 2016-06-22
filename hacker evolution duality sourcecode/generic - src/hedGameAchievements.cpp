/*
Name: hedGameAchievements.cpp
Desc: Game achievements management class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/hedGameAchievements.h"
#include "../generic - inc/hedGameEngine-ClassInterfaceManager.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include <stdio.h>

// Support for STEAM achievements
#if defined(BUILD_STEAM)
#include "steam_api.h"
#include "hedSteamAchievements.h"

enum EAchievements
{
	ACH_100_SERVERS_HACKED     = 0,
	ACH_100_FIREWALL_ATTACKS   = 1,
	ACH_100_DOS_ATTACKS        = 2,
	ACH_100_EMP_ATTACKS        = 3,
    ACH_100_VOICEPRINT_ATTACKS = 4,
    ACH_100_RETINA_ATTACKS     = 5,
    ACH_100_ENCRYPTION_KEYS    = 6,
	ACH_TUTORIAL_COMPLETED     = 7,
	ACH_GAME_COMPLETED         = 8,
};

// Achievement array which will hold data about the achievements and their state
Achievement_t g_Achievements[] = 
{
	_ACH_ID( ACH_100_SERVERS_HACKED,     "100 servers hacked" ),
	_ACH_ID( ACH_100_FIREWALL_ATTACKS,   "100 firewalls bypassed" ),
	_ACH_ID( ACH_100_DOS_ATTACKS,        "100 D.O.S attacks launched" ),
	_ACH_ID( ACH_100_EMP_ATTACKS,        "100 EMP attacks launched" ),
    _ACH_ID( ACH_100_VOICEPRINT_ATTACKS, "100 VOICEPRINT passwords cracked"),
    _ACH_ID( ACH_100_RETINA_ATTACKS,     "100 RETINA passwords cracked"),
    _ACH_ID( ACH_100_ENCRYPTION_KEYS,    "100 Encryption keys cracked"),
    _ACH_ID( ACH_TUTORIAL_COMPLETED,    "100 Encryption keys cracked"),
    _ACH_ID( ACH_GAME_COMPLETED,        "100 Encryption keys cracked"),
};

// Stats array which will hold data about the stats and their state
Stat_t g_Stats[] =
{
	_STAT_ID( 1, STAT_INT, "hedGamesPlayed")
};


// Global access to Achievements object
CSteamAchievements*	g_SteamAchievements = NULL;
CSteamLeaderboards* g_SteamLeaderboards = NULL;
CSteamStats*        g_SteamStats = NULL;
#endif

hedGameAchievements g_GameAchievements;

// ====================================================================================================================================================================================================
// Achievements defines
int buttonX[HED_ACHIEVEMENT_COUNT] = {20,20,20,20,20,20,20};
int buttonY[HED_ACHIEVEMENT_COUNT] = {40,80,120,160,200,240,280};
char* achievementDescription[HED_ACHIEVEMENT_COUNT] = {
"100 servers hacked.",
"100 FIREWALL attacks.",
"100 D.O.S attacks launched.",
"100 EMP attacks launched.",
"100 VOICEPRINT passwords cracked.",
"100 RETINA passwords cracked.",
"100 Encryption keys cracked.",
};

// ====================================================================================================================================================================================================
// Load game achievements
void hedGameAchievements::LoadAchievements()                                                 
{
	FILE* file;
	int   i;
	
	file = fopen(sysGetAppDataPath("achievements.dat"),"rt");
	
	// Create new achievements file, if it doesn't exist
	if(file == NULL)
	{
		for(i = 0; i < HED_ACHIEVEMENT_COUNT; i++)
        {
            achievementValue[i]  = 0;
			achievementStatus[i] = 0;
        }
        SaveAchievements();
		return;
	}

	// Load achievements
	for(i = 0; i < HED_ACHIEVEMENT_COUNT; i++)
    {
		fscanf(file,"%d\n",&achievementStatus[i]);
		fscanf(file,"%d\n",&achievementValue[i]);
    }
	fclose(file);

    // STEAM achievements
#if defined(BUILD_STEAM)
    if(SteamAPI_Init())
    //{
        //sysLogPrint("hedGameAchievements::LoadAchievements() - SteamAPI_Init() returned OK.");
        g_SteamAchievements = new CSteamAchievements(g_Achievements, 7);
		g_SteamLeaderboards = new CSteamLeaderboards();
		g_SteamLeaderboards->FindLeaderboard("Hacker Evolution Duality highscores");

		g_SteamStats = new CSteamStats(g_Stats, 1);
    //}
    //else
    //    sysLogPrint("hedGameAchievements::LoadAchievements() - SteamAPI_Init() failed.");
#endif
}

// ====================================================================================================================================================================================================
// Save game achievements
void hedGameAchievements::SaveAchievements()                           
{
	FILE* file;
	int   i;

	file = fopen(sysGetAppDataPath("achievements.dat"),"wt");

	for(i = 0; i < HED_ACHIEVEMENT_COUNT; i++)
    {
		fprintf(file,"%d\n",achievementStatus[i]);
		fprintf(file,"%d\n",achievementValue[i]);
    }
	fclose(file);
}

// ====================================================================================================================================================================================================
// Update and render achievement. Called every frame
void hedGameAchievements::Render(hedClassTextWindow* window, hedClassTextWindow* msgWindow)
{
	int i;

// STEAM achievements callback
#ifdef BUILD_STEAM
    SteamAPI_RunCallbacks();
#endif

	// Update achievement status images
	for(i = 0; i < HED_ACHIEVEMENT_COUNT; i++)
	{
        // Unlocked?
        //Robert if(achievementValue[i] >= 100 && achievementStatus[0] == 0)
        if(achievementValue[i] >= 100 && achievementStatus[i] == 0)
        {
    		//Robert achievementStatus[0] = 1;
	    	//Robert msgWindow->PrintTextN("[ACHIEVEMENT UNLOCKED] - %s",achievementDescription[0]);
    		achievementStatus[i] = 1;
	    	msgWindow->PrintTextN("[ACHIEVEMENT UNLOCKED] - %s",achievementDescription[i]);
		    SaveAchievements();

#ifdef BUILD_STEAM
            if (g_SteamAchievements)
            {
                sysLogPrint("hedGameAchievements::Render() - Unlocking achievement on STEAM. [%s]",g_Achievements[i].m_pchAchievementID);
                g_SteamAchievements->SetAchievement(g_Achievements[i].m_pchAchievementID);
                sysLogPrint("hedGameAchievements::Render() - Done!");
            }
#endif
        }

		window->attachedImageX[i] = buttonX[i];
		window->attachedImageY[i] = buttonY[i];
		window->attachedImageVisible[i] = 1;
		window->attachedImage[i] = g_InterfaceManager.skinWindowAchievementsStatus[achievementStatus[i]];

		if(achievementStatus[i] == 0)
			window->RenderText(buttonX[i] + 45, buttonY[i] + 8,0.5f,0.5f,0.5f,HED_FONT_NORMAL,"%s (%d to go). ",achievementDescription[i],100 - achievementValue[i]);
		else
			window->RenderText(buttonX[i] + 45, buttonY[i] + 8,0.67f,0.88f,0.01f,HED_FONT_NORMAL,achievementDescription[i]);
	}
}

// ====================================================================================================================================================================================================
// Upload leaderboard score to STEAM
void hedGameAchievements::STEAM_UploadLeaderboardScore(int score)
{
#ifdef BUILD_STEAM
	sysLogPrint("hedGameAchievements::STEAM_UploadLeaderboardScore(%d)",score);
	g_SteamLeaderboards->UploadScore(score);
	STEAM_RunCallbacks();

	// Store stats here
	g_SteamStats->m_pStats[0].m_iValue = 1;
	g_SteamStats->StoreStats();
#endif
}

// ====================================================================================================================================================================================================
void hedGameAchievements::STEAM_RunCallbacks()
{
#ifdef BUILD_STEAM
	SteamAPI_RunCallbacks();
#endif
}

// ====================================================================================================================================================================================================
// Unlock a STEAM achievement
void hedGameAchievements::STEAM_UnlockAchievement(int id)
{
#ifdef BUILD_STEAM
	g_SteamAchievements->SetAchievement(g_Achievements[id].m_pchAchievementID);
#endif
}

