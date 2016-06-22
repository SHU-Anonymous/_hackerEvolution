/*
Name: hedGameEngine-ClassServer.cpp
Desc: Hacker Evolution Duality - Gameplay: server class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedGameEngine-ClassServer.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include "../generic - inc/hedPlayerProfile.h"
#include "../generic - inc/hedGameSettings.h"
#include "../generic - inc/hedSound.h"
#include "../generic - inc/hedSystem.h"

#ifdef PLATFORM_IOS
#import  <QuartzCore/QuartzCore.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define stricmp strcasecmp 

#endif

//=======================================================================================================================================
// Misc variables
int   __frameCounter = 0;
float __lockProgress = 0;

//=======================================================================================================================================
// Constructor
hedClassServer::hedClassServer()
{
	this->Init();
}

//=======================================================================================================================================
// Destructor
hedClassServer::~hedClassServer()
{
}

//=======================================================================================================================================
// Initialize
void hedClassServer::Init()                    
{
	int i;

	srand(sysTimeMilliseconds());

	// Init member variables
	m_MapX = 0;
	m_MapY = 0;
	
	m_ToolEMPLevel       = 0.0f;               
	m_ToolDOSLevel       = 0.0f;        
	m_LevelIntegrity     = 10;
	m_LevelFirewall      = 4;
	m_LevelCPUPower      = 2;
	m_VoiceprintPassword = 0;

	m_TraceLevel     = 0.0f;
	m_Money          = 0;
	m_IsLocalHost    = 0;

	//
	m_FXHighlightSize  = 0.0f;
	m_FrameTime        = 0.0f;
	m_Lock             = 0;
	m_LockJustHappened = 0;


	m_ClickTime[0] = -2000;
	m_ClickTime[1] = 0;
	m_DoubleClick  = 0;

	// Reset statistics
	statHackCount = 0;
	statEMPCount  = 0;
	statDOSCount  = 0;
    statScore     = 0;
    _statScore    = 0;

	// Reset effects
	m_MouseOverSound        = 0;
    m_HostNameAnimationTime = 0.0f;

	// Reset server keys
	for(i = 0; i < GAME_MAXSERVERS; i++) m_ServerKeys[i] = -1;
	m_ServerKey                 = (rand() % 99) * (rand() % 99); 
	m_ServerKeyRegenerationTime = -1.0f;

	// Voiceprint password
	for(i = 0; i < 11; i++)
		m_VoicePrintPassword_Pattern[i] = rand() % 60;
	m_VoicePrintPassword_Speed     = (rand() % 51) + 50; // speed range is 0..100, we generate in the 50..100 range
	m_VoicePrintPassword_Length    = (rand() % 21) + 10; // speed range is 0..30,  we generate in the 10..30 range	
    m_VoicePrintPassword_Amplitude = (rand() % 11) - 5;  // amplitude range is -5..+5,  we generate in the -5..+5 range
 
	// Init AI
	m_AIHostile           = 0.0f;
	m_AIHostileCountdown  = AI_ATTACK_COUNTDOWN - (g_PlayerDifficulty * pda_AI_ATTACK_COUNTDOWN);
	m_AIAttackProgress    = 0.0f;                           
	m_AIAttackDestination = 0;

    m_AIGotMoney       = 0;
    m_AIWaitSpendMoney = 0.0f;
    m_AIWaitRespawn    = AI_WAIT_RESPAWN;
}

//=======================================================================================================================================
// Set the coordinate deltas
void hedClassServer::SetCoordinateDeltas(int deltaX, int deltaY)
{
	m_DeltaX = deltaX;
	m_DeltaY = deltaY;
}

//=======================================================================================================================================
// Check server name
int hedClassServer::IsNamed(char* hostName)                                      
{
	if(strcmp(hostName,m_HostName) == 0)
		return 1;

	return 0;
}

//=======================================================================================================================================
// Clear the lock from this server
void hedClassServer::ClearLock()
{
	m_Lock = 0;

	// Reset lock variables
	m_FXHighlightSize  = 0.0f;
	__frameCounter = 0;
	__lockProgress = 0;
}

//=======================================================================================================================================
// Update server state
void hedClassServer::Update(float frameTime,hedClassTextWindow* window,int gameMode)                    
{
    // Respawn in deathmatch mode
    if(g_AISetting_Deathmatch == 1 && m_LevelIntegrity == 0)
    {
        DECREMENT_WITH_CAP(m_AIWaitRespawn,g_FrameTime,0);
        if(m_AIWaitRespawn == 0.0f)
        {
            m_LevelIntegrity = 5;
            m_LevelFirewall  = 2;
            m_LevelCPUPower  = 2;
            //m_Money          = 5000;
            m_ServerKey      = (rand() % 99) * (rand() % 99);

            m_AIGotMoney       = 0;
            m_AIWaitSpendMoney = 0.0f;
            m_AIWaitRespawn    = AI_WAIT_RESPAWN;

            m_Visible = 1;

            window->PrintTextN("%s RESPAWNED!", m_AINickname);
            g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-system-activated.wav");
        }
    }

	if(m_Visible == 0) return;

    if(_statScore < statScore) _statScore++; // Animated scrolling score effect

	m_FrameTime = frameTime;

	// Charge EMP and DOS tools
	float frameTimeSeconds = frameTime / 1000.0f;

	if(m_ToolEMPLevel < 75.0f && gameMode != GAMEMODE_ATTACK_EMP) m_ToolEMPLevel += (75.0f * frameTimeSeconds / (SERVER_CHARGETIME_EMP - 5 * m_LevelCPUPower)); 
	if(m_ToolEMPLevel > 75.0f)
	{
		CAPMAX(m_ToolEMPLevel,75.0f);
		if(m_IsLocalHost)
		{
			g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-ready.wav");
			window->PrintTextN("localhost:// EMP (ElectroMagnetic Pulse) ready.");
		}
	}

	if(m_ToolDOSLevel < 75.0f && gameMode != GAMEMODE_ATTACK_DOS) m_ToolDOSLevel += (75.0f * frameTimeSeconds / (SERVER_CHARGETIME_DOS - 5 * m_LevelCPUPower)); 
	if(m_ToolDOSLevel > 75.0f)
	{
		CAPMAX(m_ToolDOSLevel,75.0f);
		if(m_IsLocalHost)
		{
			g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-ready.wav");
			window->PrintTextN("localhost:// DOS (Denial of Service) attack ready.");
		}
	}

	// Lock animation
	if(m_Lock == 1)
	{
		if(__lockProgress < 100.0f)
			__lockProgress += (frameTimeSeconds * 100) / SERVER_LOCK_TIME;
		
		if(__lockProgress > 100.0f)
			g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick-2.wav");
		
		CAPMAX(__lockProgress, 100.0f);
	}

	// Geographical coordinates animation upon lock
	m_latAnimated  = (m_lat * __lockProgress) / 100.0f;
	m_longAnimated = (m_long * __lockProgress) / 100.0f;

	// Update server key regeneration
	if(m_ServerKeyRegenerationTime > 0 && m_ServerKeyRegenerationTime < SERVER_KEYREGEN_TIME + 1.0f /* don't decrement permanently open servers*/) 
		m_ServerKeyRegenerationTime -= frameTimeSeconds;

	// Regenerate key
	if(m_ServerKeyRegenerationTime < 0 && m_ServerKeyRegenerationTime != -1.0f)
	{
		m_ServerKeyRegenerationTime = -1.0f;
		m_ServerKey = (rand() % 99) * (rand() % 99);
	}

	// 

}

//=======================================================================================================================================
// Process input (input is adjusted to reflect the mouse position on the map)
int hedClassServer::ProcessInput(int inputX, int inputY, int click)
{
	if(m_Visible == 0 || m_LevelIntegrity == 0) return 0;

	int i;
	int mapx,mapy;

	mapx = m_MapX + m_DeltaX;
	mapy = m_MapY + m_DeltaY;

    if(m_Orientation == 0)
        mapx -= 260;
    else
        mapx += 0;


	// Detect doubleclick
	if(click == 1 && utilPointInRect(inputX, inputY, mapx - 25, mapy - 25, mapx + 285, mapy + 50))  // doubleclick over the entire server area
	{
 
        m_ClickTime[0] = m_ClickTime[1];
		m_ClickTime[1] = sysTimeMilliseconds();
		
		if((m_ClickTime[1] - m_ClickTime[0] < 800) && m_DoubleClick == 0)
		{
			m_DoubleClick  = 1;
			m_ClickTime[0] = -2000;
			m_ClickTime[1] = 0;
		}

        // Ignore double-click on the localhost
        // This was the INFAMOUS bug of not being able at times to lock back on the localhost
        if(m_DoubleClick == 1 & m_IsLocalHost == 1)
            m_DoubleClick = 0;

		// Ignore anything else if a double click has occured. This means that we are adding/removing this server to/from the bounce link
		if(m_DoubleClick == 1)
			return 0;
	}

    // Mouse over?
	if(utilPointInRect(inputX, inputY, mapx - 25, mapy - 25, mapx + 285, mapy + 50))  // mouse over the entire server area
	{
		// Mouse over sound
		if(m_MouseOverSound == 0)
		{
			m_MouseOverSound = 1;
			g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-serverover.wav");

    		// Hostname animation
            m_HostNameAnimationTime = 0.4f;
            strcpy(m_HostNameAnimated,m_HostName);
		}

		// server highlight rectangle
		m_FXHighlightSize += (m_FrameTime / 1000.0f) * 27.0f;
		CAPMAX(m_FXHighlightSize,8.0f);
	
		// Click? If yes, lock on to this server
		if(click == 1)
		{
			if(m_Lock == 0)
			{
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-accessing.wav");
				m_LockJustHappened = 1;

			    // This was moved here to solve the "locking out of the localhost" bug
                m_Lock = 1;
            }
			
            //m_Lock = 1;
		}

		__frameCounter++;
	}
	else
	{
		// server highlight rectangle
		m_FXHighlightSize -= (m_FrameTime / 1000.0f) * 27.0f;
		CAPMIN(m_FXHighlightSize,0.0f);
		__frameCounter = 0;

		// Reset all effects when mouse is no longer over the server
		m_MouseOverSound = 0;
	}

	// Lock just happened?
	if(m_LockJustHappened == 1)
	{
		m_LockJustHappened = 0;
		return 1;
	}

	return 0;
}

//=======================================================================================================================================
// Draw the server on the map
void hedClassServer::Draw(int playerKey)
{
    // We draw a "DEAD" host if there is an uncompleted AI attack in progress

    if(m_AIAttackProgress == 0.0f)
    {
	    if(m_Visible == 0) return;
        if(m_LevelIntegrity == 0) return;
    }

	float r,g,b;
	int   xpos,ypos,deltax_text;
	int   i,found;

	r = 0.85f;
	g = 0.85f;
	b = 0.85f;

	xpos = m_DeltaX + m_MapX;
	ypos = m_DeltaY + m_MapY;

    // Hostname animation 
    if(m_HostNameAnimationTime > 0.0f)
    {
        int       delta = 'z' - 'a';
        
        DECREMENT_WITH_CAP(m_HostNameAnimationTime,g_FrameTime,0);
        for(i = 0; i < strlen(m_HostNameAnimated); i++)
            if(m_HostNameAnimated[i] != '.') m_HostNameAnimated[i] = 'a' + sysRandomNumber(delta);
    }
    else
        strcpy(m_HostNameAnimated,m_HostName);

	// Draw server
	// If there is a lock on this server, render it's graphics
	if(m_Lock == 1)
	{
		// Lock lines
        // Vertical
		g_RenderEngine.DrawDottedLine(xpos,DISPLAY_TOP_OFFSET,xpos,100 + (ypos - 100) * (__lockProgress / 100.0f),0.4f,0.4f,0.4f);
		g_RenderEngine.DrawDottedLine(xpos,ypos + (g_DisplayHeight - 100 - ypos ) * (1.0f - __lockProgress / 100.0f),xpos,g_DisplayHeight - 100,0.4f,0.4f,0.4f);
		g_RenderEngine.DrawDottedLine(xpos - 1,DISPLAY_TOP_OFFSET,xpos - 1,100 + (ypos - 100) * (__lockProgress / 100.0f),0.4f,0.4f,0.4f);
		g_RenderEngine.DrawDottedLine(xpos - 1,ypos + (g_DisplayHeight - 100 - ypos ) * (1.0f - __lockProgress / 100.0f),xpos - 1,g_DisplayHeight - 100,0.4f,0.4f,0.4f);
		
        // Horizontal
        g_RenderEngine.DrawDottedLine(0,ypos,xpos * (__lockProgress / 100.0f),ypos,0.4f,0.4f,0.4f);
		g_RenderEngine.DrawDottedLine(xpos + g_DisplayWidth * (1.0f - __lockProgress / 100.0f),ypos,g_DisplayWidth,ypos,0.4f,0.4f,0.4f);
        g_RenderEngine.DrawDottedLine(0,ypos + 1,xpos * (__lockProgress / 100.0f),ypos + 1,0.4f,0.4f,0.4f);
		g_RenderEngine.DrawDottedLine(xpos + g_DisplayWidth * (1.0f - __lockProgress / 100.0f),ypos + 1,g_DisplayWidth,ypos + 1,0.4f,0.4f,0.4f);

		// Lock circle
		float scalefactor = (__lockProgress / 100.0f);

 		g_InterfaceManager.skinServerLock[0]->Render(xpos - 48, ypos - 48,96,96);
		g_InterfaceManager.skinServerLock[1]->Render(xpos - 64 * scalefactor, ypos - 64 * scalefactor,128 * scalefactor,128 * scalefactor);
		
		// 2014 update
		g_InterfaceManager.skinServerLock[0]->m_AnimAngleSpeed = 40;
		g_InterfaceManager.skinServerLock[1]->m_AnimAngleSpeed = -20;
		//g_InterfaceManager.skinServerLock[0]->m_AnimAngleSpeed = 20;
		//g_InterfaceManager.skinServerLock[1]->m_AnimAngleSpeed = -10;

		// Flashing inner circle
		if(__lockProgress < 100.0f && ((int)(__lockProgress) % 10 != 0))
			g_InterfaceManager.skinServerLock[2]->Render(xpos - 48, ypos - 48,96,96);
	}

	// ================================================================================================================================================================================================
	// ** Draw server information texts

	//if(isFullyHacked())
	if(m_ServerKeyRegenerationTime > 0.0f && playerKey == m_ServerKey)
	{
		r = 0.8f; g = 0.0f; b = 0.0f;
	}

    // Draw server info window
    if(m_Orientation == 0)
    {
	    xpos -= 260;
        deltax_text = -10;
        g_InterfaceManager.skinServerInfoWindow[0]->Render(xpos - 25, ypos - 25);
    }
    else
    {
        deltax_text = 20;
	    g_InterfaceManager.skinServerInfoWindow[1]->Render(xpos - 25, ypos - 25);
    }

    // Server name and information
    if(m_IsLocalHost == 1)
    {
        if(m_OpenWorld == 0)
	        g_RenderEngine.RenderText(xpos + deltax_text, ypos - 15, 0.98,0.64f,0.07f, HED_FONT_BIG10,"%s.%s",g_PlayerProfileManager.getCurrentProfileName(),m_HostNameAnimated);
        else
	        g_RenderEngine.RenderText(xpos + deltax_text, ypos - 15, 0.98,0.64f,0.07f, HED_FONT_BIG10,"%s.%s",g_GameSettings.ValueStr("game_ow_name"),m_HostNameAnimated);
    }
    else
	    g_RenderEngine.RenderText(xpos + deltax_text, ypos - 15, r,g,b, HED_FONT_BIG10,"%s",m_HostNameAnimated);
	g_RenderEngine.RenderText(xpos + deltax_text, ypos, 0.98,0.64f,0.07f, HED_FONT_NORMAL,"Firewall: %d CPU: %d Integrity: %d",m_LevelFirewall,m_LevelCPUPower,m_LevelIntegrity);

	// Mode 1 - General case. Server is NOT LOCKED and the mouse is NOT OVER the server
	if(m_Lock == 0 && m_FXHighlightSize == 0.0f)
	{
		// Display AI controlled server info
		if(m_AIControlled == 1)
		{
			if(m_AIHostile == 0)
				g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15, 0.65f, 0.65f, 0.65f, HED_FONT_NORMAL,"AI controlled server (NOT HOSTILE)");
			else
				g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15, 0.65f, 0.65f, 0.65f, HED_FONT_NORMAL,"Hostile: %3.1f%% | Attack in: %3.1fs",m_AIHostile,m_AIHostileCountdown - m_AIHostile);
			g_RenderEngine.RenderText(xpos + deltax_text, ypos + 30, 0.65f, 0.65f, 0.65f, HED_FONT_NORMAL,"EMP: %3d %% | DOS: %3d %%",(int)(m_ToolEMPLevel * 100.0f / 75.0f),(int)(m_ToolDOSLevel * 100.0f / 75.0f));
		}
        else
        {
	    	// Voiceprint password present?
            int dy = 0;
    		if(m_VoiceprintPassword == 1) 
            {
                g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.65f, 0.65f, 0.65f, HED_FONT_NORMAL,"Voiceprint authentication present"); dy += 15;
            }
            if(m_RetinaPassword == 1) 
            {
                g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.65f, 0.65f, 0.65f, HED_FONT_NORMAL,"Retinal scan authentication present"); dy += 15;
            }
            if(m_EncryptionKey == 1) 
            {
                g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.65f, 0.65f, 0.65f, HED_FONT_NORMAL,"Encryption key present"); dy += 15;
            }
        }
	}

	// Mode 2 - Server is NOT LOCKED and mouse is OVER the server
	if(m_Lock == 1 || m_FXHighlightSize > 0.0f)
	{
		// Display AI controlled server info
		if(m_AIControlled == 1)
		{
		    // Voiceprint password present?
		    if(m_VoiceprintPassword == 1 || m_RetinaPassword == 1 || m_EncryptionKey == 1) 
            {
                int dy = 0;
        		if(m_VoiceprintPassword == 1) 
                {
                    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.9f, 0.9, 0.9f, HED_FONT_NORMAL,"Voiceprint authentication present"); dy += 15;
                }
                if(m_RetinaPassword == 1) 
                {
                    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.9f, 0.9, 0.9f, HED_FONT_NORMAL,"Retinal scan authentication present"); dy += 15;
                }
                if(m_EncryptionKey == 1) 
                {
                    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.9f, 0.9, 0.9f, HED_FONT_NORMAL,"Encryption key present"); dy += 15;
                }
            }
            else
            {
                if(m_AIHostile == 0)
				    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15, 0.9f, 0.9, 0.9f, HED_FONT_NORMAL,"AI controlled server (NOT HOSTILE)");
			    else
    			g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15, 0.9f, 0.9f, 0.9f, HED_FONT_NORMAL,"Hostile: %3.1f%% | Attack in: %3.1fs",m_AIHostile,m_AIHostileCountdown - m_AIHostile);
			    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 30, 0.9f, 0.9f, 0.9f, HED_FONT_NORMAL,"EMP: %3d %% | DOS: %3d %%",(int)(m_ToolEMPLevel * 100.0f / 75.0f),(int)(m_ToolDOSLevel * 100.0f / 75.0f));			
			    
            }
		}
        else
        {
                int dy = 0;
        		if(m_VoiceprintPassword == 1) 
                {
                    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.9f, 0.9, 0.9f, HED_FONT_NORMAL,"Voiceprint authentication present"); dy += 15;
                }
                if(m_RetinaPassword == 1) 
                {
                    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.9f, 0.9, 0.9f, HED_FONT_NORMAL,"Retinal scan authentication present"); dy += 15;
                }
                if(m_EncryptionKey == 1) 
                {
                    g_RenderEngine.RenderText(xpos + deltax_text, ypos + 15 + dy, 0.9f, 0.9, 0.9f, HED_FONT_NORMAL,"Encryption key present"); dy += 15;
                }
        }
	}

	// ================================================================================================================================================================================================
	// Render other elements
	// Highlight
	if(m_FXHighlightSize > 0.0f)
    {
        if(m_Orientation == 0)
		    g_RenderEngine.DrawRectangleFill(260 + xpos - m_FXHighlightSize,ypos - m_FXHighlightSize,260 + xpos + m_FXHighlightSize,ypos + m_FXHighlightSize,0.85f,0.85f,0.85f);
        else
		    g_RenderEngine.DrawRectangleFill(xpos - m_FXHighlightSize,ypos - m_FXHighlightSize,xpos + m_FXHighlightSize,ypos + m_FXHighlightSize,0.85f,0.85f,0.85f);
    }
	// Server key regeneration time
	if(m_ServerKeyRegenerationTime != -1.0f && playerKey == m_ServerKey && (m_ServerKeyRegenerationTime < SERVER_KEYREGEN_TIME + 1) /*don't display for open servers */)
    {
        if(m_Orientation == 0)
		    g_RenderEngine.RenderText(xpos + 260 - 10, ypos + 15, 0.8f, 0.0f, 0.0f, HED_FONT_NORMAL,"%3.0f",m_ServerKeyRegenerationTime);
        else
		    g_RenderEngine.RenderText(xpos - 10, ypos + 15, 0.8f, 0.0f, 0.0f, HED_FONT_NORMAL,"%3.0f",m_ServerKeyRegenerationTime);
    }
}

//=======================================================================================================================================
// Returns true if this server is fully hacked
int hedClassServer::isFullyHacked(int serverKey)
{
	if(this->m_ServerKey != serverKey) return 0;
	
    if(m_VoiceprintPassword == 1) return 0;
    if(m_EncryptionKey == 1)      return 0;
    if(m_RetinaPassword == 1)     return 0;
	
    return 1;
}

//=======================================================================================================================================
// Check if a file is present on this server
int hedClassServer::filePresent(char* fileName, char* filePath)
{
	int present = 0;
	int i;

	// 2 types of search
	if(filePath == 0)
	{
		for(i = 0; i < m_FileCount; i++)
			if(stricmp(m_FileName[i],fileName) == 0)
				present = 1;
	}
	else
	{
		for(i = 0; i < m_FileCount; i++)
			if(stricmp(m_FileName[i],fileName) == 0 && strcmp(m_FilePath[i],filePath) == 0)
				present = 1;
	}
	return present;
}

//=======================================================================================================================================
// Check if a file is present on this server
int hedClassServer::fileIndex(char* fileName)
{
	int index = -1;
	int i;

	for(i = 0; i < m_FileCount; i++)
		if(stricmp(m_FileName[i],fileName) == 0)
			index = i;

	return index;
}

//=======================================================================================================================================
// Add a file to this server
void hedClassServer::fileAdd(char* fileName, char* filePath)
{
	if(m_FileCount >= GAME_MAXFILES) return;

	strcpy(m_FileName[m_FileCount],fileName);
	strcpy(m_FilePath[m_FileCount],filePath);

	m_FileCount++;
}

//=======================================================================================================================================
// Increase AI hostile level
void hedClassServer::AI_IncreaseHostileLevel(int index, float delta)                              
{
	// Not AI controlled, just return
	if(m_AIControlled == 0)
		return;

    m_AIHostile += delta;
	CAPMAX(m_AIHostile, 100.0f);
	CAPMIN(m_AIHostile,   0.0f);

    if(m_AIHostile == 0)
        m_AIHostileCountdown = AI_ATTACK_COUNTDOWN - (g_PlayerDifficulty * pda_AI_ATTACK_COUNTDOWN);
}

//=======================================================================================================================================
// This is called every frame to initiate attacks from this server to another server
int hedClassServer::AI_SendAttack(int selfIndex, hedClassServer* serverList, int count, hedClassTextWindow* chat, int AIPaused)
{
	int   i;
	float max = 0.0f;
	int   targetIndex  = -1;
    int   bounceCPU    = 0;
    int   AIAttackType = -1;

	// Not AI controlled, so no attacks are launched from here
	if(m_AIControlled == 0)
		return 0;

    // Integrity is zero ... no more attacks
    if(m_LevelIntegrity == 0)
    {
        m_AIAttackProgress = 0.0f; // Clear any attack that might have been in progress, when this server was destroyed
        return 0;
    }
	// Update AI
	if(m_AIHostile != 0.0f && AIPaused == 0)
        DECREMENT_WITH_CAP(m_AIHostileCountdown,g_FrameTime,m_AIHostile);
    DECREMENT_WITH_CAP(m_AIWaitSpendMoney,g_FrameTime,0.0f);

	// Update AI attack progress
	if(m_AIAttackProgress > 0.0f)
	{
		m_AIAttackProgress += (g_FrameTime * 100.0f / AI_ATTACK_DURATION);
		CAPMAX(m_AIAttackProgress, 100.0f);

		// Animation completed
		if(m_AIAttackProgress == 100.0f) m_AIAttackProgress = 0.0f; 
	}

    // (OPEN WORLD) Should we spend cash?
    if(m_OpenWorld == 1 && m_AIGotMoney && g_AISetting_MoneySpend == 1 && m_AIWaitSpendMoney == 0.0f)
    {
        // We always buy integrity
        if(m_Money >= 10000 && m_LevelIntegrity < 5)
        {
            m_Money -= 10000;
            m_LevelIntegrity++;
            if(g_AISetting_MoneyDest != -1)                                serverList[g_AISetting_MoneyDest].m_Money += 10000;            // Move the money to another server
            if(g_AISetting_MoneyDest != -1)                                serverList[g_AISetting_MoneyDest].m_LevelFirewall += 1;
            if(g_AISetting_Deathmatch == 1 && g_AISetting_MoneyDest != -1) serverList[g_AISetting_MoneyDest].m_LevelIntegrity++;
            
            // AI taunt chat
            chat->Anim_DisplayText("\n%s> %s",m_AINickname,g_AITauntMessage_SPEND[rand() % AI_TAUNT_MSG_COUNT]);
       
            // AI Log
            sysLogPrintAI("[$$$][%d,%s (I:%d,F:%d,M:%d)] upgraded integrity",selfIndex,m_HostName,m_LevelIntegrity, m_LevelFirewall, m_Money);
        }

        // If we have the cash and our integrity is not critical, let's upgrade the firewall
        if(m_Money >= 5000 && m_LevelIntegrity > 2 && m_LevelFirewall < 10 && m_LevelFirewall <= m_LevelCPUPower)
        {
            m_Money -= 5000;
            m_LevelFirewall++;
            if(g_AISetting_MoneyDest != -1)                                serverList[g_AISetting_MoneyDest].m_Money += 5000;            // Move the money to another server
            if(g_AISetting_MoneyDest != -1)                                serverList[g_AISetting_MoneyDest].m_LevelFirewall += 1;
            if(g_AISetting_Deathmatch == 1 && g_AISetting_MoneyDest != -1) serverList[g_AISetting_MoneyDest].m_LevelIntegrity++;
            
            // AI taunt chat
            chat->Anim_DisplayText("\n%s> %s",m_AINickname,g_AITauntMessage_SPEND[rand() % AI_TAUNT_MSG_COUNT]);
       
            // AI Log
            sysLogPrintAI("[$$$][%d,%s (I:%d,F:%d,M:%d)] upgraded firewall",selfIndex,m_HostName,m_LevelIntegrity, m_LevelFirewall, m_Money);
        }

        // If we have the cash and our integrity is not critical, let's upgrade the CPU
        if(m_Money >= 5000 && m_LevelIntegrity > 2 && m_LevelCPUPower < 10 && m_LevelCPUPower <= m_LevelFirewall)
        {
            m_Money -= 5000;
            m_LevelCPUPower++;
            if(g_AISetting_MoneyDest != -1)                                serverList[g_AISetting_MoneyDest].m_Money += 5000;            // Move the money to another server
            if(g_AISetting_MoneyDest != -1)                                serverList[g_AISetting_MoneyDest].m_LevelFirewall += 1;
            if(g_AISetting_Deathmatch == 1 && g_AISetting_MoneyDest != -1) serverList[g_AISetting_MoneyDest].m_LevelIntegrity++;
            
            // AI taunt chat
            chat->Anim_DisplayText("\n%s> %s",m_AINickname,g_AITauntMessage_SPEND[rand() % AI_TAUNT_MSG_COUNT]);
       
            // AI Log
            sysLogPrintAI("[$$$][%d,%s (I:%d,F:%d,M:%d)] upgraded CPU",selfIndex,m_HostName,m_LevelIntegrity, m_LevelFirewall, m_Money);
        }

    }

    // Are we attacking?
    if(m_AIHostileCountdown > m_AIHostile)
        return 0;


    // ** OPEN WORLD CODE START ** // 
    if(m_OpenWorld == 1 && g_AISetting_AIAttackTarget == -1)
    {
        // Find a target to attack
        targetIndex = 0;               // Always start from the localhost
        
        // Calculate bounce CPU
        bounceCPU = m_LevelCPUPower;
        for(i = 0; i < count; i++)
            if(m_ServerKeys[i] == serverList[i].m_ServerKey) bounceCPU += serverList[i].m_LevelCPUPower;

        // Priority 1
        // Find the host with the most money, on which we can launch a firewall attack and get the cash
        // This is our priority
        for(i = count - 1; i >= 0; i--)
        {
            if(bounceCPU >= serverList[i].m_LevelFirewall && m_ServerKeys[i] != serverList[i].m_ServerKey && 
               serverList[i].m_Money >= serverList[targetIndex].m_Money && serverList[i].m_LevelIntegrity > 0
               && i != selfIndex)
            {
                targetIndex = i;
                AIAttackType = AI_ATTACK_FIREWALL;
            }
            //if(AIAttackType == -1) sysLogPrintAI("[---] P1 failed");
        }

        // Priority 2 (we can't hack any firewall but we have a loaded EMP)
        // No firewall we can hack and steal cash ... let's find something to nuke with the EMP
        // We try to nuke the server with the highest firewall
        if(m_ToolEMPLevel == 75.0f && AIAttackType == - 1)
        {
            for(i = count - 1; i >= 0; i--)
            {
                if(serverList[i].m_LevelFirewall > serverList[targetIndex].m_LevelFirewall  && serverList[i].m_LevelIntegrity > 0 && i != selfIndex)
                {
                    targetIndex = i;
                    AIAttackType = AI_ATTACK_EMP;
                }
            }
            //if(AIAttackType == -1) sysLogPrintAI("[---] P2 failed");
        }

        // Priority 2-B (we can't hack any firewall but we have a loaded EMP)
        // No firewall we can hack and steal cash ... let's find something to nuke with the EMP
        // We try to nuke the server with the highest integrity
        if(m_ToolEMPLevel == 75.0f && AIAttackType == - 1)
        {
            for(i = count - 1; i >= 0; i--)
            {
                if(serverList[i].m_LevelIntegrity > serverList[targetIndex].m_LevelIntegrity  && serverList[i].m_LevelIntegrity > 0 && i != selfIndex)
                {
                    targetIndex = i;
                    AIAttackType = AI_ATTACK_EMP;
                }
            }
            //if(AIAttackType == -1) sysLogPrintAI("[---] P2-B failed");
        }

        // Priority 3 (we can't hack any firewall, and we don't have an EMP loaded)
        // No firewall we can hack and steal cash, NO EMP ... let's find something to attack with the DOS
        // We try to attack the server with the highest firewall
        if(m_ToolDOSLevel == 75.0f && AIAttackType == - 1)
        {
            for(i = count - 1; i >= 0; i--)
            {
                if(serverList[i].m_LevelFirewall > serverList[targetIndex].m_LevelFirewall && serverList[i].m_LevelIntegrity > 0 && i != selfIndex)
                {
                    targetIndex = i;
                    AIAttackType = AI_ATTACK_DOS;
                }
            }
            //if(AIAttackType == -1) sysLogPrintAI("[---] P3 failed");
        }

        // Priority 3-B (we can't hack any firewall, and we don't have an EMP loaded)
        // No firewall we can hack and steal cash, NO EMP ... let's find something to attack with the DOS
        // We try to attack the server with the highest integrity
        if(m_ToolDOSLevel == 75.0f && AIAttackType == - 1)
        {
            for(i = count - 1; i >= 0; i--)
            {
                if(serverList[i].m_LevelIntegrity > serverList[targetIndex].m_LevelIntegrity && serverList[i].m_LevelIntegrity > 0 && i != selfIndex)
                {
                    targetIndex = i;
                    AIAttackType = AI_ATTACK_DOS;
                }
            }
            //if(AIAttackType == -1) sysLogPrintAI("[---] P3-B failed");
        }

    }
    else
    {
        // Single player mode reasoning
        // Do we have a specific target defined?
        if(g_AISetting_AIAttackTarget != -1)
            targetIndex = g_AISetting_AIAttackTarget;
        else 
            targetIndex = 0;           

        if(serverList[targetIndex].m_LevelIntegrity > 0)
        {
            if(m_ToolDOSLevel == 75.0f)                    AIAttackType = AI_ATTACK_DOS;
            if(m_ToolEMPLevel == 75.0f)                    AIAttackType = AI_ATTACK_EMP;
            if(bounceCPU >= serverList[targetIndex].m_LevelFirewall && m_ServerKeys[targetIndex] != serverList[targetIndex].m_ServerKey) 
                AIAttackType = AI_ATTACK_FIREWALL;
        }
    }

   if(AIAttackType == -1)
   {
       m_AIHostileCountdown = AI_ATTACK_COUNTDOWN - (g_PlayerDifficulty * pda_AI_ATTACK_COUNTDOWN); // Wait another round
       sysLogPrintAI("[---][%d,%s (I:%d,F:%d,M:%d)] didn't find a target to attack",selfIndex,m_HostName,m_LevelIntegrity, m_LevelFirewall, m_Money);
   }
    // ** OPEN WORLD CODE END ** //

	// Firewall attack
	if(AIAttackType == AI_ATTACK_FIREWALL)
	{
 		DECREMENT_WITH_CAP(m_AIHostile,AI_HOSTILE_DECREASE_FIREWALL,m_OpenWorld);
		DECREMENT_WITH_CAP(serverList[targetIndex].m_LevelIntegrity,1,0);
        serverList[targetIndex].AI_IncreaseHostileLevel(selfIndex, AI_HOSTILE_INCREASE_FIREWALL + pda_AI_HOSTILE_INCREASE * g_PlayerDifficulty);

        // Save firewall key
        if(serverList[targetIndex].m_ServerKeyRegenerationTime == -1.0f)
            serverList[targetIndex].m_ServerKeyRegenerationTime = SERVER_KEYREGEN_TIME;
        
        m_ServerKeys[targetIndex] = serverList[targetIndex].m_ServerKey;

        m_AIHostileCountdown = AI_ATTACK_COUNTDOWN - (g_PlayerDifficulty * pda_AI_ATTACK_COUNTDOWN);
       	if(targetIndex == 0) g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-incoming.wav");

        // Any money to steal? (only in open world mode)
        if(serverList[targetIndex].m_Money > 0 && m_OpenWorld == 1)
        {
            chat->Anim_DisplayText("\n%s> %s",m_AINickname,g_AITauntMessage_MONEY[rand() % AI_TAUNT_MSG_COUNT]);
            m_Money += serverList[targetIndex].m_Money;
            serverList[targetIndex].m_Money = 0;
            m_AIGotMoney = 1;

            // Start timer until we can spend it
            m_AIWaitSpendMoney = AI_WAIT_SPENDMONEY;
        }

		// Start attack animation sequence
		m_AIAttackDestination = targetIndex;
		m_AIAttackProgress    = 0.01f;

        AI_LastAttackedServer = targetIndex;
        m_AIStatFirewall++;

        // AI taunt chat
        chat->Anim_DisplayText("\n%s> %s: %s",m_AINickname,serverList[targetIndex].m_AINickname,g_AITauntMessage_FIREWALL[rand() % AI_TAUNT_MSG_COUNT]);

        // AI Log
        sysLogPrintAI("[FWL][%d,%s (I:%d,F:%d,M:%d)] attacks [%d, %s (I:%d,F:%d,M:%d)]",selfIndex,m_HostName,m_LevelIntegrity, m_LevelFirewall, m_Money,targetIndex,serverList[targetIndex].m_HostName,m_LevelIntegrity, serverList[targetIndex].m_LevelFirewall, serverList[targetIndex].m_Money);

		// Only one attack type per frame
		return 1;
	}

	// EMP attack launch
	if(AIAttackType == AI_ATTACK_EMP)
	{
    	DECREMENT_WITH_CAP(m_AIHostile,AI_HOSTILE_DECREASE_EMP,m_OpenWorld);
		DECREMENT_WITH_CAP(serverList[targetIndex].m_LevelIntegrity,2,0);
		DECREMENT_WITH_CAP(serverList[targetIndex].m_LevelFirewall,2,0);
        serverList[targetIndex].AI_IncreaseHostileLevel(selfIndex, AI_HOSTILE_INCREASE_EMP + pda_AI_HOSTILE_INCREASE * g_PlayerDifficulty);

		m_ToolEMPLevel = 0.0f;
		m_AIHostileCountdown = AI_ATTACK_COUNTDOWN - (g_PlayerDifficulty * pda_AI_ATTACK_COUNTDOWN);
       	if(targetIndex == 0) g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-incoming.wav");

		// Start attack animation sequence
		m_AIAttackDestination = targetIndex;
		m_AIAttackProgress    = 0.01f;

        AI_LastAttackedServer = targetIndex;
        m_AIStatEMP++;

        // AI taunt chat
        chat->Anim_DisplayText("\n%s> %s: %s",m_AINickname,serverList[targetIndex].m_AINickname,g_AITauntMessage_EMP[rand() % AI_TAUNT_MSG_COUNT]);

        // AI Log
        sysLogPrintAI("[EMP][%d,%s (I:%d,F:%d,M:%d)] attacks [%d, %s (I:%d,F:%d,M:%d)]",selfIndex,m_HostName,m_LevelIntegrity, m_LevelFirewall, m_Money,targetIndex,serverList[targetIndex].m_HostName,m_LevelIntegrity, serverList[targetIndex].m_LevelFirewall, serverList[targetIndex].m_Money);

		// Only one attack type per frame
		return 1;
	}

    // DOS attack launch
	if(AIAttackType == AI_ATTACK_DOS)
	{
		DECREMENT_WITH_CAP(m_AIHostile,AI_HOSTILE_DECREASE_DOS,m_OpenWorld);
		DECREMENT_WITH_CAP(serverList[targetIndex].m_LevelIntegrity,1,0);
		DECREMENT_WITH_CAP(serverList[targetIndex].m_LevelFirewall,1,0);
        serverList[targetIndex].AI_IncreaseHostileLevel(selfIndex, AI_HOSTILE_INCREASE_DOS + pda_AI_HOSTILE_INCREASE * g_PlayerDifficulty);

    	m_ToolDOSLevel = 0.0f;
	    m_AIHostileCountdown = AI_ATTACK_COUNTDOWN - (g_PlayerDifficulty * pda_AI_ATTACK_COUNTDOWN);
        if(targetIndex == 0) g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-incoming.wav");

		// Start attack animation sequence
		m_AIAttackDestination = targetIndex;
		m_AIAttackProgress    = 0.01f;

        AI_LastAttackedServer = targetIndex;
        m_AIStatDOS++;

        // AI taunt chat
        chat->Anim_DisplayText("\n%s> %s: %s",m_AINickname,serverList[targetIndex].m_AINickname,g_AITauntMessage_DOS[rand() % AI_TAUNT_MSG_COUNT]);

        // AI Log
        sysLogPrintAI("[DOS][%d,%s (I:%d,F:%d,M:%d)] attacks [%d, %s (I:%d,F:%d,M:%d)]",selfIndex,m_HostName,m_LevelIntegrity, m_LevelFirewall, m_Money,targetIndex,serverList[targetIndex].m_HostName,m_LevelIntegrity, serverList[targetIndex].m_LevelFirewall, serverList[targetIndex].m_Money);

        // Only one attack type per frame
		return 1;
	}

	return 0;
}
