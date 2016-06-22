/*
Name: hedGameEngine-Menu.cpp
Desc: Hacker Evolution Duality game engine - Menu management (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/hedGameEngine-Menu.h"
#include "../generic - inc/hedGameEngine-Game.h"
#include "../generic - inc/hedGameSettings.h"
#include "../generic - inc/hedGameLanguage.h"
#include "../generic - inc/hedPlayerProfile.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include "../generic - inc/hedSound.h"

#ifdef PLATFORM_IOS
#define stricmp strcasecmp 
#include <string.h>
#include <stdio.h>
#endif

int _mouse_over_return = 0;
int _last_vol_music    = -1;
int _last_vol_fx       = -1;

hedString nickname;
hedString gameOWName;
int       nickNameIndex;
hedString serialNumber;

int menuOverflowIndex = 0; // Overflow index when having more options in a list that the menu can accomodate.
//=======================================================================================================================================
// Class constructor
hedGameEngineMenu::hedGameEngineMenu()
{
	m_MenuCursorPosition     = 0;
	m_MenuCursorPositionPrev = 0;
	m_MenuCursorPositionOld  = 0;
	m_MenuMode               = MODE_MENU_MAIN;
}

//=======================================================================================================================================
// Class destructor
hedGameEngineMenu::~hedGameEngineMenu()
{
}

//=======================================================================================================================================
// Initialize the menu
void hedGameEngineMenu::Initialize()
{
	deltax = (g_DisplayWidth - GFX_BACKGROUND_WIDTH) / 2;
	deltay = (g_DisplayHeight - GFX_BACKGROUND_HEIGHT) / 2;

	// Load images	
	m_MenuListBackgroundImage.LoadFromFile("hed-graphics/res-menu-background.png");
	m_MenuListCursor.LoadFromFile("hed-graphics/res-menu-cursor.png");
	m_MenuListCursorOld.LoadFromFile("hed-graphics/res-menu-cursor.png");

    // Initializes the trail cursor that fades away when we move over a new menu option
	m_MenuListCursorOld.m_AnimAlpha      = 0.0f;
	m_MenuListCursorOld.m_AnimAlphaSpeed = 0.0f;

    // ** Options menu
    m_WindowOptions.Init(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET,512,512);
    m_WindowOptions.SetBackgroundImage(&m_MenuListBackgroundImage);
    m_WindowOptions.ForceVisible();

    m_OptionsSlider.LoadFromFile("hed-graphics/menu-options-slider-base.png");
    m_OptionsSliderCursor.LoadFromFile("hed-graphics/menu-options-slider.png");
    m_OptionsCheck[0].LoadFromFile("hed-graphics/menu-options-check-0.png");
    m_OptionsCheck[1].LoadFromFile("hed-graphics/menu-options-check-1.png");

    m_WindowOptions.AttachImage(0,&m_OptionsCheck[0], 70, 55,1);  // Fullscreen check 
    m_WindowOptions.AttachImage(1,&m_OptionsCheck[1], 70, 55,0); 
    m_WindowOptions.CreateToggle(0,g_GameSettings.ValueInt("video_fullscreen"));

    m_WindowOptions.AttachImage(2,&m_OptionsCheck[0], 70, 85,1);  // Soft touch 
    m_WindowOptions.AttachImage(3,&m_OptionsCheck[1], 70, 85,0); 
    m_WindowOptions.CreateToggle(2,g_GameSettings.ValueInt("input_softtouch"));

    m_WindowOptions.AttachImage(4,&m_OptionsSlider, 70, 118,1);  // Music volume 
    m_WindowOptions.AttachImage(5,&m_OptionsSliderCursor, 70, 118,1);
    m_WindowOptions.CreateSlider(5,200,g_GameSettings.ValueInt("sound_volume_music") * 2);

    m_WindowOptions.AttachImage(6,&m_OptionsSlider, 70, 148,1);  // Sound FX volume 
    m_WindowOptions.AttachImage(7,&m_OptionsSliderCursor, 70, 148,1);
    m_WindowOptions.CreateSlider(7,200,g_GameSettings.ValueInt("sound_volume_fx") * 2);
    
    _last_vol_music    = g_GameSettings.ValueInt("sound_volume_music") * 2;
    _last_vol_fx       = g_GameSettings.ValueInt("sound_volume_fx") * 2;

#ifdef PLATFORM_WINDOWS
    m_WindowOptions.AttachImage(8,&m_OptionsCheck[0], 70, 205,1);  // Soft touch 
    m_WindowOptions.AttachImage(9,&m_OptionsCheck[1], 70, 205,0); 
    m_WindowOptions.CreateToggle(8,g_GameSettings.ValueInt("game_autosubmit"));
#endif	

	m_DeleteProfileButton.LoadFromFile("hed-graphics/res-menu-button-profile-delete.png");
	m_LoadProfileButton.LoadFromFile("hed-graphics/res-menu-button-profile-load.png");
}

//=======================================================================================================================================
// Push input info to the menu (mouse, keyboard, touch, etc)
void hedGameEngineMenu::SetInput(int xpos, int ypos, int lbClick, int lbClickdown, int key)
{
	int temp, tempx, tempy;
	int tempMenuCursorPosition = -1;

	// Main menu
	if(m_MenuMode == MODE_MENU_MAIN)
	{
		// Calculate relative mouse coordinates
		tempx = xpos - deltax;
		tempy = ypos - deltay;

		// Calculate cursor position
		if(tempx > MENU_MAIN_CURSOR_X && tempx < MENU_MAIN_CURSOR_X + 300)
		{
			// The mouse is inside the horizontal menu area => calculate cursor position
			temp = (tempy - MENU_MAIN_CURSOR_Y) / MENU_MAIN_LIST_SPACE;
			if(temp > -1 && temp < MENU_MAIN_COUNT)
			{
				m_MenuCursorPosition   = temp;
				tempMenuCursorPosition = temp;
			}

			// If we moved to a new menu item, run the effects here
			if(m_MenuCursorPositionPrev != m_MenuCursorPosition)
			{
				// Fade out trail
				m_MenuCursorPositionOld              = m_MenuCursorPositionPrev;
				m_MenuListCursorOld.m_AnimAlpha      = 1.0f;
				m_MenuListCursorOld.m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA * 2;

				// Fade in cursor
				m_MenuListCursor.m_AnimAlpha      = 0.0f;
				m_MenuListCursor.m_AnimAlphaSpeed = ANIM_SPEED_ALPHA * 2;

				m_MenuCursorPositionPrev = m_MenuCursorPosition;
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");
			}
		}

		// Click?
		if(lbClick == 1)
		{
            // New game (profile)
			if(tempMenuCursorPosition == 0)
            {
                if(g_PlayerProfileManager.m_ProfileCount < HED_MAXPROFILES)
                {
                    //m_MenuMode = MODE_MENU_NEWPROFILE;       // New game
                    
					// UPDATE 1
					//m_MenuMode = MODE_MENU_NEWGAME_SD;
					m_MenuMode = MODE_MENU_SELECTMOD;
                    
					nickNameIndex = 9;
                    strcpy(nickname,"hedPlayer");
                }
                else
                    g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-error.wav");
            }

           // if(tempMenuCursorPosition == 1) m_MenuMode = MODE_MENU_OPENWORLDGAME;   // Open World game
            if(tempMenuCursorPosition == 1)                                           // Open World game
            {
            	//m_MenuMode = MODE_MENU_OPENWORLD_SD;      // Select open world difficulty
                m_MenuMode = MODE_MENU_OWPROFILENAME;
                strcpy(gameOWName,g_GameSettings.ValueStr("game_ow_name"));

                m_MenuCursorPosition     = 0;
	            m_MenuCursorPositionPrev = 0;
	            m_MenuCursorPositionOld  = 0;
                g_Game.m_GameLevel.SetGameMod("OpenWorld",1);
                
            }
            if(tempMenuCursorPosition == 2) 
            {
                m_MenuMode = MODE_MENU_LOADPROFILE;     // Load player profile
            	m_MenuCursorPosition     = 0;
	            m_MenuCursorPositionPrev = 0;
	            m_MenuCursorPositionOld  = 0;
            }
			if(tempMenuCursorPosition == 3) 
            {
                m_MenuMode = MODE_MENU_OPTIONS;         // Options
                strcpy(serialNumber,g_GameSettings.ValueStr("serial_number"));
            }
#if defined(PLATFORM_WINDOWS) || defined (PLATFORM_MACOS)	
			if(tempMenuCursorPosition == 4) m_MenuMode = MODE_MENU_ONLINECOMMUNITY; // Online community

#endif			
			if(tempMenuCursorPosition == 5) m_MenuMode = MODE_MENU_CREDITS;         // Credits
			if(tempMenuCursorPosition == 6) m_MenuMode = MODE_MENU_QUIT;            // Quit the menu (game)
		}
        return;
	}

    // Options menu
	if(m_MenuMode == MODE_MENU_OPTIONS)
    {
        int _return = 0;

        // Process input
        m_WindowOptions.ProcessInput(xpos,ypos,lbClick,lbClickdown,key);

        if(m_WindowOptions.mouseToWindowX(xpos) >= 70  && m_WindowOptions.mouseToWindowX(xpos) <= 140 &&
           m_WindowOptions.mouseToWindowY(ypos) >= 239 && m_WindowOptions.mouseToWindowY(ypos) <= 255)
        {
            _mouse_over_return = 1;
            if(lbClick == 1) _return = 1;
        }
        else
            _mouse_over_return = 0;
           

        if(key == HEDKEY_ESCAPE)
            _return = 1;

        // Enter serial number
        int  length = strlen(serialNumber);
        
        if((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == '-')
            if(length < 15)
            {
                serialNumber[length] = key;
                serialNumber[length + 1] = 0;
            }
        
        if(key == HEDKEY_BACKSPACE)
        {
            if(length > 0)
                serialNumber[length - 1] = 0;
        }

        // Update volume in realtime
        if(_last_vol_music != m_WindowOptions.sliderPosition[5] || _last_vol_fx != m_WindowOptions.sliderPosition[7])
        {
            _last_vol_music  = m_WindowOptions.sliderPosition[5];
            _last_vol_fx     = m_WindowOptions.sliderPosition[7];
            g_GameSettings.SetValueInt("sound_volume_music",m_WindowOptions.sliderPosition[5] / 2);
            g_GameSettings.SetValueInt("sound_volume_fx",m_WindowOptions.sliderPosition[7] / 2);

            g_SoundManager.UpdateVolumeFromSettings();
        }

        // Return to main menu
        if(_return == 1)
        {
            m_MenuMode = MODE_MENU_MAIN;

            // Save settings
            g_GameSettings.SetValueInt("video_fullscreen",m_WindowOptions.toggleStatus[0]);
            g_GameSettings.SetValueInt("input_softtouch",m_WindowOptions.toggleStatus[2]);
            g_GameSettings.SetValueInt("sound_volume_music",m_WindowOptions.sliderPosition[5] / 2);
            g_GameSettings.SetValueInt("sound_volume_fx",m_WindowOptions.sliderPosition[7] / 2);
            g_GameSettings.SetValueStr("serial_number",serialNumber);
            g_GameSettings.SetValueInt("game_autosubmit",m_WindowOptions.toggleStatus[8]);
        }
    }

    // Create new profile
	if(m_MenuMode == MODE_MENU_NEWPROFILE)
    {
		// Enable on-screen keyboard if needed
		if(g_IsTabletPC)
			g_OnScreenKeyboard = 1;

        if(key == HEDKEY_ESCAPE) 
		{
			m_MenuMode = MODE_MENU_MAIN;
			g_OnScreenKeyboard = 0;
		}

        // Type in the nickname
        int  length = strlen(nickname);
        
        if((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == '-')
        {
            if(length < 12)
            {
                nickname[length] = key;
                nickname[length + 1] = 0;
            }
        }
        
        if(key == HEDKEY_BACKSPACE)
        {
            if(length > 0)
                nickname[length - 1] = 0;
        }

        // Nickname entered, start new game
        if(key == HEDKEY_ENTER && length > 0 && g_PlayerProfileManager.GetProfileIndex(nickname) == -1)
        {
            m_MenuMode = MODE_MENU_NEWGAME;
            g_PlayerProfileManager.AddProfile(nickname,hedModActive,g_PlayerDifficulty);
            g_PlayerProfileManager.SetProfile(g_PlayerProfileManager.GetProfileCount() - 1);
            g_Game.m_GameLevel.SetGameMod(g_PlayerProfileManager.getCurrentModName(),0);

			g_OnScreenKeyboard = 0;

        }
    }

    // Openworld profile name
	if(m_MenuMode == MODE_MENU_OWPROFILENAME)
    {
		// Enable on-screen keyboard if needed
		if(g_IsTabletPC)
			g_OnScreenKeyboard = 1;
        
        if(key == HEDKEY_ESCAPE)
		{
			m_MenuMode = MODE_MENU_MAIN;
			g_OnScreenKeyboard = 0;
		}

        // Type in the nickname
        int  length = strlen(gameOWName);
        
        if((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == '-')
        {
            if(length < 12)
            {
                gameOWName[length] = key;
                gameOWName[length + 1] = 0;
            }
        }
        
        if(key == HEDKEY_BACKSPACE)
        {
            if(length > 0)
                gameOWName[length - 1] = 0;
        }

        // Nickname entered, start new game
        if(key == HEDKEY_ENTER && length > 0 && g_PlayerProfileManager.GetProfileIndex(nickname) == -1)
        {
            m_MenuMode = MODE_MENU_OPENWORLD_SD;      // Select open world difficulty
            m_MenuCursorPosition     = 0;
            m_MenuCursorPositionPrev = 0;
	        m_MenuCursorPositionOld  = 0;

            g_GameSettings.SetValueStr("game_ow_name",gameOWName);
            
            g_OnScreenKeyboard = 0;
            
            return;
        }
    }

	// Load profile
	if(m_MenuMode == MODE_MENU_LOADPROFILE)
	{
		int newSelection = 1;

		// Calculate relative mouse coordinates
		tempx = xpos - deltax;
		tempy = ypos - deltay;

		// Calculate cursor position
		if(tempx > MENU_MAIN_CURSOR_X && tempx < MENU_MAIN_CURSOR_X + 300 && lbClick == 1)
		{
			// The mouse is inside the horizontal menu area => calculate cursor position
			temp = (tempy - MENU_MAIN_CURSOR_Y) / MENU_MAIN_LIST_SPACE;
            if(temp > -1 && temp < (g_PlayerProfileManager.m_ProfileCount + 1)/*MENU_MAIN_COUNT*/)
			{
				m_MenuCursorPosition   = temp;
				tempMenuCursorPosition = temp;
			}

			// If we moved to a new menu item, run the effects here
			if(m_MenuCursorPositionPrev != m_MenuCursorPosition)
			{
                // No animations on touch devices
                
                if(g_IsTabletPC == 0)
                {
                    // Fade out trail
                    m_MenuCursorPositionOld              = m_MenuCursorPositionPrev;
                    m_MenuListCursorOld.m_AnimAlpha      = 1.0f;
                    m_MenuListCursorOld.m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA * 2;

                    // Fade in cursor
                    m_MenuListCursor.m_AnimAlpha      = 0.0f;
                    m_MenuListCursor.m_AnimAlphaSpeed = ANIM_SPEED_ALPHA * 2;
                }
                else
                {
                    // Fade out trail
                    m_MenuCursorPositionOld              = m_MenuCursorPosition;
                    m_MenuListCursorOld.m_AnimAlpha      = 0.0f;
                    m_MenuListCursorOld.m_AnimAlphaSpeed = 0.0f;
                    
                    // Fade in cursor
                    m_MenuListCursor.m_AnimAlpha      = 1.0f;
                    m_MenuListCursor.m_AnimAlphaSpeed = 0.0f;

                }
                m_MenuCursorPositionPrev = m_MenuCursorPosition;
                g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");

				if(lbClick == 0)newSelection = 0;
			}
		}

		// Click?
        /*
		if(lbClick == 1 && m_MenuCursorPosition < g_PlayerProfileManager.m_ProfileCount)
        {
			if(g_IsTabletPC == 1 && newSelection == 1)
			{
				
			}
			else
			{
                
				m_MenuMode = MODE_MENU_SELECTLEVEL;
				menuOverflowIndex = 0;

				g_PlayerProfileManager.SetProfile(m_MenuCursorPosition);
				g_PlayerDifficulty =  g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].difficulty;

				g_Game.m_GameLevel.SetGameMod(g_PlayerProfileManager.getCurrentModName(),0);
                 
			}

            return;
        }
        */
        // Tablet PC input
        if(lbClick == 1 && m_MenuCursorPosition < g_PlayerProfileManager.m_ProfileCount)
        {
            if(xpos >  deltax + MENU_MAIN_LIST_X + 320 && xpos < deltax + MENU_MAIN_LIST_X + 320 + 128 &&
               ypos >  deltay + MENU_MAIN_LIST_Y + 1 * MENU_MAIN_LIST_SPACE && ypos < deltay + MENU_MAIN_LIST_Y + 1 * MENU_MAIN_LIST_SPACE + 32)
            {
 				m_MenuMode = MODE_MENU_SELECTLEVEL;
				menuOverflowIndex = 0;
                
				g_PlayerProfileManager.SetProfile(m_MenuCursorPosition);
				g_PlayerDifficulty =  g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].difficulty;
                
				g_Game.m_GameLevel.SetGameMod(g_PlayerProfileManager.getCurrentModName(),0);
                return;
            }
            
            if(xpos >  deltax + MENU_MAIN_LIST_X + 320 && xpos < deltax + MENU_MAIN_LIST_X + 320 + 128 &&
               ypos >  deltay + MENU_MAIN_LIST_Y + 5 * MENU_MAIN_LIST_SPACE && ypos < deltay + MENU_MAIN_LIST_Y + 5 * MENU_MAIN_LIST_SPACE + 32)
            {
                g_PlayerProfileManager.DeleteProfile(m_MenuCursorPosition);
            }
        }

        // Delete profile
        if((key == HEDKEY_BACKSPACE) && tempMenuCursorPosition < g_PlayerProfileManager.m_ProfileCount)
        {
            g_PlayerProfileManager.DeleteProfile(tempMenuCursorPosition);
        }

        // ESC?
        if(key == HEDKEY_ESCAPE || (lbClick == 1 && tempMenuCursorPosition == g_PlayerProfileManager.m_ProfileCount))
        {
        	m_MenuCursorPosition     = 0;
	        m_MenuCursorPositionPrev = 0;
	        m_MenuCursorPositionOld  = 0;

            m_MenuMode = MODE_MENU_MAIN;
        }
	}
	
    // Select open world level
	if(m_MenuMode == MODE_MENU_OPENWORLD)
	{
		// Calculate relative mouse coordinates
		tempx = xpos - deltax;
		tempy = ypos - deltay;

		// Calculate cursor position
		if(tempx > MENU_MAIN_CURSOR_X && tempx < MENU_MAIN_CURSOR_X + 300)
		{
			// The mouse is inside the horizontal menu area => calculate cursor position
			temp = (tempy - MENU_MAIN_CURSOR_Y) / MENU_MAIN_LIST_SPACE;
            if(temp > -1 && temp < g_Game.m_GameLevel.m_LevelCount)
			{
				m_MenuCursorPosition   = temp;
				tempMenuCursorPosition = temp;
			}

			// If we moved to a new menu item, run the effects here
			if(m_MenuCursorPositionPrev != m_MenuCursorPosition)
			{
				// Fade out trail
				m_MenuCursorPositionOld              = m_MenuCursorPositionPrev;
				m_MenuListCursorOld.m_AnimAlpha      = 1.0f;
				m_MenuListCursorOld.m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA * 2;

				// Fade in cursor
				m_MenuListCursor.m_AnimAlpha      = 0.0f;
				m_MenuListCursor.m_AnimAlphaSpeed = ANIM_SPEED_ALPHA * 2;

				m_MenuCursorPositionPrev = m_MenuCursorPosition;
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");
			}
		}

		// Click?
		if(lbClick == 1 && m_MenuCursorPosition < g_Game.m_GameLevel.m_LevelCount)
        {
            m_MenuMode = MODE_MENU_OPENWORLDGAME;
            g_Game.m_GameLevel.m_LevelIndex = m_MenuCursorPosition;
        }

        // ESC?
        if(key == HEDKEY_ESCAPE)
        {
        	m_MenuCursorPosition     = 0;
	        m_MenuCursorPositionPrev = 0;
	        m_MenuCursorPositionOld  = 0;

            m_MenuMode = MODE_MENU_MAIN;
        }
	}
    
    // OPENWORLD_SELECT_DIFFICULTY
	if(m_MenuMode == MODE_MENU_OPENWORLD_SD || m_MenuMode == MODE_MENU_NEWGAME_SD)
	{
		// Calculate relative mouse coordinates
		tempx = xpos - deltax;
		tempy = ypos - deltay;

		// Calculate cursor position
		if(tempx > MENU_MAIN_CURSOR_X && tempx < MENU_MAIN_CURSOR_X + 300)
		{
			// The mouse is inside the horizontal menu area => calculate cursor position
			temp = (tempy - MENU_MAIN_CURSOR_Y) / MENU_MAIN_LIST_SPACE;
            if(temp > -1 && temp < 3)
			{
				m_MenuCursorPosition   = temp;
				tempMenuCursorPosition = temp;
			}

			// If we moved to a new menu item, run the effects here
			if(m_MenuCursorPositionPrev != m_MenuCursorPosition)
			{
				// Fade out trail
				m_MenuCursorPositionOld              = m_MenuCursorPositionPrev;
				m_MenuListCursorOld.m_AnimAlpha      = 1.0f;
				m_MenuListCursorOld.m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA * 2;

				// Fade in cursor
				m_MenuListCursor.m_AnimAlpha      = 0.0f;
				m_MenuListCursor.m_AnimAlphaSpeed = ANIM_SPEED_ALPHA * 2;

				m_MenuCursorPositionPrev = m_MenuCursorPosition;
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");
			}
		}

		// Click?
		if(lbClick == 1 && m_MenuCursorPosition < 3)
        {
            g_PlayerDifficulty = m_MenuCursorPosition;
            if(m_MenuMode == MODE_MENU_OPENWORLD_SD)
                m_MenuMode = MODE_MENU_OPENWORLD;
            else
                m_MenuMode = MODE_MENU_NEWPROFILE;       // New game
        }

        // ESC?
        if(key == HEDKEY_ESCAPE)
        {
        	m_MenuCursorPosition     = 0;
	        m_MenuCursorPositionPrev = 0;
	        m_MenuCursorPositionOld  = 0;

            //m_MenuMode = MODE_MENU_MAIN;
            m_MenuMode = MODE_MENU_SELECTMOD;
        }
	}

    // Select game mod
	if(m_MenuMode == MODE_MENU_SELECTMOD)
	{
		// Calculate relative mouse coordinates
		tempx = xpos - deltax;
		tempy = ypos - deltay;

		// Calculate cursor position
		if(tempx > MENU_MAIN_CURSOR_X && tempx < MENU_MAIN_CURSOR_X + 300)
		{
			// The mouse is inside the horizontal menu area => calculate cursor position
			temp = (tempy - MENU_MAIN_CURSOR_Y) / MENU_MAIN_LIST_SPACE;
            if(temp > -1 && temp < hedModCount)
			{
				m_MenuCursorPosition   = temp;
				tempMenuCursorPosition = temp;
			}

			// If we moved to a new menu item, run the effects here
			if(m_MenuCursorPositionPrev != m_MenuCursorPosition)
			{
				// Fade out trail
				m_MenuCursorPositionOld              = m_MenuCursorPositionPrev;
				m_MenuListCursorOld.m_AnimAlpha      = 1.0f;
				m_MenuListCursorOld.m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA * 2;

				// Fade in cursor
				m_MenuListCursor.m_AnimAlpha      = 0.0f;
				m_MenuListCursor.m_AnimAlphaSpeed = ANIM_SPEED_ALPHA * 2;

				m_MenuCursorPositionPrev = m_MenuCursorPosition;
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");
			}
		}

		// Click?
		if(lbClick == 1 && m_MenuCursorPosition < hedModCount)
        {
			strcpy(hedModActive,hedModList[m_MenuCursorPosition]);
			m_MenuMode = MODE_MENU_NEWGAME_SD;
        }

        // ESC?
        if(key == HEDKEY_ESCAPE)
        {
        	m_MenuCursorPosition     = 0;
	        m_MenuCursorPositionPrev = 0;
	        m_MenuCursorPositionOld  = 0;

            m_MenuMode = MODE_MENU_MAIN;
        }
	}

    // MENU_MODE_SELECTLEVEL
	if(m_MenuMode == MODE_MENU_SELECTLEVEL)
	{
		// Calculate relative mouse coordinates
		tempx = xpos - deltax;
		tempy = ypos - deltay;

		// Calculate cursor position
		if(tempx > MENU_MAIN_CURSOR_X && tempx < MENU_MAIN_CURSOR_X + 300)
		{
			// The mouse is inside the horizontal menu area => calculate cursor position
			temp = (tempy - MENU_MAIN_CURSOR_Y) / MENU_MAIN_LIST_SPACE;
			int itemCount = g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].modLevelNumberMax + 1;
			if(itemCount > 7) itemCount = 7;
   
			if(temp > -1 && temp < itemCount)
			{
				m_MenuCursorPosition   = temp;
				tempMenuCursorPosition = temp;
			}

			// If we moved to a new menu item, run the effects here
			if(m_MenuCursorPositionPrev != m_MenuCursorPosition)
			{
				// Fade out trail
				m_MenuCursorPositionOld              = m_MenuCursorPositionPrev;
				m_MenuListCursorOld.m_AnimAlpha      = 1.0f;
				m_MenuListCursorOld.m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA * 2;

				// Fade in cursor
				m_MenuListCursor.m_AnimAlpha      = 0.0f;
				m_MenuListCursor.m_AnimAlphaSpeed = ANIM_SPEED_ALPHA * 2;

				m_MenuCursorPositionPrev = m_MenuCursorPosition;
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");

				// overflow
				if(m_MenuCursorPosition == 6 && menuOverflowIndex < g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].modLevelNumberMax - 6) menuOverflowIndex++;
				if(m_MenuCursorPosition == 0 && menuOverflowIndex > 0) menuOverflowIndex--;
			}
		}

		// Click?
		if(lbClick == 1 && m_MenuCursorPosition <= g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].modLevelNumberMax)
        {
            g_Game.m_GameLevel.m_LevelIndex = m_MenuCursorPosition + menuOverflowIndex;
            g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].modLevelNumber = m_MenuCursorPosition +  + menuOverflowIndex;
            m_MenuMode = MODE_MENU_NEWGAME;       // New game
        }

        // ESC?
        if(key == HEDKEY_ESCAPE)
        {
        	m_MenuCursorPosition     = 0;
	        m_MenuCursorPositionPrev = 0;
	        m_MenuCursorPositionOld  = 0;

            m_MenuMode = MODE_MENU_MAIN;
        }
	}

    // MENU_MODE_CREDITS
	if(m_MenuMode == MODE_MENU_CREDITS)
	{
        // ESC?
        if(key == HEDKEY_ESCAPE || lbClick == 1)
        {
        	m_MenuCursorPosition     = 0;
	        m_MenuCursorPositionPrev = 0;
	        m_MenuCursorPositionOld  = 0;

            m_MenuMode = MODE_MENU_MAIN;
        }
	}
}

//=======================================================================================================================================
// Returns the current menu mode
int hedGameEngineMenu::GetMenuMode()
{	
	int temp;

	temp       = m_MenuMode;

#if defined(BUILD_STEAM)
    if(m_MenuMode != MODE_MENU_OPTIONS && m_MenuMode != MODE_MENU_NEWPROFILE && m_MenuMode != MODE_MENU_LOADPROFILE && m_MenuMode != MODE_MENU_OPENWORLD &&
       m_MenuMode != MODE_MENU_OPENWORLD_SD && m_MenuMode != MODE_MENU_NEWGAME_SD && m_MenuMode != MODE_MENU_SELECTLEVEL && m_MenuMode != MODE_MENU_OWPROFILENAME
	   && m_MenuMode != MODE_MENU_SELECTMOD)
	    m_MenuMode = MODE_MENU_MAIN;
#else
    if(m_MenuMode != MODE_MENU_OPTIONS && m_MenuMode != MODE_MENU_NEWPROFILE && m_MenuMode != MODE_MENU_LOADPROFILE && m_MenuMode != MODE_MENU_OPENWORLD &&
       m_MenuMode != MODE_MENU_OPENWORLD_SD && m_MenuMode != MODE_MENU_NEWGAME_SD && m_MenuMode != MODE_MENU_SELECTLEVEL && m_MenuMode != MODE_MENU_OWPROFILENAME && m_MenuMode != MODE_MENU_CREDITS
	   && m_MenuMode != MODE_MENU_SELECTMOD)
	    m_MenuMode = MODE_MENU_MAIN;
#endif

    return temp;
}

//=======================================================================================================================================
// Render the menu
void hedGameEngineMenu::Render(float frameTime)
{
	int    i, j, count, countx, county;
	float  r,g,b;
    hedRGB menuTextColor;

    // Draw menu items
	if(m_MenuMode == MODE_MENU_MAIN)
	{
		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

#ifdef PLATFORM_IOS
		for(i = 0; i < MENU_MAIN_COUNT - 1; i++)
#else
        for(i = 0; i < MENU_MAIN_COUNT; i++)
#endif
		{
			menuTextColor = hedRGB(1.0,1.0,1.0);

            if(i != m_MenuCursorPosition)
                menuTextColor = hedRGB(0.67f,0.88f,0.01f);
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS)
#if defined(BUILD_STEAM)
			if(i == 4 || i == 5)
			{
//#ifndef BUILD_DEMO
//				if(i != 4)
//#endif
				g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, 0.988f, 0.367f, 0.058f, HED_FONT_BIG12, g_MainMenuNames[i]);
			}
			else
#endif  //#ifndef BUILD_STEAM
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG12, GAMETEXT(g_MainMenuNames[i]));		
#else
			if(i != 4)
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG12, GAMETEXT(g_MainMenuNames[i]));		
#endif			
        }

		// Cursor and trail
		m_MenuListCursor.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPosition * MENU_MAIN_LIST_SPACE);
		m_MenuListCursorOld.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPositionOld * MENU_MAIN_LIST_SPACE);

        // Too many profiles?
        if(g_PlayerProfileManager.m_ProfileCount >= HED_MAXPROFILES)
        {
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + 210, 0.5,0.5,0.5, HED_FONT_NORMAL, 
                                      "Too many player profiles created.");		
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + 225, 0.5,0.5,0.5, HED_FONT_NORMAL, 
                                      "Delete at least one before starting a new game.");		
        }
    }

    // Options menu
    if(m_MenuMode == MODE_MENU_OPTIONS)
    {
        m_WindowOptions.Render();
#ifndef BUILD_ONLIVE
        m_WindowOptions.RenderText(100, 60,0.67f,0.88f,0.01f, HED_FONT_BIG10, "Full-screen (requires restart)");
#endif //#ifndef BUILD_ONLIVE
        m_WindowOptions.RenderText(100, 90,0.67f,0.88f,0.01f, HED_FONT_BIG10, "Mouse (touch) assistance");
        m_WindowOptions.RenderText(295,120,0.67f,0.88f,0.01f, HED_FONT_BIG10, "Music volume - %d",m_WindowOptions.sliderPosition[5] / 2);
        m_WindowOptions.RenderText(295,150,0.67f,0.88f,0.01f, HED_FONT_BIG10, "FX volume - %d",m_WindowOptions.sliderPosition[7] / 2);
        
        
        // Serial number
#ifndef BUILD_ONLIVE
        hedString temp;
        sprintf(temp,"Serial number: %s|",serialNumber);
        m_WindowOptions.RenderText(70, 180,0.67f,0.88f,0.01f, HED_FONT_BIG10, temp);
#ifdef PLATFORM_WINDOWS
        m_WindowOptions.RenderText(100, 210,0.67f,0.88f,0.01f, HED_FONT_BIG10, "Submit Open World scores");
#endif
#endif //#ifndef BUILD_ONLIVE
        if(_mouse_over_return == 0)
            m_WindowOptions.RenderText(70,240,0.67f,0.88f,0.01f, HED_FONT_BIG10, "Return");
        else
            m_WindowOptions.RenderText(70,240,1.0f,1.0f,1.0f, HED_FONT_BIG10, "Return");

    }

    // New profile
    if(m_MenuMode == MODE_MENU_NEWPROFILE)
    {
        int x,y;

		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

        x = deltax + MENU_MAIN_LIST_X;
        y = 4 + deltay + MENU_MAIN_LIST_Y;

        g_RenderEngine.RenderText(x,y,      0.67f,0.88f,0.01f, HED_FONT_BIG12, "Create new player profile.");
        g_RenderEngine.RenderText(x,y + 30, 0.67f,0.88f,0.01f, HED_FONT_BIG12, "Your nickname: %s|",nickname);

        if(g_PlayerProfileManager.GetProfileIndex(nickname) != -1)
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + 210, 0.8,0.0,0.0, HED_FONT_BIG12, 
                                      "Error: Profile name already in use.");		

    }

    // New profile
    if(m_MenuMode == MODE_MENU_OWPROFILENAME)
    {
        int x,y;

		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

        x = deltax + MENU_MAIN_LIST_X;
        y = 4 + deltay + MENU_MAIN_LIST_Y;

        g_RenderEngine.RenderText(x,y, 0.67f,0.88f,0.01f, HED_FONT_BIG12, "Your nickname: %s|",gameOWName);

        if(strlen(gameOWName) < 1)
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + 210, 0.8,0.0,0.0, HED_FONT_BIG12, 
                                      "Error: nickname too short.");		

    }
    // Load profile
	if(m_MenuMode == MODE_MENU_LOADPROFILE)
	{
		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

        for(i = 0; i < g_PlayerProfileManager.m_ProfileCount + 1; i++)
		{
			menuTextColor = hedRGB(1.0,1.0,1.0);

            if(i != m_MenuCursorPosition)
			    menuTextColor = hedRGB(0.67f,0.88f,0.01f);

            if(i != g_PlayerProfileManager.m_ProfileCount)
                g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG12, 
                                          g_PlayerProfileManager.m_ProfileList[i].name);		
            else
                g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG12, 
                                          "...return to the main menu");		
            
            // Show profile info (mod and level number)
            if(i == m_MenuCursorPosition && i < g_PlayerProfileManager.m_ProfileCount)
            {
                g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + 210, 0.5,0.5,0.5, HED_FONT_BIG10, 
                "%s / level: %d",g_PlayerProfileManager.m_ProfileList[i].modName,g_PlayerProfileManager.m_ProfileList[i].modLevelNumber);		
            }
			
			// Profile DELETE button for touch devices
			//if(g_IsTabletPC)
			{
				m_DeleteProfileButton.Render(deltax + MENU_MAIN_LIST_X + 320 - 5, deltay + MENU_MAIN_LIST_Y + 5 * MENU_MAIN_LIST_SPACE - 5);
				m_LoadProfileButton.Render(deltax + MENU_MAIN_LIST_X + 320 - 5, deltay + MENU_MAIN_LIST_Y + 1 * MENU_MAIN_LIST_SPACE - 5);
			}
        }

        g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + 225, 0.67f,0.88f,0.01f, HED_FONT_BIG10, 
                                  "press BACKSPACE to delete a profile");		
		// Cursor and trail
		m_MenuListCursor.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPosition * MENU_MAIN_LIST_SPACE);
		m_MenuListCursorOld.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPositionOld * MENU_MAIN_LIST_SPACE);
	}

    // Select open world level
	if(m_MenuMode == MODE_MENU_OPENWORLD)
	{
		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

        for(i = 0; i < g_Game.m_GameLevel.m_LevelCount; i++)
		{
			menuTextColor = hedRGB(1.0,1.0,1.0);

            if(i != m_MenuCursorPosition)
            {
			    menuTextColor = hedRGB(0.67f,0.88f,0.01f);
                if(g_GameSettings.ValueChr("game_openworld_state",i) == 'n')
                    menuTextColor = hedRGB(0.5f,0.5f,0.5f);
            }
            
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG10, 
                g_Game.m_GameLevel.m_LevelDescription[i]);		
        }

		// Cursor and trail
		m_MenuListCursor.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPosition * MENU_MAIN_LIST_SPACE);
		m_MenuListCursorOld.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPositionOld * MENU_MAIN_LIST_SPACE);
	}

    // MODE_MENU_SELECTMOD
	if(m_MenuMode == MODE_MENU_SELECTMOD)
	{
		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

        g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y - MENU_MAIN_LIST_SPACE, 0.67f,0.88f,0.01f, HED_FONT_BIG10, "Select game MOD:");
		for(i = 0; i < hedModCount; i++)
		{
			menuTextColor = hedRGB(1.0,1.0,1.0);

            if(i != m_MenuCursorPosition)
			    menuTextColor = hedRGB(0.67f,0.88f,0.01f);
            
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG10, 
                hedModListDesc[i]);		
        }

		// Cursor and trail
		m_MenuListCursor.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPosition * MENU_MAIN_LIST_SPACE);
		m_MenuListCursorOld.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPositionOld * MENU_MAIN_LIST_SPACE);
	}


    // OPEN WORLD: Difficulty selection
	if(m_MenuMode == MODE_MENU_OPENWORLD_SD || m_MenuMode == MODE_MENU_NEWGAME_SD)
	{
		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);
        g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y - MENU_MAIN_LIST_SPACE, 0.67f,0.88f,0.01f, HED_FONT_BIG10, "Select difficulty level:");

        for(i = 0; i < 3; i++)
		{
			menuTextColor = hedRGB(1.0,1.0,1.0);

            if(i != m_MenuCursorPosition)
			    menuTextColor = hedRGB(0.67f,0.88f,0.01f);

            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG12, 
                g_MainMenuDifficulty[i]);		
        }

		// Cursor and trail
		m_MenuListCursor.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPosition * MENU_MAIN_LIST_SPACE);
		m_MenuListCursorOld.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPositionOld * MENU_MAIN_LIST_SPACE);
	}

    // MODE_MENU_SELECTLEVEL
	if(m_MenuMode == MODE_MENU_SELECTLEVEL)
	{
		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

		int itemCount = g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].modLevelNumberMax + 1;
		if(itemCount > 7) itemCount = 7;

        for(i = 0; i < itemCount; i++)
		{
			menuTextColor = hedRGB(1.0,1.0,1.0);

            if(i != m_MenuCursorPosition)
			    menuTextColor = hedRGB(0.67f,0.88f,0.01f);

            hedString temp;
            strcpy(temp,g_Game.m_GameLevel.m_LevelDescription[i + menuOverflowIndex]);
            temp[26] = '.';temp[27] = '.';temp[28] = '.';temp[29] = 0;
            
            g_RenderEngine.RenderText(deltax + MENU_MAIN_LIST_X, 4 + deltay + MENU_MAIN_LIST_Y + i * MENU_MAIN_LIST_SPACE, menuTextColor.r, menuTextColor.g, menuTextColor.b, HED_FONT_BIG10, 
                temp);		
        }

		// Cursor and trail
		m_MenuListCursor.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPosition * MENU_MAIN_LIST_SPACE);
		m_MenuListCursorOld.Render(deltax + MENU_MAIN_CURSOR_X, deltay + MENU_MAIN_CURSOR_Y + m_MenuCursorPositionOld * MENU_MAIN_LIST_SPACE);
	}

    // MODE_MENU_CREDITS
	if(m_MenuMode == MODE_MENU_CREDITS)
	{
		m_MenuListBackgroundImage.Render(deltax + GFX_MAINMENU_X - GFX_MAINMENU_OFFSET, deltay + GFX_MAINMENU_Y - GFX_MAINMENU_OFFSET);

        int x = deltax + MENU_MAIN_LIST_X;
        int y = 4 + deltay + MENU_MAIN_LIST_Y;

        g_RenderEngine.RenderText(x, y      ,0.67f,0.88f,0.01f, HED_FONT_BIG10, "developed by: exosyphen studios");
        g_RenderEngine.RenderText(x, y + 25 ,0.67f,0.88f,0.01f, HED_FONT_BIG10, "programming and design: Robert Muresan");
        g_RenderEngine.RenderText(x, y + 45 ,0.67f,0.88f,0.01f, HED_FONT_BIG10, "art: Monica Friciu");
        g_RenderEngine.RenderText(x, y + 65 ,0.67f,0.88f,0.01f, HED_FONT_BIG10, "testing: William Layton");
        g_RenderEngine.RenderText(x, y + 100 ,0.67f,0.88f,0.01f, HED_FONT_BIG10, "partial music (TimeIsUp.com): ");
        g_RenderEngine.RenderText(x, y + 120,0.67f,0.88f,0.01f, HED_FONT_BIG10, "- Alexey Popov and Konstantin Shilovsky");
        g_RenderEngine.RenderText(x, y + 155,0.67f,0.88f,0.01f, HED_FONT_BIG10, "Special thanks to ALL forum members!");
	}
}
