// heGameWindow.h
// 
// Game window management class
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _heGameWindow_h_
#define _heGameWindow_h_

#include "HackerEvolutionDefines.h"
#include "HackerEvolutionUntold_Defines.h"
#include "moduleRender.h"

// Game window management class
class heGameWindow
{
public:
	heGameWindow();
	~heGameWindow();

	void Create(int x, int y, int width, int height, char *imagepath = 0, int pCaption = 1);
	void Draw();

    void renderTextContent(int xoffs = 0);           // renders the text content of the window    
    void printf(char* fmt,...);                      // prints a line of text in the window
    void printfn(char* fmt,...);                     // prints a line of text in the window (on a new line)
	void clear();                                    // clear window contents
	void displayFile(char* fileName, int clear = 0); // display the contents of a file inside the window

	void printText(int x, int y, int r, int g, int b, char* fmt, ...);
	void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b);
	void drawProgressBar(int x, int y, int width, int r, int g, int b);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * Methods
	void AttachAnimationModifier(CAnimationModifier* pAnimationModifier);
	CAnimationModifier* mAnimationModifier;

	void scrollUp();
	void scrollDown();
	void scroll(int percent);
	void scrollReset();
	int  getScrollPercent();

protected:

	int m_x,m_y,m_width,m_height;

	// Window text management
    char  m_windowText[200][HE_WINDOW_TEXTLINES];  // Buffer to store characters displayed inside the window 
    int   m_textWidth, m_textHeight;               // Number of characters that can be displayed inside the window 
    int   m_xPos,m_yPos;                           // Current cursor position inside the window 

	int   x_ofs,y_ofs;                             // Offset for displaying text 

	// * protected variables
	heImage  mBackgroundImage;
	int      mHasBackgroundImage;
    int      mScrollPos;                           // vertical scroll position

};

#endif