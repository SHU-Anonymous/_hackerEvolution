/*
Name: sys_main.cpp
Desc: applicatione entry point (win32)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
        Major overhaull: December 2014
*/

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/hedGameSettings.h"
#include "../generic - inc/hedGameEngine.h"
#include "../generic - inc/hedSystem.h"


#ifdef BUILD_STEAM
#include "steam_api.h"
#endif

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#ifdef PLATFORM_WINDOWS
#include <windows.h>							// Header File For Windows
#include "resource1.h"
#endif

#ifdef PLATFORM_LINUX
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#endif

int       quit = 0;

//=======================================================================================================================================
// Global variables
hedGameEngine gameEngine;                                   // Game engine interface

// =============================================================================================================================
// 2014 code
int main( int argc, char* args[])
{
	SDL_Event    sdlEvent;
	int          startTime = 0,endTime = 1, frameTime = 1;
	int          click           = 0;
	int          key             = 0;

#ifdef BUILD_STEAM
    SteamAPI_RestartAppIfNecessary(70120);
    SteamAPI_Init();
#endif
    

#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);
    
    chdir(path);
    printf("\nCurrent Path: %s",path);
#endif

	//Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );

#ifdef PLATFORM_LINUX
    freopen( "CON", "w", stdout );
#endif

	// System initialization
	sysInit();
	sysLogPrint("(sys_main): Starting Hacker Evolution Duality - %s-%s",GAME_VERSION,GAME_PLATFORM);
	sysPopulateModList();

    // Load settings
    g_GameSettings.Load(sysGetAppDataPath("settings.ini"));

    // Windowed or full screen mode?
    g_FullScreen = g_GameSettings.ValueInt("video_fullscreen");

	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,4096);
	Mix_AllocateChannels(8);
	SDL_ShowCursor(0);

	// Enumerate resolutions
	SDL_DisplayMode target, closest;
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);

	// Set the desired resolution, etc.
	target.w = 1280;
	target.h = 800;
	target.format = 0;  // don't care
	target.refresh_rate = 0; // don't care
	target.driverdata   = 0; // initialize to 0

	if(SDL_GetCurrentDisplayMode(0,&closest) != 0)
	{
		sysLogPrint("main()- SDL_GetCurrentDisplayMode() failed.");
		return 0;
	}

	// On multiple monitor displays, force windowed mode
	if(SDL_GetNumVideoDisplays() > 1)
	{
		g_FullScreen = 1;
	}

    //Set up screen
	if(g_FullScreen == 0)
	{
		sdlWindow   = SDL_CreateWindow("Hacker Evolution Duality", (current.w - 1280) / 2, (current.h - 800) / 2, 1280, 800, SDL_WINDOW_SHOWN);
		g_DisplayWidth  = 1280;
		g_DisplayHeight = 800;
	}
	else
	{

#ifdef PLATFORM_WINDOWS
		sdlWindow   = SDL_CreateWindow("Hacker Evolution Duality", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, closest.w, closest.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
#endif

#ifdef PLATFORM_MACOS
		sdlWindow   = SDL_CreateWindow("Hacker Evolution Duality", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, closest.w, closest.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
#endif

#ifdef PLATFORM_LINUX
		sdlWindow   = SDL_CreateWindow("Hacker Evolution Duality", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, closest.w, closest.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
#endif
        
		g_DisplayWidth  = closest.w;
		g_DisplayHeight = closest.h;
	}

	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    IMG_Init(IMG_INIT_PNG);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); 

	// Initialize the game engine
	gameEngine.Initialize();
	gameEngine.SetDisplaySize(g_DisplayWidth,g_DisplayHeight);


	startTime = SDL_GetTicks();
	SDL_StartTextInput();
	char sdlTextInputChar = 0;

	do
	{
		click = 0;
		key   = 0;

		// Process events
		SDL_PumpEvents();
		while (SDL_PollEvent(&sdlEvent))
		{
			// TEXT Input
            sdlTextInputChar = 0;
			switch (sdlEvent.type)
            {
                case SDL_TEXTINPUT:
                    /* Add new text onto the end of our text */
					sdlTextInputChar = sdlEvent.text.text[0];

					// SDL TEXT INPUT
					if(sdlTextInputChar != 0)
					{
						char c = tolower(sdlTextInputChar);
						if(c >= 'a' && c <= 'z') key = c;
						if(c >= '0' && c <= '9') key = c;
						if(c == ' '  || c == ':' || c == '_' || c == '.' || c == '-') key = c;
					}
					break;
                case SDL_TEXTEDITING:

                    break;
            }

			// Handle ALT+TAB
			if(sdlEvent.type == SDL_WINDOWEVENT)
			{
				//if(sdlEvent.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				//	gFocusLost = 1;
			}

			// Handle keyboard input
			if (sdlEvent.type == SDL_KEYDOWN)
			{

				// Special keys
				int special = 0;

				if(sdlEvent.key.keysym.sym == SDLK_RETURN)    { key = HEDKEY_ENTER; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_ESCAPE)    { key = HEDKEY_ESCAPE; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_BACKSPACE) { key = HEDKEY_BACKSPACE; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_HOME)      { key = HEDKEY_HOME; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_TAB)       { key = HEDKEY_TAB; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_PAGEUP)    { key = HEDKEY_PGUP; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_PAGEDOWN)  { key = HEDKEY_PGDN; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_UP)        { key = HEDKEY_UP; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_DOWN)      { key = HEDKEY_DOWN; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_LEFT)      { key = HEDKEY_LEFT; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_RIGHT)     { key = HEDKEY_RIGHT; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_END)       { key = HEDKEY_END; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_INSERT)    { key = HEDKEY_INS; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_PAUSE)     { key = HEDKEY_PAUSE; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_DELETE)    { key = HEDKEY_DEL; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F1)        { key = HEDKEY_F1; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F2)        { key = HEDKEY_F2; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_F3)        { key = HEDKEY_F3; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_F4)        { key = HEDKEY_F4; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_F5)        { key = HEDKEY_F5; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_F6)        { key = HEDKEY_F6; special = 1;}
				//if(sdlEvent.key.keysym.sym == SDLK_F7)        { key = HEDKEY_F7; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F8)       { key = HEDKEY_F8; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F9)       { key = HEDKEY_F9; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F10)       { key = HEDKEY_F10; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F11)       { key = HEDKEY_F11; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F12)       { key = HEDKEY_F12; special = 1;}
			}

			// Handle mouse input
			if(sdlEvent.type == SDL_MOUSEMOTION)
			{
				mouseX = sdlEvent.motion.x;
				mouseY = sdlEvent.motion.y;
			}

			if(sdlEvent.type == SDL_MOUSEBUTTONDOWN)
			{
				if(sdlEvent.button.button == SDL_BUTTON_LEFT)  _mouseLBDown = 1;
				if(sdlEvent.button.button == SDL_BUTTON_RIGHT) _mouseRBDown = 1;
			}

			if(sdlEvent.type == SDL_MOUSEBUTTONUP)
			{
				if(sdlEvent.button.button == SDL_BUTTON_LEFT)  {_mouseLBDown = 0; click = 1;}
				if(sdlEvent.button.button == SDL_BUTTON_RIGHT) _mouseRBDown = 0;
			}
		}

		// Render the frame
		SDL_RenderClear(sdlRenderer);

		// Update game engine
		//key = key;

		gameEngine.SetInput(mouseX, mouseY, click, _mouseLBDown, key);
		if(gameEngine.MainLoop(frameTime) == 0) quit = 1;

		// Compute frame time
		endTime = SDL_GetTicks();
		frameTime = endTime - startTime;
		startTime = endTime;

		SDL_RenderPresent(sdlRenderer);
	}while(quit == 0);

	Mix_CloseAudio();
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);

	sysLogPrint("(sys_main): Shutdown");
	sysShutdown();

	return 0;
}

