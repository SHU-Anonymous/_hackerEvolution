/*
Name: hedGraphicsImage.cpp (Win32 code)
Desc: 2D image loading and manipulation

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedGraphicsImage.h"
#include "../generic - inc/hedSystem.h"
#include "../generic - inc/hedRendererOpenGL.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include "SDL_image.h"
#endif

#ifdef PLATFORM_MACOS
#include "SDL_image.h"
//#include "MacOSSandBoxHelper.h"
#endif

#ifdef PLATFORM_LINUX
#include <GL/glut.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#endif

#ifdef PLATFORM_IOS
#include <stdio.h>

float GLA_Vertex[12];
float GLA_TexCoord[8];
float GLA_Color[16];

#endif
//=======================================================================================================================================
// Class constructor
hedImage::hedImage()
{
	m_pTransparent = 0;
	m_pWidth       = 0;
	m_pHeight      = 0;

	ResetAnimations();
}

//=======================================================================================================================================
// Class destructor
hedImage::~hedImage()
{
}

//=======================================================================================================================================
// Load an image from a file
int hedImage::LoadFromFile(char* fileName)
{
	int    loadedOk = 1;
	int    openglErrorCode;

	// Reset all animations
	ResetAnimations();

	// Load image
	sysLogPrint("hedImage::LoadFromFile(%s) - Loading image",fileName);
     
   	m_pImageTexture = IMG_LoadTexture(sdlRenderer, fileName);
	if(m_pImageTexture != NULL)
    {
		SDL_QueryTexture(m_pImageTexture,NULL,NULL,&m_pWidth,&m_pHeight);
        sysLogPrint("hedImage::LoadFromFile(%s) - LOADED OK",fileName);
    }
    else
	{
		sysLogPrint("hedImage::LoadFromFile(%s) - FAILED TO LOAD IMAGE",fileName);
		loadedOk = 0;
	}
	
	return loadedOk;
}

//=======================================================================================================================================
// Draw the 2D image on screen
void hedImage::Render(int x, int y, int forceWidth, int forceHeight, float alpha)
{
	float width,height;

    
	// Select the proper image width (original, or forced one) to be used for animation FX
	width  = m_pWidth;
	height = m_pHeight;

	if(forceWidth != -1)
		width = forceWidth;

	if(forceHeight != -1)
		height = forceHeight;
    
	// Process animations
	// FX: Scale in
	m_AnimScale += m_AnimScaleSpeed * g_FrameTime; CLAMP(m_AnimScale, 0.0f, 1.0f);
	x += width * (1 - m_AnimScale) / 2;
	y += height * (1 - m_AnimScale) / 2;
	width  *= m_AnimScale; height *= m_AnimScale; 

	// FX: Fade in
	m_AnimAlpha += m_AnimAlphaSpeed * g_FrameTime; CLAMP(m_AnimAlpha, 0.0f, 1.0f);

	// FX: Rotation
	if(m_AnimAngleSpeed != 0.0f)
		m_AnimAngle += m_AnimAngleSpeed * g_FrameTime;       

	// FX: Translation (horizontal)
	if(m_AnimTranslateXSpeed != 0.0f)
	{
		m_AnimTranslateX += m_AnimTranslateXSpeed * g_FrameTime;
		CLAMP(m_AnimTranslateX, 0.0f, m_AnimTranslateXSize);
	}

	// FX: Translation (vertical)
	if(m_AnimTranslateYSpeed != 0.0f)
	{
		m_AnimTranslateY += m_AnimTranslateYSpeed * g_FrameTime;
		CLAMP(m_AnimTranslateY, 0.0f, m_AnimTranslateYSize);
	}

	SDL_Rect dstRect;

	// Forcing a different alpha
	Uint8 imageAlpha;

    if(alpha == 1.0f)
        imageAlpha = m_AnimAlpha * 255.0f;
    else
        imageAlpha = alpha * 255.0f;

	SDL_SetTextureAlphaMod(m_pImageTexture,imageAlpha);

	dstRect.x = x + m_AnimTranslateX;
	dstRect.y = y + m_AnimTranslateY;
	dstRect.w = width;
	dstRect.h = height;

	//SDL_RenderCopy(sdlRenderer, m_pImageTexture, NULL, &dstRect);
	SDL_RenderCopyEx(sdlRenderer, m_pImageTexture, NULL, &dstRect,floor(m_AnimAngle),NULL,SDL_FLIP_NONE);
   
}
