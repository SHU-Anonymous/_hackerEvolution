/*
Name: hedGameLevel.cpp
Desc: Game level management class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#include "../generic - inc/hedPlayerProfile.h"
#include "../generic - inc/hedGameLevel.h"
#include "../generic - inc/hedSound.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// This is to verify if any servers overlap at small resolutions
float _serverX[GAME_MAXSERVERS], _serverY[GAME_MAXSERVERS], adjust;
int   _matrix[GAME_MAXSERVERS][GAME_MAXSERVERS];

// ===================================================================================================================================================================================================
// Class constructor
hedGameLevel::hedGameLevel()
{
	Init();
}

// ===================================================================================================================================================================================================
// Class destructor
hedGameLevel::~hedGameLevel()
{
}

// ===================================================================================================================================================================================================
// Initialize the class (cleans up all variables)
void hedGameLevel::Init()
{
	m_ServerCount = 0;
	m_ServerLocal = 0;

	m_ObjectiveCount = 0;
	m_MessageCount   = 0;
	m_LevelFailed    = 0;
	m_LevelTime      = 0.0f;

	m_RMessageCount  = 0;
	m_RMessageIndex  = -1;

	m_NewMsgReceived = 0;
	m_LevelCompleted = 0;
    m_Quit           = 0;
 }

// ===================================================================================================================================================================================================
// Sets the current game mod
void hedGameLevel::SetGameMod(char* modName, int openworld)
{
	int       i;
	FILE*     file;
	hedString line, filePath;

	strcpy(m_ModName, modName);
    m_OpenWorld = openworld;

	// Load level names
	sprintf(filePath,"hed-gamemods/%s.mod",m_ModName);
	sysLogPrintDebug("hedGameLevel::SetGameMod(%s) - opening mod file (%s)",m_ModName, filePath);
	file = fopen(filePath,"rt");
	if(file == NULL) return;

	fgets(line,1024,file);  utilStripNewLine(line); strcpy(m_ModDescription, line); 
	fgets(line,1024,file);  utilStripNewLine(line); m_LevelCount = atoi(line); 

	sysLogPrintDebug("hedGameLevel::SetGameMod(%s) - (%s, %d levels)",m_ModName,m_ModDescription,m_LevelCount);
	sysLogPrintDebug("hedGameLevel::SetGameMod(%s) - loading level names...",m_ModName);

	for(i = 0; i < m_LevelCount; i++)
	{
		fgets(line,1024,file); utilStripNewLine(line); strcpy(m_LevelList[i], line); 
		sysLogPrintDebug("hedGameLevel::SetGameMod(%s) - (%d,%s)",m_ModName,i, m_LevelList[i]);
		fgets(line,1024,file); utilStripNewLine(line); strcpy(m_LevelDescription[i], line); 
	}
	fclose(file);

	// UPDATE 1
	// Load mod DNS	
	sprintf(filePath,"hed-gamemods/%s/dns",m_ModName);
	file = fopen(filePath,"rt");
	
	sysLogPrintDebug("hedGameLevel::SetGameMod(%s) - Loading DNS entries (%s)",m_ModName,filePath);

	// Is there a global DNS for this mod?
	if(file)
	{
		hedDNSCount = 0;
		while(fgets(line,1024,file))
		{
			utilStripNewLine(line);
			sscanf(line,"%s %s",hedDNS[hedDNSCount].hostName,hedDNS[hedDNSCount].hostIP);
	
			sysLogPrintDebug("    - DNS entry (%3d/%s/%s)",hedDNSCount,hedDNS[hedDNSCount].hostName,hedDNS[hedDNSCount].hostIP);
			
			hedDNSCount++;
		}
		fclose(file);
	}
	else
	{
		sysLogPrintDebug("hedGameLevel::SetGameMod(%s) - No DNS entries for this mod...",m_ModName);
	}

	// Misc. variables
	m_LevelIndex = 0;
}

// ===================================================================================================================================================================================================
// Load a specified level
void hedGameLevel::LoadLevel(int levelIndex)
{
	int       i,j,x,y;
	FILE*     file, *file2;
	hedString line, filePath, filePath2,temp;
	float     mapCoordAdjustFactor;

	// Prerequisites 
	Init();
	m_LevelIndex         = levelIndex;

	// ** Load targetmaps
	int targetMapLoaded = 0;


	sprintf(filePath, "hed-gamemods/%s/%s/targetmap-0.png",m_ModName, m_LevelList[m_LevelIndex]);
	if(g_LowQualityGraphics == 0 && g_MaxTextureSize >= 4096)
	{
		targetMapLoaded = 1;
		sprintf(filePath2,"hed-gamemods/%s/%s/targetmap-1-1024.png",m_ModName, m_LevelList[m_LevelIndex]);
	}

	if(g_LowQualityGraphics == 1 || g_MaxTextureSize < 4096)
	{
		targetMapLoaded = 1;
		sprintf(filePath2,"hed-gamemods/%s/%s/targetmap-1-512.png",m_ModName, m_LevelList[m_LevelIndex]);
	}

	// We are running on some very crappy OpenGL card...
	if(targetMapLoaded == 0)
	{
		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - Unable to load targetmaps (verry crappy OpenGL card)");
		sprintf(filePath2,"hed-gamemods/%s/%s/targetmap-1-512.png",m_ModName, m_LevelList[m_LevelIndex]);
	}

	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading m_TargetMapImage[0]=(%s)",m_LevelList[m_LevelIndex], filePath);
	m_TargetMapImage[0].LoadFromFile(filePath);
	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading m_TargetMapImage[1]=(%s)",m_LevelList[m_LevelIndex], filePath2);
	m_TargetMapImage[1].LoadFromFile(filePath2);

    float targetmap_aspect_ratio;

    m_TargetMapImageHeight  = g_DisplayHeight - 100 - DISPLAY_TOP_OFFSET;	
    targetmap_aspect_ratio  = (float)m_TargetMapImage[1].m_pWidth / (float)m_TargetMapImage[1].m_pHeight;
    m_TargetMapImageWidth   = (float)m_TargetMapImageHeight * targetmap_aspect_ratio;
	
    //mapCoordAdjustFactor    = m_TargetMapImageWidth / TARGETMAP_WIDTH_1;
    //mapCoordAdjustFactor    = (float)m_TargetMapImageWidth / (float)m_TargetMapImage[1].m_pWidth;
    mapCoordAdjustFactor    = (float)m_TargetMapImageHeight / (float)m_TargetMapImage[1].m_pHeight;

	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - Targetmap adjusted to: (%d,%d)(%2.2f)",m_LevelList[m_LevelIndex],m_TargetMapImageWidth,m_TargetMapImageHeight,mapCoordAdjustFactor);

    // Small map loaded
    if(m_TargetMapImage[1].m_pHeight == 512)
    {        
        mapCoordAdjustFactor /= 2;
    	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - Targetmap re-adjusted to: (%d,%d)(%2.2f)",m_LevelList[m_LevelIndex],m_TargetMapImageWidth,m_TargetMapImageHeight,mapCoordAdjustFactor);
    }

	// ** Load server list
	sprintf(filePath,"hed-gamemods/%s/%s/serverlist",m_ModName,m_LevelList[m_LevelIndex]);
	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading server list (%s)",m_LevelList[m_LevelIndex], filePath);
	file = fopen(filePath,"rt");
	if(file == NULL) return;

	fscanf(file,"%d",&m_ServerCount);
	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - %d servers ",m_LevelList[m_LevelIndex], m_ServerCount);
	
	// ** Load server information 
    _levelchecksum = 0;

	for(i = 0; i < m_ServerCount; i++)
	{
		m_ServerList[i].Init();

		// Load server name
		fscanf(file,"%s",m_ServerList[i].m_HostName);
		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - (%s) ",m_LevelList[m_LevelIndex], m_ServerList[i].m_HostName);

		// Load server information
		sprintf(filePath,"hed-gamemods/%s/%s/%s/info",m_ModName,m_LevelList[m_LevelIndex],m_ServerList[i].m_HostName);
		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading server info (%s)",m_LevelList[m_LevelIndex], filePath);
		
		file2 = fopen(filePath,"rt");
		if(file2 == NULL) return;
	
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d %d\n",&x, &y); 

		_serverX[i] = (float)(x) * 0.439453125f; _serverY[i] = (float)(y) * 0.439453125f; 
		
        m_ServerList[i].SetCoordinates((int)((float)x * mapCoordAdjustFactor),(int)((float)y * mapCoordAdjustFactor));
        sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - server (%d,%d)",m_LevelList[m_LevelIndex], m_ServerList[i].m_MapX,m_ServerList[i].m_MapY);

        fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_Orientation);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_AIControlled);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_Visible);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_LevelIntegrity);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_LevelFirewall);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_LevelCPUPower);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_VoiceprintPassword);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_RetinaPassword);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_EncryptionKey);
		fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d\n",&m_ServerList[i].m_Money);

		// UPDATE 1
		// If the server doesn't have any security on it, leave it open
		if(m_ServerList[i].m_LevelFirewall == 0 && m_ServerList[i].m_RetinaPassword == 0 && m_ServerList[i].m_VoiceprintPassword == 0 && m_ServerList[i].m_EncryptionKey == 0)
		{
			m_ServerList[i].m_ServerKeyRegenerationTime = SERVER_KEYREGEN_TIME + 5;
			m_ServerList[m_ServerLocal].m_ServerKeys[i] = m_ServerList[i].m_ServerKey;
		}

		m_ServerList[i].m_CameraImagePresent = 0;
		for(j = 0; j < 3; j++)
		{
			strcpy(m_ServerList[i].m_SwitchName[j],"N/A");
			fgets(line,1024,file2); utilStripComment(line); sscanf(line,"%d %d",&m_ServerList[i].m_SwitchType[j],&m_ServerList[i].m_SwitchState[j]);
			fgets(m_ServerList[i].m_SwitchName[j],1024,file2); utilStripNewLine(m_ServerList[i].m_SwitchName[j]); 

			// Camera view switch?
			if(m_ServerList[i].m_SwitchType[j] == SWITCH_CAMERA)
				m_ServerList[i].m_CameraImagePresent = 1;
		}

		// Bypass password (if any)
		strcpy(m_ServerList[i].m_BypassVoicePrint,"-");
		if(fgets(line,1024,file2))
		{
			sscanf(line,"%s\n",m_ServerList[i].m_BypassVoicePrint);
			LOGVAR_S("m_BypassVoicePrint",m_ServerList[i].m_BypassVoicePrint);
		}

		strcpy(m_ServerList[i].m_BypassRetinaScan,"-");
		if(fgets(line,1024,file2))
		{
			sscanf(line,"%s\n",m_ServerList[i].m_BypassRetinaScan);
			LOGVAR_S("m_BypassRetinaScan",m_ServerList[i].m_BypassRetinaScan);
		}
		
		strcpy(m_ServerList[i].m_BypassEncryptionKey,"-");
		if(fgets(line,1024,file2))
		{
			sscanf(line,"%s\n",m_ServerList[i].m_BypassEncryptionKey);
			LOGVAR_S("m_BypassEncryptionKey",m_ServerList[i].m_BypassEncryptionKey);
		}
		fclose(file2);

		m_ServerList[i].m_long = ((360.0f * m_ServerList[i].getScreenX()) / m_TargetMapImageWidth) - 180.0f;
		m_ServerList[i].m_lat  = 90.0f - (m_ServerList[i].getScreenY() * 180.0f) / m_TargetMapImageWidth;
		strcpy(m_ServerList[i].m_HostNameAnimated,m_ServerList[i].m_HostName);

		// Dump server data for debug purposes
		sysLogPrintDebug("(x=%d),(y=%d),(AI=%d),(Firewall=%d),(CPU=%d)(VP=%d)(R=%d)(E=%d),(M=%d)",x,y,m_ServerList[i].m_AIControlled,m_ServerList[i].m_LevelFirewall,m_ServerList[i].m_LevelCPUPower,m_ServerList[i].m_VoiceprintPassword,m_ServerList[i].m_RetinaPassword,m_ServerList[i].m_EncryptionKey,m_ServerList[i].m_Money);
		for(j = 0; j < 3; j++)
			sysLogPrintDebug("SWITCH: %d %d %s",m_ServerList[i].m_SwitchType[j],m_ServerList[i].m_SwitchState[j],m_ServerList[i].m_SwitchName[j]);

		// Camera image present?
		if(m_ServerList[i].m_CameraImagePresent == 1)
		{
			sprintf(filePath,"hed-gamemods/%s/%s/%s/camera-view.png",m_ModName,m_LevelList[m_LevelIndex],m_ServerList[i].m_HostName);
			HED_SAFE_DELETE(m_ServerList[i].m_CameraImage);
			m_ServerList[i].m_CameraImage = new hedImage;
			m_ServerList[i].m_CameraImage->LoadFromFile(filePath);
			sysLogPrintDebug("CAMERA VIEW PRESENT: (%s)",filePath);
		}

		// Load server files
		sprintf(filePath,"hed-gamemods/%s/%s/%s/filelist",m_ModName,m_LevelList[m_LevelIndex],m_ServerList[i].m_HostName);
		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading server files (%s)",m_LevelList[m_LevelIndex], filePath);
		
		file2 = fopen(filePath,"rt");
		if(file2 == NULL)
		{
			// No files
			m_ServerList[i].m_FileCount = 0;
			sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - NO FILES ON THIS SERVER",m_LevelList[m_LevelIndex]);
		}
		else
		{
			// Load file names
			fscanf(file2,"%d\n",&m_ServerList[i].m_FileCount);
			sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - %d FILE(S)",m_LevelList[m_LevelIndex],m_ServerList[i].m_FileCount);

			for(j = 0; j < m_ServerList[i].m_FileCount; j++)
			{
				fscanf(file2,"%s\n",m_ServerList[i].m_FileName[j]);
				sprintf(m_ServerList[i].m_FilePath[j],"hed-gamemods/%s/%s/%s/files/%s",m_ModName,m_LevelList[m_LevelIndex],m_ServerList[i].m_HostName,m_ServerList[i].m_FileName[j]);
				sysLogPrintDebug("hedGameLevel::LoadLevel - %s(%s)",m_ServerList[i].m_FileName[j],m_ServerList[i].m_FilePath[j]);
			}
		}

        // Nickname
        strcpy(m_ServerList[i].m_AINickname,m_ServerList[i].m_HostName);
        j = 0;
        while(m_ServerList[i].m_AINickname[j] != '.' && m_ServerList[i].m_AINickname[j] != 0) j++;
        m_ServerList[i].m_AINickname[j] = 0;

        sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - server (%d,%d)",m_LevelList[m_LevelIndex], m_ServerList[i].m_MapX,m_ServerList[i].m_MapY);

        // Open world?
        m_ServerList[i].m_OpenWorld = m_OpenWorld;

        if(m_OpenWorld == 1)
        {
         	m_ServerList[i].m_AIHostile           = sysRandomNumber(AI_OPENWORLD_HOSTILITY - 1) + 1;
	        m_ServerList[i].m_AIHostileCountdown  = 50 + sysRandomNumber(AI_ATTACK_COUNTDOWN - 50); 

            sysLogPrintAI("[INI][%s][Hostile=%d,HostileCountDown=%d]",m_ServerList[i].m_HostName,(int)m_ServerList[i].m_AIHostile,(int)m_ServerList[i].m_AIHostileCountdown);
       
            // AI attack statistics
            m_ServerList[i].m_AIStatFirewall = 0;          
            m_ServerList[i].m_AIStatDOS      = 0;
            m_ServerList[i].m_AIStatEMP      = 0;
            m_ServerList[i].m_AIStatScore    = 0;
        }

        // Checksum against cheating
        _levelchecksum += m_ServerList[i].m_Money * 3 + m_ServerList[i].m_LevelIntegrity * 5 + m_ServerList[i].m_LevelCPUPower * 7 + m_ServerList[i].m_LevelFirewall * 11; 
	}
	fclose(file);
    sysLogPrintDebug("CHEAT %s=%d",m_LevelList[m_LevelIndex],_levelchecksum);

	// ** Set the localhost
	m_ServerList[0].m_IsLocalHost = 1;

    // Add more money to the localhost, based on difficulty
    //m_ServerList[0].m_Money += 20000 * g_PlayerDifficulty;

	// ** Load level objectives
	sprintf(filePath,"hed-gamemods/%s/%s/objectives",m_ModName,m_LevelList[m_LevelIndex]);
	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading objectives (%s)",m_LevelList[m_LevelIndex], filePath);
	file = fopen(filePath,"rt");
	if(file == NULL) return;

	fgets(line,1024,file); 
	sscanf(line,"%d",&m_ObjectiveCount);
	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - %d objectives",m_LevelList[m_LevelIndex], m_ObjectiveCount);
	
	for(i = 0; i < m_ObjectiveCount; i++)
	{
		m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;

		fgets(line,1024,file); sscanf(line,"%d %s %s",&m_ObjectiveList[i].type, m_ObjectiveList[i].hostName,m_ObjectiveList[i].itemName);
		fgets(m_ObjectiveList[i].description[0],1024,file); utilStripNewLine(m_ObjectiveList[i].description[0]);
		fgets(m_ObjectiveList[i].description[1],1024,file); utilStripNewLine(m_ObjectiveList[i].description[1]);

		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - (objective %d)(%d,%s,%s,%s,%s)",m_LevelList[m_LevelIndex], i, m_ObjectiveList[i].type, m_ObjectiveList[i].hostName,m_ObjectiveList[i].itemName, m_ObjectiveList[i].description[0],m_ObjectiveList[i].description[1]);

		if(m_ObjectiveList[i].type == OBJECTIVE_TIME || m_ObjectiveList[i].type == OBJECTIVE_TRACE)
		{
			// Time objective always starts out as completed
			m_ObjectiveList[i].value = atof(m_ObjectiveList[i].hostName);
			m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
		}		
	}
	fclose(file);

	// ** Load level messages
	sprintf(filePath,"hed-gamemods/%s/%s/messages",m_ModName,m_LevelList[m_LevelIndex]);
	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading messages (%s)",m_LevelList[m_LevelIndex], filePath);
	file = fopen(filePath,"rt");
	if(file == NULL)
	{
		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - NO MESSAGES IN THIS LEVEL",m_LevelList[m_LevelIndex]);
	}
	else
	{
		fgets(line,1024,file); 
		sscanf(line,"%d",&m_MessageCount);
		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - %d messages",m_LevelList[m_LevelIndex], m_MessageCount);
	
		for(i = 0; i < m_MessageCount; i++)
		{
			m_MessageList[i].status = MESSAGE_STATUS_NOTRECEIVED;

			fgets(line,1024,file); sscanf(line,"%d %s %s %s",&m_MessageList[i].type, m_MessageList[i].hostName,m_MessageList[i].itemName,m_MessageList[i].itemName2);
			fgets(m_MessageList[i].fileName,1024,file); utilStripNewLine(m_MessageList[i].fileName);

			// Format filename
			sprintf(line,"hed-gamemods/%s/%s/data_messages/%s",m_ModName,m_LevelList[m_LevelIndex],m_MessageList[i].fileName);
			strcpy(m_MessageList[i].fileName,line);

			// Load message title (first line)
			file2 = fopen(m_MessageList[i].fileName,"rt");
			fgets(m_MessageList[i].subject,1024,file2); utilStripNewLine(m_MessageList[i].subject);
			fclose(file2);

			// Timed message
			if(m_MessageList[i].type == MESSAGE_TIME)
				m_MessageList[i].value = atof(m_MessageList[i].hostName);

			sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - (message %d)(%d,%s,%s,%s,%s)",m_LevelList[m_LevelIndex],i,m_MessageList[i].type, m_MessageList[i].hostName,m_MessageList[i].itemName,m_MessageList[i].itemName2,m_MessageList[i].fileName);
		}
		fclose(file);
	}

	// Check if the display resolution is large enough to allow a more detailed server information display
	// Also check if any of the servers overlap at a small resolution (1024 x 600)
	int allow = 1;

	for(i = 0; i < m_ServerCount; i++)
		for(j = 0; j < m_ServerCount; j++)
			_matrix[i][j] = 0; _matrix[j][i] = 0;

	for(i = 0; i < m_ServerCount; i++)
		for(j = 0; j < m_ServerCount; j++)
		{
			if(utilPointInRect(m_ServerList[i].getScreenX(),m_ServerList[i].getScreenY(),
				               m_ServerList[j].getScreenX(),m_ServerList[j].getScreenY(),m_ServerList[j].getScreenX() + 310,m_ServerList[j].getScreenY() + 80) == 1)
							   allow = 0;
			
			if(abs(_serverX[i] - _serverX[j]) >= 300 || abs(_serverY[i] - _serverY[j]) >= 80 || i == j)
			{
				// Position OK				
			}
			else
			{
				if(_matrix[i][j] == 0)
				sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - [MAPCHECK ERROR] [%s] overlaps with [%s][%d/300][%d/95]",m_LevelList[m_LevelIndex],m_ServerList[i].m_HostName,m_ServerList[j].m_HostName,(int)abs(_serverX[i] - _serverX[j]),(int)abs(_serverY[i] - _serverY[j]));
				_matrix[i][j] = 1; _matrix[j][i] = 1;
			}
		}

	if(allow == 1)
		g_DetailedServerInfo = 1;
	//else
	//	g_DetailedServerInfo = 0;

	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - g_DetailedServerInfo=%d",m_LevelList[m_LevelIndex],g_DetailedServerInfo);

    // ** Load AI settings
	sprintf(filePath,"hed-gamemods/%s/%s/ai",m_ModName,m_LevelList[m_LevelIndex]);
	sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - loading AI settings",m_LevelList[m_LevelIndex]);
	
    int findTheFile;

    file = fopen(filePath,"rt");
	if(file == NULL)
	{
		sysLogPrintDebug("hedGameLevel::LoadLevel(%s) - NO AI SETTINGS defined",m_LevelList[m_LevelIndex]);
        
        // Default settings
        m_AISetting_AIAttackTarget  = -1;                         
        m_AISetting_MoneyDest       = -1;                          
        m_AISetting_MoneySpend      = 0;                         
        m_AISetting_Deathmatch      = 0;
	}
	else
	{
        fgets(line,1024,file); sscanf(line,"%d",&m_AISetting_AIAttackTarget); DEBUG_PRINT("m_AISetting_AIAttackTarget",m_AISetting_AIAttackTarget);                    
        fgets(line,1024,file); sscanf(line,"%d",&m_AISetting_MoneyDest);      DEBUG_PRINT("m_AISetting_MoneyDest",m_AISetting_MoneyDest);
        fgets(line,1024,file); sscanf(line,"%d",&m_AISetting_MoneySpend);     DEBUG_PRINT("m_AISetting_MoneySpend",m_AISetting_MoneySpend);
        fgets(line,1024,file); sscanf(line,"%d",&m_AISetting_Deathmatch);     DEBUG_PRINT("m_AISetting_Deathmatch",m_AISetting_Deathmatch);
        fgets(line,1024,file); sscanf(line,"%d",&findTheFile);                DEBUG_PRINT("findTheFile",findTheFile);
        
        fclose(file);
    }
    g_AISetting_AIAttackTarget  = m_AISetting_AIAttackTarget;                         
    g_AISetting_MoneyDest       = m_AISetting_MoneyDest;                          
    g_AISetting_MoneySpend      = m_AISetting_MoneySpend;                         
    g_AISetting_Deathmatch      = m_AISetting_Deathmatch; 

    // Find the file?
    if(findTheFile == 1)
    {
        int index = sysRandomNumber(m_ServerCount) + 1;
        m_ServerList[index].fileAdd("file.flag",this->GetFilePath("data_messages/file.flag"));
        sysLogPrintDebug("findTheFile(%d,%s)",index,this->GetFilePath("data_messages/file.flag"));
    }

    // Reset map fade animation
    for(i = 0; i < 10; i++)
    {
        mapFadeX[10]    = 0;
        mapFadeAlpha[i] = 0.0f;
    }
    _targetMapScrollDelta = 0;
}

// ===================================================================================================================================================================================================
// Level management class
void hedGameLevel::Update(hedClassTextWindow* windowObjectives, hedClassTextWindow* windowNotification, float frameTimeSec)
{
	int i,j;
    int _status, newObjectiveCompleted = -1;

	m_LevelTime += g_FrameTime;
    g_LevelTime  = m_LevelTime;

	// ===============================================================================================================================================================================================
	// ** Update level objectives
	for(i = 0; i < m_ObjectiveCount; i++)
	{
        _status = m_ObjectiveList[i].status;
		// Time objective
		if(m_ObjectiveList[i].type == OBJECTIVE_TIME && m_ObjectiveList[i].status == OBJECTIVE_STATUS_COMPLETED) 
		{
			m_ObjectiveList[i].value -= frameTimeSec;
			if(m_ObjectiveList[i].value <= 0.0f)
			{
				m_ObjectiveList[i].value  = 0.0f;
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_FAILED;
			}
		}

		// Hack server
		if(m_ObjectiveList[i].type == OBJECTIVE_HACK && m_ObjectiveList[i].status == OBJECTIVE_STATUS_UNCOMPLETED)
		{
			int index = GetServerIndex(m_ObjectiveList[i].hostName);
			if(m_ServerList[index].isFullyHacked(m_ServerList[0].m_ServerKeys[index]) == 1)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;

		}

		// Destroy structural integrity
		if(m_ObjectiveList[i].type == OBJECTIVE_DESTROY && m_ObjectiveList[i].status == OBJECTIVE_STATUS_UNCOMPLETED)
		{
			for(j = 0; j < m_ServerCount; j++)
				if(m_ServerList[j].IsNamed(m_ObjectiveList[i].hostName) && m_ServerList[j].m_LevelIntegrity == 0)
					m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
		}

		// Trace objective
		if(m_ObjectiveList[i].type == OBJECTIVE_TRACE) 
		{
			if(m_ServerList[0].m_TraceLevel <= m_ObjectiveList[i].value)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_FAILED;
		}

		// Obtain a certain amount of money
		if(m_ObjectiveList[i].type == OBJECTIVE_MONEY)      
		{
			if(m_ServerList[0].m_Money >= atoi(m_ObjectiveList[i].hostName))
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			// Allow the player to use the cash after it was obtained
            //else
			//	m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}
		
        // Obtain a certain amount of money
		if(m_ObjectiveList[i].type == OBJECTIVE_MONEY_3)      
		{
			if(m_ServerList[0].m_Money >= atoi(m_ObjectiveList[i].hostName))
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
            else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

		// Upload a file to a server
		if(m_ObjectiveList[i].type == OBJECTIVE_UPLOAD)
		{
			int index = GetServerIndex(m_ObjectiveList[i].hostName);

			if(m_ServerList[index].filePresent(m_ObjectiveList[i].itemName,0) == 1)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

		// Download a file
		if(m_ObjectiveList[i].type == OBJECTIVE_DOWNLOAD)
		{
			if(m_ServerList[0].filePresent(m_ObjectiveList[i].itemName,0) == 1)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

		// Turn a switch on
		if(m_ObjectiveList[i].type == OBJECTIVE_SWITCH_ON)
		{
			int index = GetServerIndex(m_ObjectiveList[i].hostName);
			int switchID = atoi(m_ObjectiveList[i].itemName);

			if(m_ServerList[index].m_SwitchState[switchID - 1] == 1)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

		// Turn a switch off
		if(m_ObjectiveList[i].type == OBJECTIVE_SWITCH_OFF)
		{
			int index = GetServerIndex(m_ObjectiveList[i].hostName);
			int switchID = atoi(m_ObjectiveList[i].itemName);

			if(m_ServerList[index].m_SwitchState[switchID - 1] == 0)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

		// Upload a certain amount of money to a server
		if(m_ObjectiveList[i].type == OBJECTIVE_MONEY_2)      
		{
			int index = GetServerIndex(m_ObjectiveList[i].itemName);
			if(m_ServerList[index].m_Money >= atoi(m_ObjectiveList[i].hostName))
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

		// UPDATE 1
		// Delete a file from a server
		if(m_ObjectiveList[i].type == OBJECTIVE_DELETE_FILE)
		{
			int index = GetServerIndex(m_ObjectiveList[i].hostName);

			if(m_ServerList[index].filePresent(m_ObjectiveList[i].itemName,0) == 0)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

        // ** Open world objectives
        // Eliminate all servers
		if(m_ObjectiveList[i].type == OBJECTIVE_OW_KILLALL && m_ObjectiveList[i].status == OBJECTIVE_STATUS_UNCOMPLETED)      
		{
            int j,ok = 1;
            
            for(j = 1; j < m_ServerCount; j++)
                if(m_ServerList[j].m_LevelIntegrity > 0 && m_ServerList[j].m_AIControlled == 1) ok = 0;

			if(ok == 1)
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_COMPLETED;
			else
				m_ObjectiveList[i].status = OBJECTIVE_STATUS_UNCOMPLETED;
		}

        // **
        if(_status != m_ObjectiveList[i].status && _status == OBJECTIVE_STATUS_UNCOMPLETED)
        {
            windowNotification->PrintTextN("[INFO] You have completed a new objective:");
            windowNotification->PrintTextN("[%s]",m_ObjectiveList[i].description[0]);
            g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");
        }

        if(_status != m_ObjectiveList[i].status && _status == OBJECTIVE_STATUS_COMPLETED)
        {
            windowNotification->PrintTextN("[INFO] You have failed an objective.");
            g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");
        }
	}

	// ===============================================================================================================================================================================================
	// ** Render objectives window
	for(i = 0; i < m_ObjectiveCount; i++)
	{
		// Objective status image
		if(m_ObjectiveList[i].status == OBJECTIVE_STATUS_UNCOMPLETED || m_ObjectiveList[i].status == OBJECTIVE_STATUS_FAILED)
			windowObjectives->attachedImage[i] = g_InterfaceManager.skinWindowObjectivesStatus[0];
		if(m_ObjectiveList[i].status == OBJECTIVE_STATUS_COMPLETED)
			windowObjectives->attachedImage[i] = g_InterfaceManager.skinWindowObjectivesStatus[1];

		windowObjectives->attachedImageVisible[i] = 1;
		windowObjectives->attachedImageX[i] = 20;
		windowObjectives->attachedImageY[i] = 45 + i * 40;
	}

	// ===============================================================================================================================================================================================
	// ** Update level messages
	int _newMsg = -1;
	int msgType;
	int msgShowHost    = -1;
	int msgAdjustTrace = -1;

	for(i = 0; i < m_MessageCount; i++)
	{
		// "hidden messages" that instead of showing a message, show a hidden server, based on the same condition
		msgType     = m_MessageList[i].type;
	
		msgShowHost    = -1;
		msgAdjustTrace = -1;

		// Adjust the tracelevel on the message condition, instead of showing a text message
		if(msgType > MESSAGE_TYPE_ADJUSTTRACE)
		{
			msgType       -= MESSAGE_TYPE_ADJUSTTRACE;
			msgAdjustTrace = 1;
		}

		// Reveals a server on the message condition, instead of showing a text message
		if(msgType > MESSAGE_TYPE_REVEAL_SERVER)
		{
			msgType     -= MESSAGE_TYPE_REVEAL_SERVER;
			msgShowHost = 1;
		}

		// Timed message
		if(msgType == MESSAGE_TIME && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			if(m_LevelTime > m_MessageList[i].value)
				_newMsg = i;
		}

		// Server hacked message
		if(msgType == MESSAGE_SERVER_HACKED && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			int index = GetServerIndex(m_MessageList[i].hostName);
			if(m_ServerList[index].isFullyHacked(m_ServerList[0].m_ServerKeys[index]))
				_newMsg = i;
		}

		// Message received when the player has a certain amount of money
		if(msgType == MESSAGE_MONEY && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			if(m_ServerList[0].m_Money >= atoi(m_MessageList[i].hostName))
				_newMsg = i;
		}

		// Message is displayed when the player has downloaded a certain file
		if(msgType == MESSAGE_DOWNLOAD && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			if(m_ServerList[0].filePresent(m_MessageList[i].hostName,0))
				_newMsg = i;
		}

		// Message is displayed when a certain switch is ON
		if(msgType == MESSAGE_SWITCH_ON && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			int index = GetServerIndex(m_MessageList[i].hostName);
            if(m_ServerList[index].m_SwitchState[atoi(m_MessageList[i].itemName) - 1] == 1)
				_newMsg = i;
		}

		// Message is displayed when a certain switch is OFF
		if(msgType == MESSAGE_SWITCH_OFF && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			int index = GetServerIndex(m_MessageList[i].hostName);
            if(m_ServerList[index].m_SwitchState[atoi(m_MessageList[i].itemName) - 1] == 0)
				_newMsg = i;
		}

		// Message is displayed when the player has uploaded a file to a server
		if(msgType == MESSAGE_UPLOAD && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
            int index = GetServerIndex(m_MessageList[i].hostName);
			if(m_ServerList[index].filePresent(m_MessageList[i].itemName,0))
				_newMsg = i;
		}

		// UPDATE 1
		// Message is displayed when a file has been deleted from a server
		if(msgType == MESSAGE_DELETE_FILE && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			int index = GetServerIndex(m_MessageList[i].hostName);

			if(m_ServerList[index].filePresent(m_MessageList[i].itemName,0) == 0)
				_newMsg = i;
		}

		// Message is displayed when a server has been destroyed
		if(msgType == MESSAGE_SERVER_DESTROYED && m_MessageList[i].status == MESSAGE_STATUS_NOTRECEIVED)
		{
			int index = GetServerIndex(m_MessageList[i].hostName);

			if(m_ServerList[index].m_LevelIntegrity == 0)
				_newMsg = i;
		}

		// Don't move this outside the for(;;) loop. We might have more than one message received in the same frame
		// Update new msg
		if(_newMsg != -1 && msgShowHost == -1 && msgAdjustTrace == -1)
		{
			m_MessageList[_newMsg].status   = MESSAGE_STATUS_RECEIVED;
			m_NewMsgReceived                = 1;
			m_RMessageList[m_RMessageCount] = _newMsg;
			m_RMessageIndex = m_RMessageCount;
			m_RMessageCount++;

			_newMsg = -1;
		}

		// 1. Show server
		if(_newMsg != -1 && msgShowHost == 1)
		{
			m_MessageList[_newMsg].status   = MESSAGE_STATUS_RECEIVED;

			// Show server
			int index = GetServerIndex(m_MessageList[_newMsg].itemName2);
			m_ServerList[index].m_Visible = 1;
			_newMsg = -1;
		}

		// 2. Adjust trace level
		if(_newMsg != -1 && msgAdjustTrace == 1)
		{
			m_MessageList[_newMsg].status   = MESSAGE_STATUS_RECEIVED;

			int traceAdjustValue = atoi(m_MessageList[_newMsg].itemName2);
	
			// Adjust trace level
			m_ServerList[0].m_TraceLevel += traceAdjustValue;
			
			_newMsg = -1;
			sysLogPrintDebug("hedGameLevel::Update() - Trace adjust trigger (%d)",traceAdjustValue);
		}

	}
	
}

// ===================================================================================================================================================================================================
// Level management class
void hedGameLevel::RenderTargetMap(float targetMapScrollDelta)
{
	int targetMapImageX, targetMapImageY;
    int i,j;


	targetMapImageX = (g_DisplayWidth  - m_TargetMapImageWidth)  / 2;
	targetMapImageY = DISPLAY_TOP_OFFSET;

    // Background layer
	for(i = 0; i < (g_DisplayWidth / 512) + 1; i++)
        for(j = 0; j < (g_DisplayHeight / 512) + 1; j++)
            m_TargetMapImage[0].Render(0 + i * 512,j * 512 + DISPLAY_TOP_OFFSET);
    
    // Map layer
    m_TargetMapImage[1].Render(targetMapImageX - targetMapScrollDelta,targetMapImageY,m_TargetMapImageWidth,m_TargetMapImageHeight);
}

// ===================================================================================================================================================================================================
// Obtains the path of a file from a level
char* hedGameLevel::GetFilePath(char* file)     
{
	sprintf(m_FilePath,"hed-gamemods/%s/%s/%s",m_ModName,m_LevelList[m_LevelIndex],file);
	return m_FilePath;
}

// ===================================================================================================================================================================================================
// Horizontal map scroll length
float hedGameLevel::GetMaxMapScrollDelta()
{
	return (m_TargetMapImageWidth - g_DisplayWidth) / 2;
}

// ===================================================================================================================================================================================================
// Returns the time left to finish the level (if the level has such a feature)
float hedGameLevel::GetTimedObjective()
{
	int i;

	for(i = 0; i < m_ObjectiveCount; i++)
		if(m_ObjectiveList[i].type == OBJECTIVE_TIME) 
			return m_ObjectiveList[i].value;

	return -1; // unlimited time
}

// ===================================================================================================================================================================================================
// Returns the index of a server
int hedGameLevel::GetServerIndex(char* hostName)                                    
{
	int i,index = -1;
	
	for(i = 0; i < m_ServerCount; i++)
		if(strcmp(m_ServerList[i].m_HostName,hostName) == 0) index = i;

	if(index == -1)
		sysLogPrint("hedGameLevel::GetServerIndex(%s) - ERROR. Server not found.",hostName);

	return index;
}

// ===================================================================================================================================================================================================
// Returns the global DNS index of a server
int hedGameLevel::GetServerDNSIndex(char* hostName)                                    
{
	int i,index = -1;
	
	for(i = 0; i < hedDNSCount; i++)
		if(strcmp(hostName,hedDNS[i].hostName) == 0) index = i;

	if(index == -1)
		sysLogPrint("hedGameLevel::GetServerDNSIndex(%s) - ERROR. Server not found.",hostName);

	return index;
}
// ===================================================================================================================================================================================================
// Returns true if all objectives are completed
int hedGameLevel::ObjectivesCompleted()
{
	int i,retvalue = 1;

	for(i = 0; i < m_ObjectiveCount; i++)
		if(m_ObjectiveList[i].status == OBJECTIVE_STATUS_UNCOMPLETED || m_ObjectiveList[i].status == OBJECTIVE_STATUS_FAILED)
			retvalue = 0;

	return retvalue;
}

// ===================================================================================================================================================================================================
// Returns true if any of the objectives is failed
int hedGameLevel::ObjectivesFailed()
{
	int i,retvalue = 0;

	// Check all level objectives
	for(i = 0; i < m_ObjectiveCount; i++)
		if(m_ObjectiveList[i].status == OBJECTIVE_STATUS_FAILED)
			retvalue = 1;

	return retvalue;
}

// ===================================================================================================================================================================================================
// Update final level statistics
void hedGameLevel::UpdateFinalStatistics()
{
    int i;

    m_Stat_FlawlessLevel = 1;                                    
    m_Stat_LevelTime     = m_LevelTime;

    // STAT - Discovered all servers?
    m_Stat_DiscoveredAllServers = 1;
    
    for(i = 0; i < m_ServerCount; i++)
        if(m_ServerList[i].m_Visible == 0) 
        {
            m_Stat_DiscoveredAllServers = 0;
            m_Stat_FlawlessLevel = 0;                                    
        }
    
    // STAT - Zero trace?
    if(m_ServerList[0].m_TraceLevel > 0.0f)
        m_Stat_FlawlessLevel = 0;
}

// ===================================================================================================================================================================================================
// Display OPEN WORLD statistics
void hedGameLevel::ShowOpenWorldStatics(hedClassTextWindow* window)
{
    int i,i2,x,y;
    int sa[GAME_MAXSERVERS];

    // Skip, in single player
    if (m_OpenWorld == 0) return;

    // Update player scores
    for(i = 0; i < m_ServerCount; i++)
    {
        sa[i] = i;

        m_ServerList[i].m_AIStatScore = m_ServerList[i].m_AIStatFirewall * SCORE_FIREWALL + m_ServerList[i].m_AIStatDOS * SCORE_DOS + m_ServerList[i].m_AIStatEMP * SCORE_EMP;
        // Calculate attack speed
        int speed = -1;

        if((m_ServerList[i].m_AIStatFirewall + m_ServerList[i].m_AIStatDOS + m_ServerList[i].m_AIStatEMP) > 0)
            speed = (int)g_LevelTime / (m_ServerList[i].m_AIStatFirewall + m_ServerList[i].m_AIStatDOS + m_ServerList[i].m_AIStatEMP);

        m_ServerList[i].m_AIStatSpeed   = speed;
        m_ServerList[i].m_AIStatAttacks = m_ServerList[i].m_AIStatFirewall + m_ServerList[i].m_AIStatDOS + m_ServerList[i].m_AIStatEMP;                
    }

    if (window->isVisible() == 0) return;

    x = 20;
    y = 135;

    // Sort scores
    int found = 0;
    int temp;
    do
    {
        found = 0;
        for(i = 0; i < m_ServerCount - 1; i++)
            if(m_ServerList[sa[i]].m_AIStatScore < m_ServerList[sa[i + 1]].m_AIStatScore)
            {
                found = 1;
                temp = sa[i]; sa[i] = sa[i + 1]; sa[i + 1] = temp;
            }
    }while(found == 1);

    // Display scores
    window->RenderText(x,y,0.67f,0.87f,0.003f, HED_FONT_BIG10,"Player");
    window->RenderText(x + 200,y,0.67f,0.87f,0.003f, HED_FONT_BIG10,"Firewall");
    window->RenderText(x + 275,y,0.67f,0.87f,0.003f, HED_FONT_BIG10,"DOS");
    window->RenderText(x + 350,y,0.67f,0.87f,0.003f, HED_FONT_BIG10,"EMP");
    window->RenderText(x + 425,y,0.67f,0.87f,0.003f, HED_FONT_BIG10,"Speed");
    window->RenderText(x + 500,y,0.67f,0.87f,0.003f, HED_FONT_BIG10,"Score");

    i2 = 0;
    for(i = 0; i < m_ServerCount; i++)
    {
        if(m_ServerList[sa[i]].m_AIControlled == 1 || sa[i] == 0)
        {
            float r = 0.7,g = 0.7,b = 0.7;

            if(sa[i] == 0) {r = 0.8; g = 0.0; b = 0.0;};

            window->RenderText(x,      y + (i2 + 2) * 15,r,g,b, HED_FONT_BIG10,m_ServerList[sa[i]].m_AINickname);
            window->RenderText(x + 200,y + (i2 + 2) * 15,r,g,b, HED_FONT_BIG10,"%d",m_ServerList[sa[i]].m_AIStatFirewall);
            window->RenderText(x + 275,y + (i2 + 2) * 15,r,g,b, HED_FONT_BIG10,"%d",m_ServerList[sa[i]].m_AIStatDOS);
            window->RenderText(x + 350,y + (i2 + 2) * 15,r,g,b, HED_FONT_BIG10,"%d",m_ServerList[sa[i]].m_AIStatEMP);
            window->RenderText(x + 425,y + (i2 + 2) * 15,r,g,b, HED_FONT_BIG10,"%d s",m_ServerList[sa[i]].m_AIStatSpeed);
            window->RenderText(x + 500,y + (i2 + 2) * 15,r,g,b, HED_FONT_BIG10,"%d",m_ServerList[sa[i]].m_AIStatScore);
            i2++;
        }
    }
}
