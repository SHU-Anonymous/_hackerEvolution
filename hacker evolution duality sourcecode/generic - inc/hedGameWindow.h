/*
Name: hedGameWindow.h
Desc: Hacker Evolution Duality in-game window management

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#ifndef _hedGameWindow_h_
#define _hedGameWindow_h_

#include "hedGraphicsImage.h"

// Game window management
class hedGameWindow
{
public:
	// Constructor/Destructor
	hedGameWindow();
	~hedGameWindow();

	// Functions
	void Create(char* skinPath, int xPos, int yPos, int width, int height);
	void Render();

protected:
	hedImage skinImage[9];                        // Skin images
	int      m_XPos, m_YPos, m_Width, m_Height;   // Window coordinates and size
};

#endif