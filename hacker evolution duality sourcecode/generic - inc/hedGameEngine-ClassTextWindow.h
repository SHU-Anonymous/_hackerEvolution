/*
Name: hedGameEngine-ClassTextWindow.h
Desc: Hacker Evolution Duality - Gameplay: text window class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#ifndef _hedGameEngine_ClassTextWindow_h_
#define _hedGameEngine_ClassTextWindow_h_

#include "hedGraphicsImage.h"

//=======================================================================================================================================
// Constants and defines
#define HED_WINDOW_TEXTLINES    1024
#define HED_WINDOW_MAXSUBIMAGES 64

//#define HED_ANIMATED_TEXT_SPEED 1/50.0f  // "Typing effect" text speed - 1/chars_per_sec
//#define HED_ANIMATED_TEXT_SOUND 13       // "Tick" sound at every <x> characters

#define HED_ANIMATED_TEXT_SPEED 1/66.0f  // "Typing effect" text speed - 1/chars_per_sec
#define HED_ANIMATED_TEXT_SOUND 13        // "Tick" sound at every <x> characters

// Shortcuts
#define CREATE_BUTTON(window,id,image,x,y) \
	window.AttachImage(id,image[0],x,y,1); \
	window.AttachImage(id + 1,image[1],x,y,0); \
	window.CreateButton(id,id + 1);


//=======================================================================================================================================
// Class definition
class hedClassTextWindow
{
public:
	// Constructor / Destructor
	hedClassTextWindow();
	~hedClassTextWindow();

	void Init(int xpos, int ypos, int width, int height, char* bgImage = 0);  // Initialize the text window
	void SetBackgroundImage(hedImage* image);                                 // Set a pre-loaded background image
	void SetTextOffsets(int xoffs, int yoffs);                                // Text offsets for rendering
	void SetTextColor(float r, float g, float b);                             // Font color used in this window
	void PrintText(char* fmt,...);                                            // Print text inside the window
	void PrintTextN(char* fmt,...);                                           // Print text inside the window (and insert a new line automatically after it)
	void DisplayFile(char* fileName);                                         // Display the contents of a text file inside the window
	void Render();                                                            // Render the text window

	void ToggleVisibility();                                                  // Show/Hide window
	void ForceHide();                                                         // Forces the window to become invisible, skipping the entire animation sequences
	void ForceVisible();                                                      // Forces the window to become visible, skipping the entire animation sequences
	void Clear();                                                             // Clear the contents of the window
	int  isVisible();                                                         // Checks if the window is visible

	// Rendering functions (inside the window)
	void DrawLine(int x1, int y1, int x2, int y2, float r, float g, float b, int width = 1);           // Draw a line (antialiased with a texture)
	void DrawRectangleFill(int x1, int y1, int x2, int y2, float r, float g, float b, float a = 1.0f); // Draw a rectangle
	void RenderText(int x, int y, float r, float g, float b, int fontSize, const char *fmt, ...);      // Render a formated text line
	
	void Anim_DisplayText(char* fmt,...);                                     // This will display text, with a "typing effect"
	void Anim_DisplayFile(char* fileName);                                    // This will display a file, with a "typing effect"
	void Anim_PushCharacters();                                               // Push animated characters to the window

	// Window elements
	void ProcessInput(int xpos, int ypos, int lbClick, int lbClickDown, int key);
	int  ButtonClicked(int imageID);                                            // Check if a button was clicked
	void AttachImage(int imageID, hedImage* image, int x, int y, int visible);  // Attach an image to the window
	void CreateSlider(int imageID, int length, int position);                   // Create a slider bar using one of the attached images (imageID);
	void CreateButton(int imageID, int imageIDOver);                            // Create a button from 2 images
	void CreateToggle(int imageID, int status);                                 // Create and ON/OFF toggle

	// Convert screen mouse coordinates, to coordinates relative to the window
	int  mouseToWindowX(int x){return (x - m_DisplayX);};
	int  mouseToWindowY(int y){return (y - m_DisplayY);};

public:
	// Public variables
	int   m_Visible;                                   // Is the window visible?
    int   m_xPos,m_yPos;                               // Current cursor position inside the window
	int   m_DisplayX, m_DisplayY;                      // Display coordinates (where the window is rendered)
	int   m_textWidth, m_textHeight;                   // Number of characters that can be displayed inside the window 

	// This are images that can be attached to a window and drawn on top of it
	// This is usefull to create complex windows with buttons,etc
	hedImage* attachedImage[HED_WINDOW_MAXSUBIMAGES];         // Pointers to images
	int       attachedImageX[HED_WINDOW_MAXSUBIMAGES];        // Coordinates
	int       attachedImageY[HED_WINDOW_MAXSUBIMAGES];
	int       attachedImageVisible[HED_WINDOW_MAXSUBIMAGES];  // Visible or not

	// Sliders
	int       sliderLength[HED_WINDOW_MAXSUBIMAGES];
	int       sliderPosition[HED_WINDOW_MAXSUBIMAGES];
	int       m_InputSliderID, m_InputSliderLastX;

	// Buttons
	int       button[HED_WINDOW_MAXSUBIMAGES];
	int       buttonClicked[HED_WINDOW_MAXSUBIMAGES];        
	int       buttonWidth[HED_WINDOW_MAXSUBIMAGES];
	int       buttonMouseOverLast;

	// ON/OFF toggle
	int       toggleStatus[HED_WINDOW_MAXSUBIMAGES];

	int       m_Font;                                         // Font used in this window
	int       m_FontHeight;                                   // Font height

	// Background image
	hedImage* m_bgImage;                               // Background image
	int       m_bgImagePresent;       

    // "Typing text" rendering effect
	char  m_AnimTextBuffer[HED_WINDOW_TEXTLINES * 256];
	float m_AnimTextTimer;
	int   m_AnimTextTimerSound; 

//protected:
	char  m_windowText[1024][HED_WINDOW_TEXTLINES];    // Buffer to store characters displayed inside the window 
	int   m_Width, m_Height;                           // Window width and height
	int   m_xOffs,m_yOffs;                             // Text offsets for rendering
	float m_r,m_g,m_b;                                 // Font color used in this window
};

#endif