// moduleRender.h
// 
// Graphics render device interface (win32/directx)
//

#include "moduleRender.h"
#include "heLog.h"
#include "HackerEvolutionDefines.h"
#include <stdio.h>
#include <stdarg.h>
#include "SDL.h"
#include "SDL_image.h"

//==============================================================================================================================================================
// * New stuff in the 2014 update

// SDL global stuff
SDL_Renderer *sdlRenderer = nullptr;
SDL_Window   *sdlWindow   = nullptr;

heImage       AAPoint;

hedRendererFont fontSmall,fontMedium,fontLarge;

//================================================================================================
// Initialize rendering device
int  render_Init()
{
    gApplicationLog.printf("\nModule::render_Init()...");

	// SDLTODO Load fonts here
	fontMedium.Load("he-2014update/hed-font-512-10-bg",10);
	fontLarge.Load("he-2014update/hed-font-512-12-bg",12);
	fontSmall.Load("he-2014update/hed-font-interface-anonymouspro10bold",10);

	// Antialiased line texture
	AAPoint.LoadFromFile("he-2014update/res-aaline.png",1,1,0);

	return 0;
}
//================================================================================================
// Close rendering device
void render_Close()
{
    gApplicationLog.printf("\nModule::render_Close()...");
}
//================================================================================================
// Begin rendering frame
void render_BeginFrame(int clear)
{
}

//================================================================================================
// End rendering frame
void render_EndFrame()
{
}

//================================================================================================
// Draw a 2D line
void render_DrawLine(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	SDL_SetRenderDrawColor(sdlRenderer,r,g,b,255);
	SDL_RenderDrawLine(sdlRenderer,x1,y1,x2,y2);
	SDL_SetRenderDrawColor(sdlRenderer,0,0,0,0);
}
//================================================================================================
// Draw a rectangle
void render_DrawRectangle(int x, int y, int width, int height, int r, int g, int b, int transparent)
{
	// SDLTODO - This draws a filled rectangle
}
//================================================================================================
// Draw a rectangle (empty)
void render_DrawRectangle_Empty(int x, int y, int width, int height, int r, int g, int b)
{
	render_DrawLine(x,y,x + width, y, r,g,b);
	render_DrawLine(x + width,y,x + width, y + height, r,g,b);
	render_DrawLine(x + width, y + height,x,y + height, r,g,b);
	render_DrawLine(x,y + height,x,y, r,g,b);
}
//================================================================================================
// Draw text
void render_DrawText(int x, int y, char* text, int r, int g, int b, int s)
{
	fontSmall.RenderText(x,y,r,g,b,10,text);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draws formated text
void render_DrawText_Formated(int x, int y, int r, int g, int b, char* fmt, ...)
{
    char     buffer[1024];
    va_list  param;

    // printf formated text to buffer 
    va_start(param,fmt);
    vsprintf(buffer,fmt,param);
    va_end(param);

	fontSmall.RenderText(x,y,r,g,b,10,buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Draw text (medium)
void render_DrawText_Medium(int x, int y, char* text, int r, int g, int b, int shadow, int center)
{
	fontSmall.RenderText(x,y,r,g,b,10,text);
}
//================================================================================================
// Draw text (large)
void render_DrawText_Large(int x, int y, char* text, int r, int g, int b, int shadow, int center)
{
	int _x = x;

	if(center == 1)
		_x -= fontLarge.TextWidth(text) / 2;

	fontLarge.RenderText(_x,y,r,g,b,12,text);
}
//================================================================================================
// Image management class
//================================================================================================
// Constructor
heImage::heImage()
{
	// * Initialize image
	mAnimationModifier = 0;
	mAlphaOn           = 1;
}
//================================================================================================
// Destructor
heImage::~heImage()
{
	if(mAnimationModifier) delete mAnimationModifier;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Attach animation modifier to the image
void heImage::AttachAnimationModifier(CAnimationModifier* pAnimationModifier)
{
	mAnimationModifier = pAnimationModifier;
	mAnimationModifier->Attach(mVertexList);
	mAnimationModifier->Reset();
}
//================================================================================================
// Load image from file
void heImage::LoadFromFile(char* filename, int width, int height, int adjustToScreen)
{
	int          i;

    gApplicationLog.printf("\nheImage::LoadFromFile(%s) - ",filename);

	imageTexture = IMG_LoadTexture(sdlRenderer, filename);
	SDL_QueryTexture(imageTexture,NULL,NULL,&m_width,&m_height);
	
	m_adjustToScreen = adjustToScreen;

	// Populate vertex buffers for the animation modifiers
	for(i = 0; i < 4; i++)
	{
		mVertexList[i].r = 255;
		mVertexList[i].g = 255;
		mVertexList[i].b = 255;
		mVertexList[i].a = 255;
	}
}
//================================================================================================
// Draw image
void heImage::Draw(int x, int y)
{
	SDL_Rect srcRect,dstRect;
	int      i;

	// If this image, has an animation modifier, update it
	if(mAnimationModifier != 0)
		mAnimationModifier->UpdateFrame();

	// Create rectangle and draw it
	heVertex tmpVertex[4];
	tmpVertex[0].x = x;           tmpVertex[0].y = y;
	tmpVertex[1].x = x + m_width; tmpVertex[1].y = y;
	tmpVertex[2].x = x + m_width; tmpVertex[2].y = y + m_height;
	tmpVertex[3].x = x;           tmpVertex[3].y = y + m_height;

	for(i = 0; i < 4; i++)
	{
		tmpVertex[i].x += mVertexList[i].dx; 
		tmpVertex[i].y += mVertexList[i].dy; 
	}

	dstRect.x = tmpVertex[0].x;
	dstRect.y = tmpVertex[0].y;
	dstRect.w = tmpVertex[1].x - tmpVertex[0].x;
	dstRect.h = tmpVertex[3].y - tmpVertex[0].y;

	if(mVertexList[0].a > 255) mVertexList[0].a = 255;
	Uint8 imageAlpha = (Uint8)(mVertexList[0].a);

	SDL_SetTextureAlphaMod(imageTexture,imageAlpha);
	SDL_RenderCopy(sdlRenderer, imageTexture, NULL, &dstRect);
}

//================================================================================================
// Draw image
void heImage::ForceDraw(int x, int y, int w, int h)
{
	SDL_Rect srcRect,dstRect;

	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = w;
	dstRect.h = h;

	SDL_RenderCopy(sdlRenderer, imageTexture, NULL, &dstRect);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Release image resource
void heImage::ReleaseImage()
{
	SDL_DestroyTexture(imageTexture);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Draws an antialiased line
void render_DrawLine_AA(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	//render_DrawLine( x1 + 1,  y1 + 1,  x2 + 1,  y2 + 1, r/2, g/2, b/2);
	render_DrawLine( x1,  y1,  x2,  y2,  r,  g,  b);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation modifier classes
// * Animation : Alpha fade in, from zero to full color

// * Attach image vertices and initialize
void CAnimationModifier_AlphaFadeIn::Attach(TVertex* pVertexList)
{
	mVertexList = pVertexList;
	mFadeSpeed  = 128.0f;
	mStarted    = 0;	
	mDirection  = 1.0f;
}
// * Reset animation
void CAnimationModifier_AlphaFadeIn::Reset(int pParam1, int pParam2)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		mVertexList[i].r = 0;
		mVertexList[i].g = 0;
		mVertexList[i].b = 0;
		mVertexList[i].a = 0;
	}

	// * Set fade speed, if specified
	if(pParam1 != 0) mFadeSpeed = pParam1;
}
// * Start animation
void CAnimationModifier_AlphaFadeIn::Start()
{	
	mStarted   = 1;
	mDirection = 1.0f;
}
// * Stop animation
void CAnimationModifier_AlphaFadeIn::Stop()
{
	mStarted = 0;
}
// * Revert animation
void CAnimationModifier_AlphaFadeIn::Revert()
{
	mDirection = -0.50f;
}
// * Update frame
void CAnimationModifier_AlphaFadeIn::UpdateFrame()
{
	int i;

	if(mStarted == 1)
	{
		for(i = 0; i < 4; i++)
		{
			mVertexList[i].r += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].g += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].b += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].a += mFadeSpeed * gFrameTime * mDirection; 

			if(mVertexList[i].r > 255.0f) { mVertexList[i].r = 255.0f; Stop(); }
			if(mVertexList[i].g > 255.0f) { mVertexList[i].g = 255.0f; Stop(); }
			if(mVertexList[i].b > 255.0f) { mVertexList[i].b = 255.0f; Stop(); }
			if(mVertexList[i].a > 255.0f) { mVertexList[i].a = 255.0f; Stop(); }

			if(mVertexList[i].r < 0.0f) { mVertexList[i].r = 0.0f; Stop(); }
			if(mVertexList[i].g < 0.0f) { mVertexList[i].g = 0.0f; Stop(); }
			if(mVertexList[i].b < 0.0f) { mVertexList[i].b = 0.0f; Stop(); }
			if(mVertexList[i].a < 0.0f) { mVertexList[i].a = 0.0f; Stop(); }
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation : Vertical scan

// * Attach image vertices and initialize
void CAnimationModifier_VScan::Attach(TVertex* pVertexList)
{
	mVertexList = pVertexList;
	mSpeed      = 128.0f;
	mStarted    = 0;	
	mDirection  = 1.0f;
}
// * Reset animation
void CAnimationModifier_VScan::Reset(int pParam1, int pParam2)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		mVertexList[i].dx = 0;
		mVertexList[i].dy = 0;
	}
}
// * Start animation
void CAnimationModifier_VScan::Start()
{	
	mStarted   = 1;
	mDirection = 1.0f;
}
// * Stop animation
void CAnimationModifier_VScan::Stop()
{
	mStarted = 0;
}
// * Revert animation
void CAnimationModifier_VScan::Revert()
{
	mDirection *= -1.0f;
}
// * Update frame
void CAnimationModifier_VScan::UpdateFrame()
{
	int i;

	if(mStarted == 1)
	{
		for(i = 0; i < 4; i++)
		{
			mSpeed = mVertexList[i].dy + 10;
			mVertexList[i].dy += mSpeed * gFrameTime * mDirection; 
		}

		if(mVertexList[2].dy > g_screen_height) { mVertexList[2].dy = g_screen_height; Revert(); }
		if(mVertexList[2].dy < 0.0f) { mVertexList[2].dy = 0.0f; Revert(); }
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation : Vertical scan

// * Attach image vertices and initialize
void CAnimationModifier_VScan2::Attach(TVertex* pVertexList)
{
	mVertexList = pVertexList;
	mSpeed      = 128.0f;
	mStarted    = 0;	
	mDirection  = 1.0f;
}
// * Reset animation
void CAnimationModifier_VScan2::Reset(int pParam1, int pParam2)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		mVertexList[i].dx = 0;
		mVertexList[i].dy = 0;
	}

	mParam1 = pParam1;
	mParam2 = pParam2;
}
// * Start animation
void CAnimationModifier_VScan2::Start()
{	
	mStarted   = 1;
	mDirection = 1.0f;
}
// * Stop animation
void CAnimationModifier_VScan2::Stop()
{
	mStarted = 0;
}
// * Revert animation
void CAnimationModifier_VScan2::Revert()
{
	mDirection *= -1.0f;
}
// * Update frame
void CAnimationModifier_VScan2::UpdateFrame()
{
	int i;

	if(mStarted == 1)
	{
		for(i = 0; i < 4; i++)
		{
			// mSpeed = mVertexList[i].dy + 30;
			mVertexList[i].dy += mSpeed * gFrameTime * mDirection; 
		}

		if(mVertexList[2].dy >= mParam2) { mVertexList[2].dy = mParam2; Stop(); }
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation : Horizontal scan

// * Attach image vertices and initialize
void CAnimationModifier_HScan::Attach(TVertex* pVertexList)
{
	mVertexList = pVertexList;
	mSpeed      = 100.0f;
	mStarted    = 0;	
	mDirection  = 1.0f;
}
// * Reset animation
void CAnimationModifier_HScan::Reset(int pParam1, int pParam2)
{
	int i;

	for(i = 0; i < 4; i++)
	{
		mVertexList[i].dx = 0;
		mVertexList[i].dy = 0;
	}
	
	mParam1 = pParam1;
	mParam2 = pParam2;

}
// * Start animation
void CAnimationModifier_HScan::Start()
{	
	mStarted   = 1;
	mDirection = 1.0f;
}
// * Stop animation
void CAnimationModifier_HScan::Stop()
{
	mStarted = 0;
}
// * Revert animation
void CAnimationModifier_HScan::Revert()
{
	mDirection *= -1.0f;
}
// * Update frame
void CAnimationModifier_HScan::UpdateFrame()
{
	int i;

	if(mStarted == 1)
	{
		for(i = 0; i < 4; i++)
		{			
			mVertexList[i].dx += mSpeed * gFrameTime * mDirection; 
		}

		if(mVertexList[2].dx < mParam1) 
		{
			Revert(); 
			for(i = 0; i < 4; i++) mVertexList[i].dx = mParam1;
		}
		if(mVertexList[2].dx > mParam2) 
		{
			Revert(); 
			for(i = 0; i < 4; i++) mVertexList[i].dx = mParam2;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation : Scale in from null, to full size

// * Attach image vertices and initialize
void CAnimationModifier_ScaleIn::Attach(TVertex* pVertexList)
{
	mVertexList = pVertexList;
	mSpeedX      = 0.0f;
	mSpeedY      = 0.0f;
	mFadeSpeed  = 128.0f;

	mStarted    = 0;	
	mDirection  = -1.0f;

	mParam1 = 0;
	mParam2 = 0;

	for(int i = 0; i < 4; i++)
	{
		mVertexList[i].dx = 0;
		mVertexList[i].dy = 0;
	}
}
// * Reset animation
void CAnimationModifier_ScaleIn::Reset(int pParam1, int pParam2)
{
	int i;

	for(i = 0; i < 4; i++)
	{
		mVertexList[i].dx = 0;
		mVertexList[i].dy = 0;
	}

	mDirection  = -1.0f;

	// pParam1 and pParam2 are the image width and height
	mParam1 = pParam1;
	mParam2 = pParam2;

	mSpeedX      = pParam1 * 1.5;
	mSpeedY      = pParam2 * 1.5;

	mFadeSpeed   = 128.0f;

	mVertexList[0].dx = pParam1 / 2;   mVertexList[0].dy = pParam2 / 2;
	mVertexList[1].dx = - pParam1 / 2; mVertexList[1].dy = pParam2 / 2;
	mVertexList[2].dx = pParam1 / 2;   mVertexList[2].dy = - pParam2 / 2;
	mVertexList[3].dx = - pParam1 / 2; mVertexList[3].dy = - pParam2 / 2;

}
// * Start animation
void CAnimationModifier_ScaleIn::Start()
{	
	mStarted   = 1;
}
// * Stop animation
void CAnimationModifier_ScaleIn::Stop()
{
	mStarted = 0;
}
// * Revert animation
void CAnimationModifier_ScaleIn::Revert()
{
	mDirection *= -1.0f;
}
// * Update frame
void CAnimationModifier_ScaleIn::UpdateFrame()
{
	int i;

	if(mStarted == 1)
	{
		mVertexList[0].dx -= (mSpeedX * gFrameTime * mDirection);
		mVertexList[0].dy -= (mSpeedY * gFrameTime * mDirection);

		mVertexList[1].dx += (mSpeedX * gFrameTime * mDirection);
		mVertexList[1].dy -= (mSpeedY * gFrameTime * mDirection);

		mVertexList[2].dx -= (mSpeedX * gFrameTime * mDirection);
		mVertexList[2].dy += (mSpeedY * gFrameTime * mDirection);

		mVertexList[3].dx += (mSpeedX * gFrameTime * mDirection);
		mVertexList[3].dy += (mSpeedY * gFrameTime * mDirection);

		if(mVertexList[0].dx < 0)
		{
			mStarted = 0;
			for(i = 0; i < 4; i++)
			{
				mVertexList[i].dx = 0;
				mVertexList[i].dy = 0;
			}
		}
		if(mVertexList[0].dx > mParam1 / 2 || mVertexList[0].dy > mParam2 / 2)
		{
			mStarted = 0;

			mVertexList[0].dx = mParam1 / 2;   mVertexList[0].dy = mParam2 / 2;
			mVertexList[1].dx = - mParam1 / 2; mVertexList[1].dy = mParam2 / 2;
			mVertexList[2].dx = mParam1 / 2;   mVertexList[2].dy = - mParam2 / 2;
			mVertexList[3].dx = - mParam1 / 2; mVertexList[3].dy = - mParam2 / 2;
		}

		// Adjust alpha
		for(i = 0; i < 4; i++)
		{
			mVertexList[i].r += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].g += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].b += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].a += mFadeSpeed * gFrameTime * mDirection; 

			if(mVertexList[i].r > 255.0f) { mVertexList[i].r = 255.0f;}
			if(mVertexList[i].g > 255.0f) { mVertexList[i].g = 255.0f;}
			if(mVertexList[i].b > 255.0f) { mVertexList[i].b = 255.0f;}
			if(mVertexList[i].a > 255.0f) { mVertexList[i].a = 255.0f;}

			if(mVertexList[i].r < 0.0f) { mVertexList[i].r = 0.0f;}
			if(mVertexList[i].g < 0.0f) { mVertexList[i].g = 0.0f;}
			if(mVertexList[i].b < 0.0f) { mVertexList[i].b = 0.0f;}
			if(mVertexList[i].a < 0.0f) { mVertexList[i].a = 0.0f;}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation : Scale in from null, to full size with alpha blending (horizontal only)

// * Attach image vertices and initialize
void CAnimationModifier_ScaleInH::Attach(TVertex* pVertexList)
{
	mVertexList = pVertexList;
	mSpeedX      = 0.0f;
	mFadeSpeed  = 128.0f;

	mStarted    = 0;	
	mDirection  = -1.0f;

	mParam1 = 0;
	mParam2 = 0;

	for(int i = 0; i < 4; i++)
		mVertexList[i].dx = 0;
}
// * Reset animation
void CAnimationModifier_ScaleInH::Reset(int pParam1, int pParam2)
{
	int i;

	for(i = 0; i < 4; i++)
		mVertexList[i].dx = 0;

	// pParam1 and pParam2 are the image width and height
	mParam1 = pParam1;
	mParam2 = pParam2;

	mSpeedX      = pParam1 * 1.5;

	//mFadeSpeed   = 512.0f;
	mFadeSpeed   = mSpeedX;

	mVertexList[1].dx = - pParam1;
	mVertexList[3].dx = - pParam1;
}
// * Start animation
void CAnimationModifier_ScaleInH::Start()
{	
	mStarted   = 1;
}
// * Stop animation
void CAnimationModifier_ScaleInH::Stop()
{
	mStarted = 0;
}
// * Revert animation
void CAnimationModifier_ScaleInH::Revert()
{
	mDirection *= -1.0f;
}
// * Update frame
void CAnimationModifier_ScaleInH::UpdateFrame()
{
	int i;

	if(mStarted == 1)
	{
		mVertexList[1].dx += (mSpeedX * gFrameTime * mDirection);
		mVertexList[3].dx += (mSpeedX * gFrameTime * mDirection);

		if(mVertexList[1].dx > 0)
		{
			mStarted = 0;
			for(i = 0; i < 4; i++)
				mVertexList[i].dx = 0;
		}
		if(mVertexList[1].dx < - mParam1)
		{
			mStarted = 0;

			mVertexList[1].dx = - mParam1;
			mVertexList[3].dx = - mParam1;
		}

		// Adjust alpha
		for(i = 0; i < 4; i++)
		{
			mVertexList[i].r += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].g += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].b += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].a += mFadeSpeed * gFrameTime * mDirection; 

			if(mVertexList[i].r > 255.0f) { mVertexList[i].r = 255.0f;}
			if(mVertexList[i].g > 255.0f) { mVertexList[i].g = 255.0f;}
			if(mVertexList[i].b > 255.0f) { mVertexList[i].b = 255.0f;}
			if(mVertexList[i].a > 255.0f) { mVertexList[i].a = 255.0f;}

			if(mVertexList[i].r < 0.0f) { mVertexList[i].r = 0.0f;}
			if(mVertexList[i].g < 0.0f) { mVertexList[i].g = 0.0f;}
			if(mVertexList[i].b < 0.0f) { mVertexList[i].b = 0.0f;}
			if(mVertexList[i].a < 0.0f) { mVertexList[i].a = 0.0f;}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation : Scale in from null, to full size with alpha blending (horizontal only)

// * Attach image vertices and initialize
void CAnimationModifier_ScaleInHCenter::Attach(TVertex* pVertexList)
{
	mVertexList = pVertexList;
	mSpeedX      = 0.0f;
	mSpeedY      = 0.0f;
	mFadeSpeed  = 128.0f;

	mStarted    = 0;	
	mDirection  = -1.0f;

	mParam1 = 0;
	mParam2 = 0;

	for(int i = 0; i < 4; i++)
	{
		mVertexList[i].dx = 0;
		mVertexList[i].dy = 0;
	}
}
// * Reset animation
void CAnimationModifier_ScaleInHCenter::Reset(int pParam1, int pParam2)
{
	int i;

	for(i = 0; i < 4; i++)
		mVertexList[i].dx = 0;

	// pParam1 and pParam2 are the image width and height
	mParam1 = pParam1;

	mSpeedX      = pParam1 * 1.5;

	mFadeSpeed   = 512.0f;

	mVertexList[0].dx = pParam1 / 2;   
	mVertexList[1].dx = - pParam1 / 2; 
	mVertexList[2].dx = pParam1 / 2;   
	mVertexList[3].dx = - pParam1 / 2; 
}
// * Start animation
void CAnimationModifier_ScaleInHCenter::Start()
{	
	mStarted   = 1;
}
// * Stop animation
void CAnimationModifier_ScaleInHCenter::Stop()
{
	mStarted = 0;
}
// * Revert animation
void CAnimationModifier_ScaleInHCenter::Revert()
{
	mDirection *= -1.0f;
}
// * Update frame
void CAnimationModifier_ScaleInHCenter::UpdateFrame()
{
	int i;

	if(mStarted == 1)
	{
		mVertexList[0].dx -= (mSpeedX * gFrameTime * mDirection);
		mVertexList[1].dx += (mSpeedX * gFrameTime * mDirection);
		mVertexList[2].dx -= (mSpeedX * gFrameTime * mDirection);
		mVertexList[3].dx += (mSpeedX * gFrameTime * mDirection);

		if(mVertexList[0].dx < 0)
		{
			mStarted = 0;
			for(i = 0; i < 4; i++)
				mVertexList[i].dx = 0;
		}
		if(mVertexList[0].dx > mParam1 / 2)
		{
			mStarted = 0;

			mVertexList[0].dx = mParam1 / 2;
			mVertexList[1].dx = - mParam1 / 2;
			mVertexList[2].dx = mParam1 / 2;  
			mVertexList[3].dx = - mParam1 / 2;
		}

		// Adjust alpha
		for(i = 0; i < 4; i++)
		{
			mVertexList[i].r += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].g += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].b += mFadeSpeed * gFrameTime * mDirection; 
			mVertexList[i].a += mFadeSpeed * gFrameTime * mDirection; 

			if(mVertexList[i].r > 255.0f) { mVertexList[i].r = 255.0f;}
			if(mVertexList[i].g > 255.0f) { mVertexList[i].g = 255.0f;}
			if(mVertexList[i].b > 255.0f) { mVertexList[i].b = 255.0f;}
			if(mVertexList[i].a > 255.0f) { mVertexList[i].a = 255.0f;}

			if(mVertexList[i].r < 0.0f) { mVertexList[i].r = 0.0f;}
			if(mVertexList[i].g < 0.0f) { mVertexList[i].g = 0.0f;}
			if(mVertexList[i].b < 0.0f) { mVertexList[i].b = 0.0f;}
			if(mVertexList[i].a < 0.0f) { mVertexList[i].a = 0.0f;}
		}
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
	SDL_Surface* temp = 0;

	temp = IMG_Load(fileNameFont);
	if(temp == 0) 
			printf("IMG_Load: %s\n", IMG_GetError());

	m_FontTexture = SDL_CreateTextureFromSurface(sdlRenderer, temp);
	SDL_FreeSurface(temp);

	// Load font widths
	file = fopen(fileNameFontWidth,"rt");
    if(file == NULL)
	{
		gApplicationLog.printf("hedRendererFont::Load(%s) - failed to open file");
		return;
	}

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
// Width (in pixels) of a given line of text
int hedRendererFont::TextWidth(char* text)
{
	int i,width = 0;

	for(i = 0; i < strlen(text); i++)
		width += m_FontWidth[text[i]];

	return width;
}

//=======================================================================================================================================
// Render a formated text line
void hedRendererFont::RenderText(int x, int y, float r, float g, float b, int fontSize, const char *fmt)
{							
	va_list	ap;
	int     i;
	float   xpos, ypos;
	float   cwidth,cheight;  // character width and height in pixels
	float   deltau;
	int     fontIndex;
	SDL_Rect srcRect,dstRect;


	// print the formatted text to a string
	//va_start(ap, fmt);									
    //vsprintf(text, fmt, ap);						
	//va_end(ap);											

	fontIndex = fontSize;

    xpos    = x;
	ypos    = y - (32 - m_FontSize) / 2;
    
	cheight = 32.0f;
	cwidth  = 32.0f;

	// * Check if we have color information encoded
	int      index = 0;
	hestring keyword;
	int      _r,_g,_b;

	sscanf(fmt,"%s %d %d %d",keyword,&_r,&_g,&_b);

	if(strcmp(keyword,"/color") == 0)
	{
		index = 0;
		while(fmt[index] != ' ') index++;index++;
		while(fmt[index] != ' ') index++;index++;
		while(fmt[index] != ' ') index++;index++;
		while(fmt[index] != ' ') index++;index++;
		SDL_SetTextureColorMod(m_FontTexture,_r,_g,_b);
	}
	else
		SDL_SetTextureColorMod(m_FontTexture,r,g,b);

	for(i = index; i < strlen(fmt); i++)
	{
		// Render character
		cwidth = (float)m_FontWidth[fmt[i]]; 
		deltau = cwidth / 512.0f;

		// Create rectangle and draw it
		dstRect.x = xpos;
		dstRect.y = ypos;
		dstRect.w = 32;
		dstRect.h = 32;

		// Extract character image
		int u,v;
		v = fmt[i] / 16;
		u = fmt[i] % 16;

		srcRect.x = u * 32;
		srcRect.y = v * 32;
		srcRect.w = 32;
		srcRect.h = 32;
		
		SDL_RenderCopy(sdlRenderer, m_FontTexture, &srcRect, &dstRect);
		xpos += m_FontWidth[fmt[i]];
	}
		SDL_SetTextureColorMod(m_FontTexture,255,255,255);
}

