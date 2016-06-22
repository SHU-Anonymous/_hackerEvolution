/*
Name: hedGameSettings.h
Desc: Settings file management

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#ifndef _hedGameSettings_h_
#define _hedGameSettings_h_

#include "HackerEvolutionDualityDefines.h"

//=======================================================================================================================================
// Globals
class hedGameSettings;
extern hedGameSettings g_GameSettings;

//=======================================================================================================================================
// Constants
#define HED_MAXSETTINGS 1024 // Maximum number of settings

//=======================================================================================================================================
// Datatypes
typedef struct 
{
    hedString name;
    hedString value;

    //---
    float _value[3];
    int   _parsed;

}hedSetting;

//=======================================================================================================================================
// Class definition
class hedGameSettings
{
public:
    hedGameSettings();

    void Load(char* fileName);
    void Save();

    int   ValueInt(char* setting);
    char* ValueStr(char* setting);
    char  ValueChr(char* setting, int index);
    void  SetValueInt(char* setting, int value);
    void  SetValueStr(char* setting, char* value);
    void  SetValueChr(char* setting, int index, char value);

protected:
    hedString  m_FileName;
    hedSetting m_Settings[HED_MAXSETTINGS];
    int        m_SettingsCount;
};

#endif