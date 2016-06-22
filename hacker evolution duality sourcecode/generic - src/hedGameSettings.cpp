/*
Name: hedGameSettings.h
Desc: Settings file management

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#include "../generic - inc/hedGameSettings.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedSystem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//=======================================================================================================================================
// Globals
hedGameSettings g_GameSettings;

//=======================================================================================================================================
// Class constructor
hedGameSettings::hedGameSettings()
{
    m_SettingsCount = 0;
}

//=======================================================================================================================================
// Load settings
void hedGameSettings::Load(char* fileName)
{
    FILE*     file;
    hedString line;
    int       i;

    sysLogPrint("hedGameSettings::Load(%s) - Loading settings",fileName);
    strcpy(m_FileName, fileName);

    // Open file
    file = fopen(fileName,"rt");
    if(file == NULL)
    {
        sysLogPrint("hedGameSettings::Load(%s) - ERROR opening setting file.",fileName);
        //return;
        
        // Create default settings file (usefull for sandboxed environments)
        file = fopen(fileName,"wt");
        
            fprintf(file,"video_fullscreen 1\n");
            fprintf(file,"video_windowed_width 1024\n");
            fprintf(file,"video_windowed_height 600\n");
            fprintf(file,"sound_volume_music 60\n");
            fprintf(file,"sound_volume_fx 50\n");
            fprintf(file,"input_softtouch 1\n");
            fprintf(file,"serial_number 0\n");
            fprintf(file,"game_autosubmit 0\n");
            fprintf(file,"game_ow_name hedPlayer\n");
            fprintf(file,"game_openworld_state nnnnnnnnn\n");
        
        fclose(file);
        
        file = fopen(fileName,"rt");
    }

    // Load settings
    while(fgets(line,1024,file))
    {
        //MacOS removed STRIP_NEWLINE(line);

        // Skip comments and empty lines
        if(line[0] != '/' && line[1] != '/' && line[0] != '\n' && line[0] != 0)
        {
            // Name
            sscanf(line,"%s",m_Settings[m_SettingsCount].name);

            // Value
            i = 0;
            while(line[i] != ' ') i++;
            strcpy(m_Settings[m_SettingsCount].value, (char *)&(line[i + 1]));
            utilStripNewLine(m_Settings[m_SettingsCount].value);

            m_Settings[m_SettingsCount]._parsed = 0;

            sysLogPrintDebug("hedGameSettings::Load(%s) - \"%s\"= \"%s\"",fileName,m_Settings[m_SettingsCount].name,m_Settings[m_SettingsCount].value);

            m_SettingsCount++;
        }
    }

    fclose(file);
}

//=======================================================================================================================================
// Save settings
void hedGameSettings::Save()
{
// Game settings are irrelevant under iOS   
    FILE* file;
    int   i;

    file = fopen(m_FileName,"wt");

    if(file == NULL)
    {
        printf("\nError opening file... (%s)",m_FileName);
        return;
    }
    
    for(i = 0; i < m_SettingsCount; i++)
        fprintf(file,"%s %s\n",m_Settings[i].name,m_Settings[i].value);

    fclose(file);
}

//=======================================================================================================================================
// Returns the (int) value of a setting
int hedGameSettings::ValueInt(char* setting)
{
    int i;

    for(i = 0; i < m_SettingsCount; i++)
        if(strcmp(m_Settings[i].name, setting) == 0)
        {
            // Parsed?
            if(m_Settings[i]._parsed == 0)
            {
                m_Settings[i]._value[0] = atof(m_Settings[i].value);
                m_Settings[i]._parsed   = 1;
            }
            //sysLogPrint("\"%s\"=\"%d\"(%s)",setting,(int)m_Settings[i]._value[0],m_Settings[i].name);
            return (int)m_Settings[i]._value[0];
        }
    return -1;
}

//=======================================================================================================================================
// Returns the (string) value of a setting
char* hedGameSettings::ValueStr(char* setting)
{
    int i;

    for(i = 0; i < m_SettingsCount; i++)
        if(strcmp(m_Settings[i].name, setting) == 0)
        {
            return m_Settings[i].value;
        }
    return 0;
}

//=======================================================================================================================================
// Returns a character index from a  setting
char hedGameSettings::ValueChr(char* setting, int index)
{
    int i;

    for(i = 0; i < m_SettingsCount; i++)
        if(strcmp(m_Settings[i].name, setting) == 0)
        {
            return m_Settings[i].value[index];
        }
    return 0;
}

//=======================================================================================================================================
// Sets the (int) value of a setting
void hedGameSettings::SetValueInt(char* setting, int value)
{
    int i;

    for(i = 0; i < m_SettingsCount; i++)
        if(strcmp(m_Settings[i].name, setting) == 0)
        {
            sprintf(m_Settings[i].value,"%d",value);
            m_Settings[i]._parsed = 1;
            m_Settings[i]._value[0] = value;
        }

    Save();
}

//=======================================================================================================================================
// Sets the (string) value of a setting
void hedGameSettings::SetValueStr(char* setting, char* value)
{
    int i;

    for(i = 0; i < m_SettingsCount; i++)
        if(strcmp(m_Settings[i].name, setting) == 0)
            strcpy(m_Settings[i].value,value);

    Save();
}

//=======================================================================================================================================
// Returns a character index from a  setting
void  hedGameSettings::SetValueChr(char* setting, int index, char value)
{
    int i;

    for(i = 0; i < m_SettingsCount; i++)
        if(strcmp(m_Settings[i].name, setting) == 0)
            m_Settings[i].value[index] = value;

    Save();
}
