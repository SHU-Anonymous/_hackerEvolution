/*
Name: hedPlayerProfile.cpp
Desc: Player profile management

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#include "../generic - inc/hedPlayerProfile.h"
#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

hedPlayerProfileManager g_PlayerProfileManager;
float                   g_PlayerDifficulty = 0.0f;

//=======================================================================================================================================
// Init profiles
void hedPlayerProfileManager::InitProfiles()
{
    FILE* file;
    int   i;

    file = fopen(sysGetAppDataPath("profiles"),"rt");
    if(file != NULL)
    {
        fscanf(file,"%d\n",&m_ProfileCount);
        sysLogPrintDebug("hedPlayerProfileManager::InitProfiles() Loading %d profiles",m_ProfileCount); 
        for(i = 0; i < m_ProfileCount; i++)
        {
            fgets(m_ProfileList[i].name,1024,file);
            utilStripNewLine(m_ProfileList[i].name);
            sysLogPrintDebug("hedPlayerProfileManager::InitProfiles() -%s",m_ProfileList[i].name); 

            // Force loading of profile info
            SetProfile(i);
        }
        fclose(file);

        m_ProfileCurrent = 0;
    }
    else
        m_ProfileCount = 0;
}

//=======================================================================================================================================
// Set the current profile
void hedPlayerProfileManager::SetProfile(int index)
{
    FILE*     file;
    hedString filePath, line;

    sprintf(filePath,"%s.profile",m_ProfileList[index].name);
    file = fopen(sysGetAppDataPath(filePath),"rt");

    fgets(m_ProfileList[index].modName,1024,file); utilStripNewLine(m_ProfileList[index].modName);
    fgets(line,1024,file); m_ProfileList[index].modLevelNumber    = atoi(line);
    fgets(line,1024,file); m_ProfileList[index].modLevelNumberMax = atoi(line);
    fgets(line,1024,file); m_ProfileList[index].score             = atoi(line);
    fgets(line,1024,file); m_ProfileList[index].difficulty        = atoi(line);

    fclose(file);

    m_ProfileCurrent = index;
}

//=======================================================================================================================================
// Add a profile
void hedPlayerProfileManager::AddProfile(char* profileName, char* modName,int playerDifficulty)
{
    FILE* file;
    int   i;

    if(m_ProfileCount < HED_MAXPROFILES)
    {
        strcpy(m_ProfileList[m_ProfileCount].name, profileName);
        strcpy(m_ProfileList[m_ProfileCount].modName, modName);
        m_ProfileList[m_ProfileCount].modLevelNumber    = 0;
        m_ProfileList[m_ProfileCount].modLevelNumberMax = 0;
        m_ProfileList[m_ProfileCount].score             = 0;
        m_ProfileList[m_ProfileCount].difficulty        = playerDifficulty;

        m_ProfileCurrent = m_ProfileCount;
        SaveProfile(m_ProfileCurrent);

        m_ProfileCount++;

        // Save profile list
        file = fopen(sysGetAppDataPath("profiles"),"wt");
        fprintf(file,"%d\n",m_ProfileCount);
        for(i = 0; i < m_ProfileCount; i++)
            fprintf(file,"%s\n",m_ProfileList[i].name);
        fclose(file);
    }
    else
        sysLogPrint("hedPlayerProfileManager::AddProfile(%s) - Maximum number of profiles exceeded.",profileName); 
}

//=======================================================================================================================================
// Delete a profile
void hedPlayerProfileManager::DeleteProfile(int index)
{
    hedString filePath;
    int       i;
    FILE*     file;

    sysLogPrintDebug("hedPlayerProfileManager::DeleteProfile(%d) - Deleting (%s)", index, m_ProfileList[index].name);
    if(index < 0 || index > m_ProfileCount - 1) return;

    sprintf(filePath,"%s.profile",m_ProfileList[index].name);
    remove(sysGetAppDataPath(filePath));

    for(i = index; i < m_ProfileCount - 1; i++)
        m_ProfileList[i] = m_ProfileList[i + 1];
    m_ProfileCount--;

    // Save profile list
    file = fopen(sysGetAppDataPath("profiles"),"wt");
    fprintf(file,"%d\n",m_ProfileCount);
    for(i = 0; i < m_ProfileCount; i++)
        fprintf(file,"%s\n",m_ProfileList[i].name);
    fclose(file);

    InitProfiles();
}

//=======================================================================================================================================
// Save a profile
void hedPlayerProfileManager::SaveProfile(int index)
{
    FILE*     file;
    hedString filePath;

    if(m_ProfileList[index].modLevelNumber > m_ProfileList[index].modLevelNumberMax)
        m_ProfileList[index].modLevelNumberMax = m_ProfileList[index].modLevelNumber;

    sprintf(filePath,"%s.profile",m_ProfileList[index].name);
    file = fopen(sysGetAppDataPath(filePath),"wt");

    fprintf(file,"%s\n",m_ProfileList[index].modName);
    fprintf(file,"%d\n",m_ProfileList[index].modLevelNumber);
    fprintf(file,"%d\n",m_ProfileList[index].modLevelNumberMax);
    fprintf(file,"%d\n",m_ProfileList[index].score);
    fprintf(file,"%d\n",m_ProfileList[index].difficulty);

    fclose(file);
}

//=======================================================================================================================================
// Get profile count
int hedPlayerProfileManager::GetProfileCount()
{
    return m_ProfileCount;
}

//=======================================================================================================================================
// Get profile index
int hedPlayerProfileManager::GetProfileIndex(char* profileName)
{
    int i;

    for(i = 0; i < m_ProfileCount; i++)
        if(stricmp(m_ProfileList[i].name,profileName) == 0) return i;

    return -1;
}
