/*
Name: hedPlayerProfile.h
Desc: Player profile management

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#ifndef _hedPlayerProfile_h_
#define _hedPlayerProfile_h_

#include "HackerEvolutionDualityDefines.h"

//=======================================================================================================================================
// Constants
#define HED_MAXPROFILES 6

extern float g_PlayerDifficulty;

//=======================================================================================================================================
// Data types
typedef struct
{
    hedString name;
    hedString modName;
    int       modLevelNumber;
    
    int       score;
    int       difficulty;

    //----
    int      modLevelNumberMax;

}hedPlayerProfile;

class  hedPlayerProfileManager;
extern hedPlayerProfileManager g_PlayerProfileManager;

//=======================================================================================================================================
// Class definition
class hedPlayerProfileManager
{
public:
    void InitProfiles();
    void SetProfile(int index);
    void AddProfile(char* profileName, char* modName, int playerDifficulty);
    void DeleteProfile(int index);
    void SaveProfile(int index);
    int  GetProfileCount();
    int  GetProfileIndex(char* profileName);

    char* getCurrentProfileName(){return m_ProfileList[m_ProfileCurrent].name;}
    char* getCurrentModName(){return m_ProfileList[m_ProfileCurrent].modName;}
    int   getCurrentLevel(){return m_ProfileList[m_ProfileCurrent].modLevelNumber;}

    hedPlayerProfile m_ProfileList[HED_MAXPROFILES];
    int              m_ProfileCount;
    int              m_ProfileCurrent;

};

#endif