/*
Name: hedRendererOpenGL.h
Desc: OpenGL rendering class

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _hedRendererOpenGL_h_
#define _hedRendererOpenGL_h_

#include "hedGraphicsImage.h"
#include "HackerEvolutionDualityUtil.h"

extern int gl_LastTextureID;
#define GL_DISABLE_TEXTURE glDisable(GL_TEXTURE_2D); gl_LastTextureID = -1;

//=======================================================================================================================================
// Font constants and definitions
#define HED_FONT_BIG10  0
#define HED_FONT_BIG12  1
#define HED_FONT_NORMAL 2

#define HED_FONT_COUNT 3

//=======================================================================================================================================
// Font class
class hedRendererFont
{
public:

	// Constructor / Destructor
	hedRendererFont();
	~hedRendererFont();

	// Functions
	void Load(char* fontTextureName, int fontSize);                                                 // Load font
	void BindTexture();                                                                             // Bind font texture
	int  TextWidth(char* text);                                                                     // Width (in pixels) of a given line of text

	// Public variables
	float     m_FontWidth[256];                                                                     // Font character widths
	hedVector m_FontCoord1[256],m_FontCoord2[256];                                                  // Font texture coordinates (lookup table)
	int       m_FontSize;                                                                           // Font size

	hedImage  m_FontImage;                                                                          // Font image (texture)
protected:

};

//=======================================================================================================================================
// Renderer class definition

class hedRendererOpenGL
{
public:
	// Constructor / Destructor
	hedRendererOpenGL();
	~hedRendererOpenGL();

	// Functions
	void Init();	
	void InitFontRenderer();                                                                            // Initialize OpenGL font renderer
	
	void RenderText(int x, int y, float r, float g, float b, int fontSize, const char *fmt, ...);       // Render a formated text line
	void RenderText2(int x, int y, float r, float g, float b, int fontSize, const char *fmt, ...);      // Render a formated text line (on black background)
	void DrawLineAlpha(int x1, int y1, int x2, int y2, float r, float g, float b, float a);             // Draw a transparent line
	void DrawDottedLine(int x1, int y1, int x2, int y2, float r, float g, float b);                     // Draw a dotted line
	void DrawRectangleFill(int x1, int y1, int x2, int y2, float r, float g, float b, float a = 1.0f);  // Draw a rectangle (filled with color)
	void DrawLineAA(int x1, int y1, int x2, int y2, float r, float g, float b, int width = 1);          // Draw a line (antialiased with a texture)
    void DrawAnimatedDotLine(int x1, int y1, int x2, int y2, float r, float g, float b, int skip = 1);  // Animated dot line

	int             m_OpenGLNonPow2Textures;                                                           // Does the driver support non power of 2 textures?
	float           m_FontAlpha;                                                                       // Font alpha 
	
	hedRendererFont m_Font[HED_FONT_COUNT];                                                            // Fonts used by the renderer

protected:

	hedVector m_AALineTextureCoord[8];                                                                 // Antialiased line texture coordinates (lookup table)
	hedImage  m_AALineTexture;                                                                         // Antialiased line texture image   

};

extern hedRendererOpenGL g_RenderEngine;            // Graphics rendering engine

#endif