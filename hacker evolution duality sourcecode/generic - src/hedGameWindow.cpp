/*
Name: hedGameWindow.cpp
Desc: Hacker Evolution Duality in-game window management

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#include "../generic - inc/hedGameWindow.h"
#include <stdio.h>

//=======================================================================================================================================
// Constructor
hedGameWindow::hedGameWindow()
{
}

//=======================================================================================================================================
// Destructor
hedGameWindow::~hedGameWindow()
{
}

//=======================================================================================================================================
// Create window
void hedGameWindow::Create(char* skinPath, int xPos, int yPos, int width, int height)
{
	char imagePath[1024];
	int  i;

	// Save coordinates and size
	m_XPos   = xPos;
	m_YPos   = yPos;
	m_Width  = width;
	m_Height = height;

	// Load skin slices
	for(i = 0; i < 9; i++)
	{
		sprintf(imagePath,"%swindow-slice-%d.png",skinPath,i + 1);
		skinImage[i].LoadFromFile(imagePath);
	}
}

//=======================================================================================================================================
// Render window
void hedGameWindow::Render()
{
	int i, j, xcount, ycount;

	// Draw the corners
	skinImage[0].Render(m_XPos, m_YPos);                                  // top left corner
	skinImage[2].Render(m_XPos + m_Width - 32, m_YPos);                   // top right corner
	skinImage[6].Render(m_XPos, m_YPos + m_Height - 32);                  // bottom left corner
	skinImage[8].Render(m_XPos + m_Width - 32, m_YPos + m_Height - 32);   // bottom left corner

	// Fill the window
	xcount = (m_Width - 128 - 32) / 8;
	ycount = (m_Height - 64 - 32) / 8;

	// Top, bottom and middle fill
	for(i = 0; i < xcount + 2; i++)
	{
		skinImage[1].Render(m_XPos + 128 + i * 8 - 4, m_YPos);
		skinImage[7].Render(m_XPos + 128 + i * 8 - 4, m_YPos + m_Height - 32);

		for(j = 0; j < ycount + 1; j++)
			skinImage[4].Render(m_XPos + 128 + i * 8, m_YPos + 64 + j * 8 - 4);
	}

	// Left and right fill
	for(i = 0; i < ycount + 1; i++)
	{
		skinImage[3].Render(m_XPos, m_YPos + 64 + i * 8 - 4);
		skinImage[5].Render(m_XPos + m_Width - 32, m_YPos + 64 + i * 8 - 4);
	}
}
