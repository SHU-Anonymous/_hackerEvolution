/*
Name: hedGameAchievements.h
Desc: Game achievements management class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#ifndef _hedGameAchievements_h_
#define _hedGameAchievements_h_

#include "hedGameEngine-ClassTextWindow.h"

// ====================================================================================================================================================================================================
class hedGameAchievements;
extern hedGameAchievements g_GameAchievements;

// ====================================================================================================================================================================================================
// Achievements defines
#define HED_ACHIEVEMENT_COUNT 7 // Number of achievements that can be unlocked

#define HED_ACHIEVEMENT_SERVERSHACKED 0
#define HED_ACHIEVEMENT_FIREWALL      1
#define HED_ACHIEVEMENT_DOS           2
#define HED_ACHIEVEMENT_EMP           3
#define HED_ACHIEVEMENT_VOICEPRINT    4
#define HED_ACHIEVEMENT_RETINA        5
#define HED_ACHIEVEMENT_KEYCRACK      6

extern int buttonX[HED_ACHIEVEMENT_COUNT];
extern int buttonY[HED_ACHIEVEMENT_COUNT];
extern char* achievementDescription[HED_ACHIEVEMENT_COUNT];

// ====================================================================================================================================================================================================
// Game achievements class
class hedGameAchievements
{
public:

	// Public functions
	void LoadAchievements();                                                 // Load game achievements
	void SaveAchievements();                                                 // Save game achievements
	void Render(hedClassTextWindow* window, hedClassTextWindow* msgWindow);  // Update and render achievement. Called every frame 

	void STEAM_UploadLeaderboardScore(int score);                            // Upload leaderboard score to STEAM
	void STEAM_RunCallbacks();
	void STEAM_UnlockAchievement(int id);                                    // Unlock a STEAM achievement

	// Public variables
    int achievementValue[HED_ACHIEVEMENT_COUNT];

protected:
	int achievementStatus[HED_ACHIEVEMENT_COUNT];                            // Achievements status : locked/unlocked
};

#endif