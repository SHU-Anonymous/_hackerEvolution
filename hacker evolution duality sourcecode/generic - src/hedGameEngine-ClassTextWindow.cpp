/*
Name: hedGameEngine-ClassTextWindow.h
Desc: Hacker Evolution Duality - Gameplay: text window class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#include "../generic - inc/hedGameEngine-ClassTextWindow.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include "../generic - inc/hedSound.h"
#include "../generic - inc/hedSystem.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

//=======================================================================================================================================
// Constructor
hedClassTextWindow::hedClassTextWindow()
{
    int i,j;
	buttonMouseOverLast = -1;

    for(i = 0; i < 1024; i++)
        for(j = 0; j < HED_WINDOW_TEXTLINES; j++)
            m_windowText[i][j] = 0;
}

//=======================================================================================================================================
// Destructor
hedClassTextWindow::~hedClassTextWindow()
{
	// This was causing a crash on exit
	// HED_SAFE_DELETE(m_bgImage);
}

//=======================================================================================================================================
// Text offsets for rendering
void hedClassTextWindow::SetTextOffsets(int xoffs, int yoffs)
{
	m_xOffs = xoffs;
	m_yOffs = yoffs;
}
//=======================================================================================================================================
// Font color used in this window
void hedClassTextWindow::SetTextColor(float r, float g, float b)
{
	m_r = r;
	m_g = g;
	m_b = b;
}

//=======================================================================================================================================
// Initialize the text window
void hedClassTextWindow::Init(int xpos, int ypos, int width, int height, char* bgImage)
{
	int i;
	
	m_xPos = 0;
	m_yPos = 0;
	m_textWidth  = width / 7;
	if(bgImage == NULL)
		m_textHeight = (height / 15);
	else
		m_textHeight = (height / 15) - 3;
	m_DisplayX   = xpos;
	m_DisplayY   = ypos;
	m_xOffs      = 20;
	m_yOffs      = 35;
	m_Width      = width;
	m_Height     = height;
	
	m_r = 0.8f;
	m_g = 0.8f;
	m_b = 0.8f;
		
	for(i = 0; i < HED_WINDOW_MAXSUBIMAGES; i++)
	{
        attachedImageVisible[i] = 0;
		attachedImage[i] = NULL;
		sliderLength[i]  = -1;
		button[i]        = -1;
		buttonClicked[i] = 0;
		buttonWidth[i]   = 100;
		toggleStatus[i]  = -1;
	}

	m_FontHeight = 15;	

	if(bgImage)
	{
		m_bgImagePresent = 1;
		m_bgImage        = new hedImage;
		m_bgImage->LoadFromFile(bgImage);

		m_bgImage->m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA;
		m_bgImage->m_AnimScaleSpeed = -ANIM_SPEED_SCALE * 2;

		// This is to avoid the window briefly popping up before it's marked hidden (all windows are created hidden)
		m_bgImage->m_AnimAlpha = 0.0f;
		m_bgImage->m_AnimScale = 0.0f;
	}
	else
		m_bgImagePresent = 0;

	m_Visible = 0;
	m_Font    = HED_FONT_NORMAL;

	// Initialize window elements
	m_InputSliderID    = -1;
	m_InputSliderLastX = -1;

	//
	strcpy(m_AnimTextBuffer,"");
	m_AnimTextTimer      = 0.0f;
	m_AnimTextTimerSound = 0;
}

//=======================================================================================================================================
// Set a pre-loaded background image
void hedClassTextWindow::SetBackgroundImage(hedImage* image)
{
	m_bgImage = image;
	m_bgImagePresent = 1;

	// We must recalculate the text widths
	m_Width      = image->m_pWidth;
	m_Height     = image->m_pHeight;
	m_textWidth  = (m_Width - 40) / 7;
	m_textHeight = (m_Height / 15) - 4;

	//
	m_bgImage->m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA;
	m_bgImage->m_AnimScaleSpeed = -ANIM_SPEED_SCALE * 2;

	// This is to avoid the window briefly popping up before it's marked hidden (all windows are created hidden)
	m_bgImage->m_AnimAlpha = 0.0f;
	m_bgImage->m_AnimScale = 0.0f;
}

//=======================================================================================================================================
// Print text inside the window
void hedClassTextWindow::PrintText(char* fmt,...)
{
    char     buffer[1024];
    va_list  param;
    int      x,y,i = 0;
    int      delta = 0;

    // print formated text to buffer 
    va_start(param,fmt);
    vsprintf(buffer,fmt,param);
    va_end(param);

    // Allow more characters on a line if there is a color code at the begining
    if(m_windowText[0][m_yPos] == '/' && m_windowText[1][m_yPos] == 'c') delta = 3;
    if(buffer[0] == '/' && buffer[1] == 'c')                             delta = 3;

    // Insert text into console buffer 
    while(buffer[i] != 0 && i < strlen(buffer))
    {
        // put character into window buffer 
        switch(buffer[i])
        {
			// new line 
            case '\n':
				m_windowText[m_xPos][m_yPos] = 0;
                m_xPos = 0; m_yPos++;
                break;

			// begining of line 
            case '\r':        
                // clear until end of line 
                for(x = 0; x < m_textWidth + delta;x++)
                    m_windowText[x][m_yPos] = 0;
                m_xPos = 0;
                break;

            default:
                m_windowText[m_xPos][m_yPos] = buffer[i];
                m_xPos++;
                break; // LCA
        }

        // end of line 
        if(m_xPos > m_textWidth + delta - 1)
        {
            for(x = m_xPos; x < 1024; x++) m_windowText[x][m_yPos] = 0;
 
            if(buffer[i + 1] == '\n') 
            {
                buffer[i + 1] = 0; // Avoid jumping to a new line twice
			    m_windowText[m_xPos][m_yPos] = 0;
                m_xPos = 0; m_yPos++;
            }
            else
            {
			    m_windowText[m_xPos][m_yPos] = 0;
                m_xPos = 0; m_yPos++;
            }
        }

        // page is full, scroll up 
        //if(m_yPos > HED_WINDOW_TEXTLINES - 1)
        if(m_yPos > m_textHeight - 1)
        {
            m_yPos--;
            
            for(y = 0; y < m_textHeight - 1; y++)
                for(x = 0; x < 1024; x++)
                m_windowText[x][y] = m_windowText[x][y+1];
            
            for(x = 0; x < 1024; x++)
                m_windowText[x][m_textHeight - 1] = 0;
        }

        // next character 
        i++;
    }
	m_windowText[m_xPos][m_yPos] = 0;
}

//=======================================================================================================================================
// Print text inside the window (and insert a new line automatically after it)
void hedClassTextWindow::PrintTextN(char* fmt,...)
{
    char     buffer[1024];
    va_list  param;

    // print formated text to buffer 
    va_start(param,fmt);
    vsprintf(buffer,fmt,param);
    va_end(param);

	this->PrintText("%s\n",buffer);
}

//=======================================================================================================================================
// Display the contents of a text file inside the window
void hedClassTextWindow::DisplayFile(char* fileName)
{
	FILE* f;
	char  line[1000];

	f = fopen(fileName,"rt");

	if(f == 0)
	{
		PrintText("File not found : %s",fileName);
		return;
	}

	while(fgets(line,1000,f))
    {
        utilStripNewLine(line);
		PrintText("%s\n",line);
    }
	fclose(f);

}
//=======================================================================================================================================
// Render the text window
void hedClassTextWindow::Render()                                                 
{
    int i,j;
	char line[1000];

	// Update visibility
	if(m_bgImagePresent == 1)
	{
		if(m_bgImage->m_AnimAlpha == 0.0f && m_bgImage->m_AnimScale == 0.0f)
			m_Visible = 0;

		if(m_bgImage->m_AnimAlpha == 1.0f && m_bgImage->m_AnimScale == 1.0f)
			m_Visible = 1;
		
		if(m_bgImage->m_AnimAlpha > 0.0f && m_bgImage->m_AnimScale > 0.0f)
			m_bgImage->Render(m_DisplayX, m_DisplayY);

		// This is here to keep rendering a window that is fading out
		if(m_bgImage->m_AnimAlpha > 0.0f && m_bgImage->m_AnimAlphaSpeed < 0.0f)
			return;
	}

	// Skip rendering if the window is hidden
	if(m_Visible == 0)
		return;

	// Push animated characters
	Anim_PushCharacters();

	// Render background image (if present)
	if(m_bgImagePresent == 1)
	{
		//m_bgImage->Render(m_DisplayX, m_DisplayY);
		
		// Finished fading out?
		if(m_bgImage->m_AnimAlpha <= 0.0f)
			m_Visible = 0;

		// Skip rendering text, if the window is not fully visible
		if(m_bgImage->m_AnimAlpha != 1.0f)
			return;
	}

	// Render attached images
	for(i = 0; i < HED_WINDOW_MAXSUBIMAGES; i++)
		if(attachedImageVisible[i] != 0)
		{
			// Check if it's a slider image
			if(sliderLength[i] != -1)
				attachedImage[i]->Render(m_DisplayX + attachedImageX[i] + sliderPosition[i],m_DisplayY + attachedImageY[i]);
			else
				attachedImage[i]->Render(m_DisplayX + attachedImageX[i],m_DisplayY + attachedImageY[i]);
		}

	// Render text content 
    int starty = m_yPos - m_textHeight;
    int endy   = m_yPos;

    if(starty < 0)
    {
        endy -= starty;
        starty = 0;
    }

    for(i = starty; i <= endy; i++)
	{
        j = 0;
        while(m_windowText[j][i] != 0) {line[j] = m_windowText[j][i]; j++;} line[j] = 0;

        if(line[0] != 0)
        {
            float r,g,b;
            int   index = 0;

            r = m_r; g = m_g; b = m_b;

            // Color coding
            if(line[0] == '/' && line[1] == 'c' && line[2] == '1') {r = 0.67f; g = 0.87f; b = 0.003f; index = 3;}
            if(line[0] == '/' && line[1] == 'c' && line[2] == '2') {r = 0.8f; g = 0.0f; b = 0.0f; index = 3;}

			g_RenderEngine.RenderText(m_DisplayX + m_xOffs, m_DisplayY + m_yOffs + i * m_FontHeight, r, g, b, m_Font,&line[index]);
        }
	}

}

//=======================================================================================================================================
// Show/Hide window
void hedClassTextWindow::ToggleVisibility()
{
	// Wait until all animated texts are displayed.
	if(strlen(m_AnimTextBuffer) > 0)
		return;

	if(m_bgImagePresent)
	{
		m_bgImage->m_AnimAlphaSpeed = -m_bgImage->m_AnimAlphaSpeed;
		m_bgImage->m_AnimScaleSpeed = -m_bgImage->m_AnimScaleSpeed;

		// Force an animation frame
		m_bgImage->m_AnimAlpha += g_FrameTime * m_bgImage->m_AnimAlphaSpeed;
		m_bgImage->m_AnimScale += g_FrameTime * m_bgImage->m_AnimScaleSpeed;
	}
	else
	{
		// No background image, just toggle
		if(m_Visible == 1)
			m_Visible = 0;
		else
			m_Visible = 1;
	}
}

//=======================================================================================================================================
// Forces the window to become invisible, skipping the entire animation sequences
void hedClassTextWindow::ForceHide()
{
	m_Visible = 0;

	if(m_bgImagePresent == 1)
	{
		m_bgImage->m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA;
		m_bgImage->m_AnimScaleSpeed = -ANIM_SPEED_SCALE * 2;
		m_bgImage->m_AnimAlpha = 0.0f;
		m_bgImage->m_AnimScale = 0.0f;
	}
}

//=======================================================================================================================================
// Forces the window to become visible, skipping the entire animation sequences
void hedClassTextWindow::ForceVisible()
{
	m_Visible = 1;

	if(m_bgImagePresent == 1)
	{
		m_bgImage->m_AnimAlphaSpeed = ANIM_SPEED_ALPHA;
		m_bgImage->m_AnimScaleSpeed = ANIM_SPEED_SCALE * 2;
		m_bgImage->m_AnimAlpha = 1.0f;
		m_bgImage->m_AnimScale = 1.0f;
	}
}
//=======================================================================================================================================
// Clear the contents of the window
void hedClassTextWindow::Clear()
{
	int i,j;

    m_xPos = 0; 
    m_yPos = 0;

	for(i = 0; i < 256; i++)
		for(j = 0; j < HED_WINDOW_TEXTLINES; j++)
			m_windowText[i][j] = 0;
}

//=======================================================================================================================================
// Checks if the window is visible
int hedClassTextWindow::isVisible()
{
	// Update visibility
	if(m_bgImagePresent)
	{
		if(m_bgImage->m_AnimAlpha == 0.0f && m_bgImage->m_AnimScale == 0.0f)
			return 0;

		if(m_bgImage->m_AnimAlpha == 1.0f && m_bgImage->m_AnimScale == 1.0f)
			return 1;
		
		if(m_bgImage->m_AnimAlpha > 0.0f && m_bgImage->m_AnimScale > 0.0f)
			return 0;

		// This is here to keep rendering a window that is fading out
		if(m_bgImage->m_AnimAlpha > 0.0f && m_bgImage->m_AnimAlphaSpeed < 0.0f)
			return 0;
	}

	return m_Visible;
}

//=======================================================================================================================================
// Draw a rectangle
void hedClassTextWindow::DrawRectangleFill(int x1, int y1, int x2, int y2, float r, float g, float b, float a)
{
	// Skip rendering if the window is hidden
	if(isVisible() == 0)
		return;

	g_RenderEngine.DrawRectangleFill(m_DisplayX + x1, m_DisplayY + y1,m_DisplayX + x2,m_DisplayY + y2,r,g,b,a);
}

//=======================================================================================================================================
// Draw a line (antialiased with a texture)
void hedClassTextWindow::DrawLine(int x1, int y1, int x2, int y2, float r, float g, float b, int width)
{
	// Skip rendering if the window is hidden
	if(isVisible() == 0)
		return;

	g_RenderEngine.DrawLineAA(m_DisplayX + x1, m_DisplayY + y1,m_DisplayX + x2,m_DisplayY + y2,r,g,b,width);
}

//=======================================================================================================================================
// Render a formated text line
void hedClassTextWindow::RenderText(int x, int y, float r, float g, float b, int fontSize, const char *fmt, ...)
{
	char	text[4096];								
	va_list	ap;
	int     i;
	float   xpos, ypos;
	float   cwidth,cheight;  // character width and height in pixels
	float   deltau;
	int     fontIndex;

	// Skip rendering if the window is hidden
	if(isVisible() == 0)
		return;

	// print the formatted text to a string
	va_start(ap, fmt);									
    vsprintf(text, fmt, ap);						
	va_end(ap);											

	g_RenderEngine.RenderText(m_DisplayX + x, m_DisplayY + y,r,g,b,fontSize,text);
}

//=======================================================================================================================================
// This will display text, with a "typing effect"
void hedClassTextWindow::Anim_DisplayText(char* fmt,...)
{
    char     buffer[1024];
    va_list  param;

    // print formated text to buffer 
    va_start(param,fmt);
    vsprintf(buffer,fmt,param);
    va_end(param);

	strcat(m_AnimTextBuffer,buffer);
}

//=======================================================================================================================================
// This will display a file, with a "typing effect"
void hedClassTextWindow::Anim_DisplayFile(char* fileName)
{
	FILE* f;
	char  line[1000];

	f = fopen(fileName,"rt");

	if(f == 0)
	{
		("File not found : %s",fileName);
		return;
	}

	while(fgets(line,1000,f))
    {
        utilStripNewLine(line);
		strcat(m_AnimTextBuffer,line); strcat(m_AnimTextBuffer,"\n");
    }
	fclose(f);
}

//=======================================================================================================================================
// Push a character to the window
void hedClassTextWindow::Anim_PushCharacters()
{
	int i, length;

	m_AnimTextTimer += g_FrameTime;

	if(m_AnimTextTimer > HED_ANIMATED_TEXT_SPEED)
	{
		// Update timer
		m_AnimTextTimer -= HED_ANIMATED_TEXT_SPEED;

		// See if there are any character left to be pushed
		if(strlen(m_AnimTextBuffer) > 0)
		{
            // Color codes
            if(m_AnimTextBuffer[0] == '/' && m_AnimTextBuffer[1] == 'c')
            {
			    PrintText("%c",m_AnimTextBuffer[0]);
			    PrintText("%c",m_AnimTextBuffer[1]);
			    PrintText("%c",m_AnimTextBuffer[2]);
			    PrintText("%c",m_AnimTextBuffer[3]);

    			length = strlen(m_AnimTextBuffer);
	    		for(i = 0; i < length - 4; i++)
		    		m_AnimTextBuffer[i] = m_AnimTextBuffer[i + 4];
			    m_AnimTextBuffer[length - 4] = 0;
            }else
            {
			    PrintText("%c",m_AnimTextBuffer[0]);

                length = strlen(m_AnimTextBuffer);
	    		for(i = 0; i < length - 1; i++)
		    		m_AnimTextBuffer[i] = m_AnimTextBuffer[i + 1];

			    m_AnimTextBuffer[length - 1] = 0;
            }


			// Sound effect
			m_AnimTextTimerSound++;
			if(m_AnimTextTimerSound > HED_ANIMATED_TEXT_SOUND)
			{
				m_AnimTextTimerSound -= HED_ANIMATED_TEXT_SOUND;
				g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-typetick.wav");
			}
		}

	}
}

//=======================================================================================================================================
// Process input
void hedClassTextWindow::ProcessInput(int xpos, int ypos, int lbClick, int lbClickDown, int key)
{
	int i;
	int deltax, x, y;

	// Process sliders
	if(lbClickDown == 0) m_InputSliderID = -1;

	if(m_InputSliderID != -1)
	{
		deltax = (xpos - m_InputSliderLastX);
		m_InputSliderLastX = xpos;

		sliderPosition[m_InputSliderID] += deltax;
		if(sliderPosition[m_InputSliderID] < 0) sliderPosition[m_InputSliderID] = 0;
		if(sliderPosition[m_InputSliderID] > sliderLength[m_InputSliderID]) sliderPosition[m_InputSliderID] = sliderLength[m_InputSliderID];
	}

	for(i = 0; i < HED_WINDOW_MAXSUBIMAGES; i++)
	{
		if(lbClickDown == 1 && m_InputSliderID == -1 && sliderLength[i] != -1)
		{
			x = m_DisplayX + attachedImageX[i] + sliderPosition[i];
			y = m_DisplayY + attachedImageY[i];

			if(xpos > x && ypos > y && xpos < x + 16 && ypos < y + 16)
			{
				m_InputSliderID = i;
				m_InputSliderLastX = xpos;
			}
		}
	}

	// Process buttons
	for(i = 0; i < HED_WINDOW_MAXSUBIMAGES; i++)
	{
		// Mouse over?
		if(button[i] != -1)
		{
			// Overlay image on/off
			if(xpos > attachedImageX[i] + m_DisplayX && xpos < attachedImageX[i] + m_DisplayX + buttonWidth[i] &&
               ypos > attachedImageY[i] + m_DisplayY && ypos < attachedImageY[i] + m_DisplayY + 26)
			{
			   attachedImageVisible[button[i]] = 1;

			   // Mouse over sound effect
			   if(buttonMouseOverLast != button[i])
			   {
				   g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");
				   buttonMouseOverLast = button[i];
			   }
			}
			else
			   attachedImageVisible[button[i]] = 0;

			// Button clicked?
			if(lbClick == 1 && attachedImageVisible[button[i]] == 1)
				buttonClicked[i] = 1;
		}

		// ON/OFF toggles
		if(toggleStatus[i] != -1 && lbClick == 1 &&
		   xpos > attachedImageX[i] + m_DisplayX && xpos < attachedImageX[i] + m_DisplayX + 590 &&
           ypos > attachedImageY[i] + m_DisplayY && ypos < attachedImageY[i] + m_DisplayY + 15)
		{
			TOGGLE(toggleStatus[i]);
			TOGGLE(attachedImageVisible[i]);
			TOGGLE(attachedImageVisible[i + 1]);
			g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");
		}
	}
}

//=======================================================================================================================================
// Check if a button was clicked
int hedClassTextWindow::ButtonClicked(int imageID)
{
	if(buttonClicked[imageID] == 1)
	{
		buttonClicked[imageID] = 0;
		return 1;
	}

	return 0;
}

//=======================================================================================================================================
// Attach an image to the window
void hedClassTextWindow::AttachImage(int imageID, hedImage* image, int x, int y, int visible)              
{
	attachedImage[imageID]        = image;
	attachedImageVisible[imageID] = visible;
	attachedImageX[imageID]       = x;
	attachedImageY[imageID]       = y;
}

//=======================================================================================================================================
// Create a slider bar using one of the attached images (imageID);
void hedClassTextWindow::CreateSlider(int imageID, int length, int position)
{
	sliderLength[imageID]   = length;
	sliderPosition[imageID] = position;
}

//=======================================================================================================================================
// Create a button from 2 images
void hedClassTextWindow::CreateButton(int imageID, int imageIDOver)
{
	button[imageID] = imageIDOver;
}

//=======================================================================================================================================
// Create a toggle
void hedClassTextWindow::CreateToggle(int imageID, int status)
{
	toggleStatus[imageID] = status;

	if(status == 0)
	{
		attachedImageVisible[imageID] = 1;
		attachedImageVisible[imageID + 1] = 0;
	}
	else
	{
		attachedImageVisible[imageID] = 0;
		attachedImageVisible[imageID + 1] = 1;
	}
}
