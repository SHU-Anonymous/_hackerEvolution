// heEngine.cpp
// 
// Game engine
// Code: Robert Muresan (mrobert@exosyphen.com, exoSyphen Studios)
//

#include "HackerEvolutionUntold_Defines.h"
#include "heutGameLanguage.h"

#include "heEngine.h"
#include "heGameWindow.h"
#include "heSystem.h"
#include "bshInput.h"
#include "moduleRender.h"
#include "heLog.h"
#include "HackerEvolutionDefines.h"
#include "bshAudioPlaylist.h"
#include "bshAudioDirectX.h"
#include "heKeyStack.h"
#include "hePlayerInfo.h"
#include "heCommandEngine.h"
#include "heGameLevel.h"
#include <stdio.h>

#if defined(PLATFORM_WINDOWS)
#include <io.h>
#endif

#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_IOS)
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * These variables are to adjust to various screen resolutions
int gDeltaY;
int gDeltaYBackground;
int gDeltaXBackground;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Main menu items
#define MM_ITEMCOUNT 6
char* menuItems[MM_ITEMCOUNT];

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
#define DISPLAY_MODE_NEXTLEVEL        6

int gDisplayMode = DISPLAY_MODE_NORMAL;

// Variables used during the intro screen rendering
int   gIntroFileOpen  = 0, gIntroFileFinished = 0;
int   gIntroFileIndex = 0;
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
heImage      windowConsoleScroll;
int          lastScrollY = -1;

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
// * Game intro
heImage       imageBackgroundIntro;
heGameWindow  windowIntro;
long          introStartTime;
long          introFrameTime;
int           introTextDone1 = 0, introTextDone2 = 0, introTextDone3 = 0;
FILE*         introFile = NULL;
int           introCharIndex = 0;
int           skipIntro = 0;

// * main game menu
int      gMenuX,gMenuY;
heImage  gImageCursorBackground[MM_ITEMCOUNT * 2];
heImage  gImageMainMenu_Bar;
heImage  gImageMainMenu_WorldMap;
int      gLastMenuItemIndex = -1;

heImage  gMenuSkinPreview;
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

// * Text animation
heConsoleTextAnimation consoleTextAnimation;
int           	       gMapZoomedIn = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Level intro elements
heImage      gImageLevelIntroWindow;
heImage      gImageLevelIntroLocation;
heGameWindow gWindowLevelIntroLocation;
heGameWindow gWindowLevelIntroDescription;
heImage      gImageModCompleted;
heGameWindow gWindowModCompleted;

// * Level completed elements
heImage      gImageLevelCompletedWindow;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Game interface
heImage      gImageMapArea;

//heImage      gWindowSlider;
heImage      gImageTargetMapSlider;
heImage      gMouseCursor;
heImage      gHostToolTip;
heImage      gHostToolTipScan;
heImage      gWindowMessagesHighlight;
heImage      gWindowMessagesHighlight2;
int          gNewMessage = 0;            // This controls the flashing animation for new message

heGameWindow gWindowQuickHelp;
heGameWindow gWindowLevelObjectives;
heGameWindow gWindowLevelOverview;

heImage      gImageObjectiveCompleted;
heImage      gImageObjectiveUnCompleted;

heGameWindow gWindowHelpTooltip;

heGameWindow gWindowQuitLevel;
heGameWindow gWindowNextLevel;
heGameWindow gWindowPaused;

heImage      gImageCursorBackgroundQuit[2];
int          gQuitMenuIndex = -1;
int          gNextMenuIndex = -1;

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

int          gTraceCountDown = 99;

// * Engine stats and performance
float gAverageFps = 0;
float gCurrentFps = 0;
long  gFrameCount = 0;
long  gLevelTime  = 0;
long  gMaxFps     = 0;

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
// Robert mod selection update
hestring gModFileNames[100];
hestring gModNames[100];
int      gModCount = 0;
int      gModCountReal = 0;    // Actual number of mods, not capped at 5
int      gModListIndex = 0;    // Index in the mod list

// * Interface skins
hestring gSkinNames[HE_MAX_PROFILES];
int      gSkinCount = 0;
int      gSkinIndex = 0;

void loadProfiles();
void loadMods();
void loadSkins();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Bounced links 
int gBouncedLinkCount = 0;
int gBouncedLinkHostList[HE_MAX_BOUNCEDLINK];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draw game menu background
void drawMenuBackground()
{
		imageBackgroundMenu.Draw(gDeltaXBackground,gDeltaYBackground);		
        gImageMainMenu_WorldMap.Draw(ADJUST_TO_WIDTH(0),(g_screen_height - 530) / 2);
		gImageMainMenu_Bar.Draw(ADJUST_TO_WIDTH(HEUT_MANIMENU_ANIMBAR_X), ADJUST_TO_HEIGHT(HEUT_MANIMENU_ANIMBAR_Y));
		imageMainMenu.Draw(ADJUST_TO_WIDTH(HEUT_MAINMENUWINDOW_X),ADJUST_TO_HEIGHT(HEUT_MAINMENUWINDOW_Y));	        
}

//===============================================================================================
// Initialize game engine
void initGameEngine()
{
	int i;

    gApplicationLog.printf("\nModule::heEngine - Starting...");
	
	// * Load generic game options
	loadGameOptions();
	setGameMod(gModPath);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Load and initialize language stuff
	gApplicationLog.printf("\nModule::heEngine - Loading languages...");
	gameLanguage = new heGameLanguage;
	gameLanguage->LoadLanguageFile("heuntold-languages/languagedefines.txt");
	gameLanguage->SetCurrentLanguage(gGameLanguage);
	gApplicationLog.printf("\nModule::heEngine - Finished loading languages.");

	//strcpy(gSkinName,"terminal");
    strcpy(gSkinName,"heut-default");
	loadSkinColors();

    gApplicationLog.printf("\nModule::heEngine - Starting renderer...");

	if (render_Init() == -1) return;


if(gDemo == 1)
	imageBackgroundMenu.LoadFromFile(GFILE_MAINMENU_BACKGROUND_DEMO,1440,1024,0);
else
	imageBackgroundMenu.LoadFromFile(GFILE_MAINMENU_BACKGROUND,1440,1024,0);    

	imageBackgroundGame.LoadFromFile(GFILE_MAIN_BACKGROUND,1440,1024,0);
	imageBackgroundIntro.LoadFromFile(GFILE_INTRO_BACKGROUND,1440,1024,0);
	windowIntro.Create((g_screen_width - 800) / 2, (g_screen_height - 600) / 2,800,600);

	// * Load main menu graphics
	imageMainMenu.LoadFromFile(GFILE_MAINMENU_WINDOW,450,300);
	imageMainMenu.AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
	imageMainMenu.mAnimationModifier->Reset();
	imageMainMenu.mAnimationModifier->Start();

	gImageMainMenu_Bar.LoadFromFile(GFILE_MAINMENU_ANIMBAR,960,16);
	gImageMainMenu_Bar.AttachAnimationModifier(new CAnimationModifier_VScan);
	gImageMainMenu_Bar.mAnimationModifier->Reset();
	gImageMainMenu_Bar.mAnimationModifier->Start();

	gImageMainMenu_WorldMap.LoadFromFile(GFILE_MAINMENU_ANIM_WORLDMAP,1280,530);
	gImageMainMenu_WorldMap.AttachAnimationModifier(new CAnimationModifier_HScan);
	gImageMainMenu_WorldMap.mAnimationModifier->Reset(-1280,1280);
	gImageMainMenu_WorldMap.mAnimationModifier->Start();

	gMenuReturn.LoadFromFile(GFILE_MAINMENU_RETURN,80,20);
	gMenuVolumeBar.LoadFromFile(GFILE_MAINMENU_VOLUMEBAR,310,20);
	gMenuVolumeBarSlider.LoadFromFile(GFILE_MAINMENU_VOLUMEBAR_SLIDER,8,8);

    gMenuCheckTrue.LoadFromFile(GFILE_MAINMENU_CHECK_TRUE,20,20);
    gMenuCheckFalse.LoadFromFile(GFILE_MAINMENU_CHECK_FALSE,20,20);

	for(i = 0; i < MM_ITEMCOUNT; i++)
	{
		gImageCursorBackground[i].LoadFromFile(GFILE_MAINMENU_WINDOW_CURSORBACKGROUND,310,20);		

		// * Attach animation modifier
		gImageCursorBackground[i].AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
		gImageCursorBackground[i].mAnimationModifier->Reset();
	}
	
	gMenuX = ADJUST_TO_WIDTH(HEUT_MAINMENU_ITEM_X);
	gMenuY = ADJUST_TO_HEIGHT(HEUT_MAINMENU_ITEM_Y);

	// * Load mouse cursor
	gMouseCursor.LoadFromFile(createSkinPath("skin-mousecursor.png"),20,20);	

	woffset_x = (g_screen_width - 1024) / 3;
	woffset_x -= 5;
	gGameLevel.woffset_x = woffset_x;

	// Initialize playlist
	//directXAudio_Initialize(hWndMain);	

	// * Start intro
	gameMode = MODE_INTRO;

	// Load and play menu playlist
	// gameMusic.loadPlayList(HC_PLAYLIST_MENU);
	// gameMusic.Play();

    // * Sound fx
    // audio_PlaySoundFX("heuntold-soundfx/voice-welcome.wav");

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

	register_Command("deletelogs",command_DeleteLogs);
	register_Command("nextlevel",command_NextLevel);
	register_Command("tutorial",command_Tutorial);

	// * Load command aliases and register them
	gApplicationLog.printf("\n initGameEngine() - loading command aliases");

	FILE*     aliasFile;
	hestring  commandAlias, commandName;
	hestring  temp;
	heCommand *commandHandler;

	aliasFile = fopen("heuntold-data/command.alias","rt");
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
    gDeltaY = (g_screen_height - 800) / 2;
	gDeltaYBackground = (g_screen_height - 1024) / 2;
    gDeltaXBackground = (g_screen_width - 1440) / 2;
	gApplicationLog.printf("\ninitGameEngine() - gDeltaXBackground=%d, gDeltaYBackground=%d",gDeltaXBackground,gDeltaYBackground);
	// gMenuY += gDeltaY;
}

//===============================================================================================
// Update game engine (run a game frame)
void updateGameEngine(long frame_time)
{
	int i;

	// Set global frametime variable
	gFrameTime = frame_time / 1000.0f;
	gAverageFps += 1000.0f / (float) frame_time;
	gCurrentFps  = 1000.0f / (float) frame_time;
	gFrameCount++;

	render_BeginFrame(1);

	// Game intro mode
	if(gameMode == MODE_INTRO)
	{
		int intro1_done = 0;

		intro_elapsed += frame_time;
		if(intro_elapsed > intro_duration)
		{
			intro_elapsed = 0;
			intro1_done   = 1;
   		}

		// Render intro text
		render_DrawText_Large(g_screen_width / 2, g_screen_height / 2,GAMETEXT("I know I can change the future. But I wish I could change the past."),
			                  intro_color,intro_color,intro_color,0,1);
		
		// Fade color in;
		intro_color += intro_sign;
		if(intro_color > 255) {intro_color = 255; intro_sign = -1;}
		if(intro_color ==  0) {intro_color = 0;   intro_sign = 0;}

		// Continue to level if ENTER is pressed or the continue button is clicked
		if(input_getLastKey() == HEKEY_ENTER || mouseLBClick() == 1)
			intro1_done = 1;

		// Done with the first part of the intro?
		if(intro1_done == 1)
		{
            gApplicationLog.printf("\nupdateGameEngine() - finished first intro sequence");
			gameMode = MODE_INTRO2;
			gameMusic.loadPlayList(HC_PLAYLIST_INTRO);
			gameMusic.Play();

			introStartTime = system_TimeMilliSeconds();
			introFrameTime = system_TimeMilliSeconds();
		}
	}
	// ======================================================================================================================
	// Intro animation
	if(gameMode == MODE_INTRO2)
	{
		int done;
		done = runIntro();

		// Skip on mouse click or enter
		if(input_getLastKey() == HEKEY_ENTER || mouseLBClick() == 1)
		{
			if(skipIntro == 1)
				done = 1;
			else 
				skipIntro = 1;
		}
		if(done == 1)
		{
			gameMode = MODE_MENU;

			// Load and play menu playlist
			gameMusic.loadPlayList(HC_PLAYLIST_MENU);
			gameMusic.Play();

            // * Sound fx
            audio_PlaySoundFX("heuntold-soundfx/voice-welcome.wav");
		}
	}
	// ======================================================================================================================
	// Game menu mode
	if(gameMode == MODE_MENU)
	{
		static int itemIndex = -1;

		menuItems[0] = GAMETEXT("New Game");
		menuItems[1] = GAMETEXT("Load or Delete profile"); 
	if(gDemo == 0)
	{
		menuItems[2] = GAMETEXT("Select mod (DLC)");
	}
	else
		menuItems[2] = GAMETEXT("Select mod (unavailable in demo)");
	
		menuItems[3] = GAMETEXT("Options");
		menuItems[4] = GAMETEXT("Credits"); 
		menuItems[5] = GAMETEXT("Exit");


        drawMenuBackground();

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
		for(i = 0; i < MM_ITEMCOUNT; i++)
		{
			gImageCursorBackground[i].Draw(gMenuX + 5, gMenuY + 4 + 25 * (i + 1));

			if(i != itemIndex)
				render_DrawText_Large(gMenuX + 10, gMenuY + 25 * (i + 1), (char*)menuItems[i], HEUT_MENU_COLOR_1,0);
		}

		// * Display game mode name
		render_DrawText_Formated(gMenuX + 10, gMenuY + 25 * 7 + 5, HEUT_MENU_COLOR_1,"%s %s",GAMETEXT("Current game mod:"),gModName);

		// * Render selected menu item
		if(itemIndex >= 0 && itemIndex < MM_ITEMCOUNT)
		{
			if(itemIndex != gLastMenuItemIndex)
			{
				if(gLastMenuItemIndex != -1) 
				{
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Start();
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->mDirection = -1.0f;
					//gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Revert();
					//audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
					audio_PlaySoundFX(HE_SOUNDFX_MENU_TICK);
				}

				gLastMenuItemIndex = itemIndex;

				if(itemIndex != -1)
				{
					gImageCursorBackground[itemIndex].mAnimationModifier->Start();
					gImageCursorBackground[itemIndex].mAnimationModifier->mDirection = 1.0f;
				}
			}

			render_DrawText_Large(gMenuX + 10, gMenuY + 25 * (itemIndex + 1), (char*)menuItems[itemIndex], MM_COLOR_2,0,0);

			// Sound click on mouse-over
			if(gLastMenuItemIndex != itemIndex)
			{
				gLastMenuItemIndex = itemIndex;
				audio_PlaySoundFX(HE_SOUNDFX_MENU_TICK);
			}

			// Check if player clicked on any menu item
			if(mouseLBClick() == 1 || key == HEKEY_ENTER)
			{
				// Start new game
				if(itemIndex == 0)
				{
					audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
					strcpy(gPlayerNickname,"");
					strcpy(gPlayerNicknameError,"");

					gameMode = MODE_MENU_NEWGAME;

                    // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-piy.wav");

				}
				// * Load profile
				if(itemIndex == 1)
				{
					audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
					loadProfiles();
					gameMode = MODE_MENU_LOAD;
				}

				// * Select game mod
			if(gDemo == 0)
			{
				if(itemIndex == 2)
				{
					audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
					loadMods();
					gameMode = MODE_MENU_SELECTMOD;
				}
			}
			// * Game Options
				if(itemIndex == 3)
				{
					audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
					// * Load skins, and locate the current skin index
					loadSkins();
					for(i = 0; i < gSkinCount; i++)
						if(strcmp(gSkinNames[i],gSkinName) == 0) gSkinIndex = i;

					gameMode = MODE_MENU_OPTIONS;
				}
				
				// * Game credits
				if(itemIndex == 4)
				{
					audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
					gameMode = MODE_MENU_CREDITS;
				}

				// * Exit the game
				if(itemIndex == 5) 
				{
					audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
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

        drawMenuBackground();

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
				render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 4, (char*)gProfileNames[i], HEUT_MENU_COLOR_1,0);
		}

		// * Render selected menu item
		if(itemIndex >= 0 && itemIndex < gProfileCount)
		{
			if(itemIndex != gLastMenuItemIndex)
			{
				if(gLastMenuItemIndex != -1) 
				{
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Start();
					//gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Revert();
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->mDirection = -1.0f;

					if(itemIndex <= MM_ITEMCOUNT)
						audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
				}

				gLastMenuItemIndex = itemIndex;

				if(itemIndex != -1)
				{
					//gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Revert();
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->mDirection = 1.0f;
					gImageCursorBackground[itemIndex].mAnimationModifier->Start();
				}
			}

			render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (itemIndex + 1) + 4, (char*)gProfileNames[itemIndex], MM_COLOR_2,0,0);

			// Sound click on mouse-over
			if(gLastMenuItemIndex != itemIndex)
			{
				gLastMenuItemIndex = itemIndex;
				audio_PlaySoundFX("heuntold-soundfx/hesfx-click.wav");
			}

			// Check if player clicked on any menu item
			if(mouseLBClick() == 1 && itemIndex < gProfileCount)
			{
				if(itemIndex == gProfileCount - 1)
					gameMode = MODE_MENU;
				else
				{

                    // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-loading.wav");

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
		render_DrawText_Formated(gMenuX, gMenuY + 25 * 7 + 5, HEUT_MENU_COLOR_1,GAMETEXT("Click to load profile, or press DEL to delete it."));

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

        drawMenuBackground();

		// * Find active menu item
		if(mouseX < gMenuX + 310 && mouseX > gMenuX)
		{
			itemIndex = (mouseY - gMenuY) / 25;
			itemIndex--;
		}

		// * Draw menu items
		// Robert mod selection update
		//for(int i = 0; i < gModCount; i++)
		int i;
		for(i = 0; i < gModCount; i++)
		{
			gImageCursorBackground[i].Draw(gMenuX + 5, gMenuY + 4 + 25 * (i + 1));

			if(i != itemIndex)
				render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 4, (char*)gModNames[i + gModListIndex], HEUT_MENU_COLOR_1,0);
		}

		// Robert mod selection update
		if(itemIndex != i)
		{
			if(gModListIndex + gModCount < gModCountReal)
				render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 4, "... click to see more mods >>", HEUT_MENU_COLOR_1,0);
			else
				render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 4, GAMETEXT("... return to Main Menu"), HEUT_MENU_COLOR_1,0);
		}

		// * Render selected menu item
		if(itemIndex >= 0 && itemIndex < gModCount + 1)
		{
			if(itemIndex != gLastMenuItemIndex)
			{
				if(gLastMenuItemIndex != -1) 
				{
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Start();
					//gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->Revert();
					gImageCursorBackground[gLastMenuItemIndex].mAnimationModifier->mDirection = -1.0f;

					if(itemIndex <= MM_ITEMCOUNT)
						audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
				}

				gLastMenuItemIndex = itemIndex;

				if(itemIndex != -1)
				{
					gImageCursorBackground[itemIndex].mAnimationModifier->Start();
					gImageCursorBackground[itemIndex].mAnimationModifier->mDirection = 1.0f;
				}
			}

			if(itemIndex < gModCount)
			{
				render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (itemIndex + 1) + 4, (char*)gModNames[itemIndex + gModListIndex], MM_COLOR_2,0,0);
			}
			else
			{
				if(gModListIndex + gModCount < gModCountReal)
					render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (itemIndex + 1) + 4, "... click to see more mods >>", MM_COLOR_2,0,0);
					//render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 4, "CLICK TO SEE MORE MODS...", HEUT_MENU_COLOR_2,0);
				else
					render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (itemIndex + 1) + 4, GAMETEXT("... return to Main Menu"), MM_COLOR_2,0,0);
					//render_DrawText_Medium(gMenuX + 10, gMenuY + 25 * (i + 1) + 4, GAMETEXT("... return to Main Menu"), HEUT_MENU_COLOR_2,0);
			}

			// Sound click on mouse-over
			if(gLastMenuItemIndex != itemIndex)
			{
				gLastMenuItemIndex = itemIndex;
				audio_PlaySoundFX("heuntold-soundfx/hesfx-click.wav");
			}

			// Check if player clicked on any menu item
			if(mouseLBClick() == 1 && itemIndex < gModCount + 1)
			{
				if(itemIndex == gModCount)
				{
					if(gModCount + gModListIndex < gModCountReal)
						gModListIndex++;
					else
					{
						gameMode = MODE_MENU;
						// Reset shuffle
						gModListIndex = 0;
					}
				}
				else
				{
					gameMode = MODE_MENU;
					strcpy(gModPath,gModFileNames[itemIndex + gModListIndex]);
					strcpy(gModName,gModNames[itemIndex + gModListIndex]);
					setGameMod(gModFileNames[itemIndex + gModListIndex]);

					// Reset shuffle
					gModListIndex = 0;
				}
			}
		}

if(gDemo == 1)
{
    	// * Display game mode name
		render_DrawText_Formated(gMenuX + 10, gMenuY + 25 * 8 + 5, HEUT_MENU_COLOR_1,"Game mods are unavailable in the demo.");
}

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
        drawMenuBackground();

		// * This is not the best code, but performance is really not an issue here :-)
		strcpy(gSkinName,gSkinNames[gSkinIndex]);
		loadSkinColors();

		gMenuSkinPreview.LoadFromFile(createSkinPath("skin-preview.png"),250,160);

		// * Draw options menu
		// * This submenu needs to be repositioned, then reset
		gMenuX -= HEUT_MAINMENU_OPTIONS_SUBMENU_OFFSET;

        // * game language

        char temp[1024];
		sprintf(temp,"%s %s",GAMETEXT("Game language (click to change):"),GAMETEXT("english")/*gameLanguage->GetCurrentLanguage()*/);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 3, temp, HEUT_MENU_COLOR_1,0);

        // * music volume
		int _volume = gSoundVolume;
		hestring volumeTxt;
		sprintf(volumeTxt,"%s %d %%",GAMETEXT("Music volume:"),_volume);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 155, volumeTxt, HEUT_MENU_COLOR_1,0);
		gMenuVolumeBar.Draw(gMenuX + 12 - 5,gMenuY + 175 - 5);

        if(_volume > 97) _volume = 97; // * This is to fit on screen
		gMenuVolumeBarSlider.Draw(gMenuX + 12 + _volume * 3,gMenuY + 176);		

        // * sound volume
		_volume = gSoundFXVolume;	
		sprintf(volumeTxt,"%s %d %%",GAMETEXT("Sound effects volume:"),_volume);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 125, volumeTxt, HEUT_MENU_COLOR_1,0);
		gMenuVolumeBar.Draw(gMenuX + 12 - 5,gMenuY + 145 - 5);
        if(_volume > 97) _volume = 97; // * This is to fit on screen
		gMenuVolumeBarSlider.Draw(gMenuX + 12 + _volume * 3,gMenuY + 146);	

        // * return
		gMenuReturn.Draw(gMenuX + 10, gMenuY + 175 + 15);		

        // * skin option
        sprintf(temp,"%s %s",GAMETEXT("Interface skin (click to change):"),gSkinName);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 24, temp, HEUT_MENU_COLOR_1,0);
		gMenuSkinPreview.Draw(gMenuX + 395 + HEUT_MAINMENU_OPTIONS_SUBMENU_OFFSET, gMenuY + 26);

        // * serial number
if(gDemo == 0)
{

		/*
		if(system_TimeSeconds() % 2 == 0)
            sprintf(temp,"%s %s|",GAMETEXT("Serial number:"),gSerialNumber);
        else
            sprintf(temp,"%s %s",GAMETEXT("Serial number:"),gSerialNumber);
        render_DrawText_Medium(gMenuX + 12, gMenuY + 45, temp, HEUT_MENU_COLOR_1,0);
		*/

		/*
        if(gAutoSubmit == 0)
            gMenuCheckFalse.Draw(gMenuX + 12, gMenuY + 65); // 95
        else
            gMenuCheckTrue.Draw(gMenuX + 12, gMenuY + 65);
        render_DrawText_Medium(gMenuX + 12 + 30,gMenuY + 65,GAMETEXT("Automatically submit hiscore after each level"),HEUT_MENU_COLOR_1,0);
		*/

        // * dinamic difficulty
        if(gDinamicDifficulty == 0)
            gMenuCheckFalse.Draw(gMenuX + 12, gMenuY + 85); // 115
        else
            gMenuCheckTrue.Draw(gMenuX + 12, gMenuY + 85);

        render_DrawText_Medium(gMenuX + 12 + 30,gMenuY + 85,GAMETEXT("Enable dynamic game difficulty"),HEUT_MENU_COLOR_1,0);

}
        // * windowed mode
        if(gFullScreen == 0)
            gMenuCheckFalse.Draw(gMenuX + 12, gMenuY + 105); // 115
        else
            gMenuCheckTrue.Draw(gMenuX + 12, gMenuY + 105);

        render_DrawText_Medium(gMenuX + 12 + 30,gMenuY + 105,GAMETEXT("Run in windowed mode (requires restart)"),HEUT_MENU_COLOR_1,0);

		// * Handle mouse
		if(mouseLBClick() == 1)
		{
			// * return button
			if(PointInRect(mouseX,mouseY,gMenuX + 10, gMenuY + 175 + 15,80,20) == 1)
			{
				audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
				gameMode = MODE_MENU;
                saveGameOptions();
			}
			
			// * scrolling through languages            
            if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 3,300,20) == 1)
			{
				gameLanguage->SetCurrentLanguageNext();

				strcpy(gGameLanguage,gameLanguage->GetCurrentLanguage());
				audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");

				// * reload font for this language
				// render_ReloadFont();
			}

			// * scrolling through skins            
            if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 24,300,20) == 1)
			{
				gSkinIndex++;
				if(gSkinIndex == gSkinCount) gSkinIndex = 0;
				audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
			}

			// * adjust volume
			if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 175,303,10) == 1)
			{
				_volume = (mouseX - gMenuX - 12) / 3;
				gSoundVolume = _volume;
				gameMusic.SetVolume(gSoundVolume);
                // audio_PlaySoundFX("heuntold-soundfx/voice-secure.wav");
			}

			// * adjust fx volume
			if(PointInRect(mouseX,mouseY,gMenuX + 12,gMenuY + 145,303,10) == 1)
			{
				_volume = (mouseX - gMenuX - 12) / 3;
				gSoundFXVolume = _volume;
				gameMusic.SetVolume(gSoundVolume);
                audio_PlaySoundFX("heuntold-soundfx/voice-secure.wav");
			}

            // * auto hiscore submit
			/*
			if(PointInRect(mouseX,mouseY,gMenuX + 12, gMenuY + 65,300,20) == 1)
            {
                audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
                if(gAutoSubmit == 0)
                    gAutoSubmit = 1;
                else
                    gAutoSubmit = 0;
            }
			*/

            // * dinamic difficulty
			if(PointInRect(mouseX,mouseY,gMenuX + 12, gMenuY + 85,300,20) == 1)
            {
                audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
                if(gDinamicDifficulty == 0)
                    gDinamicDifficulty = 1;
                else
                    gDinamicDifficulty = 0;
            }

            // * windowed mode
			if(PointInRect(mouseX,mouseY,gMenuX + 12, gMenuY + 105,300,20) == 1)
            {
                audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
                if(gFullScreen == 0)
                    gFullScreen = 1;
                else
                    gFullScreen = 0;
            }
		}

		int key = input_getLastKey();

		/*
#ifndef DEMO        
        // * Serial number input
        key = toupper(key);
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
		*/
		// * Return to main menu
		if(key == HEKEY_ESC || key == HEKEY_ENTER)
        {
            saveGameOptions();
			gameMode = MODE_MENU;
        }

		// * This submenu needs to be repositioned, NOW reset
		gMenuX += HEUT_MAINMENU_OPTIONS_SUBMENU_OFFSET;


		// * Display engine version and fps
		displayEngineInfo();

		// render mouse cursor
		gMouseCursor.Draw(mouseX,mouseY);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * Game credits
	if(gameMode == MODE_MENU_CREDITS)
	{
        drawMenuBackground();

		render_DrawText_Medium(gMenuX + 5, gMenuY + 15,LANG_TEXT_CREDITS_1,HEUT_MENU_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 5, gMenuY + 30,LANG_TEXT_CREDITS_2,HEUT_MENU_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 5, gMenuY + 60,LANG_TEXT_CREDITS_DEV1,HEUT_MENU_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 5, gMenuY + 75,LANG_TEXT_CREDITS_DEV2,HEUT_MENU_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 5, gMenuY + 90,LANG_TEXT_CREDITS_DEV3,HEUT_MENU_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 5, gMenuY + 105,LANG_TEXT_CREDITS_DEV4,HEUT_MENU_COLOR_1,0);

		render_DrawText_Medium(gMenuX + 5, gMenuY + 135, LANG_TEXT_CREDITS_4,HEUT_MENU_COLOR_1,0);
		render_DrawText_Medium(gMenuX + 5, gMenuY + 150,LANG_TEXT_CREDITS_5,HEUT_MENU_COLOR_1,0);

		render_DrawText_Medium(gMenuX + 5, gMenuY + 180,LANG_TEXT_CREDITS_6,HEUT_MENU_COLOR_1,0);

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
        drawMenuBackground();

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
				sprintf(gPlayerNicknameError,GAMETEXT("Error! Nickname to short."));
				profileOK = 0;
			} 

			// * make sure the profile doesn't already exist
			sprintf(fileName,"heuntold-savegames/%s.profile",gPlayerNickname);
			file = fopen(fileName,"rt");
			if(file)
			{
				fclose(file);
				sprintf(gPlayerNicknameError,"%s %s",GAMETEXT("Error! Profile already exists:"),gPlayerNickname);
				profileOK = 0;

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-access-denied.wav");
			}


			if(profileOK == 1)
			{

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-accepted.wav");
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

		render_DrawText_Large(gMenuX + 10, gMenuY + 25,GAMETEXT("Choose your nickname:"),HEUT_MENU_COLOR_1,0);
		render_DrawText_Large(gMenuX + 10, gMenuY + 75,temp,HEUT_MENU_COLOR_1,0);
		render_DrawText(gMenuX + 5, gMenuY + 110,GAMETEXT("Press ENTER to create new profile."),HEUT_MENU_COLOR_1,0);
		render_DrawText(gMenuX + 5, gMenuY + 125,gPlayerNicknameError,HEUT_MENU_COLOR_1,0);
		
		// Display engine version and fps
		displayEngineInfo();

		// render mouse cursor
		gMouseCursor.Draw(mouseX,mouseY);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level intro mode
	if(gameMode == MODE_LEVELINTRO)
	{
		imageBackgroundGame.Draw(gDeltaXBackground,gDeltaYBackground);
        drawMenuBackground();
		
		// * Draw intro graphics
		gImageLevelIntroWindow.Draw(240 - ((1280 - g_screen_width) / 2),135  + gDeltaY);
		gImageLevelIntroLocation.Draw(HE_LEVELINTRO_IMAGE_X - ((1280 - g_screen_width) / 2),HE_LEVELINTRO_IMAGE_Y + gDeltaY);

		gWindowLevelIntroLocation.displayFile(createModLevelFilePath("levelintro-text-1"),1);

if(gDemo == 0)
{
        // * Dinamic performance adjustment
        if(gDinamicDifficulty == 1)
        {
            gWindowLevelIntroLocation.printfn(GAMETEXT("Game difficulty increase: %d"),hePlayer.getDinamicPerformanceMultiplier());
        }
}

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

				gIntroFileIndex++;

				if(gIntroFileIndex % 4 == 0)
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");

				//if(c == '\n') system_DelayMS(100);
				//if(c == ' ') system_DelayMS(100);
			}
		}
		
		// Continue to level if ENTER is pressed or the continue button is clicked
		if((input_getLastKey() == HEKEY_ENTER) || (mouseLBClick() == 1))
		{
			gameMode = MODE_GAME;
			gIntroFileOpen     = 0;
			gIntroFileFinished = 0;
			gIntroFileIndex    = 0;

            // * Reset this here, to avoid the game being paused if there was an ALT+TAB
            // * While in the main menu
            gFocusLost = 0;

           // * Sound fx
           // audio_PlaySoundFX("heuntold-soundfx/voice-all-systems-operational.wav");
		}
		displayEngineInfo();
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level completed mode
	if(gameMode == MODE_LEVELCOMPLETED)
	{
		imageBackgroundGame.Draw(gDeltaXBackground,gDeltaYBackground);
		
		// * Draw intro graphics
		gImageLevelCompletedWindow.Draw(240 - ((1280 - g_screen_width) / 2),135 + gDeltaY);
		gImageLevelIntroLocation.Draw(HE_LEVELINTRO_IMAGE_X - ((1280 - g_screen_width) / 2),HE_LEVELINTRO_IMAGE_Y + gDeltaY);

		gWindowLevelIntroLocation.clear();
		gWindowLevelIntroDescription.clear();
		gWindowLevelIntroLocation.printfn(GAMETEXT("Congratulations!"));
		gWindowLevelIntroLocation.printf("\n\n%s",GAMETEXT("Level completed successfully!"));
		
		// * Display statistics
		gWindowLevelIntroDescription.printfn(GAMETEXT("Score            :  %d"),hePlayer.mScore);
		gWindowLevelIntroDescription.printfn(GAMETEXT("Money            : $%d"),hePlayer.mMoney);
		gWindowLevelIntroDescription.printfn(GAMETEXT("Succesfull hacks :  %d"),hePlayer.mHackCount);
		gWindowLevelIntroDescription.printfn(GAMETEXT("Trace count      :  %d"),hePlayer.mTraceCount);
		gWindowLevelIntroDescription.printfn(GAMETEXT("Lost count       :  %d"),hePlayer.mLostCount);
		gWindowLevelIntroDescription.printf("\n");
		if(gLevelTime % 60 < 10)
			gWindowLevelIntroDescription.printfn(GAMETEXT("Level time       :  %d:%0d"),gLevelTime / 60,gLevelTime % 60);
		else
			gWindowLevelIntroDescription.printfn(GAMETEXT("Level time       :  %d:%d"),gLevelTime / 60,gLevelTime % 60);

		if(hePlayer.mMinuteCount % 60 < 10)
			gWindowLevelIntroDescription.printfn(GAMETEXT("Total time       :  %d:%0d"),hePlayer.mMinuteCount / 60,hePlayer.mMinuteCount % 60);
		else
			gWindowLevelIntroDescription.printfn(GAMETEXT("Total time       :  %d:%d"),hePlayer.mMinuteCount / 60,hePlayer.mMinuteCount % 60);

		gWindowLevelIntroDescription.printf("\n\n%s",GAMETEXT("-Special achievements:"));
		if(gAchievement_ZeroTrace == 1)
			gWindowLevelIntroDescription.printfn(GAMETEXT("Level completed with 0%% global trace : yes (+500 score points)"));
		else
			gWindowLevelIntroDescription.printfn(GAMETEXT("Level completed with 0%% global trace : no"));
		if(gAchievement_AllHosts == 1)
			gWindowLevelIntroDescription.printfn(GAMETEXT("Found all servers in the level       : yes (+500 score points)"));
		else
			gWindowLevelIntroDescription.printfn(GAMETEXT("Found all servers in the level       : no"));

if(gDemo == 0)
{
        if(gDinamicDifficulty == 1)
            gWindowLevelIntroDescription.printfn(GAMETEXT("Dynamic game difficulty              : yes (+%d score points)"),HE_SCORE_SPECIAL * (hePlayer.getDinamicPerformanceMultiplier() - 1));
        else
            gWindowLevelIntroDescription.printfn(GAMETEXT("Dynamic game difficulty              : no"));
}
        if(gAchievement_Under30 > 0)
            gWindowLevelIntroDescription.printfn(GAMETEXT("Level completed under 30 minutes     : yes (+%d score points)"),gAchievement_Under30);
        else
            gWindowLevelIntroDescription.printfn(GAMETEXT("Level completed under 30 minutes     : no"));

		gWindowLevelIntroDescription.printf("\n\n%s",GAMETEXT("Press [ENTER] to advance to the next level..."));

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
				gameMusic.loadPlayList(HC_PLAYLIST_INTRO);
				gameMusic.Play();
				gameMode = MODE_MOD_COMPLETED;
				gImageModCompleted.LoadFromFile(createModFilePath("window-modcompleted.png"),810,510);
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mod completed
	if(gameMode == MODE_MOD_COMPLETED)
	{
		imageBackgroundGame.Draw(gDeltaXBackground,gDeltaYBackground);
		
		// * Draw graphics
		gImageModCompleted.Draw(235 - ((1280 - g_screen_width) / 2),130 + gDeltaY);
		gWindowModCompleted.renderTextContent();

		// * Draw the mod completed text
		// * We keep backward compatibility with mods from the original Hacker Evolution
		// * game (if the file is not present, just skip it).
		
		char  c;
		if(gIntroFileOpen == 0)
		{
			gIntroFile     = fopen(createModFilePath("message-modcompleted.txt"),"rt");
			gIntroFileOpen = 1;
			gWindowModCompleted.clear();

			if(gIntroFile == 0)
				gIntroFileFinished = 0;
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
				gWindowModCompleted.printf("%c",c);

				gIntroFileIndex++;

				if(gIntroFileIndex % 4 == 0)
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
			}
		}
		
		// * Continue if ENTER or left click is pressed (return to main menu)
		if((input_getLastKey() == HEKEY_ENTER) || (mouseLBClick() == 1))
		{			
			gameMode = MODE_MENU;
			gDisplayMode = DISPLAY_MODE_NORMAL;

			gIntroFileOpen     = 0;
			gIntroFileFinished = 0;
			gIntroFileIndex    = 0;
    
            // * switch to menu playlist
			gameMusic.loadPlayList(HC_PLAYLIST_MENU);
			gameMusic.Play();
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level lost mode
	if(gameMode == MODE_LEVELLOST)
	{
		imageBackgroundGame.Draw(gDeltaXBackground,gDeltaYBackground);
		
		// * Draw intro graphics
		gImageLevelCompletedWindow.Draw(240 - ((1280 - g_screen_width) / 2),135 + gDeltaY);
		gImageLevelIntroLocation.Draw(HE_LEVELINTRO_IMAGE_X - ((1280 - g_screen_width) / 2),HE_LEVELINTRO_IMAGE_Y + gDeltaY);

		gWindowLevelIntroLocation.clear();
		gWindowLevelIntroDescription.clear();
		gWindowLevelIntroLocation.printf("\n\n%s",GAMETEXT("Level lost!"));
		gWindowLevelIntroLocation.printfn(GAMETEXT("Your trace level has reached 100%%."));
		gWindowLevelIntroLocation.printfn(GAMETEXT("Please restart the level."));
		
		gWindowLevelIntroLocation.renderTextContent();
		gWindowLevelIntroDescription.renderTextContent();
		
		//* Continue if ENTER or left click is pressed
		if((input_getLastKey() == HEKEY_ENTER) || (mouseLBClick() == 1))
		{
			gameMode = MODE_MENU;

			// * switch to menu playlist
			gameMusic.loadPlayList(HC_PLAYLIST_MENU);
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
		// Robert
		gGameLevel.UpdateMessages(&gWindowMessagesHighlight,&gWindowMessagesHighlight2);
		if(gWindowMessagesHighlight2.mAnimationModifier->mStarted == 1)
			gNewMessage = 1;

		imageBackgroundGame.Draw(gDeltaXBackground,gDeltaYBackground);
        //gImageMapArea.Draw();

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

		// * Suspend input when a text animation is running
		if(consoleTextAnimation.Update() == 1)
		{
			if(key != HEKEY_PAUSE) key = 0;

			gMouseLBClick = 0;
			gMouseRBClick = 0;
		}
		else
		{
			if(	gMapZoomedIn == 0)
			{
				gMapZoomedIn = 1;
				audio_PlaySoundFX("heuntold-soundfx/voice-all-systems-operational.wav");
				audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_mapzoomin.wav");
			}

			// * Console has booted, fade in targetmap image
			gGameLevel.startTargetMapAnimation();
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

		// * Dismiss next level dialog
		if(key == HEKEY_ESC && gDisplayMode == DISPLAY_MODE_NEXTLEVEL)
		{
			gDisplayMode = DISPLAY_MODE_NORMAL;
			gWindowNextLevel.mAnimationModifier->Revert();
			gWindowNextLevel.mAnimationModifier->Start();

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
               audio_PlaySoundFX("heuntold-soundfx/voice-please-confirm.wav");
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
                audio_PlaySoundFX("heuntold-soundfx/voice-system-activated.wav");
			}
			else
			{
				gDisplayMode = DISPLAY_MODE_PAUSE;
				gWindowPaused.mAnimationModifier->Revert();
				gWindowPaused.mAnimationModifier->Start();
				gameMusic.Pause();

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-pleasehold.wav");
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
			if(consoleTextAnimation.isRunning() == 0)
				windowConsole.printf("\r%s%s",hePlayer.getCommandPrompt(),commandConsole.getDisplayCommand());
		}

		// Draw console windows
		windowConsole.Draw();

		// * Console scroll bar drawing and handling
		int scrollpos;
		int x,y;

		scrollpos = windowConsole.getScrollPercent();
		scrollpos = (scrollpos * (HE_SCROLL_AREAHEIGHT - HE_SCROLL_HEIGHT)) / 100;

		x = woffset_x + HE_SCROLL_X;
		y = g_screen_height - 380 + HE_SCROLL_Y + scrollpos;

		if(windowConsole.mAnimationModifier->mStarted == 0)
			windowConsoleScroll.Draw(x,y);

		// * Check for scroll bar dragging
		// * Check if we have started scrolling
		//if(gMouseLBClick == 1)
		if(_mouseLBReleased == 0)
		{
			// * Scroll the window when the scrollbar is dragged
			//if(PointInRect(mouseX,mouseY,x,y,HE_SCROLL_WIDTH,HE_SCROLL_HEIGHT) == 1)

			// * Allow to scroll the window by draggin the mouse on the entire window area
			if(PointInRect(mouseX,mouseY,woffset_x,g_screen_height - 380, 720,380) == 1)
			{
				if(lastScrollY == - 1)
					lastScrollY = mouseY - y;
			}
		}
		// * Are we dragging the scrollbar?
		if(lastScrollY != -1 && _mouseLBReleased == 0)
		{
			//if(PointInRect(mouseX,mouseY,x,y,HE_SCROLL_WIDTH,HE_SCROLL_HEIGHT) == 1)
			{
				int percent = HE_SCROLL_AREAHEIGHT - ((mouseY + lastScrollY) - (g_screen_height - 380 + HE_SCROLL_Y));
			    percent = (percent * 100) / (HE_SCROLL_AREAHEIGHT);

				//2014gApplicationLog.printf("\n[SCROLL: %d]",percent);
				windowConsole.scroll(percent);

				lastScrollY = 0;
			}
		}
		// * Done scrolling?
		if(lastScrollY != -1)
		{
			if(mouseLBReleased() == 1)
			{
				lastScrollY = -1;
			}
		}
		
		// * Mouse wheel scroll?
		// 2014 update
		//if(mouseScroll > 0)	windowConsole.scrollUp();
		//if(mouseScroll < 0)	windowConsole.scrollDown();
		//mouseScroll = 0;

		windowWorldMap.Draw();
		gWindowSystemPanel.Draw();
		gWindowMessages.Draw();

        gImageMapArea.Draw(woffset_x, HC_WINDOW_OFFS_Y - 10);

        // * Draw window highlight, when new message has arrived
		if(gWindowMessagesHighlight.mAnimationModifier->mStarted == 1)
			gWindowMessagesHighlight.Draw((g_screen_width - 310 - woffset_x) + 5,(g_screen_height - 370) + 30);

		// * Flashing envelope highlight on new message
		if(gWindowMessagesHighlight2.mAnimationModifier->mStarted == 0 && gNewMessage == 1)
		{
			gWindowMessagesHighlight2.mAnimationModifier->Start();
			gWindowMessagesHighlight2.mAnimationModifier->Revert();
		}
		if(gWindowMessagesHighlight2.mAnimationModifier->mStarted == 1)
			gWindowMessagesHighlight2.Draw((g_screen_width - 310 - woffset_x) + 5,(g_screen_height - 370) + 25);

		// Draw others (target map and hosts)
		gGameLevel.drawTargetMap(woffset_x + WINDOW_X_OFFS + 10,WINDOW_Y_OFFS - 5);
		gGameLevel.drawHosts(woffset_x + WINDOW_X_OFFS + 10, WINDOW_Y_OFFS - 5);

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
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
					commandWindow->printfn(GAMETEXT(" Host added to bounced link: [%s]"),gGameLevel.hostList[newHostIndex].hostName);
					commandWindow->printf("\n");

					char tmpmsg[1024];
					sprintf(tmpmsg,"%s\n%s\n%s",GAMETEXT("To learn about bounced links"),GAMETEXT("type BOUNCEHELP"),GAMETEXT("in the console."));
					setHelpToolTip(tmpmsg);
				}

				// * Check if we can still bounce through this host
				if(gGameLevel.hostList[newHostIndex].mBounceCount == 0)
				{
					commandWindow->printfn(GAMETEXT(" Error: You can't bounce anymore through [%s]"),gGameLevel.hostList[newHostIndex].hostName);
					commandWindow->printfn("\n");
				}

				if(gGameLevel.hostList[newHostIndex].hackedCompletely() != 1)
				{
					commandWindow->printfn(GAMETEXT(" Error: All services on [%s] must be hacked,"),gGameLevel.hostList[newHostIndex].hostName);
					commandWindow->printfn(" %s",GAMETEXT("in order to be able to bounce through it."));
					commandWindow->printfn("%s\n",GAMETEXT(" Type: BOUNCEHELP in the console to learn more about bounced links."));
				}
			}
			else
			{
				for(i = found; i < gBouncedLinkCount - 1; i++)
					gBouncedLinkHostList[i] = gBouncedLinkHostList[i + 1];
				gBouncedLinkCount--;
				audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
				commandWindow->printfn(GAMETEXT(" Host removed from bounced link: [%s]"),gGameLevel.hostList[newHostIndex].hostName);
				commandWindow->printfn("");
			}
		}

		// * Now draw the bounced links on the map
		gGameLevel.drawBouncedLink(woffset_x + WINDOW_X_OFFS + 10, WINDOW_Y_OFFS - 5,gBouncedLinkCount,gBouncedLinkHostList);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		gGameLevel.drawHostToolTip(mouseX - woffset_x,mouseY);		

		gGameLevel.GetLevelTimeText(temp);
		gWindowSystemPanel.printText(HE_TEXT_CLOCK_X,HE_TEXT_CLOCK_Y,SKINCOLOR(gSkinColorTargetMap),temp);

		// Draw messages
		if(gGameLevel.mActiveMessageCount > 0)
		{
			gGameLevel.DisplayMessage(&gWindowMessages,gGameLevel.mActiveMessageIndex);
		}

		// Print message count
		if(gGameLevel.mActiveMessageCount > 0)
			sprintf(temp,"  [%d %s %d]",gGameLevel.mActiveMessageIndex + 1,GAMETEXT("of"),gGameLevel.mActiveMessageCount);
		else
			sprintf(temp,GAMETEXT("No messages"));
		gWindowMessages.printText(HE_TEXT_MESSAGEINFO_X,HE_TEXT_MESSAGEINFO_Y,SKINCOLOR(gSkinColorTargetMap),temp);

		// Draw text contents
		windowConsole.renderTextContent();
		gWindowMessages.renderTextContent();
		gWindowSystemPanel.renderTextContent();
		gWindowSystemPanel.printText(HE_TEXT_STATUS_X,HE_TEXT_STATUS_Y,SKINCOLOR(gSkinColorTargetMap),gGameLevel.statusPanelMessage);

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
			gWindowSystemPanel.drawProgressBar(95,335,download_PercentDone * 2,SKINCOLOR(gSkinColorProgressBar));
			gWindowSystemPanel.printText(HE_TEXT_PROGRESS_X,HE_TEXT_PROGRESS_Y,SKINCOLOR(gSkinColorProgress),GAMETEXT("Downloading: %d%%"),download_PercentDone);

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
				sprintf(gGameLevel.statusPanelMessage,GAMETEXT("idle"));

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-complete.wav");
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
			gWindowSystemPanel.drawProgressBar(95,335,upload_PercentDone * 2,SKINCOLOR(gSkinColorProgressBar));
			gWindowSystemPanel.printText(HE_TEXT_PROGRESS_X,HE_TEXT_PROGRESS_Y,SKINCOLOR(gSkinColorProgress),GAMETEXT("Uploading: %d%%"),upload_PercentDone);

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
				sprintf(gGameLevel.statusPanelMessage,GAMETEXT("idle"));

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-complete.wav");
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

				// Update host
				gGameLevel.hostList[crack_HostIndex].mTraceAdded += HE_TRACE_CRACK;

				// * Stop the tracer
				trace_InProgress = -1;
				strcpy(trace_Msg,GAMETEXT("Trace lost"));
				sprintf(gGameLevel.statusPanelMessage,GAMETEXT("idle"));

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-complete.wav");

			}
			else
			{
				// * Update ticker
				/*
				if( (time / 100) > crack_Tick)
				{
					crack_Tick++;
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");

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
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
				}

				sprintf(password,GAMETEXT("%s-[%3d chars left]"),crack_Password2,strlen(crack_Password) - crack_PercentDone);
				
				// * Draw crack animation
				x = gGameLevel.hostList[crack_HostIndex].mapx + WINDOW_X_OFFS + gGameLevel.woffset_x;
				y = gGameLevel.hostList[crack_HostIndex].mapy + WINDOW_Y_OFFS;			

				sprintf(gGameLevel.statusPanelMessage,"crack");
				gWindowSystemPanel.printText(HE_TEXT_PROGRESS_X,HE_TEXT_PROGRESS_Y,SKINCOLOR(gSkinColorProgress),"%s",password);
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

				// * Update host
				gGameLevel.hostList[decrypt_HostIndex].mTraceAdded += HE_TRACE_DECRYPT;

				// * Stop the tracer
				trace_InProgress = -1;
				strcpy(trace_Msg,GAMETEXT("Trace lost"));
				sprintf(gGameLevel.statusPanelMessage,GAMETEXT("idle"));

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-complete.wav");
			}
			else
			{
				// * Update ticker
				if( (time / 100) > decrypt_Tick)
				{
					decrypt_Tick++;
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
				}

				
				// * Draw decrypt animation
				x = gGameLevel.hostList[decrypt_HostIndex].mapx + WINDOW_X_OFFS + gGameLevel.woffset_x;
				y = gGameLevel.hostList[decrypt_HostIndex].mapy + WINDOW_Y_OFFS;			

				sprintf(gGameLevel.statusPanelMessage,"decr");
				gWindowSystemPanel.printText(HE_TEXT_PROGRESS_X,HE_TEXT_PROGRESS_Y,SKINCOLOR(gSkinColorProgress),GAMETEXT("decrypting (%4d-.-%d)"), gGameLevel.hostList[decrypt_HostIndex].mEncryptionKey - decrypt_PercentDone,decrypt_PercentDone);
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

			gGameLevel.drawTraceProgress(tracePercent, trace_HostIndex, woffset_x + WINDOW_X_OFFS + 10,WINDOW_Y_OFFS - 5,pLastBounceHost);

			if(time <= 0)
			{
				// * Player has been traced
				strcpy(trace_Msg,GAMETEXT("You have been traced!"));
				trace_InProgress = -1;
				hePlayer.mGlobalTrace += HE_TRACE_TRACED;
				hePlayer.mTraceCount++;
			}

			else
			{
				sprintf(trace_Msg,"%s    : %2d:%1d s",GAMETEXT("Trace ETA"),time / 1000, (time % 1000) / 100);

				// When we have less that 10 seconds, start a countdown
				int seconds = time / 1000;

				if(seconds <= 14 && gTraceCountDown == 99)
				{
					gTraceCountDown = 14;
					audio_PlaySoundFX("heuntold-soundfx/countdown-disconnect.wav");
				}

				if(seconds <= 14 && seconds != gTraceCountDown)
				{
					// * Refresh time here, to avoi desync of the countdown sound fx
					time = trace_Length - (system_TimeMilliSeconds() - trace_StartTime);
					seconds = time / 1000;

					gTraceCountDown = seconds;
					if(seconds <= 10)
					{
						char temp[1024];
						sprintf(temp,"heuntold-soundfx/%d.wav",gTraceCountDown);
						audio_PlaySoundFX(temp);
					}
				}
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
			gWindowSystemPanel.drawProgressBar(95,335,transfer_PercentDone * 2,SKINCOLOR(gSkinColorProgressBar));
			gWindowSystemPanel.printText(HE_TEXT_PROGRESS_X,HE_TEXT_PROGRESS_Y,SKINCOLOR(gSkinColorProgress),GAMETEXT("Transferring: %d%%"),transfer_PercentDone);

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
				sprintf(gGameLevel.statusPanelMessage,GAMETEXT("idle"));

				// * Stop the tracer
				trace_InProgress = -1;
				strcpy(trace_Msg,GAMETEXT("Trace lost"));

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-complete.wav");

                // * Global trace update
                hePlayer.mGlobalTrace += HE_TRACE_TRANSFER;

				// * Update host
				gGameLevel.hostList[gGameLevel.con_hostindex].mTraceAdded += HE_TRACE_TRANSFER;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Display trace message
		// render_DrawText_Formated(woffset_x + 15, WINDOW_Y_OFFS + 332,SKINCOLOR(gSkinColorBouncedLink),trace_Msg);
		render_DrawText(woffset_x + 25, WINDOW_Y_OFFS + 327,trace_Msg,SKINCOLOR(gSkinColorBouncedLink));

		// * Render interface fx
		// gWindowSlider.Draw(woffset_x + WINDOW_HOFFS,WINDOW_Y_OFFS + WINDOW_VOFFS + 310);
		if(gImageTargetMapSlider.mAnimationModifier->mStarted == 1)
			gImageTargetMapSlider.Draw(woffset_x + WINDOW_HOFFS,WINDOW_Y_OFFS + WINDOW_VOFFS - 5);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Fade out tooltip window, if the mouse is outside of it's area			
		if(PointInRect(mouseX,mouseY,woffset_x + 5, WINDOW_Y_OFFS - 5,710,370) || (system_TimeSeconds() - gToolTipStartTime > HE_TOOLTIP_TIMEOUT))
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
				gWindowHelpTooltip.renderTextContent(-10);			
			}
			else
				gWindowHelpTooltip.Draw();			

		}
		
		// * Dismiss "new message" flashing image
		if(PointInRect(mouseX,mouseY,g_screen_width - 315 - woffset_x,g_screen_height - 375,310,370))
        {
			// * Dismiss "new message" flashing image
			gNewMessage = 0;
			gWindowMessagesHighlight2.mAnimationModifier->Stop();
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Scroll through messages
		// * previous message

		if(PointInRect(mouseX,mouseY,g_screen_width - 315 - woffset_x,g_screen_height - 375,310,370)  && gMouseLBClick == 1)
        {
		    if(PointInRect(mouseX,mouseY,(g_screen_width - 315 - woffset_x) + 20,(g_screen_height - 375) + 342,75,15))
		    {
			    audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
			    gGameLevel.PrevMessage();
		    }
           else
		   //if(PointInRect(mouseX,mouseY,(g_screen_width - 310 - woffset_x) + 120,(g_screen_height - 370) + 342,75,15) && gMouseLBClick == 1)
            {
			    audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
			    gGameLevel.NextMessage();
            }
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// * Render player hardware
		int temp_x = g_screen_width - 310 - woffset_x;
		int temp_y = WINDOW_Y_OFFS - 15;

		gHardwareCPU1[hePlayer.cpu1].Draw(temp_x + 85,temp_y + 195);
		if(hePlayer.cpu2 > -1) gHardwareCPU2[hePlayer.cpu2].Draw(temp_x + 85,temp_y + 255);
		gHardwareMem1[hePlayer.mem1].Draw(temp_x + 20,temp_y + 195);
		if(hePlayer.mem2 > -1) gHardwareMem2[hePlayer.mem2].Draw(temp_x + 20,temp_y + 255);
		gHardwareModem[hePlayer.modem].Draw(temp_x + 170,temp_y + 235);
		gHardwareFirewall[hePlayer.firewall].Draw(temp_x + 170,temp_y + 195);

		if(hePlayer.nnadapter == 0) gHardwareNNAdapter.Draw(temp_x + 200, temp_y + 275);

        // * Dirty hack to avoid the firewall icon being drawed with a messed up alpha channel
        // * when the game is paused
        // gHardwareFirewall[hePlayer.firewall].Draw(2000,2000);

		// * Hardware tooltips and info
		hestring hardwareToolTipText;
		sprintf(hardwareToolTipText,"");

		if(PointInRect(mouseX,mouseY,temp_x + 5,temp_y + 190,290,120))
			sprintf(hardwareToolTipText,GAMETEXT("Type UPGRADE, to upgrade your hardware"));

		// * cpu
		if(PointInRect(mouseX,mouseY,temp_x + 85,temp_y + 195,50,50))
			sprintf(hardwareToolTipText,GAMETEXT("CPU1: %s [%dx speed increase]"),HE_NAME_CPU[hePlayer.cpu1],hePlayer.getCPUMultiplier());

		if(PointInRect(mouseX,mouseY,temp_x + 85,temp_y + 255,50,50))
			if(hePlayer.cpu2 != -1)
				sprintf(hardwareToolTipText,GAMETEXT("CPU2: %s [%dx speed increase]"),HE_NAME_CPU[hePlayer.cpu2],hePlayer.getCPUMultiplier());
			else
				sprintf(hardwareToolTipText,GAMETEXT("CPU2: none"),hePlayer.getCPUMultiplier());
		

		// * mem
		if(PointInRect(mouseX,mouseY,temp_x + 20,temp_y + 195,30,50))
			sprintf(hardwareToolTipText,GAMETEXT("MEM1: %s"),HE_NAME_MEM[hePlayer.mem1]);

		if(PointInRect(mouseX,mouseY,temp_x + 20,temp_y + 255,30,50))
			if(hePlayer.mem2 != -1)
				sprintf(hardwareToolTipText,GAMETEXT("MEM2: %s"),HE_NAME_MEM[hePlayer.mem2]);
			else
				sprintf(hardwareToolTipText,GAMETEXT("MEM2: none"));

		// * modem
		if(PointInRect(mouseX,mouseY,temp_x + 170,temp_y + 235,110,30))
			sprintf(hardwareToolTipText,GAMETEXT("%s [%dx faster transfers]"),HE_NAME_MODEM[hePlayer.modem],hePlayer.getModemMultiplier());

		// * firewall
		if(PointInRect(mouseX,mouseY,temp_x + 170,temp_y + 195,110,30))
			if(hePlayer.firewall == 0)
				sprintf(hardwareToolTipText,GAMETEXT("No firewall installed."));
			else
				sprintf(hardwareToolTipText,GAMETEXT("%s [%dx slower trace]"),HE_NAME_FIREWALL[hePlayer.firewall],hePlayer.getFirewallMultiplier());
		
		// * neural network adapter
		if(PointInRect(mouseX,mouseY,temp_x + 200, temp_y + 275,80,30))
		{
			if(hePlayer.nnadapter == 0)
				sprintf(hardwareToolTipText,GAMETEXT("Neural Network Adapter: 2x more speed."));
			else
				sprintf(hardwareToolTipText,GAMETEXT("Neural Network Adapter: Not installed."));
		}

        //
        //if(gDisplayMode != DISPLAY_MODE_PAUSE && gDisplayMode != DISPLAY_MODE_QUIT)
        if(gDisplayMode == DISPLAY_MODE_NORMAL)
		    gWindowSystemPanel.printText(HE_TEXT_HARDWAREINFO_X, HE_TEXT_HARDWAREINFO_Y, SKINCOLOR(gSkinColorTargetMap), hardwareToolTipText);
		
		gWindowSystemPanel.clear();
		gWindowSystemPanel.printf(GAMETEXT("Score: %d"),hePlayer.mScore);

		if(hePlayer.mGlobalTrace >= HE_GLOBALTRACE_BLINK)
		{
			// * This should now blink
			if(system_TimeMilliSeconds() % 1000 > 500)
				if(hePlayer.mGlobalTrace < HE_GLOBALTRACE_BLINK_RED)
					gWindowSystemPanel.printfn("/%s %d %%",GAMETEXT("Trace:"),hePlayer.mGlobalTrace);
				else
					gWindowSystemPanel.printfn("/color 255 0 0 %s %d %%",GAMETEXT("Trace:"),hePlayer.mGlobalTrace);
			else
				gWindowSystemPanel.printfn(" ");
		}
		else
			gWindowSystemPanel.printfn(GAMETEXT("Trace: %d %%"),hePlayer.mGlobalTrace);

        gWindowSystemPanel.printfn(GAMETEXT("Dynamic difficulty factor: %d"),hePlayer.getDinamicPerformanceMultiplier());

		//gWindowSystemPanel.printf("\n");
		gWindowSystemPanel.printfn(GAMETEXT("Money       : $%d"),hePlayer.mMoney);
		gWindowSystemPanel.printfn(GAMETEXT("Hack count  :  %d"),hePlayer.mHackCount);
		gWindowSystemPanel.printfn(GAMETEXT("Trace count :  %d"),hePlayer.mTraceCount);

		if(gBouncedLinkCount == 0)
			gWindowSystemPanel.printf("\n\n%s",GAMETEXT("Bounced link : (not in use)"));
		else
		{
			if(gBouncedLinkCount == 1)
			{
				gWindowSystemPanel.printf("\n\n");
				gWindowSystemPanel.printf(GAMETEXT("Bounced link : through %d host"),gBouncedLinkCount);
			}
			else
			{
				gWindowSystemPanel.printf("\n\n");
				gWindowSystemPanel.printf(GAMETEXT("Bounced link : through %d hosts"),gBouncedLinkCount);
			}
		}

		// * Double the trace message here aswell
		if(trace_InProgress == 1)
			gWindowSystemPanel.printf("\n%s",trace_Msg);

		// * If a new objective has been completed notify the player
		int lastObjective = gGameLevel.getLastCompletedObjective();
		if(lastObjective != -1)
		{
			hestring msg;
			hestring msg2;

			sprintf(msg2,GAMETEXT("Level objective %d completed!"),lastObjective + 1);
			sprintf(msg,"%s\n%s",msg2,GAMETEXT("Press F2 to view level objectives."));
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

				// *
				commandWindow->printfn("/ %s",GAMETEXT("CONGRATULATIONS!"));
				commandWindow->printfn("/ %s",GAMETEXT("You have now completed the game level."));
				commandWindow->printfn("/ %s",GAMETEXT("You can continue to explore the level for another 10 minutes."));
				commandWindow->printfn("/ %s",GAMETEXT("Type: NEXTLEVEL in the command console when you want to proceed to the next game level."));
				commandWindow->printf("\n");

                // * Stop any ongoing actions
                download_InProgress = 0;
                upload_InProgress = 0;
                crack_InProgress = 0;
                decrypt_InProgress = 0;
                transfer_InProgress = 0;
                trace_InProgress = 0;

				// * Ask the player if he wants to advance to the next level
				gDisplayMode = DISPLAY_MODE_NEXTLEVEL;
				gWindowNextLevel.mAnimationModifier->Revert();
				gWindowNextLevel.mAnimationModifier->Start();
				gNextMenuIndex = -1;

                // * Sound fx
               audio_PlaySoundFX("heuntold-soundfx/voice-please-confirm.wav");
			}

			if(gameMode == MODE_LEVELCOMPLETED_WAIT)
			{
				if(system_TimeMilliSeconds() % 1000 < 500)
				{
					//gWindowSystemPanel.printText(HE_TEXT_PROGRESS_X,HE_TEXT_PROGRESS_Y,SKINCOLOR(gSkinColorTargetMap),LANG_TEXT_LEVELCOMP,HE_LEVELDONE_WAITTIME - (system_TimeSeconds() - gLevelDoneTime));
					gWindowSystemPanel.printfn(GAMETEXT("Level completed. %2ds left."),HE_LEVELDONE_WAITTIME - (system_TimeSeconds() - gLevelDoneTime));
				}
				// * waiting time over, jump to the next level
				if(system_TimeSeconds() - gLevelDoneTime > HE_LEVELDONE_WAITTIME || gNextLevel == 1)
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
if(gDemo == 0)
{
                    // * Submit hiscore if case
					/*
                    if(gAutoSubmit == 1)
                    {
                        command_SubmitHiscore("");
                    }
					*/
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
		//if(gDisplayMode != DISPLAY_MODE_NORMAL)
		//	render_DrawRectangle(0,0,g_screen_width,g_screen_height,64,64,64,1);

		gWindowQuickHelp.Draw();
		if (gDisplayMode == DISPLAY_MODE_QUICKHELP) gWindowQuickHelp.renderTextContent();
		gWindowLevelObjectives.Draw();
		if (gDisplayMode == DISPLAY_MODE_LEVELOBJECTIVES) 
		{
			// * Render objective overlays
			int x = (g_screen_width - 720) / 2;
			int y = (g_screen_height - 380) / 2;

			x += 15;
			y += 42;

			if(gWindowLevelObjectives.mAnimationModifier->mStarted == 0)
			for(int i = 0; i < gGameLevel.mLevelObjectiveCount; i++)
			{
				if(gGameLevel.mLevelObjectiveList[i].mCompleted == 1)
					gImageObjectiveCompleted.Draw(x,y + i * 42);
				else
					gImageObjectiveUnCompleted.Draw(x,y + i * 42);
			}

			gWindowLevelObjectives.renderTextContent();
		}
		gWindowLevelOverview.Draw();
		if (gDisplayMode == DISPLAY_MODE_LEVELOVERVIEW) gWindowLevelOverview.renderTextContent();

		gWindowQuitLevel.Draw();

		// * Game paused screen
		//if(gDisplayMode == DISPLAY_MODE_PAUSE)
		//	render_DrawRectangle(0,0,g_screen_width,g_screen_height,64,64,64,1);

		gWindowPaused.Draw();

		if(gDisplayMode == DISPLAY_MODE_QUIT && gWindowQuitLevel.mAnimationModifier->mStarted == 0)
		{			
			int xpos, ypos;
			xpos = (g_screen_width - 310) / 2;
			ypos = (g_screen_height - 160) / 2;

			//xpos += 25;
			//ypos += 80;			
			xpos += 20;
			ypos += 75;			

			gImageCursorBackgroundQuit[0].Draw(xpos,ypos);
			gImageCursorBackgroundQuit[1].Draw(xpos,ypos + 30);

			gQuitMenuIndex = -1;

			if(PointInRect(mouseX,mouseY,xpos,ypos,270, 20) && gQuitMenuIndex != 0)
			{
				gImageCursorBackgroundQuit[0].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[1].mAnimationModifier->Start();
				//gImageCursorBackgroundQuit[1].mAnimationModifier->Revert();
				gImageCursorBackgroundQuit[0].mAnimationModifier->mDirection = 1.0f;
				gImageCursorBackgroundQuit[1].mAnimationModifier->mDirection = -1.0f;
				gQuitMenuIndex = 0;
			}

			if(PointInRect(mouseX,mouseY,xpos,ypos + 30,270,20) && gQuitMenuIndex != 1)
			{
				gImageCursorBackgroundQuit[1].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[0].mAnimationModifier->Start();
				//gImageCursorBackgroundQuit[0].mAnimationModifier->Revert();
				gImageCursorBackgroundQuit[0].mAnimationModifier->mDirection = -1.0f;
				gImageCursorBackgroundQuit[1].mAnimationModifier->mDirection = 1.0f;
				gQuitMenuIndex = 1;
			}

			// Move the "yes" and "no" texts
			xpos += 10;
			ypos -= 5;

			if(gQuitMenuIndex == 0) render_DrawText_Large(xpos,ypos,GAMETEXT("yes"),32,32,32,0,0);
			else
				render_DrawText_Large(xpos,ypos,GAMETEXT("yes"),255,255,255,0,0);

			if(gQuitMenuIndex == 1) render_DrawText_Large(xpos,ypos + 30,GAMETEXT("no"),32,32,32,0,0);
			else
				render_DrawText_Large(xpos,ypos + 30,GAMETEXT("no"),255,255,255,0,0);

			// * Check if player click on an option
			if(gMouseLBClick == 1)
			{
				if(gQuitMenuIndex == 0)
				{

                    // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-closing.wav");

					gameMode = MODE_MENU;
					gDisplayMode = DISPLAY_MODE_NORMAL;
					gWindowQuitLevel.mAnimationModifier->Revert();
					gWindowQuitLevel.mAnimationModifier->Start();

					// * switch to menu playlist
					gameMusic.loadPlayList(HC_PLAYLIST_MENU);
					gameMusic.Play();

					gNextLevel   = 0;
					gSkipLevel   = 0;
				}
				if(gQuitMenuIndex == 1)
				{
					gDisplayMode = DISPLAY_MODE_NORMAL;
					gWindowQuitLevel.mAnimationModifier->Revert();
					gWindowQuitLevel.mAnimationModifier->Start();
				}
			} // if(gMouseLBClick == 1)
		} // if(gDisplayMode == DISPLAY_M...


		// * Confirmation to advance to the next level
		gWindowNextLevel.Draw();

		if(gDisplayMode == DISPLAY_MODE_NEXTLEVEL && gWindowNextLevel.mAnimationModifier->mStarted == 0)
		{			
			int xpos, ypos;
			xpos = (g_screen_width - 310) / 2;
			ypos = (g_screen_height - 160) / 2;
		
			xpos += 20;
			ypos += 65;			

			gImageCursorBackgroundQuit[0].Draw(xpos,ypos);
			gImageCursorBackgroundQuit[1].Draw(xpos,ypos + 30);

			gNextMenuIndex = -1;

			if(PointInRect(mouseX,mouseY,xpos,ypos,270, 20) && gNextMenuIndex != 0)
			{
				gImageCursorBackgroundQuit[0].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[1].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[0].mAnimationModifier->mDirection = 1.0f;
				gImageCursorBackgroundQuit[1].mAnimationModifier->mDirection = -1.0f;
				gNextMenuIndex = 0;
			}

			if(PointInRect(mouseX,mouseY,xpos,ypos + 30,270,20) && gNextMenuIndex != 1)
			{
				gImageCursorBackgroundQuit[1].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[0].mAnimationModifier->Start();
				gImageCursorBackgroundQuit[0].mAnimationModifier->mDirection = -1.0f;
				gImageCursorBackgroundQuit[1].mAnimationModifier->mDirection = 1.0f;
				gNextMenuIndex = 1;
			}

			// Move the "yes" and "no" texts
			xpos += 10;
			ypos -= 5;

			if(gNextMenuIndex == 0) render_DrawText_Large(xpos,ypos,GAMETEXT("yes"),32,32,32,0,0);
			else
				render_DrawText_Large(xpos,ypos,GAMETEXT("yes"),255,255,255,0,0);

			if(gNextMenuIndex == 1) render_DrawText_Large(xpos,ypos + 30,GAMETEXT("no"),32,32,32,0,0);
			else
				render_DrawText_Large(xpos,ypos + 30,GAMETEXT("no"),255,255,255,0,0);

			// * Check if player click on an option
			if(gMouseLBClick == 1)
			{
				if(gNextMenuIndex == 0)
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
if(gDemo == 0)
{
					// * Submit hiscore if case
					/*
                    if(gAutoSubmit == 1)
                    {
                        command_SubmitHiscore("");
                    }
					*/
}

					gApplicationLog.printf("\nLEVEL COMPELETED IN : %d:%d",gLevelTime / 60,gLevelTime % 60);
				}
				if(gNextMenuIndex == 1)
				{
					gDisplayMode = DISPLAY_MODE_NORMAL;
					gWindowNextLevel.mAnimationModifier->Revert();
					gWindowNextLevel.mAnimationModifier->Start();
				}
			} // if(gMouseLBClick == 1)
		} // if(gDisplayMode == DISPLAY_M...
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

	delete gameLanguage;

    gApplicationLog.printf("\nModule::heEngine - Closed.");
}
//===============================================================================================
// Initialize game windows
void init_NewGame(int pResetPlayerInfo)
{
	int i;

	gApplicationLog.printf("\n init new game...");

	gNextLevel   = 0;
	gMapZoomedIn = 0;

	// * Reset player info
	if(pResetPlayerInfo == 1)
	{
		hePlayer.resetPlayerInfo();
		hePlayer.setHostInfo(gPlayerNickname,"localhost");
	}

	// * Initialize windows

    // * These are adjusted to the current resolution
	gWindowLevelIntroLocation.Create(((g_screen_width - 800) / 2) + 270,124 + 25 + gDeltaY,600,180);	
    gWindowLevelIntroDescription.Create(((g_screen_width - 800) / 2) + 20,124 + 220 + gDeltaY,800,370);

	gWindowModCompleted.Create(235 - ((1280 - g_screen_width) / 2) + 380,10 + 130 + gDeltaY,405,510);

	//

    gImageMapArea.LoadFromFile(createSkinPath("skin-window-worldmap.png"),720,380);

	gImageLevelIntroWindow.LoadFromFile(createSkinPath("skin-window-levelintro.png"),810,510);
    gImageLevelCompletedWindow.LoadFromFile(createSkinPath("skin-window-levelcompleted.png"),810,510);
	gMouseCursor.LoadFromFile(createSkinPath("skin-mousecursor.png"),20,20);	
	windowConsoleScroll.LoadFromFile(createSkinPath("skin-window-scroll.png"),10,60);

	// April 15, 2010
	// These are the 4 main game windows

	windowConsole.Create(woffset_x,g_screen_height - 380, 720,380,createSkinPath("skin-window-console.png"));
	gWindowMessages.Create(g_screen_width - 320 - woffset_x,g_screen_height - 380,320,380,createSkinPath("skin-window-messages.png"));

	windowWorldMap.Create(woffset_x, WINDOW_Y_OFFS,710,370,0);
	gWindowSystemPanel.Create(g_screen_width - 320 - woffset_x, WINDOW_Y_OFFS - 10,320,380,createSkinPath("skin-window-systempanel.png"));

	// * Attach animation modifiers to the game GUI windows
	windowConsole.AttachAnimationModifier(new CAnimationModifier_FlipIn);
	windowConsole.mAnimationModifier->Reset(720,380);
	windowConsole.mAnimationModifier->SetSpeed(20);
	windowConsole.mAnimationModifier->Revert();
	windowConsole.mAnimationModifier->Start();

	// * Create popup windows
	gWindowQuickHelp.Create((g_screen_width - 720) / 2,(g_screen_height - 380) / 2,720,380,createSkinPath("skin-window-quickhelp.png"));
	gWindowQuickHelp.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowQuickHelp.mAnimationModifier->Reset(720,380);
	gWindowLevelObjectives.Create((g_screen_width - 720) / 2,(g_screen_height - 380) / 2,720,380,createSkinPath("skin-window-levelobjectives.png"));
	gWindowLevelObjectives.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowLevelObjectives.mAnimationModifier->Reset(720,380);

    gImageObjectiveCompleted.LoadFromFile(createSkinPath("skin-levelobjective-completed.png"),690,45);
    gImageObjectiveUnCompleted.LoadFromFile(createSkinPath("skin-levelobjective-uncompleted.png"),690,45);

	gWindowLevelOverview.Create((g_screen_width - 720) / 2,(g_screen_height - 380) / 2,720,380,createSkinPath("skin-window-leveloverview.png"));
	gWindowLevelOverview.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowLevelOverview.mAnimationModifier->Reset(720,380);

	gWindowQuitLevel.Create((g_screen_width - 320) / 2, (g_screen_height - 170) / 2,320,170,createSkinPath("skin-window-quitlevel.png"));
	gWindowQuitLevel.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowQuitLevel.mAnimationModifier->Reset(320,170);

	gWindowNextLevel.Create((g_screen_width - 320) / 2, (g_screen_height - 170) / 2,320,170,createSkinPath("skin-window-nextlevel.png"));
	gWindowNextLevel.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowNextLevel.mAnimationModifier->Reset(320,170);

	gWindowPaused.Create((g_screen_width - 320) / 2, (g_screen_height - 170) / 2,320,170,createSkinPath("skin-window-paused.png"));
	gWindowPaused.AttachAnimationModifier(new  CAnimationModifier_ScaleIn);
	gWindowPaused.mAnimationModifier->Reset(320,170);

	for(i = 0; i < 2; i++)
	{
		gImageCursorBackgroundQuit[i].LoadFromFile(createSkinPath("skin-cursorbackground.png"),270,20);		

		// * Attach animation modifier
		gImageCursorBackgroundQuit[i].AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
		gImageCursorBackgroundQuit[i].mAnimationModifier->Reset(768);
	}

	// final initializations
	windowConsole.printf("%s\n",GAMETEXT("console initialized."));

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

	// * Attach player info to level
	gGameLevel.mPlayerInfo = &hePlayer;

	gGameLevel.SetLevelInfo(gModName,gModLevelList[gModLevelCurrent]);
	gGameLevel.LoadFromFile();

	// Misc
	strcpy(gGameLevel.statusPanelMessage,GAMETEXT("idle"));

	// * Initialize new game
	/*
	gWindowSlider.LoadFromFile(createSkinPath("skin-slider.png"),10,10);
	gWindowSlider.AttachAnimationModifier(new CAnimationModifier_HScan);
	gWindowSlider.mAnimationModifier->Reset(20,670);
	gWindowSlider.mAnimationModifier->Start();
	*/

	gImageTargetMapSlider.LoadFromFile(createSkinPath("skin-worldmap-slider.png"),20,320);
	gImageTargetMapSlider.AttachAnimationModifier(new CAnimationModifier_HScan2);
	gImageTargetMapSlider.mAnimationModifier->Reset(20,670);
	//gImageTargetMapSlider.mAnimationModifier->Start();

	// * Initialize popup window contents
	gWindowQuickHelp.displayFile("heuntold-data/popup-quickhelp.txt",1);
	gWindowLevelOverview.displayFile(createModLevelFilePath("levelintro-text-1"),1);
	gWindowLevelOverview.printf("\n");
	gWindowLevelOverview.displayFile(createModLevelFilePath("levelintro-text-2"),0);

	// * Initialize help tooltip
	gWindowHelpTooltip.Create(woffset_x + 430, WINDOW_Y_OFFS + 260,260,85,createSkinPath("skin-tooltip.png"),0);
	gWindowHelpTooltip.AttachAnimationModifier(new  CAnimationModifier_AlphaFadeIn);
	gWindowHelpTooltip.mAnimationModifier->Reset();
	gWindowHelpTooltip.mAnimationModifier->Start();

	// * Initialize host tooltip image
	gHostToolTip.LoadFromFile(createSkinPath("skin-tooltip-host.png"),260,85);
	gHostToolTip.AttachAnimationModifier(new  CAnimationModifier_ScaleInH);
	gHostToolTip.mAnimationModifier->Reset(260,85,2);
	gGameLevel.AttachToolTipImage(&gHostToolTip);

	// * Initialize host scan tooltip image
	gHostToolTipScan.LoadFromFile(createSkinPath("skin-tooltip-host.png"),260,85);
	gHostToolTipScan.AttachAnimationModifier(new  CAnimationModifier_ScaleInHCenter);
	gHostToolTipScan.mAnimationModifier->Reset(260,85);
	gHostToolTipScan.mAnimationModifier->Start();
	gHostToolTipScan.mAnimationModifier->Revert();

	// * Message window highlight
	gWindowMessagesHighlight.LoadFromFile(createSkinPath("skin-messages-highlightbar.png"),290,15);
	gWindowMessagesHighlight.AttachAnimationModifier(new CAnimationModifier_VScan2);

	gWindowMessagesHighlight2.LoadFromFile(createSkinPath("skin-window-messages-highlight.png"),290,70);
	gWindowMessagesHighlight2.AttachAnimationModifier(new CAnimationModifier_AlphaFadeIn);
	gWindowMessagesHighlight2.mAnimationModifier->Reset();
	
	// * Set initial tooltip
	char tmpmsg[1024];

	sprintf(tmpmsg,"\n%s",GAMETEXT("Press F1 for quick help..."));
	setHelpToolTip(tmpmsg);

	// * Attach player info to level
	//gGameLevel.mPlayerInfo = &hePlayer;

	// * Load player hardware icons
	for(i = 0; i < HE_MAX_CPU; i++)
	{
		gHardwareCPU1[i].LoadFromFile(createSkinPath(HE_ICON_NAME_CPU[i]),50,50);
		gHardwareCPU2[i].LoadFromFile(createSkinPath(HE_ICON_NAME_CPU[i]),50,50);
		gHardwareMem1[i].LoadFromFile(createSkinPath(HE_ICON_NAME_MEM[i]),30,50);
		gHardwareMem2[i].LoadFromFile(createSkinPath(HE_ICON_NAME_MEM[i]),30,50);
		gHardwareModem[i].LoadFromFile(createSkinPath(HE_ICON_NAME_MODEM[i]),110,30);
		gHardwareFirewall[i].LoadFromFile(createSkinPath(HE_ICON_NAME_FIREWALL[i]),110,30);
		gHardwareNNAdapter.LoadFromFile(createSkinPath("skin-icon-nnadapter.png"),80,30);
	}

	// * Clear windows
	windowConsole.clear();
	gWindowMessages.clear();

	// * Reset command engine
	reset_CommandEngine();

	// * Clear bounced links
	gBouncedLinkCount = 0;

	// * Load and play game playlist
	gameMusic.loadPlayList("heuntold-music/hc-playlist-game.txt");
	gameMusic.Play();

	// * Console bootup animation
	consoleTextAnimation.PlayTextAnimation(&windowConsole,"heuntold-data/textanim-consoleboot.txt");
}

//===============================================================================================
// Execute command
void execCommand(char* command)
{
	char cmd[1024], param[1024];
	char _command[1024];
	int  i,index;

	// * Start a slider animation over the targetmap
	gImageTargetMapSlider.mAnimationModifier->Reset(20,670);
	gImageTargetMapSlider.mAnimationModifier->Start();

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
	{
		char tmpmsg[1024];
		sprintf(tmpmsg,"%s\n%s",GAMETEXT("Type HELP,"),GAMETEXT("to see available commands."));
		(tmpmsg);
	}
	gApplicationLog.printf("\nexecCommand : [%s][%s]",cmd,param);
}
//===============================================================================================
// Game intro
int runIntro()
{
	long       timeleft;
	char       c;

	introFrameTime = system_TimeMilliSeconds();

	imageBackgroundIntro.Draw(gDeltaXBackground,gDeltaYBackground);
    gImageMainMenu_WorldMap.Draw(ADJUST_TO_WIDTH(0),(g_screen_height - 530) / 2);

	timeleft = (106000 - (system_TimeMilliSeconds() - introStartTime));

	// * wait
	if(skipIntro == 0)
	{	
		while((system_TimeMilliSeconds() - introFrameTime) < HE_CHARANIM_WAITTIME)
		{
		}

		// 47, 88

		// * Loop through the 3 intro introFiles
		if(introTextDone1 == 0)
		{
			introFile = fopen("heuntold-data/text-intro-1.txt","rt");
			introTextDone1 = 1;
		}

		if(introTextDone2 == 0 && timeleft < 58000)
		{
			introFile = fopen("heuntold-data/text-intro-2.txt","rt");
			introTextDone2 = 1;
		}

		if(introTextDone3 == 0 && timeleft < 18000)
		{
			introFile = fopen("heuntold-data/text-intro-3.txt","rt");
			introTextDone3 = 1;
		}

		// * Display text
		if(introFile != NULL)
		{
			c = fgetc(introFile);
			if(c == EOF)
			{
				fclose(introFile);
				introFile = NULL;
			}
			else
			{
				introCharIndex++;
                if(c != '\r')
                    windowIntro.printf("%c",c);
				if(introCharIndex % 4 == 0)
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
			}
		}
	} // if(skipIntro == 0)
	else
	{
		windowIntro.clear();
		windowIntro.displayFile("heuntold-data/text-intro-1.txt");
		windowIntro.displayFile("heuntold-data/text-intro-2.txt");
		windowIntro.displayFile("heuntold-data/text-intro-3.txt");
	}

	// * Finished with the intro
	if(timeleft < 0) 
	{
		timeleft = 0;
		gameMusic.Stop();
		skipIntro = 1;
		//return 1;
	}

	//render_DrawText_Formated(5,5,128,128,128,"%6d",timeleft);

	if(timeleft > 0)
		render_DrawText_Formated((g_screen_width - 800) / 2 + 20, (g_screen_height - 600) / 2,
								 192,192,192,"%3.2f%% until login... | press ENTER to continue",((float)timeleft * 100.0f) / 106000.0f);
	else
		render_DrawText_Formated((g_screen_width - 800) / 2 + 20, (g_screen_height - 600) / 2,
								 192,192,192,"press ENTER to continue");

	windowIntro.renderTextContent();

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Set help tooltip
void setHelpToolTip(char* msg)
{
	gWindowHelpTooltip.clear();
	gWindowHelpTooltip.printf("%s\n%s",GAMETEXT("Tip:"),msg);

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

if(gDemo == 1)
{
	// Don't allow mod change in the demo
	strcpy(gModPath,"hemod-untold-demo.mod");
}

	file = fopen(pModFilePath,"r");
	if(file == nullptr)
	{
		gApplicationLog.printf("\n setGameMod() - ERROR opening mod file.");
	}
		fgets(gModName,1024,file);        stripNewLine(gModName);
		fgets(gModFolder,1024,file);      stripNewLine(gModFolder);
		fgets(gModDescription,1024,file); stripNewLine(gModDescription);
	fclose(file);

	// * Load level list
	gApplicationLog.printf("\n setGameMod(%s) - loading level list - ",createModFilePath("level-list"));
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
	sprintf(filename,"heuntold-savegames/%s.profile",gPlayerNickname);
	hePlayer.saveGame(filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load savegame list
void loadProfiles()
{
	FILE*              mFile;

	gProfileCount = 0;

#if defined(PLATFORM_WINDOWS)
	struct _finddata_t fileinfo;
	long               filehandle = -1L;
	int                nRes;

	// * Find all existing saved profiles
	filehandle = _findfirst("heuntold-savegames/*.profile",&fileinfo);
	if(filehandle != -1L)
	{
		strcpy(gProfileFileNames[gProfileCount],"heuntold-savegames/");
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
			strcpy(gProfileFileNames[gProfileCount],"heuntold-savegames/");
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

    strcpy(buff,"heuntold-savegames");

    if(((dirp=opendir(buff))==NULL))
    	return;

    while(dptr = readdir(dirp))
    {
    	if(strstr(dptr->d_name,".profile") != NULL)
    	{
			strcpy(gProfileFileNames[gProfileCount],"heuntold-savegames/");
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
	strcpy(gProfileNames[gProfileCount],GAMETEXT("... return to Main Menu"));
	gProfileCount++;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load mod list
void loadMods()
{
	FILE*              mFile;

	gModCount = 0;

#if defined(PLATFORM_WINDOWS)
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
		

		// Robert mod selection update
		//while((-1L != nRes) && (gModCount < HE_MAX_PROFILES - 1))
		while((-1L != nRes))
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

		// Cap the number of mods
		gModCountReal = gModCount;
		if(gModCount >= (HE_MAX_PROFILES - 1))
		{
			gModCount = HE_MAX_PROFILES - 1;
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
	// Robert mod selection update
	//strcpy(gModNames[gModCount],GAMETEXT("... return to Main Menu"));
	//gModCount++;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load skin list
void loadSkins()
{
	gSkinCount = 0;

#if	defined(PLATFORM_WINDOWS)
	struct _finddata_t fileinfo;
	long               filehandle = -1L;
	int                nRes;

	// * Find all skins
	filehandle = _findfirst("heuntold-skins/*.*",&fileinfo); // skip "."
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

    strcpy(buff,"heuntold-skins");

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
	int      _gMaxFps;
	
	_gMaxFps = gAverageFps / gFrameCount;
	if(_gMaxFps > gMaxFps && _gMaxFps <= 200) 
		gMaxFps = _gMaxFps;

	sprintf(temp,"%s",HEUT_ENGINE_VERSION);
	render_DrawText(woffset_x + 475,2,temp,HEUT_MENU_COLOR_1);
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
	return power_of(2,gBouncedLinkCount);
	//return (gBouncedLinkCount + 1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Updates the hosts in the bounced links
void  bouncedLink_UpdateHosts()
{
	/*
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
	*/
	int i,j,found = 1;

	for(i = 0; i < gBouncedLinkCount; i++)
	{
		if(gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount > 0)
			gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount--;
	}

	while(found == 1)
	{
		found = 0;
		
		for(i = 0; i < gBouncedLinkCount; i++)
		{
			if(gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount == 0)
			{
				found = 1;

				for(j = i; j < gBouncedLinkCount - 1; j++)
					gBouncedLinkHostList[j] = gBouncedLinkHostList[j + 1];
				
				gBouncedLinkCount--;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Save generic game options
void saveGameOptions()
{
	FILE* file;

    if(strlen(gSerialNumber) == 0) strcpy(gSerialNumber,"0");

	file = fopen("heuntold-data/engine.ini","wt");
	fprintf(file,"skin %s\n",gSkinName);
	fprintf(file,"volume %d\n",gSoundVolume);
	fprintf(file,"fxvolume %d\n",gSoundFXVolume);
	fprintf(file,"mod %s\n",gModPath);
	fprintf(file,"serialnumber %s\n",gSerialNumber);
//	fprintf(file,"autosubmit %d\n",gAutoSubmit);
	fprintf(file,"dynamic_difficulty %d\n",gDinamicDifficulty);
	fprintf(file,"windowed_mode %d\n",gFullScreen);
	fprintf(file,"language %s\n",gGameLanguage);
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

	file = fopen("heuntold-data/engine.ini","rt");
	fscanf(file,"%s %s",temp,gSkinName);
	fscanf(file,"%s %d",temp,&gSoundVolume);
	fscanf(file,"%s %d",temp,&gSoundFXVolume);
	fscanf(file,"%s %s",temp,&gModPath);
	fscanf(file,"%s %s",temp,gSerialNumber);
//	fscanf(file,"%s %d",temp,&gAutoSubmit);
	fscanf(file,"%s %d",temp,&gDinamicDifficulty);
	fscanf(file,"%s %d",temp,&gFullScreen);
	fscanf(file,"%s %s",temp,&gGameLanguage);
	fscanf(file,"%s %d",temp,&gDemo);
	fclose(file);

    gApplicationLog.printf("\n loadGameOptions() - loading skin colors...");
	loadSkinColors();

	gApplicationLog.printf("\n done.");
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Class to handle text animations in the console window
// * Class constructor
heConsoleTextAnimation::heConsoleTextAnimation()
{
	animCount = 0;
	animIndex = 0;
}
// * Class destructor
heConsoleTextAnimation::~heConsoleTextAnimation()
{
}
// * Start rendering the animation
void heConsoleTextAnimation::PlayTextAnimation(heGameWindow* window,char* fileName)
{
	animCount = 0;

	// * Load animation from file
	FILE* file;
	char  line[1000];

	file = fopen(fileName,"rt");
	if(file == 0)
	{
		gApplicationLog.printf("\nheConsoleTextAnimation::PlayTextAnimation() - failed to open : %s",fileName);
		return;
	}
	else
	{
		while(fgets(line,1000,file))
		{
			stripNewLine(line);
			strcpy(animLines[animCount],line);
			animCount++;
		}
		fclose(file);
	}

	// * Initialize variables
	animIndex  = 0;
	animWindow = window;
	lastTime   = system_TimeMilliSeconds();
}
// * Update frame
// * Return 1 if the animation is still running, and 0 if it's finished.
int heConsoleTextAnimation::Update()
{
	long time;

	if(gDisplayMode == DISPLAY_MODE_PAUSE || gFocusLost == 1)
	{
		if(animCount > 0) return 1;
		return 0;
	}

	if(animIndex < animCount)
	{
		time = system_TimeMilliSeconds();
		if(time - lastTime > HE_TEXTANIM_WAITTIME)
		{
			lastTime = time;
			animWindow->printf("\n%s",animLines[animIndex]);
			animIndex++;

			// * last line?
			if(animIndex == animCount)
				animWindow->printf("\n");

			audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
		}
	}
	else if (animIndex >= animCount)
	{
		animCount = 0;
		animIndex = 0;
	}
	if(animCount > 0) return 1;
	return 0;
}
