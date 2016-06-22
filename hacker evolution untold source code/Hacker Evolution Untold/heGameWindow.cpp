// heGameWindow.cpp
// 
// Game window management class
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "heGameWindow.h"
#include "moduleRender.h"
#include "heLog.h"

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class constructor
heGameWindow::heGameWindow()
{
	// * Vertical scroll position
	mScrollPos = 0;

	// * Clear text contents
	this->clear();

	// * Initializations
	mHasBackgroundImage = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class destructor
heGameWindow::~heGameWindow()
{
}
// ============================================================================================================================
// Create window
void heGameWindow::Create(int x, int y, int width, int height, char* imagePath, int pCaption)
{
	// save window coordinates and sizes
	m_x = x; m_y = y; m_width = width; m_height = height;

	// load backgroundimage, if any
	if(imagePath != 0)
	{
		mHasBackgroundImage = 1;

		gApplicationLog.printf("\nLoading bg image.... %s",imagePath);
		mBackgroundImage.LoadFromFile(imagePath,width,height);
		gApplicationLog.printf("\nheGameWindow::create() - loaded background image : %s",imagePath);
	}

	// text specific stuff
    //m_textWidth  = (width  / 6);
	m_textWidth  = 190;
    m_textHeight = (height / FONT_SIZE) - 3;

	// * This is for smaller windows
	if(m_textHeight < 5) m_textHeight = 5;

	x_ofs = m_x + HE_WINDOW_IHOFFS; 
	y_ofs = m_y + HE_WINDOW_IVOFFS;

	// * Check if the window has caption space
	if(pCaption == 0) y_ofs = m_y + 15;
	

	gApplicationLog.printf("\nheGameWindow::create() - text area : %d x %d",m_textWidth, m_textHeight);
}
// ============================================================================================================================
// Draw window
void heGameWindow::Draw()
{
	// draw background image, if any
	if(mHasBackgroundImage)
	{
		mBackgroundImage.Draw(m_x,m_y);
	}
}
// ============================================================================================================================
// renders the text content of the window
void heGameWindow::renderTextContent(int xoffs)
{
	// render text content 
    int i,j;
	char line[1000];

	// * if we have an animation modifier running, don't render the text
	if (mAnimationModifier != 0)
		if (mAnimationModifier->mStarted == 1) return;

    int starty = m_yPos - m_textHeight + 2 - mScrollPos;
    int endy   = m_yPos - mScrollPos;

    if(starty < 0)
    {
        endy -= starty;
        starty = 0;
    }

    for(i = starty; i <= endy; i++)
	{
        for(j = 0; j < m_textWidth; j++)
        {
            line[j] = m_windowText[j][i];
        }
        if(line[0] != 0)
		    render_DrawText(xoffs + x_ofs ,y_ofs + (i - starty) * FONT_SIZE,line,SKINCOLOR(gSkinColorWindowText));
	}
}
// ============================================================================================================================
// prints a line of text in the window
void heGameWindow::printf(char* fmt,...)
{
    char     buffer[1024];
    va_list  param;
    int      x,y;
	int      i = 0;

    // printf formated text to buffer 
    va_start(param,fmt);
    vsprintf(buffer,fmt,param);
    va_end(param);

    // Insert text into console buffer 
    while(buffer[i])
    {
        // end of line 
        if(m_xPos >= m_textWidth)
        {
			m_windowText[m_xPos][m_yPos] = 0;
            m_xPos = 0; m_yPos++;
        }

        // page is full, scroll up 
        if(m_yPos > HE_WINDOW_TEXTLINES - 1)
        {
            m_yPos--;
            //
            for(y = 0; y < m_textHeight - 1; y++)
                for(x = 0; x < m_textWidth; x++)
                m_windowText[x][y] = m_windowText[x][y+1];
            
            for(y = 0; y < HE_WINDOW_TEXTLINES - 1; y++)
                for(x = 0; x < m_textWidth; x++)
                m_windowText[x][y] = m_windowText[x][y+1];


            for(x = 0; x < m_textWidth; x++)
                m_windowText[x][m_textHeight] = 0;
        }

        // put character into window buffer 
        switch(buffer[i])
        {
			// new line 
            case '\n':
                m_xPos = 0; m_yPos++;
                break;

			// begining of line 
            case '\r':          
                // clear until end of line 
                for(x = 0; x < m_textWidth;x++)
                    m_windowText[x][m_yPos] = 0;
                m_xPos = 0;
                break;

            default:
                m_windowText[m_xPos][m_yPos] = buffer[i];
                m_xPos++;
        }

        // next character 
        i++;
    }
	m_windowText[m_xPos][m_yPos] = 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
// prints a line of text in the window (on a new line)
void heGameWindow::printfn(char* fmt,...)
{
    char     buffer[1024];
    va_list  param;
    int      x,y;
	int      i = 0;

	this->printf("\n");

    // printf formated text to buffer 
    va_start(param,fmt);
    vsprintf(buffer,fmt,param);
    va_end(param);

    // Insert text into console buffer 
    while(buffer[i])
    {
        // end of line 
        if(m_xPos >= m_textWidth)
        {
			m_windowText[m_xPos][m_yPos] = 0;
            m_xPos = 0; m_yPos++;
        }

        // page is full, scroll up 
        if(m_yPos > HE_WINDOW_TEXTLINES - 1)
        {
            m_yPos--;
            //
            for(y = 0; y < m_textHeight - 1; y++)
                for(x = 0; x < m_textWidth; x++)
                m_windowText[x][y] = m_windowText[x][y+1];
            
            for(y = 0; y < HE_WINDOW_TEXTLINES - 1; y++)
                for(x = 0; x < m_textWidth; x++)
                m_windowText[x][y] = m_windowText[x][y+1];


            for(x = 0; x < m_textWidth; x++)
                m_windowText[x][m_textHeight] = 0;
        }

        // put character into window buffer 
        switch(buffer[i])
        {
			// new line 
            case '\n':
                m_xPos = 0; m_yPos++;
                break;

			// begining of line 
            case '\r':          
                // clear until end of line 
                for(x = 0; x < m_textWidth;x++)
                    m_windowText[x][m_yPos] = 0;
                m_xPos = 0;
                break;

            default:
                m_windowText[m_xPos][m_yPos] = buffer[i];
                m_xPos++;
        }

        // next character 
        i++;
    }
	m_windowText[m_xPos][m_yPos] = 0;
}
// ============================================================================================================================
// Clear window contents
void heGameWindow::clear()
{
    m_xPos = 0; 
    m_yPos = 0;
	m_windowText[0][0] = 0;

	/* Clear text buffer */
    /*
	//for(int i = 0; i < 200; i++)
		for(int j = 0; j < HE_WINDOW_TEXTLINES; j++)
			//m_windowText[i][j] = 0;
            m_windowText[0][j] = 0;
    */
	/* Clear text buffer */
	for(int i = 0; i < 200; i++)
		for(int j = 0; j < HE_WINDOW_TEXTLINES; j++)
			m_windowText[i][j] = 0;

    //ZeroMemory(m_windowText, 200 * HE_WINDOW_TEXTLINES);

	mScrollPos = 0;
}

// ============================================================================================================================
// Display the contents of a file in the window
void heGameWindow::displayFile(char* fileName, int clear)
{
	FILE* f;
	char  line[1000];

	if(clear == 1) this->clear();

	f = fopen(fileName,"rt");

	if(f == 0)
	{
		gApplicationLog.printf("\nheGameWindow::displayFile() - failed to open : %s",fileName);
		return;
	}

	while(fgets(line,1000,f))
    {
        //2014 update if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = 0;
        stripNewLine(line);
		this->printf("%s\n",line);
    }
	fclose(f);
}

// ============================================================================================================================
// prints a line of text anywhere in the window, with a given color
void heGameWindow::printText(int x, int y, int r, int g, int b, char* fmt, ...)
{
    char     buffer[1024];
    va_list  param;

    // printf formated text to buffer 
    va_start(param,fmt);
    vsprintf(buffer,fmt,param);
    va_end(param);

	render_DrawText(this->m_x + x, this->m_y + y,buffer,r,g,b);
}

// ============================================================================================================================
// draws a line anywhere in the window, with a given color
void heGameWindow::drawLine(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	render_DrawLine(x1 + m_x, y1 + m_y, x2 + m_x, y2 + m_y, r, g, b);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Draws a progress bar
void heGameWindow::drawProgressBar(int x, int y, int width, int r, int g, int b)
{
	int i;

	
	for(i = 0; i < width; i++)
		if(i % 2 == 0)
			drawLine(x + i,y + 1,x + i,y + 18,r,g,b);	

	/* Original Hacker Evolution style
	for(i = 0; i < width; i++)
	{
		if(i % 10 == 0)
			drawLine(x + i,y + 1,x + i,y + 18,r,g,b);
		else
			drawLine(x + i,y,x + i,y + 19,r,g,b);
	}
	*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Attach animation modifier to the background image
void heGameWindow::AttachAnimationModifier(CAnimationModifier* pAnimationModifier)
{
		mBackgroundImage.AttachAnimationModifier(pAnimationModifier);
		mBackgroundImage.mAnimationModifier->Reset();		

		mAnimationModifier = pAnimationModifier;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Scroll window text up
void heGameWindow::scrollUp()
{
    mScrollPos += HE_WINDOW_SCROLLSIZE;
    if(mScrollPos > m_yPos - m_textHeight + 2)
        mScrollPos = m_yPos - m_textHeight + 2;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Scroll window text down
void heGameWindow::scrollDown()
{
    mScrollPos -= HE_WINDOW_SCROLLSIZE;
    if(mScrollPos < 0) mScrollPos = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * By a given percentage
void heGameWindow::scroll(int percent)
{
    //mScrollPos += (percent * (m_yPos - m_textHeight + 2)) / 100;
    mScrollPos = (percent * (m_yPos - m_textHeight + 2)) / 100;

    if(mScrollPos < 0) mScrollPos = 0;
    if(mScrollPos > m_yPos - m_textHeight + 2)
        mScrollPos = m_yPos - m_textHeight + 2;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Reset window scroll position
void heGameWindow::scrollReset()
{
	mScrollPos = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Return the scroll percentage
int heGameWindow::getScrollPercent()
{
	int total,percent,ret;

	total   = m_yPos - m_textHeight + 2;
	percent = total - mScrollPos;

	if(total == 0) return 100;

	ret = (percent * 100) / total;

	return ret;
}	

