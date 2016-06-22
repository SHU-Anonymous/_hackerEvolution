/*
Name: hedGameEngine.h
Desc: Hacker Evolution Duality game engine (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _hedGameEngine_h_
#define _hedGameEngine_h_

#include "HackerEvolutionDualityDefines.h"
#include "hedGraphicsImage.h"
#include "hedRendererOpenGL.h"
#include "hedGameEngine-Menu.h"
#include "hedGameEngine-Game.h"
#include "hedGameEngine-ClassTextWindow.h"

//=======================================================================================================================================
// Constants and defines
// :: Engine modes
#define ENGINE_MODE_MENU           1  
#define ENGINE_MODE_LEVELINTRO     2
#define ENGINE_MODE_LEVELCOMPLETED 3
#define ENGINE_MODE_GAMECOMPLETED  4
#define ENGINE_MODE_INGAME         5

class hedGameEngine
{
public:
	// Constructor / destructor
	hedGameEngine();
	~hedGameEngine();

	// Functions
	void Initialize();                                                        // Initialize the game engine
	void Shutdown();                                                          // Shutdown the engine
	void SetDisplaySize(int width, int height);                               // Set the display size
	void SetInput(int xpos, int ypos, int lbClick, int lbClickDown, int key); // Push input info to the engine (mouse, keyboard, touch, etc)
	void ClearInput();                                                        // Clear all input data
	int  MainLoop(long frameTime);                                            // Main loop - update and render game frame
    
    int  GetEngineMode(){return m_pEngineMode;}

    int  GetEALinkButton()
    {
        int temp = EALinkButton;
        EALinkButton = -1;
        return temp;
    }
    int  EALinkButton;
    
protected:

	// Classes
	hedGameEngineMenu   m_GameMenu;              // Game menu
	hedImage            m_MouseCursor;           // Mouse cursor image

	// Graphics (global)
	hedImage m_BackgroundImage;                  // Menu background image
	hedImage m_BackgroundImageExt;               // Left and right background image extension
	hedImage m_BackgroundImageFill;              // Background fill pattern
	hedImage m_MenuSpinningLogo;                 // Spinning logo 
	hedImage m_MenuSlidingBar;                   // Background sliding bar image
	hedImage m_MenuSlidingWorldMap;              // Background sliding world map

	hedImage m_Keyboard;                         // On-screen keyboard for tablets, etc
	hedImage m_EAButtons;                        // EA Games buttons

    hedClassTextWindow*  m_ModEndWindow;         // Mod end elements
    hedImage*            m_ModEndWindowBG;

	// Variables
	int m_pEngineMode;                           // Game engine mode
	int m_pDisplayWidth, m_pDisplayHeight;       // Display resolution
	int deltax, deltay;                          // Used for centering items on the screen

	int m_pInputX, m_pInputY;                    // Input device (pointer) coordinates (mouse, touch, etc)
	int m_LBClick, m_LBClickDown;
	int m_Key;

	long  m_TotalTime;                           // Total engine time and frames rendered (for statistics)
	long  m_FramesRendered;                      //
	float m_MaxFPS;                              // Max FPS.

};

#endif