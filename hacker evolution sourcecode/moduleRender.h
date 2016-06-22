// moduleRender.h
// 
// Graphics render device interface
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _moduleRender_h_
#define _moduleRender_h_

#include "heLog.h"
#include "SDL.h"
#include "SDL_image.h"
#include <math.h>


//==============================================================================================================================================================
// * New stuff in the 2014 update
typedef struct heColor
{
	float r,g,b,a;
}heColor;

typedef struct
{
	float x,y;
}heVertex;

// SDL global stuff
extern SDL_Renderer *sdlRenderer;
extern SDL_Window   *sdlWindow;

//=======================================================================================================================================
// 2D Vector class definiton
#define HED_PI 3.14159265f

class hedVector
{
public:
	hedVector() { x = 0; y = 0;};
	hedVector(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	// + operator
	hedVector operator+(const hedVector& v) const
	{
		return hedVector(x + v.x, y + v.y);
	}
	// - operator
	hedVector operator-(const hedVector& v) const
	{
		return hedVector(x - v.x, y - v.y);
	}
	hedVector operator-() const
	{
		return hedVector(-x,-y);
	}
	// * operator
	hedVector operator*(float value) const
	{
		return hedVector(x * value, y * value);
	}
	// Vector length
	float Length() const
	{
		return sqrt(x * x + y * y);
	}
	// Normalized value
	hedVector Normalized() const
	{
		hedVector normalizedVector(x / this->Length(), y / this->Length());
		return normalizedVector;
	}

    float Dot(const hedVector& v) const
    {
        return x * v.x + y * v.y;
    } 

    float Cross(const hedVector& v) const
    {
        return (x * v.y - y * v.x);
    } 

	void Rotate(float angle)
	{
		float _x, _y;

        _x = x * cos(angle * HED_PI / 180.0f) - y * sin(angle * HED_PI / 180.0f);         
		_y = y * cos(angle * HED_PI / 180.0f) + x * sin(angle * HED_PI / 180.0f); 

		x = _x;
		y = _y;
	}

	float x,y;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Global variables and defines
//
// * vertex datatypes
struct DXCustomVertex
{
    float   x, y, z, rhw;   // The transformed position for the vertex.
    int     dwColor;        // The vertex colour.
    float   tu, tv;         // Texture co-ordinates.
};

// * Vertex data type for images
typedef struct
{
	double r,g,b,a;
	double dx,dy;
}TVertex;

// * Forward declaration
class CAnimationModifier;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Functions

int  render_Init();
void render_SetState();
void render_Close();
void render_BeginFrame(int clear = 0);
void render_EndFrame();

void render_DrawLine(int x1, int y1, int x2, int y2, int r, int g, int b);
void render_DrawRectangle(int x, int y, int width, int height, int r, int g, int b, int transparent = 0);
void render_DrawRectangle_Empty(int x, int y, int width, int height, int r, int g, int b);
void render_DrawText(int x, int y, char* text, int r, int g, int b, int s = 0);
void render_DrawText_Formated(int x, int y, int r, int g, int b, char* fmt, ...);
void render_DrawText_Medium(int x, int y, char* text, int r, int g, int b, int shadow = 1, int center = 0);
void render_DrawText_Large(int x, int y, char* text, int r, int g, int b, int shadow = 1, int center = 0);

// * Misc rendering functions
void render_DrawLine_AA(int x1, int y1, int x2, int y2, int r, int g, int b);

// Image management class
class heImage
{
public:
    heImage();
    ~heImage();

    void LoadFromFile(char* filename,  int width, int height, int adjustToScreen = 0);
    void Draw(int x, int y);
	void ForceDraw(int x, int y, int w, int h);

	// * Member functions
	void SetAlpha(int pValue) {mAlphaOn = pValue;};

	// * The image animation modifier
	void AttachAnimationModifier(CAnimationModifier* pAnimationModifier);
	CAnimationModifier* mAnimationModifier;

	void ReleaseImage();

protected:

	char  m_filename[1024];
	int   m_width,m_height,m_adjustToScreen;

	SDL_Texture* imageTexture;

	// * Member variables
	TVertex mVertexList[4];   
	int     mAlphaOn;
};


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
	int  TextWidth(char* text);                                                                     // Width (in pixels) of a given line of text
	void RenderText(int x, int y, float r, float g, float b, int fontSize, const char *fmt);        // Render a line of text

	// Public variables
	float     m_FontWidth[256];                                                                     // Font character widths
	hedVector m_FontCoord1[256],m_FontCoord2[256];                                                  // Font texture coordinates (lookup table)
	int       m_FontSize;                                                                           // Font size

protected:

	SDL_Texture* m_FontTexture;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Animation modifier classes
// * These are attached to images, to perform misc. animations
class CAnimationModifier
{
public:
	virtual void Attach(TVertex* pVertexList) = 0;            // attach image vertices
	virtual void Reset(int pParam1 = 0, int pParam2 = 0) = 0; // reset animation
	virtual void Start() = 0;                                 // start animation
	virtual void Stop() = 0;                                  // stop animation
	virtual void Revert() = 0;                                // revert animation
	virtual void UpdateFrame() = 0;	                          // update animation
	void ForceDirection(int pDirection, int pStarted)         // force animation direction
	{
		mDirection = pDirection;
		mStarted   = pStarted;
	}

	TVertex* mVertexList;        // pointer to image vertexes
	int      mStarted;
	int      mParam1,mParam2;    // Animation parameters
	float	 mDirection;         // Animation direction

protected:

};

// * Animation modifier class
// * Animation : Alpha fade in, from zero to full color
class CAnimationModifier_AlphaFadeIn : public CAnimationModifier
{
public:
	void Attach(TVertex* pVertexList);                // attach image vertices
	void Reset(int pParam1 = 0, int pParam2 = 0);     // reset animation
	void Start();                                     // start animation
	void Stop();                                      // stop animation
	void Revert();                                    // stop animation
	void UpdateFrame();	                              // update animation

protected:

	float mFadeSpeed;
};

// * Animation modifier class
// * Animation : Horizontal motion (up/down scan)
class CAnimationModifier_VScan : public CAnimationModifier
{
public:
	void Attach(TVertex* pVertexList);                // attach image vertices
	void Reset(int pParam1 = 0, int pParam2 = 0);     // reset animation
	void Start();                                     // start animation
	void Stop();                                      // stop animation
	void Revert();                                    // stop animation
	void UpdateFrame();	                              // update animation

protected:

	float mSpeed;
};

// * Animation modifier class
// * Animation : Vertical motion (down scan and stop)
class CAnimationModifier_VScan2 : public CAnimationModifier
{
public:
	void Attach(TVertex* pVertexList);                // attach image vertices
	void Reset(int pParam1 = 0, int pParam2 = 0);     // reset animation
	void Start();                                     // start animation
	void Stop();                                      // stop animation
	void Revert();                                    // stop animation
	void UpdateFrame();	                              // update animation

protected:

	float mSpeed;
};

// * Animation modifier class
// * Animation : Vertical motion (left/right scan)
class CAnimationModifier_HScan : public CAnimationModifier
{
public:
	void Attach(TVertex* pVertexList);                // attach image vertices
	void Reset(int pParam1 = 0, int pParam2 = 0);     // reset animation
	void Start();                                     // start animation
	void Stop();                                      // stop animation
	void Revert();                                    // stop animation
	void UpdateFrame();	                              // update animation

protected:

	float mSpeed;
};

// * Animation modifier class
// * Animation : Scale in from null, to full size with alpha blending
class CAnimationModifier_ScaleIn : public CAnimationModifier
{
public:
	void Attach(TVertex* pVertexList);                // attach image vertices
	void Reset(int pParam1 = 0, int pParam2 = 0);     // reset animation
	void Start();                                     // start animation
	void Stop();                                      // stop animation
	void Revert();                                    // stop animation
	void UpdateFrame();	                              // update animation

protected:

	float mSpeedX, mSpeedY;
	float mFadeSpeed;
};

// * Animation modifier class
// * Animation : Scale in from null, to full size with alpha blending (horizontal only)
class CAnimationModifier_ScaleInH : public CAnimationModifier
{
public:
	void Attach(TVertex* pVertexList);                // attach image vertices
	void Reset(int pParam1 = 0, int pParam2 = 0);     // reset animation
	void Start();                                     // start animation
	void Stop();                                      // stop animation
	void Revert();                                    // stop animation
	void UpdateFrame();	                              // update animation

protected:

	float mSpeedX, mSpeedY;
	float mFadeSpeed;
};

// * Animation modifier class
// * Animation : Scale in from null, to full size with alpha blending (horizontal only), from center
class CAnimationModifier_ScaleInHCenter : public CAnimationModifier
{
public:
	void Attach(TVertex* pVertexList);                // attach image vertices
	void Reset(int pParam1 = 0, int pParam2 = 0);     // reset animation
	void Start();                                     // start animation
	void Stop();                                      // stop animation
	void Revert();                                    // stop animation
	void UpdateFrame();	                              // update animation

protected:

	float mSpeedX, mSpeedY;
	float mFadeSpeed;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Bitmap font management class
/*
class heBitmapFont
{
public:
    heBitmapFont();
    ~heBitmapFont();

    void LoadFromFile(char* filename);
    void DrawText(int x, int y, char* pText, int r, int g, int b);
    void DrawChar(int x, int y, char c, int color);
    void DrawText2(int x, int y, char* pText, int r, int g, int b);

protected:

    DXCustomVertex          vertices[4];
    DXCustomVertex          mVertices[2048];

	char  m_filename[1024];
	int   m_width,m_height,m_adjustToScreen;

	// * Member variables
	TVertex mVertexList[4];   
	int     mAlphaOn;

	DXCustomVertex   *pVertices;
};

*/
#endif
