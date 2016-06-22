// heEngine.h
// 
// Game engine
// Code: Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "heEngine.h"
#include "heGameWindow.h"
#include "heSystem.h"
#include "bshInput.h"
#include "moduleRender.h"
#include "heLog.h"
#include "HackerEvolutionDefines.h"
#include "HackerEvolutionLanguage.h"
#include "bshAudioPlaylist.h"
#include "heKeyStack.h"
#include "hePlayerInfo.h"
#include "heCommandEngine.h"
#include "heGameLevel.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_IOS)
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

#if defined(PLATFORM_WINDOWS)
#include <io.h>
#endif

#include <stdio.h>
#include <stdlib.h>

//==============================================================================================================================
// 2014 update
int charTickCounter = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * These variables are to adjust to various screen resolutions
int gDeltaY;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Main menu items
#define MM_ITEMCOUNT 6
const char* menuItems[MM_ITEMCOUNT] = {MENU_PLAY, MENU_LOADGAME, MENU_LOADMOD, MENU_OPTIONS, MENU_CREDITS, MENU_EXIT};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables and classes
heGameLevel     gGameLevel; // The game level

// Display mode inside the level
#define DISPLAY_MODE_NORMAL           0
#define DISPLAY_MODE_QUICKHELP        1
#define DISPLAY_MODE_LEVELOBJECTIVES  2
#define DISPLAY_MODE_LEVELOVERVIEW    3
#define DISPLAY_MODE_QUIT             4
#define DISPLAY_MODE_PAUSE            5

int gDisplayMode = DISPLAY_MODE_NORMAL;

// Variables used during the intro screen rendering
int   gIntroFileOpen = 0, gIntroFileFinished = 0;
FILE* gIntroFile;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
heImage imageBackgroundMenu,imageBackgroundGame;
heImage imageMainMenu;

int   intro_duration = 10 * 1000;
int   intro_elapsed  = 0;
int   intro_color    = 0;
int   intro_sign     = 1;

// window offset, based on resolution
int woffset_x;

// 
#define MODE_INTRO               0
#define MODE_INTRO2              1
#define MODE_MENU                2
#define MODE_LEVELINTRO          3
#define MODE_GAME                4
#define MODE_PAUSED              5
#define MODE_LEVELCOMPLETED      6
#define MODE_LEVELLOST           7
#define MODE_MENU_NEWGAME        8
#define MODE_LEVELCOMPLETED_WAIT 9
#define MODE_MENU_LOAD           10
#define MODE_MENU_CREDITS        11
#define MODE_MENU_SELECTMOD      12
#define MODE_MENU_OPTIONS        13
#define MODE_MOD_COMPLETED       14

int gameMode;

// Game windows
heGameWindow windowConsole;
heGameWindow windowWorldMap;
heGameWindow gWindowSystemPanel;

heGameWindow gWindowMessages;

heKeyStack      commandConsole;
char            commandLine[MAX_COMMANDLINE_CHARS];
hePlayerInfo    hePlayer;

// * Special achievements
int gAchievement_ZeroTrace = 0;
int gAchievement_AllHosts  = 0;
int gAchievement_Under30   = 0;

//===============================================================================================
// Intro animation variables
int     introStarted = 0;
heImage introImage;
int     overlayColor = 0;
long    startTime;

#define INTRO_TOOLTIP_COUNT 5
#define INTRO_TOOLTIP_SIZE  60
#define INTRO_DESC_TOOLTIP_SIZE  200

int     intro_tooltip_id = 0;
int     intro_tooltip_x[INTRO_TOOLTIP_COUNT];
int     intro_tooltip_y[INTRO_TOOLTIP_COUNT];
int     intro_tooltip_size[INTRO_TOOLTIP_COUNT];

int     intro_desc_x, intro_desc_y, intro_desc_size;

heGameWindow intro_tooltip_window[INTRO_TOOLTIP_COUNT];
heGameWindow intro_desc_window;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Global variables
//
// * main game menu
int      gMenuX,gMenuY;
heImage  gImageCursorBackground[MM_ITEMCOUNT * 2];
heImage  gImageMainMenu_Bar;
int      gLastMenuItemIndex = -1;

heImage  gMenuSkinPreview;
heImage  gMenuArrowRight;
heImage  gMenuReturn;
heImage  gMenuVolumeBar;
heImage  gMenuVolumeBarSlider;
heImage  gMenuCheckTrue;
heImage  gMenuCheckFalse;

// * intro sequence
#define INTRO_WINDOW_COUNT 5
heImage gImageIntroWindow[INTRO_WINDOW_COUNT];
int     gImageIntroWindowIndex = 0;
int     gIntroTextIndex,gIntroTextPos;

double  gIntroWaitTime = 0;  

char*   intro_tooltip_text[INTRO_WINDOW_COUNT] = {INTRO_TOOLTIP_1, INTRO_TOOLTIP_2, INTRO_TOOLTIP_3, INTRO_TOOLTIP_4,
INTRO_TOOLTIP_5};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Level intro elements
heImage      gImageLevelIntroWindow;
heImage      gImageLevelIntroLocation;
heGameWindow gWindowLevelIntroLocation;
heGameWindow gWindowLevelIntroDescription;
heImage      gImageModCompleted;

// * Level completed elements
heImage      gImageLevelCompletedWindow;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Game interface
heImage      gWindowSlider;
heImage      gMouseCursor;
heImage      gHostToolTip;
heImage      gHostToolTipScan;
heImage      gWindowMessagesHighlight;

heGameWindow gWindowQuickHelp;
heGameWindow gWindowLevelObjectives;
heGameWindow gWindowLevelOverview;

heGameWindow gWindowHelpTooltip;

heGameWindow gWindowQuitLevel;
heGameWindow gWindowPaused;

heImage      gImageCursorBackgroundQuit[2];
int          gQuitMenuIndex = -1;

// * Player hardware
heImage      gHardwareCPU1[HE_MAX_CPU];
heImage      gHardwareCPU2[HE_MAX_CPU];
heImage      gHardwareMem1[HE_MAX_MEM];
heImage      gHardwareMem2[HE_MAX_MEM];
heImage      gHardwareModem[HE_MAX_MODEM];
heImage      gHardwareFirewall[HE_MAX_FIREWALL];
heImage      gHardwareNNAdapter;

int          gToolTipStartTime;
hestring     gPlayerNickname;
hestring     gPlayerNicknameError;

int          gLevelDoneTime;
extern int   gSkipLevel;

// * Engine stats and performance
double gAverageFps = 0;
float  gCurrentFps = 0;
long   gFrameCount = 0;
long   gLevelTime  = 0;

// * Mouse click
int   gMouseLBClick = 0;
int   gMouseRBClick = 0;

// * Keyboard or mouse navigation (for the menus)
int   gKeyboardMode = 0;
int   gLastMouseX, gLastMouseY;

// * Player profiles
hestring gProfileFileNames[HE_MAX_PROFILES];
hestring gProfileNames[HE_MAX_PROFILES];
int      gProfileCount = 0;

// * Game mods
hestring gModFileNames[HE_MAX_PROFILES];
hestring gModNames[HE_MAX_PROFILES];
int      gModCount = 0;

// * Interface skins
hestring gSkinNames[HE_MAX_PROFILES];
int      gSkinCount = 0;
int      gSkinIndex = 0;

void loadProfiles();
void loadMods();
void loadSkins();

// * Load and save general game options
void saveGameOptions();
void loadGameOptions();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Bounced links 
int gBouncedLinkCount = 0;
int gBouncedLinkHostList[HE_MAX_BOUNCEDLINK];

//===============================================================================================
// Initialize game engine
void initGameEngine()
{
	int i;

    gApplicationLog.printf("\nModule::heEngine - Starting...");

	strcpy(gSkinName,"terminal");
	loadSkinColors();

    gApplicationLog.printf("\nModule::heEngine - Starting renderer...");

	if (render_Init() == -1) return;

	// * Load generic game options
	loadGameOptions();
	setGameMod(gModPath);

	if(gDemo == 1)
		imageBackgroundMenu.LoadFromFile("he-graphics/mainmenu-background-demo.png",1280,768,1);
	else
	
		imageBackgroundMenu.LoadFromFile("he-graphics/mainmenu-background.png",1280,768,1);    
	imageBackgroundGame.LoadFromFile("he-graphics/main-background.png",1280,768,1);

	// * Load main menu graphics
	imageMainMenu.LoadFromFile("he-graphics/mainmenu-window.png",354,203);
	imageMainMenu.AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
	imageMainMenu.mAnimationModifier->Reset();
	imageMainMenu.mAnimationModifier->Start();

	gImageMainMenu_Bar.LoadFromFile("he-graphics/mainmenu-animbar.png",1024,16);
	gImageMainMenu_Bar.AttachAnimationModifier(new CAnimationModifier_VScan);
	gImageMainMenu_Bar.mAnimationModifier->Reset();
	gImageMainMenu_Bar.mAnimationModifier->Start();

	gMenuArrowRight.LoadFromFile("he-graphics/mainmenu-arrowright.png",20,20);
	gMenuReturn.LoadFromFile("he-graphics/mainmenu-return.png",80,20);
	gMenuVolumeBar.LoadFromFile("he-graphics/mainmenu-volumebar.png",300,10);
	gMenuVolumeBarSlider.LoadFromFile("he-graphics/mainmenu-volumebar-slider.png",8,8);

    gMenuCheckTrue.LoadFromFile("he-graphics/mainmenu-check-true.png",20,20);
    gMenuCheckFalse.LoadFromFile("he-graphics/mainmenu-check-false.png",20,20);

	for(i = 0; i < MM_ITEMCOUNT; i++)
	{
		gImageCursorBackground[i].LoadFromFile("he-graphics/mainmenu-window-cursorbackground.png",310,20);		

		// * Attach animation modifier
		gImageCursorBackground[i].AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
		gImageCursorBackground[i].mAnimationModifier->Reset();
	}
	
	gMenuX = MM_MENU_X - ((1280.0f - g_screen_width) / 2);
	gMenuY = MM_MENU_Y;

	// * Load mouse cursor
	gMouseCursor.LoadFromFile(createSkinPath("mouse.png"),20,20);	

	woffset_x = (g_screen_width - 1024) / 3;
	gGameLevel.woffset_x = woffset_x;

	// Initialize playlist
	//directXAudio_Initialize(hWndMain);	

	// * Start intro
	gameMode = MODE_INTRO;

	// Register console commands
	register_Command("help",command_Help);
	register_Command("scan",command_Scan);
	register_Command("connect",command_Connect);
	register_Command("logout",command_Logout);
	register_Command("ls",command_LS);
	register_Command("cat",command_Cat);
	register_Command("download",command_Download);
	register_Command("upload",command_Upload);
	register_Command("exec",command_Exec);
	register_Command("clear",command_Clear);
	register_Command("crack",command_Crack);
	register_Command("decrypt",command_Decrypt);
	register_Command("transfer",command_Transfer);
	register_Command("abort",command_Abort);
	register_Command("config",command_Config);
	register_Command("login",command_Login);
	register_Command("upgrade",command_Upgrade);
	register_Command("bouncehelp",command_BounceHelp);
	register_Command("bounceinfo",command_BounceInfo);
	register_Command("killtrace",command_KillTrace);
	register_Command("delete",command_Delete);
	register_Command("downgrade",command_Downgrade);
	register_Command("bounce",command_Bounce);

	// * Load command aliases and register them
	gApplicationLog.printf("\n initGameEngine() - loading command aliases");

	FILE*     aliasFile;
	hestring  commandAlias, commandName;
	hestring  temp;
	heCommand *commandHandler;

	aliasFile = fopen("he-data/command.alias","rt");
	while(fgets(temp,1024,aliasFile))
	{
		// check if line is not a comment
		if(temp[0] != ';')
		{
			sscanf(temp,"%s %s",commandAlias,commandName);
			commandHandler = getHandler_Command(commandName);

			if(commandHandler == 0)
				gApplicationLog.printf("\nError (alias): No such command: [%s]",commandName);
			else
			{				
				// Register command alias (and make sure we avoid duplicates)
				if(getHandler_Command(commandAlias) == 0)
				{
					register_Command(commandAlias,commandHandler->commandHandler);
					gApplicationLog.printf("\nAdded alias: [%s] -> [%s]",commandAlias, commandName);
				}
			}
		}
	}
	fclose(aliasFile);

	// * Cheat commands
#ifdef ENABLE_CHEATS
	register_Command("xskiplevel",command_Cheat_SkipLevel);
	register_Command("xmoney",command_Cheat_Money);
	register_Command("xshowhosts",command_Cheat_ShowHosts);
#endif

    // * Easter egg :)
    register_Command("exosyphen",command_EasterEgg_exosyphen);
    // register_Command("xsubmit",command_SubmitHiscore);   
    
    // * For screen adjustment
    gDeltaY = (g_screen_height - 768) / 2;
    gMenuY += gDeltaY;
}

//===============================================================================================
// Update game engine (run a game frame)
void updateGameEngine(long frame_time)
{
	// Set global frametime variable
	gFrameTime = frame_time / 1000.0f;
	gAverageFps += 1000.0f / (float) frame_time;
	gCurrentFps  = 1000.0f / (float) frame_time;
	gFrameCount++;

	render_BeginFrame(1);

	// Game intro mode
	if(gameMode == MODE_INTRO)
	{
		intro_elapsed += frame_time;
		if(intro_elapsed > intro_duration)
		{
			intro_elapsed = 0;
			gameMode = MODE_INTRO2;
            gApplicationLog.printf("\nupdateGameEngine() - finished first intro sequence");

#ifdef MOD_REINSERTION
            gameMode = MODE_MENU;

			// Load and play menu playlist
			gameMusic.loadPlayList("he-music/he-playlist-menu.txt");
			gameMusic.Play();

            // * Sound fx
            gameMusic.PlayWav("he-soundfx/voice-welcome.wav");
#endif
		}

		// Render intro text
		render_DrawText_Large(g_screen_width / 2, g_screen_height / 2,INTRO_TEXT,intro_color,intro_color,intro_color,0,1);
		
		// Fade color in;
		intro_color += intro_sign;
		if(intro_color > 255) {intro_color = 255; intro_sign = -1;}
		if(intro_color ==  0) {intro_color = 0;   intro_sign = 0;}

		// Continue to level if ENTER is pressed or the continue button is clicked
		if(input_getLastKey() == HEKEY_ENTER || mouseLBClick() == 1)
        {
			gameMode = MODE_INTRO2;
#ifdef MOD_REINSERTION
            gameMode = MODE_MENU;
			// Load and play menu playlist
			gameMusic.loadPlayList("he-music/he-playlist-menu.txt");
			gameMusic.Play();
            // * Sound fx
            gameMusic.PlayWav("he-soundfx/voice-welcome.wav");
#endif
        }
	}
	// ======================================================================================================================
	// Intro animation
	if(gameMode == MODE_INTRO2)
	{
		int done;
		done = runIntro();

		// Skin on mouse click or enter
		if(input_getLastKey() == HEKEY_ENTER || mouseLBClick() == 1)
			done = 1;

		if(done == 1)
		{
			gameMode = MODE_MENU;

			// Load and play menu playlist
			gameMusic.loadPlayList("he-music/he-playlist-menu.txt");
			gameMusic.Play();

            // * Sound fx
            gameMusic.PlayWav("he-soundfx/voice-welcome.wav");
		}
	}
	// ======================================================================================================================
	// Game menu mode
	if(gameMode == MODE_MENU)
	{
		static int itemIndex = -1;

		imageBackgroundMenu.Draw(0,gDeltaY);		

		gImageMainMenu_Bar.Draw(315 - ((1280.0f - g_screen_width) / 2), 130 + gDeltaY);
		imageMainMenu.Draw(350 - ((1280.0f - g_screen_width) / 2),200 + gDeltaY);		

		// * Find active menu item
		if(gKeyboardMode == 0)
		{
			if(mouseX < gMenuX + 310 && mouseX > gMenuX)
			{
				itemIndex = (mouseY - gMenuY) / 25;
				itemIndex--;
			}
		}

		// * Keyboard navigation?
		int key = input_getLastKey();

		if(gKeyboardMode == 0)
		{
			// * Switch to keyboard input
			if(key == HEKEY_UP || key == HEKEY_DOWN)
			{
				itemIndex = 0;
				gLastMouseX = mouseX;
				gLastMouseY = mouseY;
				gKeyboardMode = 1;
			}
		}
		else
		{
			if(key == HEKEY_UP)   itemIndex--;
			if(key == HEKEY_DOWN) itemIndex++;

			if(itemIndex > 5) itemIndex = 0;
			if(itemIndex < 0) itemIndex = 5;

			// * Switch to mouse mode
			if(mouseX != gLastMouseX || mouseY != gLastMouseY)
			{
				gApplicationLog.printf("\nKEYBOARD OFF");
				gKeyboardMode = 0;
			}
		}

		// * Draw menu items
		for(int i = 0; i < MM_ITEMCOUNT; i++)
		{
			gImageCursorBackground[i].Draw(gMenuX + 5, gMenuY + 4 + 25 * (i + 1));
			
			// 2014 update
			if(i != itemIndex)
			{
				if(i == 2 && gDemo == 1)
					render_DrawText_Large(gMenuX + 10, gMenuY + 8 + 25 * (i + 1), (char*)MENU_LOADMOD_DEMO, MM_COLOR_1,0);
				else
					render_DrawText_Large(gMenuX + 10, gMenuY + 8 + 25 * (i + 1), (char*)menuItems[i], MM_COLOR_1,0);
			}
		}

		// * Display game mode name
		render_DrawText_Formated(gMenuX + 10, gMenuY + 25 * 8 + 5, MM_COLOR_1,"%s: %s",LANG_TEXT_CURRENTMOD,gModName);

		// * Render selected menu item
		if(itemIndex >= 0 && itemIndex < MM_ITEMCOUNT)
		{
			if(itemIndex != gLastMenuItemIndex)
			{
				if(gLastMenuItemIndex != -1) 
				{
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Start();
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Revert();
					gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
				}

				gLastMenuItemIndex = itemIndex;

				if(itemIndex != -1)
					gImageCursorBackground[itemIndex].mAnimationModifier->Start();
			}
			
			// 2014 update
			if(itemIndex == 2 && gDemo == 1)
				render_DrawText_Large(gMenuX + 10, gMenuY + 8 + 25 * (itemIndex + 1), (char*)MENU_LOADMOD_DEMO, MM_COLOR_2,0,0);
			else
				render_DrawText_Large(gMenuX + 10, gMenuY + 8 + 25 * (itemIndex + 1), (char*)menuItems[itemIndex], MM_COLOR_2,0,0);

			// Sound click on mouse-over
			if(gLastMenuItemIndex != itemIndex)
			{
				gLastMenuItemIndex = itemIndex;
				gameMusic.PlayWav("he-soundfx/hesfx-click.wav");
			}

			// Check if player clicked on any menu item
			if(mouseLBClick() == 1 || key == HEKEY_ENTER)
			{
				// Start new game
				if(itemIndex == 0)
				{
					strcpy(gPlayerNickname,"");
					strcpy(gPlayerNicknameError,"");

					gameMode = MODE_MENU_NEWGAME;
                    // * Sound fx
                    gameMusic.PlayWav("he-soundfx/voice-piy.wav");

				}
				// * Load profile
				if(itemIndex == 1)
				{
					loadProfiles();
					gameMode = MODE_MENU_LOAD;
				}

				// * Select game mod
				if(itemIndex == 2 && gDemo == 0)
				{
					loadMods();
					gameMode = MODE_MENU_SELECTMOD;
				}
				// * Game Options
				if(itemIndex == 3)
				{
					// * Load skins, and locate the current skin index
					loadSkins();
					for(int i = 0; i < gSkinCount; i++)
						if(strcmp(gSkinNames[i],gSkinName) == 0) gSkinIndex = i;

					gameMode = MODE_MENU_OPTIONS;
				}
				
				// * Game credits
				if(itemIndex == 4)
					gameMode = MODE_MENU_CREDITS;

				// * Exit the game
				if(itemIndex == 5) 
				{
					quit = 1;

					// * Save game options
					saveGameOptions();
				}
			}
		}
			
		// Display engine version and fps
		displayEngineInfo();

		// render mouse cursor		
		gMouseCursor.Draw(mouseX,mouseY);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load profile mode
	if(gameMode == MODE_MENU_LOAD)
	{
		int itemIndex = -1;

		imageBackgroundMenu.Draw(0,gDeltaY);		
		gImageMainMenu_Bar.Draw(315 - ((1280.0f - g_screen_width) / 2), 130 + gDeltaY);
		imageMainMenu.Draw(350 - ((1280.0f - g_screen_width) / 2),200 + gDeltaY);		

		// * Find active menu item
		if(mouseX < gMenuX + 310 && mouseX > gMenuX)
		{
			itemIndex = (mouseY - gMenuY) / 25;
			itemIndex--;
		}

		// * Draw menu items
		for(int i = 0; i < gProfileCount; i++)
		{
			gImageCursorBackground[i].Draw(gMenuX + 5, gMenuY + 4 + 25 * (i + 1));

			if(i != itemIndex)
				render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 10, (char*)gProfileNames[i], MM_COLOR_1,0);
		}

		// * Render selected menu item
		if(itemIndex >= 0 && itemIndex < gProfileCount)
		{
			if(itemIndex != gLastMenuItemIndex)
			{
				if(gLastMenuItemIndex != -1) 
				{
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Start();
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Revert();

					if(itemIndex <= MM_ITEMCOUNT)
						gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
				}

				gLastMenuItemIndex = itemIndex;

				if(itemIndex != -1)
					gImageCursorBackground[itemIndex].mAnimationModifier->Start();
			}

			render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (itemIndex + 1) + 10, (char*)gProfileNames[itemIndex], MM_COLOR_2,0,0);

			// Sound click on mouse-over
			if(gLastMenuItemIndex != itemIndex)
			{
				gLastMenuItemIndex = itemIndex;
				gameMusic.PlayWav("he-soundfx/hesfx-click.wav");
			}

			// Check if player clicked on any menu item
			if(mouseLBClick() == 1 && itemIndex < gProfileCount)
			{
				if(itemIndex == gProfileCount - 1)
					gameMode = MODE_MENU;
				else
				{

                    // * Sound fx
                    gameMusic.PlayWav("he-soundfx/voice-loading.wav");

					gameMode = MODE_LEVELINTRO;

                    // * Validate and load profile
                    if(hePlayer.validateSaveGame(gProfileFileNames[itemIndex]) != 0)
                    {
					    hePlayer.loadGame(gProfileFileNames[itemIndex]);
                        hePlayer.fileCount = 0;
					    strcpy(gPlayerNickname,hePlayer.mNickname);
					    setGameMod(gModPath);					 
                        init_NewGame(0);
                    }
                    else
                        gameMode = MODE_MENU;
				}
			}
		}

		// * Keyboard input handling
		int key = input_getLastKey();

		// * Return to main menu
		if(key == HEKEY_ESC)
			gameMode = MODE_MENU;

		// * Profile deletion
		if(key == HEKEY_DEL)
		{
			if(itemIndex >= 0 && itemIndex <= gProfileCount && gProfileCount > 1)
			{
				remove(gProfileFileNames[itemIndex]);
				gameMode = MODE_MENU;
			}
		}


		// * Display help tip
		render_DrawText_Formated(gMenuX + 10, gMenuY + 25 * 8 + 5, MM_COLOR_1,LANG_TEXT_LOADPROFILE);

		// Display engine version and fps
		displayEngineInfo();

		// render mouse cursor
		gMouseCursor.Draw(mouseX,mouseY);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load Mod mode
	if(gameMode == MODE_MENU_SELECTMOD)
	{
		int itemIndex = -1;

		imageBackgroundMenu.Draw(0,gDeltaY);		
		gImageMainMenu_Bar.Draw(315 - ((1280.0f - g_screen_width) / 2), 130 + gDeltaY);
		imageMainMenu.Draw(350 - ((1280.0f - g_screen_width) / 2),200 + gDeltaY);		

		// * Find active menu item
		if(mouseX < gMenuX + 310 && mouseX > gMenuX)
		{
			itemIndex = (mouseY - gMenuY) / 25;
			itemIndex--;
		}

		// * Draw menu items
		for(int i = 0; i < gModCount; i++)
		{
			gImageCursorBackground[i].Draw(gMenuX + 5, gMenuY + 4 + 25 * (i + 1));

			//if(i != itemIndex)
			//	render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 4, (char*)gModNames[i], MM_COLOR_1,0);

			// 2014 update
			if(i != itemIndex)
				render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 10, (char*)gModNames[i], MM_COLOR_1,0);
		}

		// * Render selected menu item
		if(itemIndex >= 0 && itemIndex < gModCount)
		{
			if(itemIndex != gLastMenuItemIndex)
			{
				if(gLastMenuItemIndex != -1) 
				{
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Start();
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Revert();

					if(itemIndex <= MM_ITEMCOUNT)
						gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
				}

				gLastMenuItemIndex = itemIndex;

				if(itemIndex != -1)
					gImageCursorBackground[itemIndex].mAnimationModifier->Start();
			}

			//render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (itemIndex + 1) + 4, (char*)gModNames[itemIndex], MM_COLOR_2,0,0);
			// 2014 updaye
			render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (itemIndex + 1) + 10, (char*)gModNames[itemIndex], MM_COLOR_2,0,0);

			// Sound click on mouse-over
			if(gLastMenuItemIndex != itemIndex)
			{
				gLastMenuItemIndex = itemIndex;
				gameMusic.PlayWav("he-soundfx/hesfx-click.wav");
			}

			// Check if player clicked on any menu item
			if(mouseLBClick() == 1 && itemIndex < gModCount)
			{
				if(itemIndex == gModCount - 1)
					gameMode = MODE_MENU;
				else
				{
					gameMode = MODE_MENU;
					strcpy(gModPath,gModFileNames[itemIndex]);
					strcpy(gModName,gModNames[itemIndex]);
					setGameMod(gModFileNames[itemIndex]);
				}
			}
		}

    	// * Display game mode name
		if(gDemo == 1)
			render_DrawText_Formated(gMenuX + 10, gMenuY + 25 * 8 + 5, MM_COLOR_1,"Game mods are unavailable in the demo.");

		// * Return to main menu
		if(input_getLastKey() == HEKEY_ESC)
			gameMode = MODE_MENU;

		// Display engine version and fps
		displayEngineInfo();

		// render mouse cursor
		gMouseCursor.Draw(mouseX,mouseY);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * Game options
	if(gameMode == MODE_MENU_OPTIONS)
	{
		imageBackgroundMenu.Draw(0,gDeltaY);		
		gImageMainMenu_Bar.Draw(315 - ((1280.0f - g_screen_width) / 2), 130 + gDeltaY); 
		imageMainMenu.Draw(350 - ((1280.0f - g_screen_width) / 2),200 + gDeltaY);		
		
		// * This is not the best code, but performance is really not an issue here :-)
		strcpy(gSkinName,gSkinNames[gSkinIndex]);
		loadSkinColors();

		gMenuSkinPreview.LoadFromFile(createSkinPath("preview.png"),240,144);

		int delta2014 = 6;
		// * Draw options
        // * return
		gMenuReturn.Draw(gMenuX + 10, gMenuY + 155 + 15);		

        // * music volume
		int _volume = gSoundVolume;
		hestring volumeTxt;
		sprintf(volumeTxt,"%s %d %%",LANG_TEXT_MVOL,_volume);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 135 + delta2014, volumeTxt, MM_COLOR_1,0);
		gMenuVolumeBar.Draw(gMenuX + 12,gMenuY + 155);

        if(_volume > 97) _volume = 97; // * This is to fit on screen
		gMenuVolumeBarSlider.Draw(gMenuX + 12 + _volume * 3,gMenuY + 156);		

        // * sound volume
		_volume = gSoundFXVolume;	
		sprintf(volumeTxt,"%s %d %%",LANG_TEXT_SFXVOL,_volume);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 105 + delta2014, volumeTxt, MM_COLOR_1,0);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 105 + delta2014, volumeTxt, MM_COLOR_1,0);
		gMenuVolumeBar.Draw(gMenuX + 12,gMenuY + 125);
        if(_volume > 97) _volume = 97; // * This is to fit on screen
		gMenuVolumeBarSlider.Draw(gMenuX + 12 + _volume * 3,gMenuY + 126);		

        // * skin option
        char temp[1024];
        sprintf(temp,"%s %s",LANG_TEXT_INTERFACESKIN,gSkinName);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 24 + delta2014, temp, MM_COLOR_1,0);
        //render_DrawText_Medium(gMenuX + 12 + 30,gMenuY + 50,"click here change the skin",MM_COLOR_1,0);
        //gMenuArrowRight.Draw(gMenuX + 12,gMenuY + 50);
		gMenuSkinPreview.Draw(gMenuX + 10 + 350, gMenuY + 29 + 5);

        // * serial number
		if(gDemo == 0)
		{
			if(gFullScreen == 0)
				gMenuCheckFalse.Draw(gMenuX + 12, gMenuY + 65); // 95
			else
				gMenuCheckTrue.Draw(gMenuX + 12, gMenuY + 65);
			//render_DrawText_Medium(gMenuX + 12 + 30,gMenuY + 65 + delta2014,LANG_TEXT_AUTOSUBMIT,MM_COLOR_1,0);
			render_DrawText_Medium(gMenuX + 12 + 30,gMenuY + 65 + delta2014,"Run game in windowed mode",MM_COLOR_1,0);

			// * dinamic difficulty
			if(gDinamicDifficulty == 0)
				gMenuCheckFalse.Draw(gMenuX + 12, gMenuY + 85); // 115
			else
				gMenuCheckTrue.Draw(gMenuX + 12, gMenuY + 85);

			render_DrawText_Medium(gMenuX + 12 + 30,gMenuY + 85 + delta2014,LANG_TEXT_DDIFFICULTY,MM_COLOR_1,0);		
		}
		// * Handle mouse
		if(mouseLBClick() == 1)
		{
			// * return button
			if(PointInRect(mouseX,mouseY,gMenuX + 10, gMenuY + 155 + 15,80,20) == 1)
			{
				gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
				gameMode = MODE_MENU;
                saveGameOptions();
			}

			// * scrolling through skins
            //if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 50,200,20) == 1)
            if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 24,300,20) == 1)
			{
				gSkinIndex++;
				if(gSkinIndex == gSkinCount) gSkinIndex = 0;
				gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
			}

			// * adjust volume
			if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 155,303,10) == 1)
			{
				_volume = (mouseX - gMenuX - 12) / 3;
				gSoundVolume = _volume;
				gameMusic.SetVolume(gSoundVolume);
                // gameMusic.PlayWav("he-soundfx/voice-secure.wav");
			}

			// * adjust fx volume
			if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 125,303,10) == 1)
			{
				_volume = (mouseX - gMenuX - 12) / 3;
				gSoundFXVolume = _volume;
				//gameMusic.SetVolume(gSoundVolume);
                gameMusic.PlayWav("he-soundfx/voice-secure.wav");
			}

            // * auto hiscore submit
			if(PointInRect(mouseX,mouseY,gMenuX + 12, gMenuY + 65,300,20) == 1)
            {
                gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
                if(gFullScreen == 0)
                    gFullScreen = 1;
                else
                    gFullScreen = 0;
            }

            // * dinamic difficulty
			if(PointInRect(mouseX,mouseY,gMenuX + 12, gMenuY + 85,300,20) == 1)
            {
                gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
                if(gDinamicDifficulty == 0)
                    gDinamicDifficulty = 1;
                else
                    gDinamicDifficulty = 0;
            }
		}

		int key = input_getLastKey();

#ifndef DEMO        
        // * Serial number input
        // SDLTODO key = toupper(key);
		if(key == HEKEY_BACKSPACE)
		{
			int index = strlen(gSerialNumber);
			gSerialNumber[index - 1] = 0;
		}

		if(isAscii(key) && strlen(gSerialNumber) < HE_MAX_SERIALNUMBER_LENGTH)
		{
			int index = strlen(gSerialNumber);
			gSerialNumber[index] = key;
			gSerialNumber[index + 1] = 0;
		}
#endif
		// * Return to main menu
		if(key == HEKEY_ESC || key == HEKEY_ENTER)
        {
            saveGameOptions();
			gameMode = MODE_MENU;
        }

		// * Display engine version and fps
		displayEngineInfo();

		// render mouse cursor
		gMouseCursor.Draw(mouseX,mouseY);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * Game credits
	if(gameMode == MODE_MENU_CREDITS)
	{
		imageBackgroundMenu.Draw(0,gDeltaY);		
		gImageMainMenu_Bar.Draw(315 - ((1280.0f - g_screen_width) / 2), 130 + gDeltaY);
		imageMainMenu.Draw(350 - ((1280.0f - g_screen_width) / 2),200 + gDeltaY);		

		render_DrawText_Medium(gMenuX + 10, gMenuY + 20 + 10,LANG_TEXT_CREDITS_1,MM_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 10, gMenuY + 50 + 10,LANG_TEXT_CREDITS_2,MM_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 10, gMenuY + 65 + 10,LANG_TEXT_CREDITS_3,MM_COLOR_1,0);

		render_DrawText_Medium(gMenuX + 10, gMenuY + 95 + 10, LANG_TEXT_CREDITS_4,MM_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 10, gMenuY + 110 + 10,LANG_TEXT_CREDITS_5,MM_COLOR_1,0);

		render_DrawText_Medium(gMenuX + 10, gMenuY + 140 + 10,LANG_TEXT_CREDITS_6,MM_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 10, gMenuY + 155 + 10,LANG_TEXT_CREDITS_7,MM_COLOR_1,0);

		// * Return to main menu
		int key = input_getLastKey();
		if(key == HEKEY_ESC || key == HEKEY_ENTER || mouseLBClick() == 1)
			gameMode = MODE_MENU;

		// * Display engine version and fps
		displayEngineInfo();

		// render mouse cursor
		gMouseCursor.Draw(mouseX,mouseY);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// New game mode
	if(gameMode == MODE_MENU_NEWGAME)
	{
		imageBackgroundMenu.Draw(0,gDeltaY);		
		gImageMainMenu_Bar.Draw(315 - ((1280.0f - g_screen_width) / 2), 130 + gDeltaY);
		imageMainMenu.Draw(350 - ((1280.0f - g_screen_width) / 2),200 + gDeltaY);		

		// Get input
		int key;
		key = input_getLastKey();

		// * Return to main menu, on ESC
		if(key == HEKEY_ESC)
			gameMode = MODE_MENU;


		if(key == HEKEY_ENTER || mouseLBClick() == 1)
		{
			int   profileOK = 1;
			FILE*    file;
			hestring fileName;

			// * check if nickname is valid
			if(strlen(gPlayerNickname) < HE_NICKNAME_MINLENGTH)
			{
				sprintf(gPlayerNicknameError,LANG_TEXT_NEWPROFILE2);
				profileOK = 0;
			} 

			// * make sure the profile doesn't already exist
			sprintf(fileName,"he-savegames/%s.profile",gPlayerNickname);
			file = fopen(fileName,"rt");
			if(file)
			{
				fclose(file);
				sprintf(gPlayerNicknameError,LANG_TEXT_NEWPROFILE3,gPlayerNickname);
				profileOK = 0;

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-access-denied.wav");
			}


			if(profileOK == 1)
			{

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-accepted.wav");
                system_Delay(2);

				strcpy(hePlayer.mNickname,gPlayerNickname);
				gameMode = MODE_LEVELINTRO;
				gModLevelCurrent = 0;
				init_NewGame();
			}
		}

		// * Nickname input

		if(key == HEKEY_BACKSPACE)
		{
			int index = strlen(gPlayerNickname);
			gPlayerNickname[index - 1] = 0;
		}

		if(isAscii(key))
		{
			int index = strlen(gPlayerNickname);
			gPlayerNickname[index] = key;
			gPlayerNickname[index + 1] = 0;

            // * Limit nickname length to 15 characters
            if(strlen(gPlayerNickname) > 15)
                gPlayerNickname[strlen(gPlayerNickname) - 1] = 0;
		}

		// Display text
		hestring temp;
		sprintf(temp,"> %s",gPlayerNickname);
		if(system_TimeMilliSeconds() % 1000 > 500) strcat(temp,"|");

		render_DrawText_Large(gMenuX + 10, gMenuY + 25,LANG_TEXT_NICKNAME,MM_COLOR_1,0);
		render_DrawText_Large(gMenuX + 10, gMenuY + 75,temp,MM_COLOR_1,0);
		render_DrawText(gMenuX + 5, gMenuY + 110,LANG_TEXT_NEWPROFILE,MM_COLOR_1,0);
		render_DrawText(gMenuX + 5, gMenuY + 125,gPlayerNicknameError,MM_COLOR_1,0);
		
		// Display engine version and fps
		displayEngineInfo();

		// render mouse cursor
		gMouseCursor.Draw(mouseX,mouseY);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level intro mode
	if(gameMode == MODE_LEVELINTRO)
	{
		imageBackgroundGame.Draw(0,gDeltaY);
		
		// * Draw intro graphics
		gImageLevelIntroWindow.Draw(240 - ((1280 - g_screen_width) / 2),134  + gDeltaY);
		gImageLevelIntroLocation.Draw(240 + 30 - ((1280 - g_screen_width) / 2),134 + 40 + gDeltaY);

		gWindowLevelIntroLocation.displayFile(createModLevelFilePath("levelintro-text-1"),1);

#ifndef DEMO
        // * Dinamic performance adjustment
        if(gDinamicDifficulty == 1)
        {
            gWindowLevelIntroLocation.printf(LANG_TEXT_MISC_GAMEDIFF,hePlayer.getDinamicPerformanceMultiplier());
        }
#endif		
		gWindowLevelIntroLocation.renderTextContent();
		gWindowLevelIntroDescription.renderTextContent();

		char  c;

		if(gIntroFileOpen == 0)
		{
			gIntroFile     = fopen(createModLevelFilePath("levelintro-text-2"),"rt");
			gIntroFileOpen = 1;
			gWindowLevelIntroDescription.clear();
		}

		
		if(gIntroFileFinished == 0 && gIntroFile != 0)
		{
			c = fgetc(gIntroFile);
			if(c == EOF)
			{
				gIntroFileFinished = 1;
				fclose(gIntroFile);
			}
			else
			{
				// Display character and play sound effect
                if(c != '\r')
                    gWindowLevelIntroDescription.printf("%c",c);

				charTickCounter++;
				if(charTickCounter % 4 == 0)
					gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
			}
		}
		
		// Continue to level if ENTER is pressed or the continue button is clicked
		if((input_getLastKey() == HEKEY_ENTER) || (mouseLBClick() == 1))
		{
			gameMode = MODE_GAME;
			gIntroFileOpen = 0;
			gIntroFileFinished = 0;

            // * Reset this here, to avoid the game being paused if there was an ALT+TAB
            // * While in the main menu
            gFocusLost = 0;

			// * Sound fx
			gameMusic.PlayWav("he-soundfx/voice-all-systems-operational.wav");
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level completed mode
	if(gameMode == MODE_LEVELCOMPLETED)
	{
		imageBackgroundGame.Draw(0,gDeltaY);
		
		// * Draw intro graphics
		gImageLevelCompletedWindow.Draw(240 - ((1280 - g_screen_width) / 2),134 + gDeltaY);
		gImageLevelIntroLocation.Draw(240 + 30 - ((1280 - g_screen_width) / 2),134 + 40 + gDeltaY);

		gWindowLevelIntroLocation.clear();
		gWindowLevelIntroDescription.clear();
		gWindowLevelIntroLocation.printf(LANG_TEXT_CONGRATS);
		gWindowLevelIntroLocation.printf(LANG_TEXT_COMPLETED);
		
		// * Display statistics
		gWindowLevelIntroDescription.printf(LANG_TEXT_SCORE,hePlayer.mScore);
		gWindowLevelIntroDescription.printf(LANG_TEXT_MONEY,hePlayer.mMoney);
		gWindowLevelIntroDescription.printf(LANG_TEXT_HACKS,hePlayer.mHackCount);
		gWindowLevelIntroDescription.printf(LANG_TEXT_TRACE,hePlayer.mTraceCount);
		gWindowLevelIntroDescription.printf(LANG_TEXT_LOSTC,hePlayer.mLostCount);
		gWindowLevelIntroDescription.printf("\n");
		if(gLevelTime % 60 < 10)
			gWindowLevelIntroDescription.printf(LANG_TEXT_TIME1,gLevelTime / 60,gLevelTime % 60);
		else
			gWindowLevelIntroDescription.printf(LANG_TEXT_TIME2,gLevelTime / 60,gLevelTime % 60);

		if(hePlayer.mMinuteCount % 60 < 10)
			gWindowLevelIntroDescription.printf(LANG_TEXT_TTIME1,hePlayer.mMinuteCount / 60,hePlayer.mMinuteCount % 60);
		else
			gWindowLevelIntroDescription.printf(LANG_TEXT_TTIME2,hePlayer.mMinuteCount / 60,hePlayer.mMinuteCount % 60);

		gWindowLevelIntroDescription.printf(LANG_TEXT_SPECIAL);
		if(gAchievement_ZeroTrace == 1)
			gWindowLevelIntroDescription.printf(LANG_TEXT_SPECIAL1);
		else
			gWindowLevelIntroDescription.printf(LANG_TEXT_SPECIAL2);
		if(gAchievement_AllHosts == 1)
			gWindowLevelIntroDescription.printf(LANG_TEXT_ALLSERV1);
		else
			gWindowLevelIntroDescription.printf(LANG_TEXT_ALLSERV2);

#ifndef DEMO
        if(gDinamicDifficulty == 1)
            gWindowLevelIntroDescription.printf(LANG_TEXT_DDIFF1,HE_SCORE_SPECIAL * (hePlayer.getDinamicPerformanceMultiplier() - 1));
        else
            gWindowLevelIntroDescription.printf(LANG_TEXT_DDIFF2);
#endif
        if(gAchievement_Under30 > 0)
            gWindowLevelIntroDescription.printf(LANG_TEXT_U301,gAchievement_Under30);
        else
            gWindowLevelIntroDescription.printf(LANG_TEXT_U302);

		gWindowLevelIntroDescription.printf(LANG_TEXT_PRESSENTER);

		gWindowLevelIntroLocation.renderTextContent();
		gWindowLevelIntroDescription.renderTextContent();
		
		//* Continue if ENTER or left click is pressed
		if((input_getLastKey() == HEKEY_ENTER) || (mouseLBClick() == 1))
		{
			
			// * Advance to next level
			if(gModLevelCurrent < gModLevelCount - 1)
			{
				// * advance to next level
				gModLevelCurrent++;
				gameMode = MODE_LEVELINTRO;

				// * save current game
				saveGame();
				init_NewGame(0);

				// * Reset player info
				hePlayer.setHostInfo(gPlayerNickname,"localhost");
				hePlayer.fileCount = 0;

			}
			else 
			{
				// * mod completed, show ending screen
				gameMode = MODE_MOD_COMPLETED;
				gImageModCompleted.LoadFromFile(createModFilePath("mod-completed.png"),800,500);
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mod completed
	if(gameMode == MODE_MOD_COMPLETED)
	{
		imageBackgroundGame.Draw(0,gDeltaY);
		
		// * Draw graphics
		gImageModCompleted.Draw(240 - ((1280 - g_screen_width) / 2),134 + gDeltaY);
		
		// * Continue if ENTER or left click is pressed (return to main menu)
		if((input_getLastKey() == HEKEY_ENTER) || (mouseLBClick() == 1))
		{			
			gameMode = MODE_MENU;
			gDisplayMode = DISPLAY_MODE_NORMAL;
    
            // * switch to menu playlist
			gameMusic.loadPlayList("he-music/he-playlist-menu.txt");
			gameMusic.Play();
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level lost mode
	if(gameMode == MODE_LEVELLOST)
	{
		imageBackgroundGame.Draw(0,gDeltaY);
		
		// * Draw intro graphics
		gImageLevelCompletedWindow.Draw(240 - ((1280 - g_screen_width) / 2),134 + gDeltaY);
		gImageLevelIntroLocation.Draw(240 + 30 - ((1280 - g_screen_width) / 2),134 + 40 + gDeltaY);

		gWindowLevelIntroLocation.clear();
		gWindowLevelIntroDescription.clear();
		gWindowLevelIntroLocation.printf(LANG_TEXT_LOST1);
		gWindowLevelIntroLocation.printf(LANG_TEXT_LOST2);
		gWindowLevelIntroLocation.printf(LANG_TEXT_LOST3);
		
		gWindowLevelIntroLocation.renderTextContent();
		gWindowLevelIntroDescription.renderTextContent();
		
		//* Continue if ENTER or left click is pressed
		if((input_getLastKey() == HEKEY_ENTER) || (mouseLBClick() == 1))
		{
			gameMode = MODE_MENU;

			// * switch to menu playlist
			gameMusic.loadPlayList("he-music/he-playlist-menu.txt");
			gameMusic.Play();
		}
	}
    // ======================================================================================================================
	// Game play mode
	if(gameMode == MODE_GAME || gameMode == MODE_LEVELCOMPLETED_WAIT)
	{
		char temp[1024];

		// Update game level time
		if (gDisplayMode != DISPLAY_MODE_PAUSE) gGameLevel.AddLevelTime(frame_time);

		// Update misc. game engine logics
		gGameLevel.UpdateMessages(&gWindowMessagesHighlight);
		imageBackgroundGame.Draw(0,gDeltaY);

		// Display engine version and fps
		displayEngineInfo();

		// Get input
		int key;
		key = input_getLastKey();

		gMouseLBClick = mouseLBClick();
		gMouseRBClick = mouseRBClick();


        // * No input is needed when the game is paused        
        if(gDisplayMode == DISPLAY_MODE_PAUSE)
        {            
            gMouseLBClick = 0;
            gMouseRBClick = 0;

            // * Also dismiss the pause screen on ESC
            if(key == HEKEY_ESC)
            {
			    gDisplayMode = DISPLAY_MODE_NORMAL;
			    gWindowPaused.mAnimationModifier->Revert();
			    gWindowPaused.mAnimationModifier->Start();
			    gameMusic.Play();
            }

            if(key != HEKEY_PAUSE) key = 0;
        }

		// * If we have a special key, handle it, if not, then add the key to the command buffer
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Volume management
		if(key == HEKEY_F11)
			gameMusic.AdjustVolume(-1);
		if(key == HEKEY_F12)
			gameMusic.AdjustVolume(1);

		// * Jump to the next track in the playlist
		if(key == HEKEY_F4)
			gameMusic.nextTrack();

		// * Dismiss quit level dialog
		if(key == HEKEY_ESC && gDisplayMode == DISPLAY_MODE_QUIT)
		{
			gDisplayMode = DISPLAY_MODE_NORMAL;
			gWindowQuitLevel.mAnimationModifier->Revert();
			gWindowQuitLevel.mAnimationModifier->Start();

			key = 0;
		}

		// * Dismiss popup windows with ESC aswell
		if(key == HEKEY_ESC || key == HEKEY_F1 || key == HEKEY_F2 || key == HEKEY_F3)
		{
			if(gDisplayMode == DISPLAY_MODE_QUICKHELP)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowQuickHelp.mAnimationModifier->Revert();
				gWindowQuickHelp.mAnimationModifier->Start();
				if (key == HEKEY_ESC || key == HEKEY_F1) key = 0;
			}
			if(gDisplayMode == DISPLAY_MODE_LEVELOBJECTIVES)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowLevelObjectives.mAnimationModifier->Revert();
				gWindowLevelObjectives.mAnimationModifier->Start();
				if (key == HEKEY_ESC || key == HEKEY_F2) key = 0;
			}
			if(gDisplayMode == DISPLAY_MODE_LEVELOVERVIEW)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowLevelOverview.mAnimationModifier->Revert();
				gWindowLevelOverview.mAnimationModifier->Start();
				if (key == HEKEY_ESC || key == HEKEY_F3) key = 0;
			}
		}

		// * Quick help
		if(key == HEKEY_F1 && (gDisplayMode == DISPLAY_MODE_NORMAL || gDisplayMode == DISPLAY_MODE_QUICKHELP))
		{
			// * Toggle window
			if(gDisplayMode == DISPLAY_MODE_QUICKHELP)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowQuickHelp.mAnimationModifier->Revert();
				gWindowQuickHelp.mAnimationModifier->Start();
			}
			else
			{
				gDisplayMode = DISPLAY_MODE_QUICKHELP;
				gWindowQuickHelp.mAnimationModifier->Revert();
				gWindowQuickHelp.mAnimationModifier->Start();
			}

			// Clear the key
			key = 0;
		}

		// * Level objectives
		if(key == HEKEY_F2 && (gDisplayMode == DISPLAY_MODE_NORMAL || gDisplayMode == DISPLAY_MODE_LEVELOBJECTIVES))
		{
			// * Toggle window
			if(gDisplayMode == DISPLAY_MODE_LEVELOBJECTIVES)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowLevelObjectives.mAnimationModifier->Revert();
				gWindowLevelObjectives.mAnimationModifier->Start();
			}
			else
			{
				gDisplayMode = DISPLAY_MODE_LEVELOBJECTIVES;
				gWindowLevelObjectives.mAnimationModifier->Revert();
				gWindowLevelObjectives.mAnimationModifier->Start();
			}

			// Clear the key
			key = 0;
		}

		// * Level overview
		if(key == HEKEY_F3  && (gDisplayMode == DISPLAY_MODE_NORMAL || gDisplayMode == DISPLAY_MODE_LEVELOVERVIEW))
		{
			// * Toggle window
			if(gDisplayMode == DISPLAY_MODE_LEVELOVERVIEW)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowLevelOverview.mAnimationModifier->Revert();
				gWindowLevelOverview.mAnimationModifier->Start();
			}
			else
			{
				gDisplayMode = DISPLAY_MODE_LEVELOVERVIEW;
				gWindowLevelOverview.mAnimationModifier->Revert();
				gWindowLevelOverview.mAnimationModifier->Start();
			}

			// Clear the key
			key = 0;
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * ESC - quit level
		if(key == HEKEY_ESC && gDisplayMode == DISPLAY_MODE_NORMAL)
		{
			// * Toggle window
			if(gDisplayMode == DISPLAY_MODE_QUIT)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowQuitLevel.mAnimationModifier->Revert();
				gWindowQuitLevel.mAnimationModifier->Start();
			}
			else
			{
				gDisplayMode = DISPLAY_MODE_QUIT;
				gWindowQuitLevel.mAnimationModifier->Revert();
				gWindowQuitLevel.mAnimationModifier->Start();
				gQuitMenuIndex = -1;

                // * Sound fx
               gameMusic.PlayWav("he-soundfx/voice-please-confirm.wav");
			}

			// Clear the key
			key = 0;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * PAUSE - pause the game
		if(key == HEKEY_PAUSE && (gDisplayMode == DISPLAY_MODE_NORMAL || gDisplayMode == DISPLAY_MODE_PAUSE))
		{
			if(gDisplayMode == DISPLAY_MODE_PAUSE)
			{
				gDisplayMode = DISPLAY_MODE_NORMAL;
				gWindowPaused.mAnimationModifier->Revert();
				gWindowPaused.mAnimationModifier->Start();
				gameMusic.Play();

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-system-activated.wav");
			}
			else
			{
				gDisplayMode = DISPLAY_MODE_PAUSE;
				gWindowPaused.mAnimationModifier->Revert();
				gWindowPaused.mAnimationModifier->Start();
				gameMusic.Pause();

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-pleasehold.wav");
			}
		}

		// * Also pause the game when ALT+TABBED
		if(gFocusLost == 1 && gDisplayMode != DISPLAY_MODE_PAUSE)
		{
			// * Dismiss any popup windows
			if(gDisplayMode == DISPLAY_MODE_QUIT)
				gWindowQuitLevel.mAnimationModifier->Reset(310,160);
			
			if(gDisplayMode == DISPLAY_MODE_QUICKHELP)
				gWindowQuickHelp.mAnimationModifier->Reset(710,370);

			if(gDisplayMode == DISPLAY_MODE_LEVELOBJECTIVES)
				gWindowLevelObjectives.mAnimationModifier->Reset(710,370);

			if(gDisplayMode == DISPLAY_MODE_LEVELOVERVIEW)
				gWindowLevelOverview.mAnimationModifier->Reset(710,370);


			gFocusLost = 0;
			gDisplayMode = DISPLAY_MODE_PAUSE;
			gWindowPaused.mAnimationModifier->Revert();
			gWindowPaused.mAnimationModifier->Start();

			gameMusic.Pause();
		}

		if(gFocusLost == 1 && gDisplayMode == DISPLAY_MODE_PAUSE)
			gFocusLost = 0;

		if(gDisplayMode == DISPLAY_MODE_PAUSE)
			system_AddPauseTime(frame_time);

		if(key != 0)
		{
			commandConsole.addKey(key,&windowConsole);
			
			if(commandConsole.getCommand() != 0)
			{
				strcpy(commandLine,commandConsole.getCommand());
				commandConsole.reset();
				windowConsole.printf("\r%s%s",hePlayer.getCommandPrompt(),commandLine);
				execCommand(commandLine);
			}

			// Post command updates
			if(gGameLevel.connected == 1)
			{
				char hostname[1024];
				sprintf(hostname,"%s[%d]",gGameLevel.hostList[gGameLevel.con_hostindex].hostName,gGameLevel.con_port);
				hePlayer.setHostInfo("root",hostname);
			}
			else
				hePlayer.setHostInfo(gPlayerNickname,"localhost");
		}
		else
		{
			// Display command console
			windowConsole.printf("\r%s%s",hePlayer.getCommandPrompt(),commandConsole.getDisplayCommand());
		}

		// Draw console windows
		windowConsole.Draw();
		windowWorldMap.Draw();
		gWindowSystemPanel.Draw();
		gWindowMessages.Draw();

		// * Draw window highlight, when new message has arrived
		if(gWindowMessagesHighlight.mAnimationModifier->mStarted == 1)
			gWindowMessagesHighlight.Draw((g_screen_width - 310 - woffset_x) + 10,(g_screen_height - 370) + 30);

		// Draw others		
		gGameLevel.drawTargetMap(woffset_x + WINDOW_X_OFFS,WINDOW_Y_OFFS);
		gGameLevel.drawHosts(woffset_x + WINDOW_X_OFFS, WINDOW_Y_OFFS);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Bounced links
		int newHostIndex;
		int i,found = -1;
		
		newHostIndex = gGameLevel.mouseOverHost(mouseX - woffset_x,mouseY);

		// * Check if we must add or remove the host from the bounced link
		if(gMouseRBClick == 1 && newHostIndex != -1)
		{
			command_Scan2(gGameLevel.hostList[newHostIndex].hostName);
		}

		// * Check if we must add or remove the host from the bounced link
		if(gMouseLBClick == 1 && newHostIndex != -1 && gGameLevel.connected == 0)
		{
			for(i = 0; i < gBouncedLinkCount; i++)
				if(gBouncedLinkHostList[i] == newHostIndex)
					found = i;

			// * If the host already exists in the bounced link list, remove it, otherwise add it
			if(found == -1)
			{
				// * Only hosts that are hacked, can be added to the bounced link
				if(gGameLevel.hostList[newHostIndex].hackedCompletely() == 1 && gGameLevel.hostList[newHostIndex].mBounceCount > 0)
				{
					gBouncedLinkHostList[gBouncedLinkCount] = newHostIndex;
					gBouncedLinkCount++;
					gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
					commandWindow->printf(LANG_TEXT_CMD_BOUNCE_3_1,gGameLevel.hostList[newHostIndex].hostName);
					setHelpToolTip(LANG_TEXT_TIPBOUNCE);
				}

				// * Check if we can still bounce through this host
				if(gGameLevel.hostList[newHostIndex].mBounceCount == 0)
				{
					commandWindow->printf(LANG_TEXT_BOUNCE_5,gGameLevel.hostList[newHostIndex].hostName);
				}

				if(gGameLevel.hostList[newHostIndex].hackedCompletely() != 1)
				{
					commandWindow->printf(LANG_TEXT_BOUNCE_1,gGameLevel.hostList[newHostIndex].hostName);
					commandWindow->printf(LANG_TEXT_BOUNCE_2);
				}
			}
			else
			{
				for(i = found; i < gBouncedLinkCount - 1; i++)
					gBouncedLinkHostList[i] = gBouncedLinkHostList[i + 1];
				gBouncedLinkCount--;
				gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
				commandWindow->printf(LANG_TEXT_CMD_BOUNCE_4_1,gGameLevel.hostList[newHostIndex].hostName);
			}
		}

		// * Now draw the bounced links on the map
		gGameLevel.drawBouncedLink(woffset_x + WINDOW_X_OFFS, WINDOW_Y_OFFS,gBouncedLinkCount,gBouncedLinkHostList);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		gGameLevel.drawHostToolTip(mouseX - woffset_x,mouseY);		

		gGameLevel.GetLevelTimeText(temp);
		gWindowSystemPanel.printText(232,34,SKINCOLOR(gSkinColorTargetMap),temp);

		// Draw messages
		if(gGameLevel.mActiveMessageCount > 0)
		{
			gGameLevel.DisplayMessage(&gWindowMessages,gGameLevel.mActiveMessageIndex);
		}

		// Print message count
		if(gGameLevel.mActiveMessageCount > 0)
			sprintf(temp,"  [%d %s %d]",gGameLevel.mActiveMessageIndex + 1,LANG_TEXT_OF,gGameLevel.mActiveMessageCount);
		else
			sprintf(temp,LANG_TEXT_NOMSG);
		gWindowMessages.printText(200,342,SKINCOLOR(gSkinColorTargetMap),temp);

		// Draw text contents
		windowConsole.renderTextContent();
		gWindowMessages.renderTextContent();
		gWindowSystemPanel.renderTextContent();
		gWindowSystemPanel.printText(18,332,SKINCOLOR(gSkinColorTargetMap),gGameLevel.statusPanelMessage);

		// Update any running events ==========================================================================================
		// FILE DOWNLOAD
		if(download_InProgress == 1)
		{
			long time,temp,i;
			time = system_TimeMilliSeconds() - download_StartTime;

			temp = download_FilePointer->size - (time * download_Speed) / 1000;
			if(temp < 0) temp = 0; // in case download is finished.

			download_PercentDone = (int)((download_FilePointer->size - temp) * 100) / download_FilePointer->size;

			// Draw progress
			for(i = 0; i < download_PercentDone * 2; i++)
			{
				if(i % 10 == 0)
					gWindowSystemPanel.drawLine(91 + i,331,91 + i,330 + 18, SKINCOLOR(gSkinColorTargetMap));
				else
					gWindowSystemPanel.drawLine(91 + i,330,91 + i,330 + 19, SKINCOLOR(gSkinColorTargetMap));
			}
			gWindowSystemPanel.printText(95,332,SKINCOLOR(gSkinColorProgress),LANG_TEXT_MISC_DOWNLOADING,download_PercentDone);

			strcpy(gGameLevel.statusPanelMessage,"-dl-");

			// Check if download is finished
			if(download_PercentDone >= 100)
			{
				download_InProgress = 0;
				currentCommand = COMMAND_NONE;
				hePlayer.fileList[hePlayer.fileCount].size = download_FilePointer->size;
				strcpy(hePlayer.fileList[hePlayer.fileCount].filePath,download_FilePointer->filePath);
				strcpy(hePlayer.fileList[hePlayer.fileCount].fileName,download_FilePointer->fileName);

				hePlayer.fileCount++;

				// Cancel command status
				commandInProgress = -1;
				sprintf(gGameLevel.statusPanelMessage,"idle");

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-complete.wav");
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * File UPLOAD
		if(upload_InProgress == 1)
		{
			long time,temp,i;
			time = system_TimeMilliSeconds() - upload_StartTime;

			temp = upload_FilePointer->size - (time * upload_Speed) / 1000;
			if(temp < 0) temp = 0; // in case upload is finished.

			upload_PercentDone = (int)((upload_FilePointer->size - temp) * 100) / upload_FilePointer->size;

			// * Draw progress
			for(i = 0; i < upload_PercentDone * 2; i++)
			{
				if(i % 10 == 0)
					gWindowSystemPanel.drawLine(91 + i,331,91 + i,330 + 18, SKINCOLOR(gSkinColorTargetMap));
				else
					gWindowSystemPanel.drawLine(91 + i,330,91 + i,330 + 19, SKINCOLOR(gSkinColorTargetMap));
			}
			gWindowSystemPanel.printText(95,332,SKINCOLOR(gSkinColorProgress),LANG_TEXT_MISC_UPLOADING,upload_PercentDone);

			strcpy(gGameLevel.statusPanelMessage,"-ul-");

			// * Check if upload is finished
			if(upload_PercentDone >= 100)
			{
				upload_InProgress = 0;
				currentCommand = COMMAND_NONE;

				int hostindex = gGameLevel.con_hostindex;
				int fileindex = gGameLevel.hostList[hostindex].fileCount;
				
				gGameLevel.hostList[hostindex].fileList[fileindex].size = upload_FilePointer->size;
				strcpy(gGameLevel.hostList[hostindex].fileList[fileindex].filePath,upload_FilePointer->filePath);
				strcpy(gGameLevel.hostList[hostindex].fileList[fileindex].fileName,upload_FilePointer->fileName);

				gGameLevel.hostList[hostindex].fileCount++;

				// * Create a rule for the file to be visible on the current port
				strcpy(gGameLevel.hostList[hostindex].ruleList[gGameLevel.hostList[hostindex].ruleCount].fileName,upload_FilePointer->fileName);
				gGameLevel.hostList[hostindex].ruleList[gGameLevel.hostList[hostindex].ruleCount].port = gGameLevel.con_port;
				gGameLevel.hostList[hostindex].ruleCount++;

				// * Cancel command status
				commandInProgress = -1;
				sprintf(gGameLevel.statusPanelMessage,"idle");

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-complete.wav");
			}
		}
		// ====================================================================================================================
		// CRACK
		if(crack_InProgress == 1)
		{			
			int      i,x,y;
			hestring password;

			long time;
			time = system_TimeMilliSeconds() - crack_StartTime;

			crack_PercentDone = time / crack_Speed;

			// * Check if crack is completed
			if(crack_PercentDone >= strlen(crack_Password))
			{
				crack_InProgress  = 0;
				currentCommand    = COMMAND_NONE;
				commandInProgress = -1;

				gGameLevel.hostList[crack_HostIndex].serviceList[crack_ServiceIndex].hasPassword = 0;

				// * Update player info
				hePlayer.mScore       += HE_SCORE_CRACK;
				hePlayer.mGlobalTrace += HE_TRACE_CRACK;
				hePlayer.mHackCount++;

				// * Stop the tracer
				trace_InProgress = -1;
				strcpy(trace_Msg,LANG_TEXT_TRACE_3);
				sprintf(gGameLevel.statusPanelMessage,"idle");

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-complete.wav");

			}
			else
			{
				// * Update ticker
				/*
				if( (time / 100) > crack_Tick)
				{
					crack_Tick++;
					gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");

					// * Build password crack text
					for(i = 0; i < strlen(crack_Password2); i++)
					{
						crack_Password2[i] = 'a' + (rand() % 10);
					}

				}

				for(i = 0; i < crack_PercentDone; i++) password[i] = crack_Password[i];
				password[i] = '[';
				i++; password[i] = crack_Password2[crack_PercentDone];
				i++; password[i] = ']'; i++;

				for(;i < strlen(crack_Password) + 2; i++)
				{
					password[i] = crack_Password2[i - 2];
				}
				password[i] = 0;
				*/

				int _temp = strlen(crack_Password) - crack_PercentDone;
				if(_temp <= 2) _temp = 2;

				crack_Tick = system_TimeSeconds() % 4;
				for(i = 0; i < crack_Tick; i++)
					crack_Password2[i] = 'a' + (rand() % (_temp / 2));
				crack_Password2[i] = '['; i++;
				crack_Password2[i] = 'a' + (rand() % _temp); i++;
				crack_Password2[i] = ']'; i++;
				for(; i < 6; i++)
					crack_Password2[i] = 'a' + (rand() % (_temp / 2));
				crack_Password2[i] = 0;

				if(crack_LastTick != crack_Tick)
				{
					crack_LastTick = crack_Tick;
					gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
				}

				sprintf(password,LANG_TEXT_MISC_CRACKING,crack_Password2,strlen(crack_Password) - crack_PercentDone);
				
				// * Draw crack animation
				x = gGameLevel.hostList[crack_HostIndex].mapx + WINDOW_X_OFFS + gGameLevel.woffset_x;
				y = gGameLevel.hostList[crack_HostIndex].mapy + WINDOW_Y_OFFS;			

				sprintf(gGameLevel.statusPanelMessage,"crack");
				gWindowSystemPanel.printText(95,332,SKINCOLOR(gSkinColorProgress),"%s",password);
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * DECRYPT
		if(decrypt_InProgress == 1)
		{			
			int      x,y;
			long     time;

			time = system_TimeMilliSeconds() - decrypt_StartTime;

			decrypt_PercentDone = ((float)time / (float)decrypt_Speed) * 128;

			// * Check if decrypt is completed			
			if(decrypt_PercentDone >= gGameLevel.hostList[decrypt_HostIndex].mEncryptionKey)
			{
				decrypt_InProgress  = 0;
				currentCommand    = COMMAND_NONE;
				commandInProgress = -1;

				gGameLevel.hostList[decrypt_HostIndex].mEncryptionKey = 0;

				// * Update player info
				hePlayer.mScore       += HE_SCORE_DECRYPT;
				hePlayer.mGlobalTrace += HE_TRACE_DECRYPT;
				hePlayer.mHackCount++;

				// * Stop the tracer
				trace_InProgress = -1;
				strcpy(trace_Msg,LANG_TEXT_TRACE_3);
				sprintf(gGameLevel.statusPanelMessage,"idle");

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-complete.wav");
			}
			else
			{
				// * Update ticker
				if( (time / 100) > decrypt_Tick)
				{
					decrypt_Tick++;
					gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
				}

				
				// * Draw decrypt animation
				x = gGameLevel.hostList[decrypt_HostIndex].mapx + WINDOW_X_OFFS + gGameLevel.woffset_x;
				y = gGameLevel.hostList[decrypt_HostIndex].mapy + WINDOW_Y_OFFS;			

				sprintf(gGameLevel.statusPanelMessage,"decr");
				gWindowSystemPanel.printText(95,332,SKINCOLOR(gSkinColorProgress),LANG_TEXT_MISC_DECRYPTING, gGameLevel.hostList[decrypt_HostIndex].mEncryptionKey - decrypt_PercentDone,decrypt_PercentDone);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Trace
		if(trace_InProgress == 1)
		{
			long time, tracePercent;			

			time = trace_Length - (system_TimeMilliSeconds() - trace_StartTime);
			tracePercent = ((system_TimeMilliSeconds() - trace_StartTime) * 100) / trace_Length;

			int pLastBounceHost = -1;
			if(gBouncedLinkCount > 0)
				pLastBounceHost = gBouncedLinkHostList[gBouncedLinkCount - 1];

			gGameLevel.drawTraceProgress(tracePercent, trace_HostIndex, woffset_x + WINDOW_X_OFFS,WINDOW_Y_OFFS,pLastBounceHost);

			if(time <= 0)
			{
				// * Player has been traced
				strcpy(trace_Msg,LANG_TEXT_TRACE_2);
				trace_InProgress = -1;
				hePlayer.mGlobalTrace += HE_TRACE_TRACED;
				hePlayer.mTraceCount++;
			}

			else
			{
				sprintf(trace_Msg,LANG_TEXT_TRACE_1,time / 1000, (time % 1000) / 100);
			}

		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Money transfer
		if(transfer_InProgress == 1)
		{
			long time,temp,i;
			time = system_TimeMilliSeconds() - transfer_StartTime;

			temp = transfer_Amount - (time * transfer_Speed) / 1000;
			if(temp < 0) temp = 0; // in case download is finished.

			transfer_PercentDone = (int)((transfer_Amount - temp) * 100) / transfer_Amount;

			// Draw progress
			for(i = 0; i < transfer_PercentDone * 2; i++)
			{
				if(i % 10 == 0)
					gWindowSystemPanel.drawLine(91 + i,331,91 + i,330 + 18, SKINCOLOR(gSkinColorTargetMap));
				else
					gWindowSystemPanel.drawLine(91 + i,330,91 + i,330 + 19, SKINCOLOR(gSkinColorTargetMap));
			}
			gWindowSystemPanel.printText(95,332,SKINCOLOR(gSkinColorProgress),LANG_TEXT_MISC_TRANSFERING,transfer_PercentDone);

			strcpy(gGameLevel.statusPanelMessage,"xfer");

			// * Check if the transfer is finished
			if(transfer_PercentDone == 100)
			{
				transfer_InProgress = 0;
				currentCommand = COMMAND_NONE;

				hePlayer.mMoney += transfer_Amount;
				gGameLevel.hostList[gGameLevel.con_hostindex].mMoney -= transfer_Amount;

				// * Update player score
				hePlayer.mScore += HE_SCORE_TRANSFER;
				hePlayer.mScore += (transfer_Amount / HE_SCORE_TRANSFER_RATIO);

				// * Cancel command status
				commandInProgress = -1;
				sprintf(gGameLevel.statusPanelMessage,"idle");

				// * Stop the tracer
				trace_InProgress = -1;
				strcpy(trace_Msg,LANG_TEXT_TRACE_3);

                // * Sound fx
                gameMusic.PlayWav("he-soundfx/voice-complete.wav");

                // * Global trace update
                hePlayer.mGlobalTrace += HE_TRACE_TRANSFER;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Display trace message
		render_DrawText_Formated(woffset_x + 15, WINDOW_Y_OFFS + 332,SKINCOLOR(gSkinColorBouncedLink),trace_Msg);

		// * Render interface fx
		gWindowSlider.Draw(woffset_x + WINDOW_HOFFS,WINDOW_Y_OFFS + WINDOW_VOFFS + 320);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Fade out tooltip window, if the mouse is outside of it's area			
		if(PointInRect(mouseX,mouseY,woffset_x, WINDOW_Y_OFFS,710,370) || (system_TimeSeconds() - gToolTipStartTime > HE_TOOLTIP_TIMEOUT))
		{
			gWindowHelpTooltip.mAnimationModifier->ForceDirection(-1,1);
			gWindowHelpTooltip.Draw();		
		}
		else
		{
			// * Check to see tooltip hasn't timed out.
			if(system_TimeSeconds() - gToolTipStartTime < HE_TOOLTIP_TIMEOUT)
			{
				gWindowHelpTooltip.mAnimationModifier->ForceDirection(1,1);
			
				// * Render help tooltip	
				gWindowHelpTooltip.Draw();			
				gWindowHelpTooltip.renderTextContent();			
			}
			else
				gWindowHelpTooltip.Draw();			

		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Scroll through messages
		// * previous message
        if(PointInRect(mouseX,mouseY,g_screen_width - 310 - woffset_x,g_screen_height - 370,310,370)  && gMouseLBClick == 1)
        {
		    if(PointInRect(mouseX,mouseY,(g_screen_width - 310 - woffset_x) + 20,(g_screen_height - 370) + 342,75,15))
		    {
			    gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
			    gGameLevel.PrevMessage();
		    }
            else
		// * next message
		   //if(PointInRect(mouseX,mouseY,(g_screen_width - 310 - woffset_x) + 120,(g_screen_height - 370) + 342,75,15) && gMouseLBClick == 1)
            {
			    gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
			    gGameLevel.NextMessage();
            }
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Render player hardware
		int temp_x = g_screen_width - 310 - woffset_x;
		int temp_y = WINDOW_Y_OFFS - 15;

		gHardwareCPU1[hePlayer.cpu1].Draw(temp_x + 90,temp_y + 200);
		if(hePlayer.cpu2 > -1) gHardwareCPU2[hePlayer.cpu2].Draw(temp_x + 90,temp_y + 260);
		gHardwareMem1[hePlayer.mem1].Draw(temp_x + 25,temp_y + 200);
		if(hePlayer.mem2 > -1) gHardwareMem2[hePlayer.mem2].Draw(temp_x + 25,temp_y + 260);
		gHardwareModem[hePlayer.modem].Draw(temp_x + 175,temp_y + 240);
		gHardwareFirewall[hePlayer.firewall].Draw(temp_x + 175,temp_y + 200);

		if(hePlayer.nnadapter == 0) gHardwareNNAdapter.Draw(temp_x + 205, temp_y + 280);

        // * Dirty hack to avoid the firewall icon being drawed with a messed up alpha channel
        // * when the game is paused
        gHardwareFirewall[hePlayer.firewall].Draw(2000,2000);

		// * Hardware tooltips and info
		hestring hardwareToolTipText;
		sprintf(hardwareToolTipText,"");

		if(PointInRect(mouseX,mouseY,temp_x + 5,temp_y + 195,300,120))
			sprintf(hardwareToolTipText,LANG_TEXT_UPGRADE);

		// * cpu
		if(PointInRect(mouseX,mouseY,temp_x + 90,temp_y + 200,50,50))
			sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_1,HE_NAME_CPU[hePlayer.cpu1],hePlayer.getCPUMultiplier());

		if(PointInRect(mouseX,mouseY,temp_x + 90,temp_y + 260,50,50))
			if(hePlayer.cpu2 != -1)
				sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_2,HE_NAME_CPU[hePlayer.cpu2],hePlayer.getCPUMultiplier());
			else
				sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_3,hePlayer.getCPUMultiplier());
		

		// * mem
		if(PointInRect(mouseX,mouseY,temp_x + 25,temp_y + 200,30,50))
			sprintf(hardwareToolTipText,"MEM1: %s",HE_NAME_MEM[hePlayer.mem1]);

		if(PointInRect(mouseX,mouseY,temp_x + 25,temp_y + 260,30,50))
			if(hePlayer.mem2 != -1)
				sprintf(hardwareToolTipText,"MEM2: %s",HE_NAME_MEM[hePlayer.mem2]);
			else
				sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_4);

		// * modem
		if(PointInRect(mouseX,mouseY,temp_x + 175,temp_y + 240,110,30))
			sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_5,HE_NAME_MODEM[hePlayer.modem],hePlayer.getModemMultiplier());

		// * firewall
		if(PointInRect(mouseX,mouseY,temp_x + 175,temp_y + 200,110,30))
			if(hePlayer.firewall == 0)
				sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_6);
			else
				sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_7,HE_NAME_FIREWALL[hePlayer.firewall],hePlayer.getFirewallMultiplier());
		
		// * neural network adapter
		if(PointInRect(mouseX,mouseY,temp_x + 205, temp_y + 280,80,30))
		{
			if(hePlayer.nnadapter == 0)
				sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_8);
			else
				sprintf(hardwareToolTipText,LANG_TEXT_HW_TIP_9);
		}

        //
        //if(gDisplayMode != DISPLAY_MODE_PAUSE && gDisplayMode != DISPLAY_MODE_QUIT)
        if(gDisplayMode == DISPLAY_MODE_NORMAL)
		    gWindowSystemPanel.printText(15, 302, SKINCOLOR(gSkinColorTargetMap), hardwareToolTipText);
		
		gWindowSystemPanel.clear();
		gWindowSystemPanel.printf(LANG_TEXT_SPSCORE,hePlayer.mScore);

		if(hePlayer.mGlobalTrace >= HE_GLOBALTRACE_BLINK)
		{
			// * This should now blink
			if(system_TimeMilliSeconds() % 1000 > 500)
				gWindowSystemPanel.printf(LANG_TEXT_SPTRACE1,hePlayer.mGlobalTrace);
			else
				gWindowSystemPanel.printf(LANG_TEXT_SPTRACE2);
		}
		else
			gWindowSystemPanel.printf(LANG_TEXT_SPTRACE1,hePlayer.mGlobalTrace);

        gWindowSystemPanel.printf(LANG_TEXT_SPDDIFF,hePlayer.getDinamicPerformanceMultiplier());

		gWindowSystemPanel.printf("\n");
		gWindowSystemPanel.printf(LANG_TEXT_SPMONEY,hePlayer.mMoney);
		gWindowSystemPanel.printf(LANG_TEXT_SPHACK,hePlayer.mHackCount);
		gWindowSystemPanel.printf(LANG_TEXT_SPTRACE,hePlayer.mTraceCount);

		if(gBouncedLinkCount == 0)
			gWindowSystemPanel.printf(LANG_TEXT_SPBOUNCE1);
		else
		{
			if(gBouncedLinkCount == 1)
				gWindowSystemPanel.printf(LANG_TEXT_SPBOUNCE2,gBouncedLinkCount);
			else
				gWindowSystemPanel.printf(LANG_TEXT_SPBOUNCE3,gBouncedLinkCount);
		}

		// * Double the trace message here aswell
		if(trace_InProgress == 1)
			gWindowSystemPanel.printf("\n   %s",trace_Msg);

		// * If a new objective has been completed notify the player
		int lastObjective = gGameLevel.getLastCompletedObjective();
		if(lastObjective != -1)
		{
			hestring msg;

			sprintf(msg,LANG_TEXT_OBJECTIVE,lastObjective + 1);
			setHelpToolTip(msg);
		}
		// * Update level objectives and check if level is completed
		if(gGameLevel.UpdateLevelObjectives(&gWindowLevelObjectives) == 1 || gSkipLevel == 1)
		{            
			// * Level completed, jump to end screen
			if(gameMode == MODE_GAME)
			{
				gLevelDoneTime = system_TimeSeconds(); 
				gameMode = MODE_LEVELCOMPLETED_WAIT;
				gApplicationLog.printf("\nheEngine :: switch to MODE_LEVELCOMPLETED_WAIT");

                // * Stop any ongoing actions
                download_InProgress = 0;
                upload_InProgress = 0;
                crack_InProgress = 0;
                decrypt_InProgress = 0;
                transfer_InProgress = 0;
                trace_InProgress = 0;
			}

			if(gameMode == MODE_LEVELCOMPLETED_WAIT)
			{
				if(system_TimeMilliSeconds() % 1000 < 500)
					gWindowSystemPanel.printText(95,332,SKINCOLOR(gSkinColorTargetMap),LANG_TEXT_LEVELCOMP,HE_LEVELDONE_WAITTIME - (system_TimeSeconds() - gLevelDoneTime));

				// * waiting time over, jump to the next level
				if(system_TimeSeconds() - gLevelDoneTime > HE_LEVELDONE_WAITTIME)
				{
					gameMode     = MODE_LEVELCOMPLETED;
					gDisplayMode = DISPLAY_MODE_NORMAL;
					gSkipLevel   = 0;

					// * Save completion time
					gLevelTime = gGameLevel.GetLevelTimeSeconds();
					hePlayer.mMinuteCount += gLevelTime;

					// * Special achievements that add score
					gAchievement_ZeroTrace = 0;
					gAchievement_AllHosts  = 1;
                    gAchievement_Under30   = 0;
					
					// * 0% trace level
					if(hePlayer.mGlobalTrace == 0)
					{
						gAchievement_ZeroTrace  = 1;
						hePlayer.mScore        += HE_SCORE_SPECIAL;
					}

					// * discovered all servers
					for(int i = 0; i < gGameLevel.hostCount; i++)
						if(gGameLevel.hostList[i].mVisible == 0)
							gAchievement_AllHosts = 0;
					if(gAchievement_AllHosts == 1)
					{						
						hePlayer.mScore        += HE_SCORE_SPECIAL;
					}

                    // * Dinamic game difficulty
                    if(gDinamicDifficulty == 1)
                    {
                        hePlayer.mScore += HE_SCORE_SPECIAL * (hePlayer.getDinamicPerformanceMultiplier() - 1);
                    }

                    // * Completed level under 30 minutes
                    if(gGameLevel.GetLevelTimeSeconds() / 60 < 30)
                    {
                        gAchievement_Under30 = (30 - (gGameLevel.GetLevelTimeSeconds() / 60));
                        gAchievement_Under30 *= 10;
						hePlayer.mScore      += gAchievement_Under30;
                    }

					gApplicationLog.printf("\nLEVEL COMPELETED IN : %d:%d",gLevelTime / 60,gLevelTime % 60);
				}
			}
		}

		// * Check if level is lost
		if(hePlayer.mGlobalTrace >= 100)
		{
			gameMode = MODE_LEVELLOST;
			
			// * Reset trace level
			hePlayer.mGlobalTrace = 0;
			hePlayer.mLostCount++;

			gDisplayMode = DISPLAY_MODE_NORMAL;

            // * Stop any ongoing actions
            download_InProgress = 0;
            upload_InProgress = 0;
            crack_InProgress = 0;
            decrypt_InProgress = 0;
            transfer_InProgress = 0;
            trace_InProgress = 0;
		}

		// * Render popup windows
		if(gDisplayMode != DISPLAY_MODE_NORMAL)
			render_DrawRectangle(0,0,g_screen_width,g_screen_height,64,64,64,1);

		gWindowQuickHelp.Draw();
		if (gDisplayMode == DISPLAY_MODE_QUICKHELP) gWindowQuickHelp.renderTextContent();
		gWindowLevelObjectives.Draw();
		if (gDisplayMode == DISPLAY_MODE_LEVELOBJECTIVES) gWindowLevelObjectives.renderTextContent();
		gWindowLevelOverview.Draw();
		if (gDisplayMode == DISPLAY_MODE_LEVELOVERVIEW) gWindowLevelOverview.renderTextContent();

		gWindowQuitLevel.Draw();

		// * Game paused screen
		if(gDisplayMode == DISPLAY_MODE_PAUSE)
			render_DrawRectangle(0,0,g_screen_width,g_screen_height,64,64,64,1);
		gWindowPaused.Draw();

		if(gDisplayMode == DISPLAY_MODE_QUIT && gWindowQuitLevel.mAnimationModifier->mStarted == 0)
		{			
			int xpos, ypos;
			xpos = (g_screen_width - 310) / 2;
			ypos = (g_screen_height - 160) / 2;

			xpos += 25;
			ypos += 80;			

			gImageCursorBackgroundQuit[0].Draw(xpos - 5,ypos + 5);
			gImageCursorBackgroundQuit[1].Draw(xpos - 5,ypos + 30 + 5);

			gQuitMenuIndex = -1;

			if(PointInRect(mouseX,mouseY,xpos,ypos,270, 20) && gQuitMenuIndex != 0)
			{
				gImageCursorBackgroundQuit[0].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[1].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[1].mAnimationModifier->Revert();
				gQuitMenuIndex = 0;
			}

			if(PointInRect(mouseX,mouseY,xpos,ypos + 30,270,20) && gQuitMenuIndex != 1)
			{
				gImageCursorBackgroundQuit[1].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[0].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[0].mAnimationModifier->Revert();
				gQuitMenuIndex = 1;
			}

			if(gQuitMenuIndex == 0) 
				render_DrawText_Large(xpos,ypos + 8,LANG_TEXT_YES,32,32,32,0,0);
			else
				render_DrawText_Large(xpos,ypos + 8,LANG_TEXT_YES,255,255,255,0,0);
			
			if(gQuitMenuIndex == 1) 
				render_DrawText_Large(xpos,ypos + 30 + 8,LANG_TEXT_NO,32,32,32,0,0);
			else
				render_DrawText_Large(xpos,ypos + 30 + 8,LANG_TEXT_NO,255,255,255,0,0);

			// * Check if player click on an option
			if(gMouseLBClick == 1)
			{
				if(gQuitMenuIndex == 0)
				{

                    // * Sound fx
                    gameMusic.PlayWav("he-soundfx/voice-closing.wav");

					gameMode = MODE_MENU;
					gDisplayMode = DISPLAY_MODE_NORMAL;
					gWindowQuitLevel.mAnimationModifier->Revert();
					gWindowQuitLevel.mAnimationModifier->Start();

					// * switch to menu playlist
					gameMusic.loadPlayList("he-music/he-playlist-menu.txt");
					gameMusic.Play();

				}
				if(gQuitMenuIndex == 1)
				{
					gDisplayMode = DISPLAY_MODE_NORMAL;
					gWindowQuitLevel.mAnimationModifier->Revert();
					gWindowQuitLevel.mAnimationModifier->Start();
				}
			}
		}
	}

	// Render mouse cursor
	if(gameMode != MODE_INTRO && gameMode != MODE_INTRO2)
	{
		// render_MouseCursor(mouseX,mouseY);
		gMouseCursor.Draw(mouseX,mouseY);
	}

	// Update music engine
	gameMusic.Update();
	
	render_EndFrame();
}

//===============================================================================================
// Close game engine
void closeGameEngine()
{
	render_Close();

	// Save FPS to log
	char temp[1024];
	sprintf(temp,"%s @ FPS : %3.2f",ENGINE_VERSION, gAverageFps / (float) gFrameCount);
	gApplicationLog.printf("\n%s",temp);

    gApplicationLog.printf("\nModule::heEngine - Closed.");
}
//===============================================================================================
// Initialize game windows
void init_NewGame(int pResetPlayerInfo)
{
	int i;

	gApplicationLog.printf("\n init new game...");

	// * Reset player info
	if(pResetPlayerInfo == 1)
	{
		hePlayer.resetPlayerInfo();
		hePlayer.setHostInfo(gPlayerNickname,"localhost");
	}

	// * Initialize windows

    // * These are adjusted to the current resolution
	gWindowLevelIntroLocation.Create(((g_screen_width - 800) / 2) + 270,124 + 25 + gDeltaY,600,180);	
    gWindowLevelIntroDescription.Create(((g_screen_width - 800) / 2) + 5,124 + 215 + gDeltaY,800,370);
    //

	gImageLevelIntroWindow.LoadFromFile(createSkinPath("window-levelintro.png"),800,500);
    gImageLevelCompletedWindow.LoadFromFile(createSkinPath("window-levelcompleted.png"),800,500);
	gMouseCursor.LoadFromFile(createSkinPath("mouse.png"),20,20);	
	windowConsole.Create(woffset_x,g_screen_height - 370, 710,370,createSkinPath("window-console.png"));
	windowWorldMap.Create(woffset_x, WINDOW_Y_OFFS,710,370,createSkinPath("window-worldmap.png"));
	gWindowSystemPanel.Create(g_screen_width - 310 - woffset_x, WINDOW_Y_OFFS,310,370,createSkinPath("window-systempanel.png"));
	gWindowMessages.Create(g_screen_width - 310 - woffset_x,g_screen_height - 370,310,370,createSkinPath("window-messages.png"));

	// * Create popup windows
	gWindowQuickHelp.Create((g_screen_width - 710) / 2,(g_screen_height - 370) / 2,710,370,createSkinPath("window-quickhelp.png"));
	gWindowQuickHelp.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowQuickHelp.mAnimationModifier->Reset(710,370);
	gWindowLevelObjectives.Create((g_screen_width - 710) / 2,(g_screen_height - 370) / 2,710,370,createSkinPath("window-levelobjectives.png"));
	gWindowLevelObjectives.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowLevelObjectives.mAnimationModifier->Reset(710,370);
	gWindowLevelOverview.Create((g_screen_width - 710) / 2,(g_screen_height - 370) / 2,710,370,createSkinPath("window-leveloverview.png"));
	gWindowLevelOverview.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowLevelOverview.mAnimationModifier->Reset(710,370);

	gWindowQuitLevel.Create((g_screen_width - 310) / 2, (g_screen_height - 160) / 2,310,160,createSkinPath("window-quitlevel.png"));
	gWindowQuitLevel.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowQuitLevel.mAnimationModifier->Reset(310,160);

	gWindowPaused.Create((g_screen_width - 310) / 2, (g_screen_height - 160) / 2,310,160,createSkinPath("window-paused.png"));
	gWindowPaused.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowPaused.mAnimationModifier->Reset(310,160);

	for(i = 0; i < 2; i++)
	{
		gImageCursorBackgroundQuit[i].LoadFromFile(createSkinPath("cursorbackground.png"),270,20);		

		// * Attach animation modifier
		gImageCursorBackgroundQuit[i].AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
		gImageCursorBackgroundQuit[i].mAnimationModifier->Reset(768);
	}

	// final initializations
	windowConsole.printf("console initialized.\n");

	// Initialize command engine
	commandWindow = &windowConsole;

	// * Reset command completion dictionaries
	gDictionaryHostNames.Reset();
	gDictionaryFileNames.Reset();
	commandConsole.clearCommandHistory();

	// Load level
	gImageLevelIntroLocation.LoadFromFile(createModLevelFilePath("introimage.bmp"),240,180);
	gImageLevelIntroLocation.AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
	gImageLevelIntroLocation.mAnimationModifier->Reset();
	gImageLevelIntroLocation.mAnimationModifier->Start();

	gGameLevel.SetLevelInfo(gModName,gModLevelList[gModLevelCurrent]);
	gGameLevel.LoadFromFile();

	// Misc
	strcpy(gGameLevel.statusPanelMessage,"idle");

	// * Initialize new game
	gWindowSlider.LoadFromFile(createSkinPath("slider.png"),10,10);
	gWindowSlider.AttachAnimationModifier(new CAnimationModifier_HScan);
	gWindowSlider.mAnimationModifier->Reset(0,674);
	gWindowSlider.mAnimationModifier->Start();

	// * Initialize popup window contents
	gWindowQuickHelp.displayFile("he-content/popup-quickhelp.txt",1);
	gWindowLevelOverview.displayFile(createModLevelFilePath("levelintro-text-1"),1);
	gWindowLevelOverview.printf("\n");
	gWindowLevelOverview.displayFile(createModLevelFilePath("levelintro-text-2"),0);

	// * Initialize help tooltip
	gWindowHelpTooltip.Create(woffset_x + 430, WINDOW_Y_OFFS + 270,260,85,createSkinPath("tooltip.png"),0);
	gWindowHelpTooltip.AttachAnimationModifier(new  CAnimationModifier_AlphaFadeIn);
	gWindowHelpTooltip.mAnimationModifier->Reset();
	gWindowHelpTooltip.mAnimationModifier->Start();

	// * Initialize host tooltip image
	gHostToolTip.LoadFromFile(createSkinPath("tooltip-host.png"),260,85);
	gHostToolTip.AttachAnimationModifier(new  CAnimationModifier_ScaleInH);
	gHostToolTip.mAnimationModifier->Reset(260,85);
	gGameLevel.AttachToolTipImage(&gHostToolTip);

	// * Initialize host scan tooltip image
	gHostToolTipScan.LoadFromFile(createSkinPath("tooltip-host.png"),260,85);
	gHostToolTipScan.AttachAnimationModifier(new  CAnimationModifier_ScaleInHCenter);
	gHostToolTipScan.mAnimationModifier->Reset(260,85);
	gHostToolTipScan.mAnimationModifier->Start();
	gHostToolTipScan.mAnimationModifier->Revert();

	// * Message window highlight
	gWindowMessagesHighlight.LoadFromFile(createSkinPath("window-messages-highlightbar.png"),290,15);
	gWindowMessagesHighlight.AttachAnimationModifier(new CAnimationModifier_VScan2);
	
	// * Set initial tooltip
	setHelpToolTip(LANG_TEXT_PRESSF1);

	// * Attach player info to level
	gGameLevel.mPlayerInfo = &hePlayer;

	// * Load player hardware icons
	for(i = 0; i < HE_MAX_CPU; i++)
	{
		gHardwareCPU1[i].LoadFromFile(createSkinPath(HE_ICON_NAME_CPU[i]),50,50);
		gHardwareCPU2[i].LoadFromFile(createSkinPath(HE_ICON_NAME_CPU[i]),50,50);
		gHardwareMem1[i].LoadFromFile(createSkinPath(HE_ICON_NAME_MEM[i]),30,50);
		gHardwareMem2[i].LoadFromFile(createSkinPath(HE_ICON_NAME_MEM[i]),30,50);
		gHardwareModem[i].LoadFromFile(createSkinPath(HE_ICON_NAME_MODEM[i]),110,30);
		gHardwareFirewall[i].LoadFromFile(createSkinPath(HE_ICON_NAME_FIREWALL[i]),110,30);
		gHardwareNNAdapter.LoadFromFile(createSkinPath("icon-nnadapter.png"),80,30);
	}

	// * Clear windows
	windowConsole.clear();
	gWindowMessages.clear();

	// * Reset command engine
	reset_CommandEngine();

	// * Clear bounced links
	gBouncedLinkCount = 0;

	// * Load and play game playlist
	gameMusic.loadPlayList("he-music/he-playlist-game.txt");
	gameMusic.Play();
}

//===============================================================================================
// Execute command
void execCommand(char* command)
{
	char cmd[1024], param[1024];
	char _command[1024];
	int  i,index;

	// 2014 Update
	// Replace ":" with spaces so that crack works with mentioning ports with :
	strcpy(_command,command);
	for(i = 0; i < strlen(_command); i++)
		if(_command[i] == ':') _command[i] = ' ';

	// Skip any preceding spaces
	i = 0;
	while(_command[i] == ' ') i++;

	// Extract command name
	index = i;
	for(; i < strlen(_command) && _command[i] != ' '; i++)
		cmd[i - index] = _command[i];
	cmd[i - index] = 0;

	// Extract parameter
	i++;
	index = i;
	for(;i < strlen(_command); i++)
		param[i - index] = _command[i];
	param[i - index] = 0;

	// Execute command statement
	if(exec_Command(cmd,param) == 0)
		setHelpToolTip(LANG_TEXT_TIPHELP);
	gApplicationLog.printf("\nexecCommand : [%s][%s]",cmd,param);
}
//===============================================================================================
// Game intro
int runIntro()
{
	int  i;

	// Load content, if not loaded already
	if(introStarted == 0)
	{
		introStarted = 1;
		introImage.LoadFromFile("he-graphics/intro-background.png",1280,768,1);
		startTime = system_TimeMilliSeconds();

		// * Initialize intro windows
		for(i = 0; i < INTRO_WINDOW_COUNT - 1; i++)
		{
			gImageIntroWindow[i].LoadFromFile("he-graphics/intro-smallwindow.png",204,153);
			gImageIntroWindow[i].AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
			gImageIntroWindow[i].mAnimationModifier->Reset();
			gImageIntroWindow[i].mAnimationModifier->Stop();
		}

		// * This is the final, larger window
		gImageIntroWindow[i].LoadFromFile("he-graphics/intro-largewindow.png",434,423);
		gImageIntroWindow[i].AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
		gImageIntroWindow[i].mAnimationModifier->Reset();
		gImageIntroWindow[i].mAnimationModifier->Stop();

		gImageIntroWindowIndex = 0;
		gImageIntroWindow[gImageIntroWindowIndex].mAnimationModifier->Start();
		gIntroTextIndex = -1;
		gIntroTextPos   = 0;

		// Start intro music
		gameMusic.loadPlayList("he-music/he-playlist-intro.txt");
		gameMusic.Play();

		// Initialize tooltips
		for(i = 0; i < INTRO_TOOLTIP_COUNT; i++)
		{
			intro_tooltip_size[i] = 0;
		}

		intro_tooltip_x[0] = ((g_screen_width  - 800) / 2) + 10;
		intro_tooltip_y[0] = ((g_screen_height - 600) / 2) + 10;

		intro_tooltip_x[1] = ((g_screen_width  - 800) / 2) + 10 - INTRO_TOOLTIP_SIZE / 2;
		intro_tooltip_y[1] = ((g_screen_height - 600) / 2) + 10 + (450 - INTRO_TOOLTIP_SIZE);

		intro_tooltip_x[2] = ((g_screen_width  - 800) / 2) + 10 + 450;
		intro_tooltip_y[2] = ((g_screen_height - 600) / 2) + 10 + (150 - INTRO_TOOLTIP_SIZE);

		intro_tooltip_x[3] = ((g_screen_width  - 800) / 2) + 10 + (800 - INTRO_TOOLTIP_SIZE * 2);
		intro_tooltip_y[3] = ((g_screen_height - 600) / 2) + 10 + (250 - INTRO_TOOLTIP_SIZE);

		intro_tooltip_x[4] = 460 - (1280 - g_screen_width) / 2;
		intro_tooltip_y[4] = 340 + (g_screen_height - 768) / 2;

		for(i = 0; i < INTRO_WINDOW_COUNT - 1; i++)
			intro_tooltip_window[i].Create(intro_tooltip_x[i] - 3,intro_tooltip_y[i] - 10,4 * INTRO_TOOLTIP_SIZE,4 * INTRO_TOOLTIP_SIZE);		

		intro_tooltip_window[4].Create(intro_tooltip_x[4],intro_tooltip_y[4] - 10,4 * INTRO_DESC_TOOLTIP_SIZE,4 * INTRO_DESC_TOOLTIP_SIZE);		

		intro_tooltip_id = 0;

		//
        intro_desc_x    = ((g_screen_width  - 800) / 2) + 350;
		intro_desc_y    = ((g_screen_height  - 800) / 2) + (750 - INTRO_DESC_TOOLTIP_SIZE);
		intro_desc_size = 0;

		intro_desc_window.Create(intro_desc_x - INTRO_DESC_TOOLTIP_SIZE - 24 - 3,intro_desc_y - INTRO_DESC_TOOLTIP_SIZE,4 * INTRO_DESC_TOOLTIP_SIZE,4 * INTRO_DESC_TOOLTIP_SIZE);
	}

	// Render intro
	introImage.Draw(0,gDeltaY);

	// * Render popup windows
	for(i = 0; i < INTRO_WINDOW_COUNT; i++)
	{
		gImageIntroWindow[i].Draw(intro_tooltip_x[i],intro_tooltip_y[i]);
		intro_tooltip_window[i].renderTextContent();
	}

	// * Advance to next window
	if(gImageIntroWindow[gImageIntroWindowIndex].mAnimationModifier->mStarted == 0)
	{
		if(gImageIntroWindowIndex < INTRO_WINDOW_COUNT - 1)
		{
			gImageIntroWindowIndex++;
			gImageIntroWindow[gImageIntroWindowIndex].mAnimationModifier->Start();

			// * Start rendering texts, as we have the first window visible
			if(gIntroTextIndex == -1)
			{
				gIntroTextIndex = 0;
				gIntroTextPos = 0;
			}
		}
	}

	// * Render window texts
	if(gIntroTextIndex != -1 && gIntroTextIndex < INTRO_WINDOW_COUNT)
	{
		if(gIntroTextPos < strlen(intro_tooltip_text[gIntroTextIndex]))
		{
			intro_tooltip_window[gIntroTextIndex].printf("%c",intro_tooltip_text[gIntroTextIndex][gIntroTextPos]);
			if(gIntroTextPos % 5 == 0)
				gameMusic.PlayWav("he-soundfx/hesfx-tick.wav");
			gIntroTextPos++;
		}
		else
		{
			// * Advance to render next window text
			gIntroTextIndex++;
			gIntroTextPos = 0;
			gApplicationLog.printf("\nIntro -> %d",gIntroTextIndex);
		}

	}

	// * Intro finished, wait a few seconds, and skip to the main menu
	if(gIntroTextIndex == INTRO_WINDOW_COUNT && gIntroTextPos == 0)
	{
		gIntroWaitTime += gFrameTime;
		if(gIntroWaitTime > HE_INTRO_WAITTIME)
			return 1;
	}

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Set help tooltip
void setHelpToolTip(char* msg)
{
	gWindowHelpTooltip.clear();
	gWindowHelpTooltip.printf("%s%s",LANG_TEXT_TIP,msg);

	gToolTipStartTime = system_TimeSeconds();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Set current game mod
void setGameMod(char* pModFilePath)
{
	FILE* file;
	int   i;

	gApplicationLog.printf("\n setGameMod() - [%s]",pModFilePath);
	strcpy(gModPath,pModFilePath);

	file = fopen(pModFilePath,"r");
		fgets(gModName,1024,file);        stripNewLine(gModName);
		fgets(gModFolder,1024,file);      stripNewLine(gModFolder);
		fgets(gModDescription,1024,file); stripNewLine(gModDescription);
	fclose(file);

	// * Load level list
	gApplicationLog.printf("\n setGameMod() - loading level list for (%s)- ",gModName);
	file = fopen(createModFilePath("level-list"),"r");
		fscanf(file,"%d",&gModLevelCount);
		gApplicationLog.printf("%d levels",gModLevelCount);
		for(i = 0; i < gModLevelCount; i++)
		{
			fscanf(file,"%s",gModLevelList[i]);
			gApplicationLog.printf("\n -[%s]",gModLevelList[i]);
		}
	fclose(file);

	gApplicationLog.printf("\n setGameMod() - done.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Save current game
void saveGame()
{
	hestring filename;
	sprintf(filename,"he-savegames/%s.profile",gPlayerNickname);
	hePlayer.saveGame(filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load savegame list
void loadProfiles()
{
	FILE*              mFile;
    gProfileCount = 0;

#ifdef PLATFORM_WINDOWS
	struct _finddata_t fileinfo;
	long               filehandle = -1L;
	int                nRes;

	// * Find all existing saved profiles
	filehandle = _findfirst("he-savegames/*.profile",&fileinfo);
	if(filehandle != -1L)
	{
		strcpy(gProfileFileNames[gProfileCount],"he-savegames/");
		strcat(gProfileFileNames[gProfileCount],fileinfo.name);

		mFile = fopen(gProfileFileNames[gProfileCount],"rt");
		fgets(gProfileNames[gProfileCount],1024,mFile);
		stripNewLine(gProfileNames[gProfileCount]);
		fclose(mFile);

		// * Make sure the savegame wasn't "edited"
		if(hePlayer.validateSaveGame(gProfileFileNames[gProfileCount]) == 1)
			gProfileCount++;

		nRes = _findnext(filehandle,&fileinfo);
		

		while((-1L != nRes) && (gProfileCount < HE_MAX_PROFILES - 1))
		{
			strcpy(gProfileFileNames[gProfileCount],"he-savegames/");
			strcat(gProfileFileNames[gProfileCount],fileinfo.name);

			mFile = fopen(gProfileFileNames[gProfileCount],"rt");
			fgets(gProfileNames[gProfileCount],1024,mFile);
			stripNewLine(gProfileNames[gProfileCount]);
			fclose(mFile);
	
			// * Make sure the savegame wasn't "edited"
			if(hePlayer.validateSaveGame(gProfileFileNames[gProfileCount]) == 1)
				gProfileCount++;

			nRes = _findnext(filehandle,&fileinfo);
		}
	}

#endif
    
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    char           buff[256];
    struct dirent *dptr;
    DIR           *dirp;
    
    strcpy(buff,"he-savegames");
    
    if(((dirp=opendir(buff))==NULL))
    	return;
    
    while(dptr = readdir(dirp))
    {
    	if(strstr(dptr->d_name,".profile") != NULL)
    	{
			strcpy(gProfileFileNames[gProfileCount],"he-savegames/");
			strcat(gProfileFileNames[gProfileCount],dptr->d_name);
            
			mFile = fopen(gProfileFileNames[gProfileCount],"rt");
			fgets(gProfileNames[gProfileCount],1024,mFile);
			stripNewLine(gProfileNames[gProfileCount]);
			fclose(mFile);
            
			// * Make sure the savegame wasn't "edited"
			if(hePlayer.validateSaveGame(gProfileFileNames[gProfileCount]) == 1)
				gProfileCount++;
    	}
    }
    closedir(dirp);
#endif

	// * Last profile is actually a name to return to the main menu
	strcpy(gProfileNames[gProfileCount],LANG_TEXT_RETURNTOMENU);
	gProfileCount++;
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load mod list
void loadMods()
{
	FILE*              mFile;
	gModCount = 0;

#ifdef PLATFORM_WINDOWS
	struct _finddata_t fileinfo;
	long               filehandle = -1L;
	int                nRes;

	// * Find all existing saved Mods
	filehandle = _findfirst("*.mod",&fileinfo);
	if(filehandle != -1L)
	{
		strcpy(gModFileNames[gModCount],fileinfo.name);

		mFile = fopen(gModFileNames[gModCount],"rt");
		fgets(gModNames[gModCount],1024,mFile);
		fgets(gModNames[gModCount],1024,mFile);
		fgets(gModNames[gModCount],1024,mFile);
		stripNewLine(gModNames[gModCount]);
		fclose(mFile);

		gModCount++;

		nRes = _findnext(filehandle,&fileinfo);
		

		while((-1L != nRes) && (gModCount < HE_MAX_PROFILES - 1))
		{
			strcpy(gModFileNames[gModCount],fileinfo.name);

			mFile = fopen(gModFileNames[gModCount],"rt");
			fgets(gModNames[gModCount],1024,mFile);
			fgets(gModNames[gModCount],1024,mFile);
			fgets(gModNames[gModCount],1024,mFile);
			stripNewLine(gModNames[gModCount]);
			fclose(mFile);
	
			gModCount++;

			nRes = _findnext(filehandle,&fileinfo);
		}
	}
#endif
    
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    char           buff[256];
    struct dirent *dptr;
    DIR           *dirp;
    
    strcpy(buff,".");
    
    if(((dirp=opendir(buff))==NULL))
    {
        gApplicationLog.printf("\nloadMods() - Failed to open mods directory.	");
    	return;
    }
    while(dptr = readdir(dirp))
    {
    	if(strstr(dptr->d_name,".mod") != NULL)
    	{
			strcpy(gModFileNames[gModCount],dptr->d_name);
            
			mFile = fopen(gModFileNames[gModCount],"rt");
			fgets(gModNames[gModCount],1024,mFile);
			fgets(gModNames[gModCount],1024,mFile);
			fgets(gModNames[gModCount],1024,mFile);
			stripNewLine(gModNames[gModCount]);
			fclose(mFile);
            
            gApplicationLog.printf("\nloadMods()-[%s]",dptr->d_name);
            
			gModCount++;
    	}
    }
    closedir(dirp);
#endif
    
	// * Last Mod is actually a name to return to the main menu
	strcpy(gModNames[gModCount],LANG_TEXT_RETURNTOMENU);
	gModCount++;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load skin list
void loadSkins()
{
    gSkinCount = 0;

#ifdef PLATFORM_WINDOWS
	struct _finddata_t fileinfo;
	long               filehandle = -1L;
	int                nRes;

	// * Find all skins
	filehandle = _findfirst("he-skins/*.*",&fileinfo); // skip "."
	nRes = _findnext(filehandle,&fileinfo); // skip ".."
	nRes = _findnext(filehandle,&fileinfo);
	while((-1L != nRes) && (gSkinCount < HE_MAX_PROFILES - 1))
	{
		strcpy(gSkinNames[gSkinCount],fileinfo.name);
		gSkinCount++;
		gApplicationLog.printf("\n enumerating skin : [%s]",fileinfo.name);
		nRes = _findnext(filehandle,&fileinfo);
	}
#endif
    
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    char           buff[256];
    struct dirent *dptr;
    DIR           *dirp;
    
    strcpy(buff,"he-skins");
    
    if(((dirp=opendir(buff))==NULL))
    	return;
    
    while(dptr = readdir(dirp))
    {
        if(strlen(dptr->d_name) > 2)
        {
            strcpy(gSkinNames[gSkinCount],dptr->d_name);
            gSkinCount++;
            gApplicationLog.printf("\n enumerating skin : [%s]",dptr->d_name);
        }
    }
    closedir(dirp);
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Display engine info and performance
void displayEngineInfo()
{
	hestring temp;

#ifndef HE_RETAILBUILD_LACEMAMBA		
	sprintf(temp,"%s - Track: %s / FPS : %4.2f (i : %4.2f)",ENGINE_VERSION,gameMusic.getTrackName(),gAverageFps / (float) gFrameCount,gCurrentFps);
    render_DrawText(woffset_x + 5,5,temp,SKINCOLOR(gSkinColorWindowText));
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * checks if a host is the bounced link
int   bouncedLink_IsHost(int pHostIndex)
{
	int i;

	for(i = 0; i < gBouncedLinkCount; i++)
		if(gBouncedLinkHostList[i] == pHostIndex)
			return 1;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Returns the tracetime multiplier for the current bounced link setup
int   bouncedLink_GetTraceMultiplier()
{
	return (gBouncedLinkCount + 1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Updates the hosts in the bounced links
void  bouncedLink_UpdateHosts()
{
	int i,j;

	for(i = 0; i < gBouncedLinkCount; i++)
	{
		if(gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount > 0)
			gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount--;

		if(gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount == 0)
		{
			for(j = i; j < gBouncedLinkCount - 1; j++)
				gBouncedLinkHostList[j] = gBouncedLinkHostList[j + 1];
			gBouncedLinkCount--;
		}
	}	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Save generic game options
void saveGameOptions()
{
	FILE* file;

    if(strlen(gSerialNumber) == 0) strcpy(gSerialNumber,"0");

	file = fopen("he-data/engine.ini","wt");
	fprintf(file,"skin %s\n",gSkinName);
	fprintf(file,"volume %d\n",gSoundVolume);
	fprintf(file,"fxvolume %d\n",gSoundFXVolume);
	fprintf(file,"mod %s\n",gModPath);
	fprintf(file,"serialnumber %s\n",gSerialNumber);
	fprintf(file,"fullscreen %d\n",gFullScreen);
	fprintf(file,"dynamic_difficulty %d\n",gDinamicDifficulty);
	fprintf(file,"demo %d\n",gDemo);
	fclose(file);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load generic game options
void loadGameOptions()
{
	FILE*    file;
	hestring temp;

    gApplicationLog.printf("\n loadGameOptions() - loading game options...");

	file = fopen("he-data/engine.ini","rt");
	fscanf(file,"%s %s",temp,gSkinName);
	fscanf(file,"%s %d",temp,&gSoundVolume);
	fscanf(file,"%s %d",temp,&gSoundFXVolume);
	fscanf(file,"%s %s",temp,&gModPath);
	fscanf(file,"%s %s",temp,gSerialNumber);
	fscanf(file,"%s %d",temp,&gFullScreen);
	fscanf(file,"%s %d",temp,&gDinamicDifficulty);
	fscanf(file,"%s %d",temp,&gDemo);
	fclose(file);

    gApplicationLog.printf("\n loadGameOptions() - loading skin colors...");
	loadSkinColors();

	gApplicationLog.printf("\n done.");
}
