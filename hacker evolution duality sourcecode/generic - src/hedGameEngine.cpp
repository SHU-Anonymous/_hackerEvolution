/*
Name: hedGameEngine.cpp
Desc: Hacker Evolution Duality game engine (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/hedGameEngine.h"
#include "../generic - inc/hedGameLanguage.h"
#include "../generic - inc/hedGameSettings.h"
#include "../generic - inc/hedPlayerProfile.h"
#include "../generic - inc/hedSound.h"
#include "../generic - inc/hedSystem.h"

#ifdef PLATFORM_WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef PLATFORM_IOS
#include <stdio.h>
#endif

#ifdef BUILD_STEAM
#include "steam_api.h"
#include "isteamutils.h"
#endif

int g_SteamApiOK = 0;
int g_LevelIntroTextFlushed = 0;

//=======================================================================================================================================
// Class constructor
hedGameEngine::hedGameEngine()
{
	m_TotalTime      = 0;
	m_FramesRendered = 0;
	m_MaxFPS         = 0.0f;
}

//=======================================================================================================================================
// Class destructor
hedGameEngine::~hedGameEngine()
{
}

//=======================================================================================================================================
// Initialize the game engine
void hedGameEngine::Initialize()
{
    EALinkButton  = -1;
    
	// Initialize the engine
	m_pEngineMode = ENGINE_MODE_MENU;
	m_pInputX     = 0;
	m_pInputY     = 0;
	m_LBClick     = 0;
	deltax        = (g_DisplayWidth - GFX_BACKGROUND_WIDTH) / 2;
	deltay        =  (g_DisplayHeight - GFX_BACKGROUND_HEIGHT) / 2;

	// Initialize engine modules
	g_RenderEngine.Init();
	g_RenderEngine.InitFontRenderer();
	m_GameMenu.Initialize();

	// Load the game skin
	g_InterfaceManager.LoadSkin("hed-black");

	// Load resources
#ifdef DEMO
	m_BackgroundImage.LoadFromFile("hed-graphics/res-background-main-demo.png");
#else
    #ifdef PLATFORM_IOS
        m_BackgroundImage.LoadFromFile("hed-ios-resources/res-background-main.png");
    #else
        m_BackgroundImage.LoadFromFile("hed-graphics/res-background-main.png");
    #endif
#endif
    m_BackgroundImageExt.LoadFromFile("hed-graphics/res-background-main-leftright.png");
	m_BackgroundImageFill.LoadFromFile("hed-graphics/hed-main-bgpattern.png");
	m_MouseCursor.LoadFromFile("hed-graphics/res-mousecursor.png");
	m_Keyboard.LoadFromFile("hed-graphics/res-keyboard.png");
	m_EAButtons.LoadFromFile("hed-graphics/menu-ea-buttons.png");

	m_MenuSpinningLogo.LoadFromFile("hed-graphics/res-menu-spinninglogo.png");
	m_MenuSlidingBar.LoadFromFile("hed-graphics/res-menu-slidingbar.png");
	m_MenuSlidingWorldMap.LoadFromFile("hed-graphics/res-menu-slidingworldmap.png");

	// Initialize other menu animations
	m_MenuSlidingWorldMap.m_AnimTranslateXSize  = GFX_MAINMENU_WMAP_DIST;
	m_MenuSlidingWorldMap.m_AnimTranslateXSpeed = GFX_MAINMENU_WMAP_SPEED;
	m_MenuSpinningLogo.m_AnimAngleSpeed = ANIM_SPEED_ANGLE;
	m_MenuSlidingBar.m_AnimTranslateYSize  = GFX_MAINMENU_SLIDER_DIST;
	m_MenuSlidingBar.m_AnimTranslateYSpeed = GFX_MAINMENU_SLIDER_SPEED;


	// Initialize sound and music
	g_SoundManager.Init();
#ifndef MODE_PARTYDEV 
	g_SoundManager.PlayList_Load("hed-music/playlist-menu.txt");
	g_SoundManager.PlayList_Play();
#endif

	g_Game.m_GameLevel.Init();

    // Load languages
    g_GameLanguage.LoadLanguageFile("hed-text/languages.txt");
    g_GameLanguage.SetCurrentLanguage("english");

    // Player profiles
    g_PlayerProfileManager.InitProfiles();

#ifdef BUILD_STEAM
	if(SteamAPI_Init())
    {
        sysLogPrint("hedGameEngine::Initialize() - SteamAPI_Init() returned OK.");
        //printf("hedGameEngine::Initialize() - SteamAPI_Init() returned OK.");
        g_SteamApiOK = 1;
    }
	else
    {
		sysLogPrint("hedGameEngine::Initialize() - SteamAPI_Init() failed.");
		//printf("hedGameEngine::Initialize() - SteamAPI_Init() failed.");
    }

#endif
}

//=======================================================================================================================================
// Shutdown the game engine
void hedGameEngine::Shutdown()
{
	g_SoundManager.Shutdown();
}

//=======================================================================================================================================
// Set the display size
void hedGameEngine::SetDisplaySize(int width, int height)
{
	// Save display resolution
	m_pDisplayWidth  = width;
	m_pDisplayHeight = height;
	
	// Switch to lower quality graphics when the resolution is smaller (netbooks)
	if(height == 600)
	{
		sysLogPrint("(hedGameEngine.cpp)hedGameEngine::SetDisplaySize() - switching to low quality graphics");		
		g_LowQualityGraphics = 1;
	}
}

//=======================================================================================================================================
// Push input info to the engine (mouse, keyboard, touch, etc)
void hedGameEngine::SetInput(int xpos, int ypos, int lbClick, int lbClickDown, int key)
{
	// Save input coordinates
	m_pInputX     = xpos;
	m_pInputY     = ypos;
	m_LBClick     = lbClick;
	m_LBClickDown = lbClickDown;
	m_Key         = key;

	// This is to simulate a key using the virtual on-screen keyboard
	if(g_OnScreenKeyboard == 1)
	{
		int left,right,top,bottom;

		left = (g_DisplayWidth - 770) / 2;
		top  = g_DisplayHeight - 260 - 5;

		left += 25;
		top  += 20;

		int key[4][10] = {'1','2','3','4','5','6','7','8','9','0',\
			              'Q','W','E','R','T','Y','U','I','O','P',\
			              'A','S','D','F','G','H','J','K','L',0,\
			              'Z','X','C','V','B','N','M',',','.','-'};

		int kx, ky;
		kx = (xpos - left) / 60;
		ky = (ypos - top)  / 60;

		if((lbClick) && kx >= 0 && kx < 10 && ky >= 0 && ky < 4 && (xpos - left) % 60 < 40 && (ypos - top) % 60 < 40) m_Key = key[ky][kx];

		// Special keys
		kx = xpos - (left - 25);
		ky = ypos - (top - 20);

		if(kx >= 625 && ky >= 20  && kx <= 745 && ky <= 60  && lbClick == 1) m_Key = HEDKEY_BACKSPACE;
		if(kx >= 625 && ky >= 140 && kx <= 745 && ky <= 180 && lbClick == 1) m_Key = HEDKEY_ENTER;
        
        // Clear click when on-screen keyboard is being used
        m_LBClick = 0;
	}
}

//=======================================================================================================================================
// Clear all input data
void hedGameEngine::ClearInput()
{
	// Clear input coordinates
	//m_pInputX = 0;
	//m_pInputY = 0;
	m_LBClick = 0;
	m_Key     = 0;
}

//=======================================================================================================================================
// Main game loop
int  hedGameEngine::MainLoop(long frameTime)
{
	int quit;

	g_FrameTime = (float)frameTime / 1000.0f;

    //===================================================================================================================================
	// Render main background
	if(m_pEngineMode != ENGINE_MODE_INGAME)
	{
		int   i, j, count, countx, county;

		countx = (g_DisplayWidth / 352) + 1;
		county = (g_DisplayHeight / 352) + 1;
        
		for(i = 0; i < countx + 1; i++)
			for(j = 0; j < county + 1; j++)
				m_BackgroundImageFill.Render(i * 352 - GFX_BACKGROUND_FILL_DX, j * 352);
        
		// Extend the background to fill the entire display
		// Left and right
		count = deltax / GFX_BACKGROUND_EXT_SIZE;
		for(i = 0; i < count + 1; i++)
		{
			m_BackgroundImageExt.Render(deltax - (i + 1) * GFX_BACKGROUND_EXT_SIZE, deltay);
			m_BackgroundImageExt.Render(GFX_BACKGROUND_WIDTH + deltax + i * GFX_BACKGROUND_EXT_SIZE, deltay);
		}

		// Draw the background
		m_BackgroundImage.Render(deltax,deltay);

	    // Update animations
	    if(m_MenuSlidingWorldMap.m_AnimTranslateX >= GFX_MAINMENU_WMAP_DIST || 	m_MenuSlidingWorldMap.m_AnimTranslateX <= 0)
			m_MenuSlidingWorldMap.m_AnimTranslateXSpeed *= -1.0f;

    	if(m_MenuSlidingBar.m_AnimTranslateY >= GFX_MAINMENU_SLIDER_DIST || m_MenuSlidingBar.m_AnimTranslateY <= 0)
			m_MenuSlidingBar.m_AnimTranslateYSpeed *= -1.0f;

	    // Draw background sliding worldmap
	    m_MenuSlidingWorldMap.Render((g_DisplayWidth - 1024) / 2 - GFX_MAINMENU_WMAP_X, deltay + GFX_MAINMENU_WMAP_Y);

	    // Draw background sliding bar
	    m_MenuSlidingBar.Render(deltax + GFX_MAINMENU_SLIDER_X, deltay + GFX_MAINMENU_SLIDER_Y);

	    // Draw the spinning logo
	    m_MenuSpinningLogo.Render(deltax + GFX_MAINMENU_SLOGO_X,deltay + GFX_MAINMENU_SLOGO_Y);
	}

	//===================================================================================================================================
	// Mode: ENGINE_MODE_MENU
	if(m_pEngineMode == ENGINE_MODE_MENU)
	{
		int menuMode;

		m_GameMenu.SetInput(m_pInputX,m_pInputY,m_LBClick,m_LBClickDown, m_Key);
		m_GameMenu.Render(g_FrameTime);
		menuMode = m_GameMenu.GetMenuMode();

#ifdef BUILD_EACHILLINGO
		// EA buttons
		m_EAButtons.Render(162,730);

		if(m_LBClick == 1)
		{
			int x = m_pInputX, y = m_pInputY;

			if(y > 730 && y < 730 + 25)
			{
				if(x > 162 && x < 162 + 200)
				{
					// Open EULA link
                    EALinkButton = 1;
				}

				if(x > 162 + 250 && x < 162 + 450)
				{
					// Open Privacy and Cookie Policy link
                    EALinkButton = 2;
				}

				if(x > 162 + 500 && x < 162 + 700)
				{
					// Open Terms of Service link
                    EALinkButton = 3;
				}
			}
		}
#endif

		// Online community
		if(menuMode == MODE_MENU_ONLINECOMMUNITY)
		{
#ifdef BUILD_STEAM
//#ifndef DEMO
			SteamFriends()->ActivateGameOverlayToWebPage("http://store.steampowered.com/dlc/70120/");
//#endif
#else
			sysOpenCommunityURL();
#endif
			ClearInput();
		}

		// Hacker Evolution NEWS (STEAM ONLY)
#ifdef BUILD_STEAM
		if(menuMode == MODE_MENU_CREDITS)
		{
			SteamFriends()->ActivateGameOverlayToWebPage("http://www.exosyphen.com/steam_hackerevolutionnews.htm");
			ClearInput();
		}
#endif

		// New open world game
		if(menuMode == MODE_MENU_OPENWORLDGAME)
		{
            //g_Game.m_GameLevel.SetGameMod("OpenWorld",1);
            g_Game.InitGameElements();
			g_Game.InitGame();
            g_Game.m_GameLevel.LoadLevel(g_Game.m_GameLevel.m_LevelIndex);

            g_SoundManager.PlayList_Load("hed-music/playlist-levelintro.txt");
			m_pEngineMode = ENGINE_MODE_LEVELINTRO;

			ClearInput();
		}	

		// New game
		if(menuMode == MODE_MENU_NEWGAME)
		{
            //g_Game.m_GameLevel.SetGameMod(g_PlayerProfileManager.getCurrentModName(),0);
            g_Game.InitGameElements();
			g_Game.InitGame();

			//g_Game.m_GameLevel.LoadLevel(g_PlayerProfileManager.getCurrentLevel());
			g_Game.m_GameLevel.LoadLevel(g_PlayerProfileManager.getCurrentLevel());
            g_Game.m_GameLevel.m_ServerList[0].statScore = g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].score;

            g_SoundManager.PlayList_Load("hed-music/playlist-levelintro.txt");
			m_pEngineMode = ENGINE_MODE_LEVELINTRO;
            m_LBClick = 0;
			ClearInput();

            g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].difficulty = g_PlayerDifficulty;
		}	

		// Quit the game?
		if(menuMode == MODE_MENU_QUIT)
			return 0;
	}

	//===================================================================================================================================
	// Mode: ENGINE_MODE_LEVELINTRO
	if(m_pEngineMode == ENGINE_MODE_LEVELINTRO)
	{
		// Draw the background
		g_Game.RenderLevelIntro(0);

		// New game
#if defined(PLATFORM_IOS)
		if(m_LBClick != 0)
#else
        if(m_LBClick != 0 || m_Key != 0)
#endif
		{
			if(g_LevelIntroTextFlushed == 0)
			{
				// Skip all text animation
				g_Game.RenderLevelIntro(-1); 
				g_LevelIntroTextFlushed = 1;
			}
			else
			{
				g_LevelIntroTextFlushed = 0;

				// Hide the level intro window
				g_Game.RenderLevelIntro(1);                

				m_pEngineMode = ENGINE_MODE_INGAME;
				//g_Game.InitGameElements();
				//g_Game.InitGame();

				// Initialize and load level
				//g_Game.m_GameLevel.LoadLevel(g_Game.m_GameLevel.m_LevelIndex);

				// Switch the playlist
				g_SoundManager.PlayList_Load("hed-music/playlist-game.txt");
			}
			ClearInput();
		}
	}

	//===================================================================================================================================
	// Mode: ENGINE_MODE_GAMECOMPLETED
	if(m_pEngineMode == ENGINE_MODE_GAMECOMPLETED)
	{
		// Draw the background
		m_ModEndWindow->Render();

		// New game
		if(m_LBClick != 0 || m_Key != 0)
		{
            HED_SAFE_DELETE(m_ModEndWindow);
            HED_SAFE_DELETE(m_ModEndWindowBG);

    		m_pEngineMode = ENGINE_MODE_MENU;
            g_Game.m_GameLevel.m_LevelIndex = 0;

			ClearInput();
		}
	}
	//===================================================================================================================================
	// Mode: ENGINE_MODE_LEVELCOMPLETED
	if(m_pEngineMode == ENGINE_MODE_LEVELCOMPLETED)
	{
		// Draw the background
		g_Game.RenderLevelEnd(0);

        
		// (Open World game) Advance to the next level
        if((m_LBClick != 0 || m_Key != 0) && g_Game.m_GameLevel.m_OpenWorld == 1)
        {
            g_Game.RenderLevelEnd(1);

			m_GameMenu.m_MenuMode = MODE_MENU_MAIN;
			m_pEngineMode         = ENGINE_MODE_MENU;
			
			g_Game.m_GameLevel.m_LevelIndex = 0;

			// Switch the playlist
			g_SoundManager.PlayList_Load("hed-music/playlist-menu.txt");
        }

		// Advance to the next level
        if((m_LBClick != 0 || m_Key != 0) && g_Game.m_GameLevel.m_OpenWorld == 0)
		{
			// Hide the level intro window
			g_Game.RenderLevelEnd(1);                
            g_Game.m_GameLevel.m_LevelIndex++;

            if(g_Game.m_GameLevel.m_LevelIndex >= g_Game.m_GameLevel.m_LevelCount)
			{
				// Game completed
                hedString path;

                m_ModEndWindow   = new hedClassTextWindow;
                m_ModEndWindowBG = new hedImage;

                sprintf(path,"hed-gamemods/%s/%s.png",g_Game.m_GameLevel.m_ModName,g_Game.m_GameLevel.m_ModName);
                m_ModEndWindowBG->LoadFromFile(path);

                m_ModEndWindow->Init((g_DisplayWidth - 1024) / 2, (g_DisplayHeight - 600) / 2,1024,600);
                m_ModEndWindow->SetBackgroundImage(m_ModEndWindowBG);
		        m_ModEndWindow->SetTextOffsets(20,60);
		        m_ModEndWindow->m_Font       = HED_FONT_BIG10;
		        m_ModEndWindow->ToggleVisibility();

                sprintf(path,"hed-gamemods/%s/%s-end.txt",g_Game.m_GameLevel.m_ModName,g_Game.m_GameLevel.m_ModName);
                m_ModEndWindow->Anim_DisplayFile(path);

				m_pEngineMode = ENGINE_MODE_GAMECOMPLETED;
			}
			else
            {
	    		//g_Game.InitGame();
                //m_pEngineMode = ENGINE_MODE_LEVELINTRO;
    
                g_Game.InitGameElements();
    			g_Game.InitGame();
		    	g_Game.m_GameLevel.LoadLevel(g_PlayerProfileManager.getCurrentLevel());
                g_Game.m_GameLevel.m_ServerList[0].statScore = g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].score;
                
                m_pEngineMode = ENGINE_MODE_LEVELINTRO;
                
            }
    
            ClearInput();
		}
        
	}

	//===================================================================================================================================
	// Mode: ENGINE_MODE_GAME
	if(m_pEngineMode == ENGINE_MODE_INGAME)
	{
		g_Game.ProcessInput(m_pInputX, m_pInputY, m_LBClick, m_LBClickDown, m_Key); 
		quit = g_Game.UpdateGame(frameTime);
		g_Game.RenderGame();

		// Game has been abandoned, return to main menu
		if(quit == 1)
		{
			m_GameMenu.m_MenuMode = MODE_MENU_MAIN;
			m_pEngineMode         = ENGINE_MODE_MENU;
			
			g_Game.m_GameLevel.m_LevelIndex = 0;

			// Switch the playlist
			g_SoundManager.PlayList_Load("hed-music/playlist-menu.txt");
		}
		
		// Level completed. Advance to the next level
		if(quit == 2)
		{
			// Switch the playlist
		    g_Game.RenderLevelEnd(1);
            g_SoundManager.PlayList_Load("hed-music/playlist-levelintro.txt");
            m_pEngineMode = ENGINE_MODE_LEVELCOMPLETED;
		}	
	}

	// Render on-screen keyboard
	if(g_OnScreenKeyboard == 1) m_Keyboard.Render((g_DisplayWidth - 770) / 2, g_DisplayHeight - 260 - 5);

	// Render mouse cursor
#ifndef PLATFORM_IOS
	m_MouseCursor.Render(m_pInputX, m_pInputY);
#endif

	// Calculate average FPS
	m_TotalTime = m_TotalTime + frameTime;
	m_FramesRendered++;

	// Display game version and FPS
	if(1000.0f / (float)frameTime > m_MaxFPS)
		m_MaxFPS = 1000.0f / (float)frameTime;

	if(m_pEngineMode == ENGINE_MODE_MENU)
	{
		
        if(g_SteamApiOK != 0)
            g_RenderEngine.RenderText(5, 5,0.67f,0.87f,0.003f,HED_FONT_NORMAL,"%s-%s(STEAM: %d)",GAME_VERSION,GAME_PLATFORM,g_SteamApiOK);
        else
            g_RenderEngine.RenderText(5, 5,0.67f,0.87f,0.003f,HED_FONT_NORMAL,"%s-%s",GAME_VERSION,GAME_PLATFORM);

		g_RenderEngine.RenderText(5,20,0.5f,0.5f,0.5f,HED_FONT_NORMAL,"fps: %3.0f (avg: %2.1f)", 1000.0f / (float)frameTime,(1000.0f * (double)m_FramesRendered) / (double)m_TotalTime);
		
	}
	//if(m_pEngineMode == ENGINE_MODE_INGAME)
	//	g_RenderEngine.RenderText(5,20,0.5f,0.5f,0.5f,HED_FONT_BIG10,"FPS: %3.0f (Avg: %2.1f)", 1000.0f / (float)frameTime,(1000.0f * (double)m_FramesRendered) / (double)m_TotalTime);
	//  g_RenderEngine.RenderText(5,50,0.5f,0.5f,0.5f,HED_FONT_BIG10,"m_pEngineMode(%d)", m_pEngineMode);

	// Update sound engine
	g_SoundManager.Update();

	// Reset input
	ClearInput();

	// Continue running the engine
	return 1;
}
