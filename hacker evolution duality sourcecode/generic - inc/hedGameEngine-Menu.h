/*
Name: hedGameEngine-Menu.h
Desc: Hacker Evolution Duality game engine - Menu management (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _hedGameEngine_Menu_h_
#define _hedGameEngine_Menu_h_

#include "hedGraphicsImage.h"
#include "hedGameEngine-ClassTextWindow.h"

// Menu modes
#define MODE_MENU_NEWGAME         0
#define MODE_MENU_OPENWORLDGAME   1
#define MODE_MENU_ONLINECOMMUNITY 4
#define MODE_MENU_QUIT            6

#define MODE_MENU_MAIN          -1
#define MODE_MENU_OPTIONS       -2
#define MODE_MENU_NEWPROFILE    -3
#define MODE_MENU_LOADPROFILE   -4
#define MODE_MENU_OPENWORLD     -5
#define MODE_MENU_OPENWORLD_SD  -6  // OpenWorld difficulty selection
#define MODE_MENU_NEWGAME_SD    -7  // Single player difficulty selection
#define MODE_MENU_SELECTLEVEL   -8  // Select level to play
#define MODE_MENU_OWPROFILENAME -9
#define MODE_MENU_CREDITS       -10

// UPDATE 1
#define MODE_MENU_SELECTMOD     -11

// Menu class
class hedGameEngineMenu
{
public:
	// Constructor / destructor
	hedGameEngineMenu();
	~hedGameEngineMenu();

	// Functions
	void Initialize();                                                         // Initialize the menu
	void SetInput(int xpos, int ypos, int lbClick, int lbClickdown, int key);  // Push input info to the menu (mouse, keyboard, touch, etc)
	int  GetMenuMode();                                                        // Returns the current menu mode
	void Render(float frameTime);                                              // Render the menu

	// Public variables
	int m_MenuMode;                                  // Current menu mode

protected:

	hedImage m_MenuListBackgroundImage;         // Menu list background image
	hedImage m_MenuListCursor;
	hedImage m_MenuListCursorOld;

    int deltax,deltay;

	int m_MenuCursorPosition;                   // Cursor position in the main menu         
	int m_MenuCursorPositionPrev;               // Previous position in the main menu
	int m_MenuCursorPositionOld;                // Old position in the main menu for the cursor trail

    // ** Options menu elements
    hedClassTextWindow  m_WindowOptions;
    hedImage            m_OptionsSlider;
    hedImage            m_OptionsSliderCursor;
    hedImage            m_OptionsCheck[2];

	hedImage            m_DeleteProfileButton; // Delete profile button (used for touch devices)
    hedImage            m_LoadProfileButton;   // Load profile button (used for touch devices)
};

#endif