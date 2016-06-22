/*
Name: hedGraphicsImage.h
Desc: 2D image loading and manipulation

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _hedGraphicsImage_h_
#define _hedGraphicsImage_h_

#include "HackerEvolutionDualityDefines.h"

int  exoIOS_LoadTexture(char* fileName, int* imageWidth, int* imageHeight);
void exoIOS_DrawTriangleStrip(float* vertices, float* colors, float* texcoords, int textureID, int vertexCount);
void exoIOS_DrawCustomVertexArray(int arrayType, float* vertices, float* colors, int vertexCount);
void exoIOS_Transform_SelfRotation(float centerx, float centery, float angle);
void exoIOS_Transform_Reset();
    
// ==============================================================================================================================================================
// 2D image loading and manipulation

#define ANIM_SPEED_ANGLE  (15.0f)      // Degrees / second
#define ANIM_SPEED_SCALE  (1.0f/1.0f)  // Speed for the scale effect to take place (scales in, in 1 seconds)
#define ANIM_SPEED_ALPHA  (1.0f/1.0f)  // Speed for the fade effect to take place (fade in, in 1 seconds)

class hedImage
{
public:
	// Constructor / Destructor
	hedImage();
	~hedImage();

	// Functions
	int  LoadFromFile(char* fileName);                                                         // Load an image from a file
	void Render(int x, int y, int forceWidth = -1, int forceHeight = -1, float alpha = 1.0f);  // Draw the 2D image on screen

	// Public variables
	int            m_pWidth,  m_pHeight;            // Image size

	//float          m_FXAngleY;               // FX: rotation angle

	// ===================================================================================================
	// Animation parameters for the image
	// These variables and functions allow us to implemet various animations for an image
	float          m_AnimAngle, m_AnimAngleSpeed;                                 // Angle by which the image is rotated
	float          m_AnimScale, m_AnimScaleSpeed;                                 // Image scaling (in and out)
	float          m_AnimAlpha, m_AnimAlphaSpeed;                                 // Image fading (in and out)
	float          m_AnimTranslateX, m_AnimTranslateXSize, m_AnimTranslateXSpeed; // Image translation (horizontal)
	float          m_AnimTranslateY, m_AnimTranslateYSize, m_AnimTranslateYSpeed; // Image translation (vertical)

	void ResetAnimations()                        // Reset all animations
	{
		m_AnimAngle = 0.0f;      m_AnimAngleSpeed = 0;
		m_AnimScale = 1.0f;      m_AnimScaleSpeed = ANIM_SPEED_SCALE;
		m_AnimAlpha = 1.0f;      m_AnimAlphaSpeed = ANIM_SPEED_ALPHA;
		m_AnimTranslateX = 0.0f; m_AnimTranslateXSize = 0.0f;          m_AnimTranslateXSpeed = 0.0f;
		m_AnimTranslateY = 0.0f; m_AnimTranslateYSize = 0.0f;          m_AnimTranslateYSpeed = 0.0f;
	}

	// SDL Code (2014)
	SDL_Texture*   m_pImageTexture;

protected:
	int            m_pTransparent;         // Is the image transparent?
	int            m_pColorChannels;       // Color channels
};    
    
#endif