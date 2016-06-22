/*
Name: hedRendererOpenGL.cpp
Desc: OpenGL rendering class

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include "../generic - inc/hedSystem.h"

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

int gl_LastTextureID = -1;
hedRendererOpenGL g_RenderEngine;            // Graphics rendering engine

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef PLATFORM_MACOS
#include <string.h>
#endif


#ifdef PLATFORM_LINUX
#include <string.h>
#endif

#ifdef PLATFORM_IOS
#import <OpenGLES/ES1/gl.h>
#include "string.h"

// OpenGL ES vertex buffers
float VB_Vertex[3 * 4096 + 100];
float VB_Colors[4 * 4096 + 100];
float VB_Texcoords[2 * 4096 + 100];

#endif

// Global variables
char*  g_OpenGLExtensions;  // OpenGL extensions
//	hedImage m_Font;                                                                               // Font image


//=======================================================================================================================================
// Class constructor
hedRendererOpenGL::hedRendererOpenGL()
{
	m_FontAlpha = 1.0f;
}

//=======================================================================================================================================
// Class destructor
hedRendererOpenGL::~hedRendererOpenGL()
{
	
}

//=======================================================================================================================================
// Initialize the renderer
void hedRendererOpenGL::Init()
{
	sysLogPrint("hedRendererOpenGL::Init() - Initializing renderer");
    
	// Initializations - antialiased line rendering
	m_AALineTextureCoord[0] = hedVector(0,0); 
	m_AALineTextureCoord[1] = hedVector(0,1); 
	m_AALineTextureCoord[2] = hedVector(0.5,0); 
	m_AALineTextureCoord[3] = hedVector(0.5,1); 
	m_AALineTextureCoord[4] = hedVector(0.5,0); 
	m_AALineTextureCoord[5] = hedVector(0.5,1); 
	m_AALineTextureCoord[6] = hedVector(1,0); 
	m_AALineTextureCoord[7] = hedVector(1,1); 

	m_AALineTexture.LoadFromFile("hed-graphics/res-aaline.png");
}

//=======================================================================================================================================
// Initialize OpenGL font renderer
void hedRendererOpenGL::InitFontRenderer()
{
	m_Font[HED_FONT_BIG10].Load("hed-graphics/fonts/hed-font-512-10-bg",10);
	m_Font[HED_FONT_BIG12].Load("hed-graphics/fonts/hed-font-512-12-bg",12);
	m_Font[HED_FONT_NORMAL].Load("hed-graphics/fonts/hed-font-interface-anonymouspro10bold",11);
}

//=======================================================================================================================================
// Render a formated text line
void hedRendererOpenGL::RenderText(int x, int y, float r, float g, float b, int fontSize, const char *fmt, ...)
{
	char	text[4096];								
	va_list	ap;
	int     i;
	float   xpos, ypos;
	float   cwidth,cheight;  // character width and height in pixels
	float   deltau;
	int     fontIndex;

	// print the formatted text to a string
	va_start(ap, fmt);									
    vsprintf(text, fmt, ap);						
	va_end(ap);											

	fontIndex = fontSize;

	// Verify valid font 
	if(fontSize >= HED_FONT_COUNT)
	{
		sysLogPrint("hedRendererOpenGL::RenderText() - WARNING: Wrong font size used [%d][%s]",fontSize,text);	
		fontIndex = HED_FONT_NORMAL;
	}

    xpos    = x;
	ypos    = y - (32 - m_Font[fontIndex].m_FontSize) / 2;
    
	cheight = 32.0f;
	cwidth  = 32.0f;
	
	SDL_SetTextureColorMod(m_Font[fontIndex].m_FontImage.m_pImageTexture,r * 255.0f,g * 255.0f,b * 255.0f);
	//glColor4f(r,g,b,m_FontAlpha);
	//glEnable(GL_BLEND);
    
	SDL_Rect srcRect,dstRect;

	for(i = 0; i < strlen(text); i++)
	{
		// Render character
		cwidth = (float)m_Font[fontIndex].m_FontWidth[text[i]]; 
		deltau = cwidth / 512.0f;

		// Create rectangle and draw it
		dstRect.x = xpos;
		dstRect.y = ypos;
		dstRect.w = cwidth;
		dstRect.h = cheight;

		srcRect.x = m_Font[fontIndex].m_FontCoord1[text[i]].x * 512.0f;
		srcRect.y = m_Font[fontIndex].m_FontCoord1[text[i]].y * 512.0f;

		srcRect.w = deltau * 512.0f;
		srcRect.h = (m_Font[fontIndex].m_FontCoord2[text[i]].y - m_Font[fontIndex].m_FontCoord1[text[i]].y) * 512.0f;
		
		SDL_RenderCopy(sdlRenderer, m_Font[fontIndex].m_FontImage.m_pImageTexture, &srcRect, &dstRect);
		xpos += m_Font[fontIndex].m_FontWidth[text[i]];
	}
	SDL_SetTextureColorMod(m_Font[fontIndex].m_FontImage.m_pImageTexture,255,255,255);
}

//=======================================================================================================================================
// Render a formated text line (on black background)
void hedRendererOpenGL::RenderText2(int x, int y, float r, float g, float b, int fontSize, const char *fmt, ...)
{
	char	text[4096];								
	va_list	ap;

	// print the formatted text to a string
	va_start(ap, fmt);									
    vsprintf(text, fmt, ap);						
	va_end(ap);											

	RenderText(x - 1,y,0,0,0,fontSize,text);
	RenderText(x + 1,y,0,0,0,fontSize,text);
	RenderText(x,y - 1,0,0,0,fontSize,text);
	RenderText(x,y + 1,0,0,0,fontSize,text);

	RenderText(x,y,r,g,b,fontSize,text);
}

//=======================================================================================================================================
// Draw a transparent line
void hedRendererOpenGL::DrawLineAlpha(int x1, int y1, int x2, int y2, float r, float g, float b, float a)
{
	SDL_SetRenderDrawBlendMode(sdlRenderer,SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdlRenderer, r * 255.0f,g * 255.0f,b * 255.0f,a * 255.0f);
	SDL_RenderDrawLine(sdlRenderer,x1,y1,x2,y2);

	SDL_SetRenderDrawBlendMode(sdlRenderer,SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(sdlRenderer, 255,255,255,255);
/*(
	GLfloat vertices[8];

	vertices[0] = x1; 	vertices[1] = y1;
	vertices[2] = x2; 	vertices[3] = y2;
    
#ifndef PLATFORM_IOS    
	glColor4f(r, g, b, a); 
	glEnable(GL_BLEND);
	GL_DISABLE_TEXTURE;
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES, 0, 2); 	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

#else

    VB_Vertex[0] = x1; VB_Vertex[1] = y1; VB_Vertex[2] = -1.0f;
    VB_Vertex[3] = x2; VB_Vertex[4] = y2; VB_Vertex[5] = -1.0f;
        
    VB_Colors[0] = r; VB_Colors[1] = g; VB_Colors[2] = b; VB_Colors[3] = a;        
    VB_Colors[4] = r; VB_Colors[5] = g; VB_Colors[6] = b; VB_Colors[7] = a;        
    
    exoIOS_DrawCustomVertexArray(GL_LINES,VB_Vertex,VB_Colors,2);    
    
#endif 
	*/
}


//=======================================================================================================================================
// Draw a dotted line
void hedRendererOpenGL::DrawDottedLine(int x1, int y1, int x2, int y2, float r, float g, float b)
{
	float   x,y,deltax,deltay,length;
	int     i;

	length = sqrt((float)(x2 - x1) * (float)(x2 - x1) + (float)(y2 - y1) * (float)(y2 - y1));
	x      = x1;
	y      = y1;
	deltax = ((float)(x2) - (float)(x1)) / length;
	deltay = ((float)(y2) - (float)(y1)) / length;

    
	for(i = 0; i < length; i++)
	{
		if(i % 2 == 0)
		{
			SDL_SetRenderDrawColor(sdlRenderer, r * 255.0f,g * 255.0f,b * 255.0f,255.0f);
			SDL_RenderDrawPoint(sdlRenderer,x,y);
		}

		x += deltax;
		y += deltay;
	}
 
}

//=======================================================================================================================================
// Draw a rectangle
void hedRendererOpenGL::DrawRectangleFill(int x1, int y1, int x2, int y2, float r, float g, float b, float a)
{
	//boxRGBA(sdlRenderer,x1,y1,x2,y2,r * 255.0f,g * 255.0f,b*255.0f,a * 255.0f);

	SDL_Rect _rect;

	_rect.x = x1;
	_rect.y = y1;
	_rect.w = (x2 - x1);
	_rect.h = (y2 - y1);

	SDL_SetRenderDrawBlendMode(sdlRenderer,SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdlRenderer, r * 255.0f,g * 255.0f,b * 255.0f,a * 255.0f);
	
	SDL_RenderFillRect(sdlRenderer,&_rect);

	SDL_SetRenderDrawBlendMode(sdlRenderer,SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(sdlRenderer, 255,255,255,255);
}

//=======================================================================================================================================
// Draw a line (antialiased with a texture)
void hedRendererOpenGL::DrawLineAA(int x1, int y1, int x2, int y2, float r, float g, float b, int width)
{     
	SDL_SetRenderDrawBlendMode(sdlRenderer,SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdlRenderer, r * 255.0f,g * 255.0f,b * 255.0f,255.0f);

	
	if(width > 1)
	{
		SDL_SetTextureColorMod(m_AALineTexture.m_pImageTexture,r * 255.0f,g * 255.0f,b * 255.0f);

		float dx = x2 - x1;
		float dy = y2 - y1;
		float dist = sqrt(dx * dx + dy * dy);
		dx /= dist;
		dy /= dist;

		float x = x1;
		float y = y1;

		for(int i = 0; i < int(dist) * 2; i++)
		{
			m_AALineTexture.Render(x,y,width,width);
			x += dx / 2.0f;
			y += dy / 2.0f;
		}
	}
	else
	{
		SDL_RenderDrawLine(sdlRenderer,x1,y1,x2,y2);
	}
	/*
	if(width > 1)
		thickLineRGBA(sdlRenderer,x1,y1,x2,y2,width,255.0f * r,255.0f * g,255.0f * b,255);
	else
		aalineRGBA(sdlRenderer,x1,y1,x2,y2,255.0f * r,255.0f * g,255.0f * b,255);
	*/
	SDL_SetRenderDrawBlendMode(sdlRenderer,SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(sdlRenderer, 255,255,255,255);
}
//=======================================================================================================================================
// Animated dot line
void hedRendererOpenGL::DrawAnimatedDotLine(int x1, int y1, int x2, int y2, float r, float g, float b, int skip)
{
	float   x,y,deltax,deltay,length;
	int     i;
    int     lastx = -1000, lasty = -1000, _x, _y;
    int     count = 0;

	length = sqrt((float)(x2 - x1) * (float)(x2 - x1) + (float)(y2 - y1) * (float)(y2 - y1));
	x      = x1;
	y      = y1;
	deltax = ((float)(x2) - (float)(x1)) / length;
	deltay = ((float)(y2) - (float)(y1)) / length;

	for(i = 0; i < length / 2; i++)
	{
        _x = x;
        _y = y;
        if(_x != lastx || _y != lasty)
        {
            if(count % 3  == 0)
                this->DrawLineAA(x,y,x, y, r,g,b,3);
            
            lastx = _x;
            lasty = _y;
        }
        
        count++;
        x += 2 * deltax;
		y += 2 * deltay;
	}
}
//=======================================================================================================================================
// Font class

//=======================================================================================================================================
// Constructor
hedRendererFont::hedRendererFont()
{
}

//=======================================================================================================================================
// Destructor
hedRendererFont::~hedRendererFont()
{
}

//=======================================================================================================================================
// Load font
void hedRendererFont::Load(char* fontTextureName, int fontSize)
{
	FILE* file;
	char  fileNameFont[1024], fileNameFontWidth[1024];
	int   i, value;
	float u,v;

	m_FontSize = fontSize;

	sprintf(fileNameFont,"%s.png",fontTextureName);
	sprintf(fileNameFontWidth,"%s.ini",fontTextureName);

	// Load font bitmap
	m_FontImage.LoadFromFile(fileNameFont);

	// Load font widths
	file = fopen(fileNameFontWidth,"rt");
    if(file == NULL)
        sysLogPrint("hedRendererFont::Load(%s) - failed to open file");

	for(i = 0; i < 256; i++)
	{
		fscanf(file,"%d",&value); m_FontWidth[i] = value;

		// Calculate lookup tables
		u = (float)(i % 16) / 16.0f + 32.0f / 512.0f;
		v = (float)(i / 16) / 16.0f + 32.0f / 512.0f;
		
		m_FontCoord1[i] = hedVector((float)(i % 16) / 16.0f, (float)(i / 16) / 16.0f);
		m_FontCoord2[i] = hedVector(u,v);
	}

	fclose(file);
}

//=======================================================================================================================================
// Bind font texture
void hedRendererFont::BindTexture()
{
}

//=======================================================================================================================================
// Width (in pixels) of a given line of text
int hedRendererFont::TextWidth(char* text)
{
	int i,width = 0;

	for(i = 0; i < strlen(text); i++)
		width += m_FontWidth[text[i]];

	return width;
}
