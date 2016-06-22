// HackerEvolution.cpp : Defines the entry point for the application.
//

#include "heEngine.h"
#include "bshInput.h"
#include "HackerEvolutionDefines.h"
#include "heLog.h"
#include "bshAudioDirectX.h"
#include "moduleRender.h"
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"


#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include "resource.h"
#endif

int sysAZERTYKeyboard = 0;

// =============================================================================================================================
// 2014 code
int main( int argc, char* args[])
{
	SDL_Event    sdlEvent;
	int          startTime = 0,endTime = 1, frameTime = 1;
	int          shiftKeyPressed = 0;

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

#ifdef PLATFORM_WINDOWS
	// Detect AZERTY keyboards

    char keyboardType[1024];
    strcpy(keyboardType,"he-unknown");

	if(GetKeyboardLayoutName(keyboardType))
        if(strcmp(keyboardType,"0000040C") == 0 || strcmp(keyboardType,"0000080C") == 0)
		{
			gApplicationLog.printf("main()-detected AZERTY keyboard \n");
            sysAZERTYKeyboard = 1;
		}
#endif

	loadGameOptions();

	//Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );

#ifdef PLATFORM_LINUX
    freopen( "CON", "w", stdout );
#endif

	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,4096);
	Mix_AllocateChannels(8);
	void audio_Init();

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

	if (SDL_GetClosestDisplayMode(0, &target, &closest) == NULL)
	{
		gApplicationLog.printf("\nmain()-No suitable display mode was found!\n\n");
		
		// In this case force the game to run with whatever resolution we have.
		closest.w = current.w;
		closest.h = current.h;
	}
	else
		gApplicationLog.printf("main()-SDL suggested resolution: \t%dx%dpx @ %dhz \n", closest.w, closest.h, closest.refresh_rate);

	// On multiple monitor displays, force windowed mode
	if(SDL_GetNumVideoDisplays() > 1)
	{
		//gFullScreen = 1;
	}

    //Set up screen
	if(gFullScreen == 1)
	{
		sdlWindow   = SDL_CreateWindow("Hacker Evolution: Untold", (current.w - 1280) / 2, (current.h - 800) / 2, 1280, 800, SDL_WINDOW_SHOWN);
		g_screen_width  = 1280;
		g_screen_height = 800;
	}
	else
	{
#ifdef PLATFORM_WINDOWS
		//sdlWindow   = SDL_CreateWindow("Hacker Evolution: Untold", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, closest.w, closest.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
		
		// Multimonitor support
		sdlWindow   = SDL_CreateWindow("Hacker Evolution: Untold", SDL_WINDOWPOS_CENTERED_DISPLAY(0), SDL_WINDOWPOS_CENTERED_DISPLAY(1), closest.w, closest.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
#endif

#ifdef PLATFORM_MACOS
		sdlWindow   = SDL_CreateWindow("Hacker Evolution: Untold", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, closest.w, closest.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
#endif

#ifdef PLATFORM_LINUX
		sdlWindow   = SDL_CreateWindow("Hacker Evolution: Untold", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, closest.w, closest.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
#endif
        
		g_screen_width  = closest.w;
		g_screen_height = closest.h;
	}

	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    IMG_Init(IMG_INIT_PNG);

	// Initialize game engine
	initGameEngine();
	input_Init();
	keyCount = 0;

	startTime = SDL_GetTicks();

	SDL_StartTextInput();
	char sdlTextInputChar = 0;

	do
	{
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
                    //strcat(text, event.text.text);
					//gApplicationLog.printf("\n[SDL_TEXTINPUT][%s]",sdlEvent.text.text);
					sdlTextInputChar = sdlEvent.text.text[0];

					// SDL TEXT INPUT
					if(sdlTextInputChar != 0)
					{
						char c = tolower(sdlTextInputChar);
						if(c >= 'a' && c <= 'z') KeyList[keyCount] = c;
						if(c >= '0' && c <= '9') KeyList[keyCount] = c;
						if(c == ' '  || c == ':' || c == '_' || c == '.' || c == '-') KeyList[keyCount] = c;
						keyCount++;
					}
					break;
                case SDL_TEXTEDITING:
                    /*
                    Update the composition text.
                    Update the cursor position.
                    Update the selection length (if any).
                    */
                    //composition = event.edit.text;
                    //cursor = event.edit.start;
                    //selection_len = event.edit.length;
                    break;
            }

			// Handle ALT+TAB
			if(sdlEvent.type == SDL_WINDOWEVENT)
			{
				if(sdlEvent.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
					gFocusLost = 1;
			}

			// Handle keyboard input
			if (sdlEvent.type == SDL_KEYDOWN)
			{

				// Special keys
				int special = 0;

				if(sdlEvent.key.keysym.sym == SDLK_RETURN)    { KeyList[keyCount] = HEKEY_ENTER; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_ESCAPE)    { KeyList[keyCount] = HEKEY_ESC; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_BACKSPACE) { KeyList[keyCount] = HEKEY_BACKSPACE; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_HOME)      { KeyList[keyCount] = HEKEY_HOME; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_TAB)       { KeyList[keyCount] = HEKEY_TAB; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_PAGEUP)    { KeyList[keyCount] = HEKEY_PGUP; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_PAGEDOWN)  { KeyList[keyCount] = HEKEY_PGDN; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_UP)        { KeyList[keyCount] = HEKEY_UP; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_DOWN)      { KeyList[keyCount] = HEKEY_DOWN; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_LEFT)      { KeyList[keyCount] = HEKEY_LEFT; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_RIGHT)     { KeyList[keyCount] = HEKEY_RIGHT; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_END)       { KeyList[keyCount] = HEKEY_END; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_INSERT)    { KeyList[keyCount] = HEKEY_INS; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_PAUSE)     { KeyList[keyCount] = HEKEY_PAUSE; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_DELETE)    { KeyList[keyCount] = HEKEY_DEL; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F1)        { KeyList[keyCount] = HEKEY_F1; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F2)        { KeyList[keyCount] = HEKEY_F2; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F3)        { KeyList[keyCount] = HEKEY_F3; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F4)        { KeyList[keyCount] = HEKEY_F4; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F5)        { KeyList[keyCount] = HEKEY_F5; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F6)        { KeyList[keyCount] = HEKEY_F6; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F7)        { KeyList[keyCount] = HEKEY_F7; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F10)       { KeyList[keyCount] = HEKEY_F10; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F11)       { KeyList[keyCount] = HEKEY_F11; keyCount++; special = 1;}
				if(sdlEvent.key.keysym.sym == SDLK_F12)       { KeyList[keyCount] = HEKEY_F12; keyCount++; special = 1;}
			}

			// Handle mouse input
			if(sdlEvent.type == SDL_MOUSEMOTION)
			{
				mouseX = sdlEvent.motion.x;
				mouseY = sdlEvent.motion.y;
			}

			if(sdlEvent.type == SDL_MOUSEBUTTONDOWN)
			{
				if(sdlEvent.button.button == SDL_BUTTON_LEFT)  {_mouseLBDown = 1; _mouseLBReleased = 0;}
				if(sdlEvent.button.button == SDL_BUTTON_RIGHT) _mouseRBDown = 1;
			}

			if(sdlEvent.type == SDL_MOUSEBUTTONUP)
			{
				if(sdlEvent.button.button == SDL_BUTTON_LEFT)  {_mouseLBDown = 0; _mouseLBReleased = 1;}
				if(sdlEvent.button.button == SDL_BUTTON_RIGHT) _mouseRBDown = 0;
			}
		}

		// Render the frame
		SDL_RenderClear(sdlRenderer);

		// Update game engine
		updateGameEngine(frameTime);

		// Compute frame time
		endTime = SDL_GetTicks();
		frameTime = endTime - startTime;
		startTime = endTime;

		SDL_RenderPresent(sdlRenderer);
	}while(quit == 0);

	Mix_CloseAudio();
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);

	closeGameEngine();
	input_Close();

	return 0;
}

