/*
Name: hedGameEngine-Game.cpp
Desc: Hacker Evolution Duality - Gameplay management

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#include "../generic - inc/hedGameEngine-Game.h"
#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedSystem.h"
#include "../generic - inc/hedGameSettings.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include "../generic - inc/hedSound.h"
#include "../generic - inc/hedGameAchievements.h"
#include "../generic - inc/hedPlayerProfile.h"

// IOS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

hedGame g_Game;                  // The GAME
int     g_ModCompleted = 0;

#if defined(PLATFORM_IOS)
// This is to be able to swipe the map on iPads
int lastTapX = -1;

void exoIOS_CheckPointLog();
#endif

//=======================================================================================================================================
// Misc variables
char* YesNoText[] = {"NO","YES"};

int   satelliteAlignStart;
int   satelliteAligned;
float satelliteAngle;

//=======================================================================================================================================
// Constructor
hedGame::hedGame()
{
	int i;

	// Variables
	m_FramesRendered         = 0;
	m_LeveIntroWindowCreated = 0;

	// =============================
	// Interface animations
	// FPS chart animation
	for(i = 0; i < 199; i++)
		m_FPSHistory[i] = 0;

	m_ClickCircleX = -100;
	m_ClickCircleY = -100;
}

//=======================================================================================================================================
// Destructor
hedGame::~hedGame()
{
}

//=======================================================================================================================================
// Render the level intro
void hedGame::RenderLevelIntro(int dismiss)
{
	// Skip all text animation
	if(dismiss == -1)
	{
		strcpy(m_WindowLevelIntro.m_AnimTextBuffer,"");
		m_WindowLevelIntro.m_AnimTextTimer      = 0.0f;
		m_WindowLevelIntro.m_AnimTextTimerSound = 0;

		m_WindowLevelIntro.Clear();
		m_WindowLevelIntro.DisplayFile(m_GameLevel.GetFilePath("levelintro.txt"));
	}

	// This is called once, to fade out the window, before entering the game
	if(dismiss == 1)
	{
		m_WindowLevelIntro.ForceHide();
		return;
	}

	// Create the level intro window
	if(m_LeveIntroWindowCreated == 0)
	{
		m_LeveIntroWindowCreated = 1;

		m_WindowLevelIntro.Init((g_DisplayWidth - 1024) / 2, (g_DisplayHeight - 600) / 2,1024,600);
		m_WindowLevelIntro.SetBackgroundImage(g_InterfaceManager.skinWindowLevelIntro);
		m_WindowLevelIntro.SetTextOffsets(330,60);
		m_WindowLevelIntro.m_Font       = HED_FONT_BIG10;
		m_WindowLevelIntro.ToggleVisibility();

		m_ImageLevelScreenshot.LoadFromFile(m_GameLevel.GetFilePath("level-screenshot.png"));
		m_WindowLevelIntro.attachedImage[0] = &m_ImageLevelScreenshot;
		m_WindowLevelIntro.attachedImageX[0] = 20;
		m_WindowLevelIntro.attachedImageY[0] = 65;
		m_WindowLevelIntro.attachedImageVisible[0] = 1;

		m_WindowLevelIntro.Clear();
		m_WindowLevelIntro.Anim_DisplayFile(m_GameLevel.GetFilePath("levelintro.txt"));
	}

	m_WindowLevelIntro.Render();
	m_WindowLevelIntro.RenderText(20,20,0.67f,0.87f,0.003f,HED_FONT_BIG12,m_GameLevel.m_LevelDescription[m_GameLevel.m_LevelIndex]);
}

//=======================================================================================================================================
// Render level end screen
void hedGame::RenderLevelEnd(int dismiss)
{
	// This is called once, to fade out the window, before entering the game
    if(dismiss == 1)
	{
        m_LeveIntroWindowCreated = 0;
		m_WindowLevelIntro.ForceHide();
		return;
	}

	// Create the level end window
	if(m_LeveIntroWindowCreated == 0)
	{

		// Submit score to leaderboards
		if(strcmp(g_PlayerProfileManager.getCurrentModName(),"HackerEvolutionDuality") == 0)
		{
			g_GameAchievements.STEAM_UploadLeaderboardScore(m_GameLevel.m_ServerList[m_ServerLocal].statScore);
		
			// STEAM achievements for completing the tutorial and the full game
			if(g_PlayerProfileManager.getCurrentLevel() == 1) g_GameAchievements.STEAM_UnlockAchievement(7);
			if(g_ModCompleted == 1) g_GameAchievements.STEAM_UnlockAchievement(8);
		}

		m_LeveIntroWindowCreated = 1;

		m_WindowLevelIntro.Init((g_DisplayWidth - 1024) / 2, (g_DisplayHeight - 600) / 2,1024,600);
		m_WindowLevelIntro.SetBackgroundImage(g_InterfaceManager.skinWindowLevelIntro);
		m_WindowLevelIntro.SetTextOffsets(330,60);
		m_WindowLevelIntro.m_Font       = HED_FONT_NORMAL;
		m_WindowLevelIntro.ToggleVisibility();

		m_ImageLevelScreenshot.LoadFromFile(m_GameLevel.GetFilePath("level-screenshot.png"));
		m_WindowLevelIntro.attachedImage[0] = &m_ImageLevelScreenshot;
		m_WindowLevelIntro.attachedImageX[0] = 20;
		m_WindowLevelIntro.attachedImageY[0] = 65;
		m_WindowLevelIntro.attachedImageVisible[0] = 1;

		m_WindowLevelIntro.Clear();

        // Level end texts
        m_WindowLevelIntro.Anim_DisplayText("/c1Congratulations!");
        m_WindowLevelIntro.Anim_DisplayText("\n/c1You have successfully completed your mission.");
        m_WindowLevelIntro.Anim_DisplayText("\n");
        m_WindowLevelIntro.Anim_DisplayText("\n");
        m_WindowLevelIntro.Anim_DisplayText("\n/c1Mission statistics: ");
        m_WindowLevelIntro.Anim_DisplayText("\n");
        m_WindowLevelIntro.Anim_DisplayText("\n-Flawless mission:                          %s",YesNoText[m_GameLevel.m_Stat_FlawlessLevel]);
        m_WindowLevelIntro.Anim_DisplayText("\n");
        m_WindowLevelIntro.Anim_DisplayText("\n-Score:                                     %d",m_GameLevel.m_ServerList[m_ServerLocal].statScore);
        m_WindowLevelIntro.Anim_DisplayText("\n-Time taken to complete the mission:        %s",utilFormatTime(m_GameLevel.m_Stat_LevelTime));
        m_WindowLevelIntro.Anim_DisplayText("\n");
        m_WindowLevelIntro.Anim_DisplayText("\n-Discovered all servers:                    %s",YesNoText[m_GameLevel.m_Stat_DiscoveredAllServers]);
        m_WindowLevelIntro.Anim_DisplayText("\n");

        // Open world statistics
        if(m_GameLevel.m_OpenWorld == 1)
        {
            g_GameSettings.SetValueChr("game_openworld_state",m_GameLevel.m_LevelIndex,'y');
                
            // Force score refresh
            m_GameLevel.ShowOpenWorldStatics(&m_WindowObjectives);

            m_WindowLevelIntro.Anim_DisplayText("\n/c1Open world statistics:\n");
            m_WindowLevelIntro.Anim_DisplayText("\n-Successful attacks:                        %d",m_GameLevel.m_ServerList[0].m_AIStatAttacks);
            m_WindowLevelIntro.Anim_DisplayText("\n-Average time between attacks:              %d seconds",m_GameLevel.m_ServerList[0].m_AIStatSpeed);
            m_WindowLevelIntro.Anim_DisplayText("\n-Score:                                     %d",m_GameLevel.m_ServerList[0].m_AIStatScore);
            m_WindowLevelIntro.Anim_DisplayText("\n");
#ifdef PLATFORM_WINDOWS
            // Cheat?
            int cheater = 1;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"1-assassin-europe"))   cheater = 0;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"2-thief-usa") == 0)    cheater = 0;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"3-against-the-world")) cheater = 0;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"4-the-bank-uk") == 0)  cheater = 0;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"5-deathmatch") == 0)   cheater = 0;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"6-find-it") == 0)      cheater = 0;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"7-act-fast") == 0)     cheater = 0;

            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"1-assassin-europe") == 0    && m_GameLevel._levelchecksum != 120752) cheater = 1;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"2-thief-usa") == 0          && m_GameLevel._levelchecksum != 210818) cheater = 1;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"3-against-the-world") == 0  && m_GameLevel._levelchecksum != 181002) cheater = 1;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"4-the-bank-uk") == 0        && m_GameLevel._levelchecksum != 3166965) cheater = 1;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"5-deathmatch") == 0         && m_GameLevel._levelchecksum != 120752)  cheater = 1;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"6-find-it") == 0            && m_GameLevel._levelchecksum != 180916)  cheater = 1;
            if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"7-act-fast") == 0           && m_GameLevel._levelchecksum != 1450)    cheater = 1;

            if(cheater)
            {
                g_GameSettings.SetValueStr("game_ow_name","cheater");
                m_WindowLevelIntro.Anim_DisplayText("\n");
                m_WindowLevelIntro.Anim_DisplayText("\nYOU IS A CHEATER!");
                m_WindowLevelIntro.Anim_DisplayText("\n");
            }

            if(
            sysSubmitScore(g_GameSettings.ValueStr("game_ow_name"),m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],
                           m_GameLevel.m_ServerList[0].m_AIStatScore,m_GameLevel.m_ServerList[0].m_AIStatSpeed,m_GameLevel.m_ServerList[0].m_AIStatAttacks,(int)m_GameLevel.m_Stat_LevelTime))
            {
                m_WindowLevelIntro.Anim_DisplayText("\n/c2Score submitted to online leaderboards.");
                m_WindowLevelIntro.Anim_DisplayText("\n/c2You can see the online leaderboards at http://www.HackerEvolutionDuality.com");
                m_WindowLevelIntro.Anim_DisplayText("\n");

            }
            else
            {
                m_WindowLevelIntro.Anim_DisplayText("\n/c2Score not submitted to online leaderboards.");
                m_WindowLevelIntro.Anim_DisplayText("\n/c2You can enable this feature from the OPTIONS menu.");
                m_WindowLevelIntro.Anim_DisplayText("\n");
            }
#endif			
        }

        m_WindowLevelIntro.Anim_DisplayText("\n/c1press ENTER or click the mouse, to continue...");
    }

	m_WindowLevelIntro.Render();
	m_WindowLevelIntro.RenderText(20,20,0.67f,0.87f,0.003f,HED_FONT_BIG12,m_GameLevel.m_LevelDescription[m_GameLevel.m_LevelIndex]);
}

//=======================================================================================================================================
// Initialize permanent elements (ie: not level specific)
void hedGame::InitGameElements()
{
	int popupWindowX, popupWindowX2, popupWindowY, popupWindowY2;

    // Settings
	// Robert (2013) - Always set this to 20
    if(g_GameSettings.ValueInt("input_softtouch") == 1) 
        g_TabletPCPrecision = 20;
    else
        g_TabletPCPrecision = 20;//2;

	// Reset this when loading a new level
	m_LeveIntroWindowCreated = 0;

	// Initialize windows
	popupWindowX = (g_DisplayWidth - 630) / 2;
	popupWindowY = 50 + ((g_DisplayHeight - 150) - 380) / 2;
	popupWindowX2 = (g_DisplayWidth - 320) / 2;
	popupWindowY2 = 50 + ((g_DisplayHeight - 150) - 195) / 2;


	m_WindowConsole.Init(popupWindowX, popupWindowY,630,380 - 30);
	m_WindowConsole.SetBackgroundImage(g_InterfaceManager.skinWindowConsole);
	m_WindowObjectives.Init(popupWindowX, popupWindowY, 630,380);
	m_WindowObjectives.SetBackgroundImage(g_InterfaceManager.skinWindowObjectives);
	m_WindowHelp.Init(popupWindowX, popupWindowY, 700,380);
	m_WindowHelp.SetBackgroundImage(g_InterfaceManager.skinWindowHelp);
#ifdef PLATFORM_IOS
	m_WindowHelp.DisplayFile("hed-ios-resources/help-game-ipad.txt");
#else
	m_WindowHelp.DisplayFile("hed-text/english/help-game.txt");
#endif
    m_WindowHelp.m_textHeight++;
    //m_WindowHelp.m_textWidth += 3;
    sysLogPrintDebug("m_WindowHelp(%d,%d)",m_WindowHelp.m_textWidth,m_WindowHelp.m_textHeight);

	m_WindowConsole.m_textHeight--;
	m_CommandConsole.Init(&m_WindowConsole);

	m_WindowAchievements.Init(popupWindowX, popupWindowY,630,380);
	m_WindowAchievements.SetBackgroundImage(g_InterfaceManager.skinWindowAchievements);

	// ====================================================================================================
	// ** Toolbar at the bottom of the screen
	m_ToolBar.Init((g_DisplayWidth - 1420) / 2, g_DisplayHeight - 110, 1400,100);
	m_ToolBar.SetBackgroundImage(g_InterfaceManager.skinToolbar);
	m_ToolBar.ToggleVisibility();

	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_FIREWALL,g_InterfaceManager.skinButtonFirewall,240,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_DOS,g_InterfaceManager.skinButtonDOS,360,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_EMP,g_InterfaceManager.skinButtonEMP,480,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_VOICEPRINT,g_InterfaceManager.skinButtonVoiceprint,600,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_RETINA,g_InterfaceManager.skinButtonRetina,720,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_KEYCRACK,g_InterfaceManager.skinButtonKeycrack,840,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_INTERFACE,g_InterfaceManager.skinButtonInterface,960,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_CONSOLE,g_InterfaceManager.skinButtonConsole,1080,20);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_HARDWARE,g_InterfaceManager.skinButtonHardware,240,70);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_MESSAGES,g_InterfaceManager.skinButtonMessages,360,70);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_OBJECTIVES,g_InterfaceManager.skinButtonObjectives,960,70);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_ACHIEVEMENTS,g_InterfaceManager.skinButtonAchievements,1080,70);
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_HELP,g_InterfaceManager.skinButtonHelp,1140,70);
	
	// Tablet stuff
#ifdef PLATFORM_IOS
	if(g_IsTabletPC == 1)
	CREATE_BUTTON(m_ToolBar,TOOLBAR_BUTTON_ESC,g_InterfaceManager.skinTabletESC,200 + 984,-660);
#endif
	// Adjustments for smaller buttons
	m_ToolBar.buttonWidth[TOOLBAR_BUTTON_ACHIEVEMENTS]     = 40;
	m_ToolBar.buttonWidth[TOOLBAR_BUTTON_ACHIEVEMENTS + 1] = 40;
	m_ToolBar.buttonWidth[TOOLBAR_BUTTON_HELP]             = 40;
	m_ToolBar.buttonWidth[TOOLBAR_BUTTON_HELP + 1]         = 40;

	m_WindowToolbar.Init(480 + (g_DisplayWidth - 1420) / 2, 60 + g_DisplayHeight - 110,460,45);
	m_WindowToolbar.ToggleVisibility();
	m_WindowToolbar.SetTextOffsets(10,1);
	m_WindowToolbar.SetTextColor(0.67f,0.87f,0.003f);
    m_WindowToolbar.m_textHeight++;

	// ====================================================================================================
	// ** System message window
	m_WindowSystemMessage.Init(popupWindowX2,popupWindowY2, 320, 195);
	m_WindowSystemMessage.SetBackgroundImage(g_InterfaceManager.skinWindowSystemMessage);
	CREATE_BUTTON(m_WindowSystemMessage,SMWINDOW_BUTTON_YES,g_InterfaceManager.skinButtonYes,20,145);
	CREATE_BUTTON(m_WindowSystemMessage,SMWINDOW_BUTTON_NO,g_InterfaceManager.skinButtonNo,200,145);
	m_WindowSystemMessage.m_Font = HED_FONT_BIG10;

	// ** System confirm window
	m_WindowSystemConfirm.Init(popupWindowX2,popupWindowY2, 320, 195);
	m_WindowSystemConfirm.SetBackgroundImage(g_InterfaceManager.skinWindowSystemConfirm);
	CREATE_BUTTON(m_WindowSystemConfirm,SMCONFIRM_BUTTON_OK,g_InterfaceManager.skinButtonOk,110,145);
	m_WindowSystemConfirm.m_Font = HED_FONT_BIG10;

	// ====================================================================================================
	// ** DOS attack tool
	m_WindowDOSTool.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowDOSTool.SetBackgroundImage(g_InterfaceManager.skinToolDOS);
	CREATE_BUTTON(m_WindowDOSTool,0,g_InterfaceManager.skinButtonCancel,510,54);
	m_WindowDOSTool.AttachImage(2,g_InterfaceManager.skinElementSlider,20,175,1);
	m_WindowDOSTool.AttachImage(3,g_InterfaceManager.skinElementSlider,20,205,1);
	m_WindowDOSTool.AttachImage(4,g_InterfaceManager.skinElementSlider,20,235,1);
	m_WindowDOSTool.AttachImage(5,g_InterfaceManager.skinElementSlider,20,265,1);

    // ====================================================================================================
	// ** Hardware tool
	m_WindowHardware.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowHardware.SetBackgroundImage(g_InterfaceManager.skinWindowHardwareManager);
	CREATE_BUTTON(m_WindowHardware,HARDWARE_BUTTON_FIREWALL,g_InterfaceManager.skinButtonFirewall_,20,275);
	CREATE_BUTTON(m_WindowHardware,HARDWARE_BUTTON_CPU,g_InterfaceManager.skinButtonCPU,265,275);
	CREATE_BUTTON(m_WindowHardware,HARDWARE_BUTTON_INTEGRITY,g_InterfaceManager.skinButtonIntegrity,510,275);

	// ====================================================================================================
	// ** Interface tool
	m_WindowInterfaceTool.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowInterfaceTool.SetBackgroundImage(g_InterfaceManager.skinToolInterface);
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_LOGOUT,g_InterfaceManager.skinButtonLogout,510,44);
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_TRANSFER,g_InterfaceManager.skinButtonTransfer,509,241);
	/* OLD version without DELETE button 
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_UPLOAD,g_InterfaceManager.skinButtonUpload,170,241);
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_DOWNLOAD,g_InterfaceManager.skinButtonDownload,20,241);
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_VIEW,g_InterfaceManager.skinButtonView,320,241);
	*/

	// UPDATE 1
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_DOWNLOAD,g_InterfaceManager.skinButtonDownload,20,241);
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_UPLOAD,g_InterfaceManager.skinButtonUpload,120,241);
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_VIEW,g_InterfaceManager.skinButtonView,220,241);
	CREATE_BUTTON(m_WindowInterfaceTool,INTERFACE_BUTTON_DELETE,g_InterfaceManager.skinButtonDelete,320,241);

	// Toggle's for services
	int i;
	for(i = 0; i < 3; i++)
	{
		m_WindowInterfaceTool.AttachImage(INTERFACE_TOGGLE_1 + i * 2 + 0,g_InterfaceManager.skinToggle[0],20,285 + i * 30,0);
		m_WindowInterfaceTool.AttachImage(INTERFACE_TOGGLE_1 + i * 2 + 1,g_InterfaceManager.skinToggle[1],20,285 + i * 30,0);
	}

	m_WindowFileView.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowFileView.SetBackgroundImage(g_InterfaceManager.skinToolFileView);
	CREATE_BUTTON(m_WindowFileView,BUTTON_DEFAULT,g_InterfaceManager.skinButtonClose,510,333);

    // ====================================================================================================
	// ** Firewall attack tool
	m_WindowFirewallAttack.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowFirewallAttack.SetBackgroundImage(g_InterfaceManager.skinToolFirewallAttack);
	CREATE_BUTTON(m_WindowFirewallAttack,INTERFACE_BUTTON_LOGOUT,g_InterfaceManager.skinButtonLogout,265,333);

    // ====================================================================================================
	// ** Keycrack tool
    m_WindowKeyCrackTool.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowKeyCrackTool.SetBackgroundImage(g_InterfaceManager.skinWindowKeyCrackTool);
    CREATE_BUTTON(m_WindowKeyCrackTool,TOOL_BUTTON_LOGIN,g_InterfaceManager.skinButtonLogin,75,330);
    CREATE_BUTTON(m_WindowKeyCrackTool,TOOL_BUTTON_CANCEL,g_InterfaceManager.skinButtonCancel,455,330);

    // ====================================================================================================
	// ** Retinal scan tool
    m_WindowRetinalScanTool.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowRetinalScanTool.SetBackgroundImage(g_InterfaceManager.skinWindowRetinalScanTool);
    CREATE_BUTTON(m_WindowRetinalScanTool,TOOL_BUTTON_LOGIN,g_InterfaceManager.skinButtonLogin,75,330);
    CREATE_BUTTON(m_WindowRetinalScanTool,TOOL_BUTTON_CANCEL,g_InterfaceManager.skinButtonCancel,455,330);

    // ====================================================================================================
	// ** Messages window
	m_WindowMessages.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowMessages.SetBackgroundImage(g_InterfaceManager.skinWindowMessages);
    CREATE_BUTTON(m_WindowMessages,BUTTON_DEFAULT,g_InterfaceManager.skinButtonClose,20,333);

	m_WindowMessageBody.Init(240 + popupWindowX,40 + popupWindowY, 370, 320);
	m_WindowMessageBody.SetTextOffsets(5,5);
	m_WindowMessageBody.ToggleVisibility();

	// ====================================================================================================
	// ** Voiceprint password tool
	m_WindowVoicePrintTool.Init(popupWindowX, popupWindowY, 630, 380);
	m_WindowVoicePrintTool.SetBackgroundImage(g_InterfaceManager.skinToolVoicePrintPassword);
	m_WindowVoicePrintTool.AttachImage(0,g_InterfaceManager.skinButtonLogin[0],510,54,1);
	m_WindowVoicePrintTool.AttachImage(1,g_InterfaceManager.skinButtonLogin[1],510,54,0);
	m_WindowVoicePrintTool.AttachImage(2,g_InterfaceManager.skinButtonCancel[0],510,102,1);
	m_WindowVoicePrintTool.AttachImage(3,g_InterfaceManager.skinButtonCancel[1],510,102,0);
	m_WindowVoicePrintTool.AttachImage(4,g_InterfaceManager.skinElementSlider,20,175,1);
	m_WindowVoicePrintTool.AttachImage(5,g_InterfaceManager.skinElementSlider,20,205,1);
	m_WindowVoicePrintTool.AttachImage(6,g_InterfaceManager.skinElementSlider,20,235,1);

    // ** New message notification window
    m_WindowNewMessageNotification.Init((g_DisplayWidth - 350) / 2, g_DisplayHeight - 145, 350, 50);
	m_WindowNewMessageNotification.SetBackgroundImage(g_InterfaceManager.skinMessageNotifier);
    CREATE_BUTTON(m_WindowNewMessageNotification,BUTTON_DEFAULT,g_InterfaceManager.skinButtonRead,235,11);

	m_WindowVoicePrintTool.CreateButton(0,1);
	m_WindowVoicePrintTool.CreateButton(2,3);

	m_VoicePrintTool_Position1 = 0;
	m_VoicePrintTool_Position2 = 0;

    // ** PAUSE window
    m_WindowPaused.Init(popupWindowX2,popupWindowY2, 320, 195);
    m_WindowPaused.SetBackgroundImage(g_InterfaceManager.skinWindowPaused);

    // ** AI Chat window
    //m_WindowAIChat.Init(5,55,500,55);
    m_WindowAIChat.Init((g_DisplayWidth - 1024) / 2,37,1024,30);
    m_WindowAIChat.SetTextColor(0.98,0.64f,0.07f);
    m_WindowAIChat.SetTextOffsets(0,0);
    m_WindowAIChat.ToggleVisibility();
    //m_WindowAIChat.Anim_DisplayText("AI chat online.");
    m_WindowAIChat.m_textHeight = 1;

	// Misc variables
	m_TargetMapScrollDelta     = 0.0f;
	m_TargetMapScrollDeltaLast = 0.0f;
	m_SecondsPlayed            = 0.0f;

	// Interface animations
	m_FlashTrace       = 0;
	m_LastTraceTick    = 0;

	// Achievements
	g_GameAchievements.LoadAchievements();
}
//=======================================================================================================================================
// Initialize non-permanent elements (ie: level specific)
void hedGame::InitGame()
{
	g_ModCompleted = 0;

	// Init servers
	m_ServerLocal  = 0;
	m_ServerLocked = -1;

	// Game variables
	m_FrameTime     = 0.0f;
	m_SecondsPlayed = 0.0f;

	// Misc
	satelliteAlignStart = 0;
	satelliteAligned    = 0;
	satelliteAngle      = 0.0f;
	satelliteX          = 5; 
	satelliteY          = g_DisplayHeight - 315;

	m_GameEvents.Start(HED_EVENT_BACKGROUND_BAR, TIME_BACKGROUNDBAR);

	// Misc
	m_Quit               = 0;
	m_LevelFailed        = 0;
	m_GameMode           = GAMEMODE_IDLE;
	m_GameActionProgress = 0.0f;
	m_GameActionSpeed    = 0.0f;
	m_BounceCount        = 0;

    m_IntegrityWarningLevel = 4;

    // Clear objectives window.
    // If we don't do this, it will crash when advancing to a level with *more* objectives than the previous one
    for(int i = 0; i < HED_WINDOW_MAXSUBIMAGES; i++)
    {
        m_WindowObjectives.attachedImage[i] = NULL;
        m_WindowObjectives.attachedImageVisible[i] = 0;
    }
    m_WindowToolbar.Clear();
    m_WindowConsole.Clear();
    m_WindowAIChat.Clear();

    // Trace
    m_TimeUntilTrace = 0.0f;

    // Display difficulty mode
    m_WindowToolbar.PrintTextN("Game difficulty: [%s]",g_MainMenuDifficulty[(int)g_PlayerDifficulty]);
}

//=======================================================================================================================================
// Update game state
int hedGame::UpdateGame(float frametime)
{
	int   i,j;
	float frameTimeSeconds;

    if(m_GameMode == GAMEMODE_PAUSED) return 0;

	frameTimeSeconds = frametime / 1000.0f;

	// ==================================================================================================================================
	// ** Process the console
	int cmd;
	cmd = m_CommandConsole.GetCommand();
	if(cmd != COMMAND_NONE)
	{
        // Close the console window
        if(cmd == COMMAND_CLOSE)
            m_WindowConsole.ToggleVisibility();
		
        m_CommandConsole.ExecCommand(cmd,&m_GameLevel,m_ServerLocked);
	}
	
	// ==================================================================================================================================
	// Update game level
	m_GameLevel.Update(&m_WindowObjectives, &m_WindowToolbar, frameTimeSeconds);

	// If there was a new message received, inform the player
	if(m_GameLevel.m_NewMsgReceived == 1 && m_GameMode == GAMEMODE_IDLE)
	{
		g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-incoming-transmission.wav");
		m_GameLevel.m_NewMsgReceived = 0;

        if(m_WindowNewMessageNotification.isVisible() == 0)
            m_WindowNewMessageNotification.ToggleVisibility();
	}
	
	// ==================================================================================================================================
	// AI
	char msg[1024];
	for(i = 0; i < m_GameLevel.m_ServerCount; i++)
	{
		// Process any attacks
		if(m_GameMode == GAMEMODE_MESSAGES)
            m_GameLevel.m_ServerList[i].AI_SendAttack(i,m_GameLevel.m_ServerList,m_GameLevel.m_ServerCount,&m_WindowAIChat,1);
        else
            m_GameLevel.m_ServerList[i].AI_SendAttack(i,m_GameLevel.m_ServerList,m_GameLevel.m_ServerCount,&m_WindowAIChat,0);

        // Is this AI going rogue?
        if((sysRandomNumber(AI_ROGUE_CHANCE) == 0) && m_GameLevel.m_ServerList[i].m_AIControlled == 1 && m_GameLevel.m_ServerList[i].m_Visible == 1 && m_GameLevel.m_ServerList[i].m_LevelIntegrity > 0)
        {
            if(m_GameLevel.m_OpenWorld == 0)
            {
                m_GameLevel.m_ServerList[i].AI_IncreaseHostileLevel(0,AI_ROGUE_VALUE);
                m_WindowToolbar.PrintTextN("[WARNING] %s went ROGUE!",m_GameLevel.m_ServerList[i].m_HostName);
            }
        }
	}
    
    // If we are performing an action that implies bouncing through AI controlled servers, increase their hostility level
	if(m_GameMode == GAMEMODE_ATTACK_FIREWALL || m_GameMode == GAMEMODE_ATTACK_DOS || m_GameMode == GAMEMODE_ATTACK_EMP || m_GameMode == GAMEMODE_ATTACK_VPTOOL || 
       m_GameMode == GAMEMODE_ATTACK_RETINA || m_GameMode == GAMEMODE_ATTACK_KEYCRACK ||
       m_GameMode == GAMEMODE_INTERFACE)
    {
        for(i = 0; i < m_BounceCount; i++)
            m_GameLevel.m_ServerList[m_BounceList[i]].AI_IncreaseHostileLevel(0,AI_HOSTILE_INCREASE_BOUNCE * frameTimeSeconds);
    }

	// ==================================================================================================================================
	// Timed events
	m_GameEvents.UpdateEvents(frameTimeSeconds);
	if(m_GameEvents.EventHappened(HED_EVENT_BACKGROUND_BAR))
	{
		g_InterfaceManager.skinFadeBar->m_AnimTranslateX      = 0;
		g_InterfaceManager.skinFadeBar->m_AnimTranslateXSize  = g_DisplayWidth + 32;
		g_InterfaceManager.skinFadeBar->m_AnimTranslateXSpeed = g_DisplayWidth / 5; // 5 seconds duration

		m_GameEvents.ReStart(HED_EVENT_BACKGROUND_BAR);
	}

	// ==================================================================================================================================
	// Check if we are being traced
	if(m_ServerLocked > 0)
	{
        DECREMENT_WITH_CAP(m_TimeUntilTrace,g_FrameTime,0.0f); 

        // Trace is on
        if(m_TimeUntilTrace == 0.0f)
        {
           INCREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel,g_FrameTime * (HED_TRACE_PER_SEC + pda_HED_TRACE_PER_SEC * g_PlayerDifficulty),100.0f);
           
           // Update AI hostility if a server is tracing us
           m_GameLevel.m_ServerList[m_ServerLocked].AI_IncreaseHostileLevel(0,AI_HOSTILE_INCREASE_TRACE * frameTimeSeconds);
        }
        // Sound tick whenever the trace level increases by 1 percent
		if((int)(m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel) != m_LastTraceTick)
		{
			m_LastTraceTick = (int)m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel;
			g_SoundManager.PlaySoundFX("hed-audio/soundfx-tick.wav");
		}

		if((int)(m_SecondsPlayed * 10) % 10 == 0)
			m_FlashTrace = 1;
		else
			m_FlashTrace = 0;

        // Trace countdown
        if(m_TimeUntilTrace <  m_LastTraceAlertDigit)
        {
            if(m_LastTraceAlertDigit != (int)m_TimeUntilTrace)
            {
                hedString soundFX;

                sprintf(soundFX,"hed-audio/countdown/%d.wav",(int)m_TimeUntilTrace);
                //sysLogPrint("hed-audio/countdown/%d.wav",(int)m_TimeUntilTrace);
    			if((int)m_TimeUntilTrace < 11)g_SoundManager.PlaySoundFX(soundFX);
                
                m_LastTraceAlertDigit--;
            }
        }

	}


	// ==================================================================================================================================
	// Check if the level was completed or failed
	if(m_GameMode == GAMEMODE_IDLE)
	{
		// Level completed?
		if(m_GameLevel.ObjectivesCompleted() && m_GameLevel.m_LevelCompleted == 0 && m_GameMode == GAMEMODE_IDLE)
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_LEVEL_COMPLETED;
		}

        // Player integrity == 0 || Player trace level == 100
		// This is handled in a different way. This won't allow the player to stay in the game.
        if((m_GameLevel.m_ServerList[m_ServerLocal].m_LevelIntegrity <= 0 || m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel >= 100.0f) && m_GameMode == GAMEMODE_IDLE)
		{
			m_WindowSystemConfirm.ToggleVisibility();
			m_GameMode = GAMEMODE_CONFIRM_FAILED;

            // Decrease player score
            DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocal].statScore,SCORE_LEVELFAILED,0);
            m_WindowToolbar.PrintTextN("You have lost %d score points.",SCORE_LEVELFAILED);
		}

		// Level failed?
		if(m_GameLevel.ObjectivesFailed() && m_LevelFailed == 0 && m_GameMode == GAMEMODE_IDLE)
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_LEVEL_FAILED;

			m_GameLevel.m_LevelCompleted = 0;
			m_LevelFailed    = 1;

            // Decrease player score
            DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocal].statScore,SCORE_LEVELFAILED,0);
            m_WindowToolbar.PrintTextN("You have lost %d score points.",SCORE_LEVELFAILED);
		}
	}

	m_FrameTime = frametime;

	// Seconds played
	m_SecondsPlayed += frametime / 1000.0f;


	// Update all servers
	for(i = 0; i < m_GameLevel.m_ServerCount; i++)
		m_GameLevel.m_ServerList[i].Update(frametime,&m_WindowToolbar,m_GameMode);

	// ** Bounced link handling
	// Detect if there was a doubleclick on a server and add/remove it from the bounced link list
	for(i = 1; i < m_GameLevel.m_ServerCount; i++)
	{
		if(m_GameLevel.m_ServerList[i].m_DoubleClick == 1)
		{
			m_GameLevel.m_ServerList[i].m_DoubleClick = 0;

			// Check if we can bounce through this server
			if(m_GameLevel.m_ServerList[i].isFullyHacked(m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[i]) == 0 || i == 0 || m_GameLevel.m_ServerList[i].m_LevelIntegrity == 0)
			{
				if(m_GameLevel.m_ServerList[i].m_LevelIntegrity == 0)
					_DisplayErrorMsg("Unable to bounce through [%s] (Integrity = 0).",m_GameLevel.m_ServerList[i].m_HostName);
				else
					_DisplayErrorMsg("Unable to bounce through [%s]",m_GameLevel.m_ServerList[i].m_HostName);
			}
			else
			{
				// Add/remove server from the bounce link list
				int serverIndex = bouncedLink_Present(i);

				if(serverIndex == -1)
					bouncedLink_Add(i);
				else
					bouncedLink_Remove(i);
			}
		}
	}

	// Remove any dead servers from the bounced link
	for(i = 0; i < m_BounceCount; i++)
	{
		int index = m_BounceList[i];
		if(m_GameLevel.m_ServerList[index].isFullyHacked(m_GameLevel.m_ServerList[0].m_ServerKeys[index]) == 0 || 
		   m_GameLevel.m_ServerList[index].m_LevelIntegrity == 0)
			bouncedLink_Remove(index);
	}

    // Low integrity warning
    if(m_GameLevel.m_ServerList[0].m_LevelIntegrity < m_IntegrityWarningLevel)
    {
        m_IntegrityWarningLevel = m_GameLevel.m_ServerList[0].m_LevelIntegrity;
        g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");
        m_WindowToolbar.PrintTextN("[WARNING] Your integrity level is low (%d)",m_GameLevel.m_ServerList[0].m_LevelIntegrity);

        // Reset
        if(m_GameLevel.m_ServerList[0].m_LevelIntegrity > m_IntegrityWarningLevel)
            m_IntegrityWarningLevel = m_GameLevel.m_ServerList[0].m_LevelIntegrity;
    }

    // Console forced a quit?
    if(m_GameLevel.m_Quit == 2) m_Quit = 2;

    // Level completed?
    if(m_Quit == 2)
    {
        g_GameAchievements.SaveAchievements();
        m_GameLevel.UpdateFinalStatistics();
        m_WindowLevelIntro.ToggleVisibility();

        // Save player profile
        if(m_GameLevel.m_OpenWorld == 0)
        {
            if(m_GameLevel.m_LevelIndex + 1 < m_GameLevel.m_LevelCount)
                g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].modLevelNumber++;
			else
				g_ModCompleted = 1;
			g_PlayerProfileManager.m_ProfileList[g_PlayerProfileManager.m_ProfileCurrent].score = m_GameLevel.m_ServerList[m_ServerLocal].statScore;
            g_PlayerProfileManager.SaveProfile(g_PlayerProfileManager.m_ProfileCurrent);
        }
    }
	// 
	return m_Quit;
}

//=======================================================================================================================================
// Process input
void hedGame::ProcessInput(int inputX, int inputY, int click, int clickdown, int key)
{
	int i;
	int interfaceButtonClicked;

	// Clear
	m_Click = 0;

	// Save input
	m_InputX = inputX;
	m_InputY = inputY;
	m_Click  = click;
	m_Key    = key;

    // ** Music control
    if(key == HEDKEY_F8) g_SoundManager.PlayList_Pause();
    if(key == HEDKEY_F9)
    {
            g_SoundManager.Update(1);
            m_WindowToolbar.PrintTextN("[MUSIC:%d] %s",g_SoundManager.m_PlayListIndex,g_SoundManager.m_PlayListTrackNames[g_SoundManager.m_PlayListIndex]);
    }

    int volumeSign = 0;
    if(key == HEDKEY_F10) volumeSign = -3;
    if(key == HEDKEY_F11) volumeSign =  3;
    if(volumeSign != 0)
    {
        int volume = g_GameSettings.ValueInt("sound_volume_music");
        volume += volumeSign; CAPMAX(volume,100); CAPMIN(volume,0);
        m_WindowToolbar.PrintTextN("[MUSIC] Volume: %d",volume);

        g_GameSettings.SetValueInt("sound_volume_music",volume);
        g_SoundManager.UpdateVolumeFromSettings();
    }

    // ** Game is PAUSED
    if(m_GameMode == GAMEMODE_PAUSED && key == HEDKEY_ESCAPE)
    {
        m_GameMode = m_GameModeBeforePause;
        m_WindowPaused.ToggleVisibility();
        return;
    }

    if(key == HEDKEY_PAUSE || (key == 'p' && m_WindowConsole.isVisible() == 0))
    {
        if(m_GameMode == GAMEMODE_PAUSED)
            m_GameMode = m_GameModeBeforePause;
        else
        {
            m_GameModeBeforePause = m_GameMode;
            m_GameMode            = GAMEMODE_PAUSED;
        }
        m_WindowPaused.ToggleVisibility();
        return;
    }

    if(m_GameMode == GAMEMODE_PAUSED) return;

	// ** Pass input to visible windows
	if(m_WindowConsole.isVisible())
    {
        m_CommandConsole.AddCharacter(key);

        // Console captures all characters, except for ESC
        if(key != HEDKEY_ESCAPE) key = 0;
    }

    if(m_GameMode == GAMEMODE_ATTACK_FIREWALL)                   m_WindowFirewallAttack.ProcessInput(inputX,inputY,click,clickdown,key);
    if(m_GameMode == GAMEMODE_ATTACK_DOS && m_DOSToolReady == 0) m_WindowDOSTool.ProcessInput(inputX,inputY,click,clickdown,key);
	if(m_GameMode == GAMEMODE_ATTACK_VPTOOL)                     m_WindowVoicePrintTool.ProcessInput(inputX,inputY,click,clickdown,key);
	if(m_GameMode == GAMEMODE_MESSAGES)                          m_WindowMessages.ProcessInput(inputX,inputY,click,clickdown,key);
	if(m_GameMode == GAMEMODE_HARDWARE)                          m_WindowHardware.ProcessInput(inputX,inputY,click,clickdown,key);
	if(m_GameMode == GAMEMODE_INTERFACE)
	{
		if(m_InterfaceFWActive == 0)
			m_WindowInterfaceTool.ProcessInput(inputX,inputY,click,clickdown,key);
		else
			m_WindowFileView.ProcessInput(inputX,inputY,click,clickdown,key);
	}
    if(m_GameMode == GAMEMODE_ATTACK_KEYCRACK)                   m_WindowKeyCrackTool.ProcessInput(inputX,inputY,click,clickdown,key);
    if(m_GameMode == GAMEMODE_ATTACK_RETINA)                     m_WindowRetinalScanTool.ProcessInput(inputX,inputY,click,clickdown,key);

	// ** Check if we should dismiss any windows on ESC
	if(key == HEDKEY_ESCAPE)
	{
		if(m_WindowObjectives.isVisible() && m_WindowObjectives.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowObjectives.ToggleVisibility(); return;}
		if(m_WindowConsole.isVisible() && m_WindowConsole.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowConsole.ToggleVisibility(); return;}
		if(m_WindowHelp.isVisible() && m_WindowHelp.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowHelp.ToggleVisibility(); return;}
		if(m_WindowAchievements.isVisible() && m_WindowAchievements.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowAchievements.ToggleVisibility(); return;}
	
		if(m_WindowSystemMessage.isVisible() && m_WindowSystemMessage.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowSystemMessage.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE; return;}

		if(m_WindowHardware.isVisible() && m_WindowHardware.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowHardware.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE; return;}
        if(m_WindowFirewallAttack.isVisible() && m_WindowFirewallAttack.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowFirewallAttack.buttonClicked[INTERFACE_BUTTON_LOGOUT] = 1;/*m_WindowFirewallAttack.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE*/; return;}
		if(m_WindowDOSTool.isVisible() && m_WindowDOSTool.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowDOSTool.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE; return;}
		if(m_GameMode == GAMEMODE_ATTACK_EMP) {m_GameMode = GAMEMODE_IDLE; return;}
		if(m_WindowVoicePrintTool.isVisible() && m_WindowVoicePrintTool.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowVoicePrintTool.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE; return;}
		if(m_WindowKeyCrackTool.isVisible() && m_WindowKeyCrackTool.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowKeyCrackTool.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE; return;}
		if(m_WindowRetinalScanTool.isVisible() && m_WindowRetinalScanTool.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowRetinalScanTool.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE; return;}
		if(m_WindowMessages.isVisible() && m_WindowMessages.m_bgImage->m_AnimAlphaSpeed > 0.0f && m_InterfaceFWActive == 0) {m_WindowMessages.ToggleVisibility(); m_GameMode = GAMEMODE_IDLE; return;}
		if(m_WindowLevelIntro.isVisible() && m_WindowLevelIntro.m_bgImage->m_AnimAlphaSpeed > 0.0f) {m_WindowLevelIntro.ToggleVisibility(); return;}

        if(m_WindowInterfaceTool.isVisible() && m_WindowLevelIntro.m_bgImage->m_AnimAlphaSpeed > 0.0f && m_InterfaceFWActive == 0) {m_WindowLevelIntro.ToggleVisibility(); return;}
	}

	// GAMEMODE_CONFIRM_FAILED
	if(m_GameMode == GAMEMODE_CONFIRM_FAILED)
	{
		m_WindowSystemConfirm.ProcessInput(inputX, inputY, click, clickdown, key);

		if(m_WindowSystemConfirm.ButtonClicked(SMCONFIRM_BUTTON_OK))
		{
			sysLogPrint("(hedGameEngine-Game.cpp)hedGame::UpdateGame() - player integrity = 0. quitting to menu");
			m_WindowSystemConfirm.ToggleVisibility();
			m_GameMode = GAMEMODE_IDLE;
			m_Quit     = 1;
			g_GameAchievements.SaveAchievements();
		}
	}

    // New message notification
    if(m_WindowNewMessageNotification.isVisible())
    {
        m_WindowNewMessageNotification.ProcessInput(inputX,inputY,click,clickdown,key);
      
        if(m_WindowNewMessageNotification.ButtonClicked(BUTTON_DEFAULT) && checkActionInProgress() == 0)
        {
            m_WindowNewMessageNotification.ToggleVisibility();

            // Open messages
			m_WindowMessages.ToggleVisibility();
			m_GameMode = GAMEMODE_MESSAGES;
        }
    }

	// GAMEMODE_CONFIRM_EXIT
	if(m_GameMode == GAMEMODE_CONFIRM_EXIT)
	{
		m_WindowSystemMessage.ProcessInput(inputX, inputY, click, clickdown, key);

		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_NO))
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_IDLE;
		}

		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_YES))
		{
            if(m_GameLevel.m_AISetting_Deathmatch == 0)
            {
			    sysLogPrint("(hedGameEngine-Game.cpp)hedGame::UpdateGame() - quitting to menu");
			    m_WindowSystemMessage.ToggleVisibility();
			    m_GameMode = GAMEMODE_IDLE;
			    m_Quit     = 1;
			    g_GameAchievements.SaveAchievements();
            }
            else
            {
                // We display a level completed screen in Open World / Deathmatch levels
			    m_WindowSystemMessage.ToggleVisibility();
			    m_GameMode = GAMEMODE_IDLE;
			    m_Quit     = 2;
			    g_GameAchievements.SaveAchievements();
            }
		}
	}

	// GAMEMODE_CONFIRM_EXIT_2
	if(m_GameMode == GAMEMODE_CONFIRM_EXIT_2)
	{
		m_WindowSystemMessage.ProcessInput(inputX, inputY, click, clickdown, key);

		// Return to the main menu
		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_NO))
		{
			sysLogPrint("(hedGameEngine-Game.cpp)hedGame::UpdateGame() - quitting to menu");
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_IDLE;
			m_Quit     = 1;
			g_GameAchievements.SaveAchievements();
		}

		// Advance to the next level
		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_YES))
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_IDLE;
			m_Quit     = 2;
			g_GameAchievements.SaveAchievements();
		}
	}
	// GAMEMODE_LEVEL_FAILED
	if(m_GameMode == GAMEMODE_LEVEL_FAILED)
	{
		m_WindowSystemMessage.ProcessInput(inputX, inputY, click, clickdown, key);

		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_NO))
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_IDLE;
		}

		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_YES))
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_IDLE;
			m_Quit     = 1;
			g_GameAchievements.SaveAchievements();
		}
	}

	// GAMEMODE_LEVEL_COMPLETED
	if(m_GameMode == GAMEMODE_LEVEL_COMPLETED)
	{
		m_WindowSystemMessage.ProcessInput(inputX, inputY, click, clickdown, key);

		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_NO))
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode                   = GAMEMODE_IDLE;
			m_GameLevel.m_LevelCompleted = 1; // Level is completed, but we remain in the game level
			g_GameAchievements.SaveAchievements();
		}

		if(m_WindowSystemMessage.ButtonClicked(SMWINDOW_BUTTON_YES))
		{
			m_WindowSystemMessage.ToggleVisibility();
			m_GameMode = GAMEMODE_IDLE;
			m_Quit     = 2;
			g_GameAchievements.SaveAchievements();
		}
	}
	// ===============================================================
	// EMP sattelite alignement
	if(m_GameMode == GAMEMODE_ATTACK_EMP)
	{
		// Start aligning the satellite on click, stop on the second click, and so on...
		if(click == 1)
		{
			if(satelliteAlignStart == 0)
			{
				g_InterfaceManager.skinSatellite->m_AnimAngleSpeed = 15;
				g_InterfaceManager.skinSatelliteAligned->m_AnimAngleSpeed = 15;
			}
			else
			{
				g_InterfaceManager.skinSatellite->m_AnimAngleSpeed = 0.001f;
				g_InterfaceManager.skinSatelliteAligned->m_AnimAngleSpeed = 0.001f;
			}
			TOGGLE(satelliteAlignStart);
		}
	}

	// Tablets (ESC button)
	if(m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_ESC)) key = HEDKEY_ESCAPE;

	// ESCAPE KEY
	if(key == HEDKEY_ESCAPE)
	{
		// Exit to menu
		if(m_GameMode == GAMEMODE_IDLE || m_GameMode == GAMEMODE_CONFIRM_EXIT || m_GameMode == GAMEMODE_CONFIRM_EXIT_2)
		{
			m_WindowSystemMessage.ToggleVisibility();
			if(m_GameMode == GAMEMODE_IDLE)
			{
				if(m_GameLevel.m_LevelCompleted == 1)
					m_GameMode = GAMEMODE_CONFIRM_EXIT_2;
				else
					m_GameMode = GAMEMODE_CONFIRM_EXIT;
			}
			else
				m_GameMode = GAMEMODE_IDLE;
		}
		else
		{
			m_WindowToolbar.PrintTextN("Please wait for the current action to complete...");
		}
	}

	// ==============================================================================================================================================
	// ** Process toolbar input
    // Do not process input if a confirmation dialog is visible
    if(m_WindowSystemMessage.isVisible() == 0 && m_WindowSystemConfirm.isVisible() == 0)
    {
        m_ToolBar.ProcessInput(inputX,inputY,click,clickdown,key);
        
	    if(key == 'f' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_FIREWALL))               buttonFuncFirewallAttack();
	    if(key == 'd' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_DOS))                    buttonFuncDOSAttack();
	    if(key == 'e' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_EMP))                    buttonFuncEMPAttack();
	    if(key == 'v' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_VOICEPRINT))             buttonFuncVoiceprint();
	    if(key == 'r' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_RETINA))                 buttonFuncRetina();
	    if(key == 'k' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_KEYCRACK))               buttonFuncKeyCrack();
	    if(key == 'i' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_INTERFACE))              buttonFuncInterface();
	    if(key == 'c' || key == '`' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_CONSOLE))  buttonFuncConsole();

	    if(key == 'h' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_HARDWARE))               buttonFuncHardware();
	    if(key == 'm' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_MESSAGES))               buttonFuncMessages();

	    if(key == 'o' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_OBJECTIVES))             buttonFuncObjectives();
	    if(key == 'a' || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_ACHIEVEMENTS))           buttonFuncAchievements();
	    if(key == HEDKEY_F1 || m_ToolBar.ButtonClicked(TOOLBAR_BUTTON_HELP))             buttonFuncHelp();
    }
	// ==============================================================================================================================================
	// ** Map scrolling
	float  maxScrollDelta;
	float  sign = 0.0f;

	maxScrollDelta = m_GameLevel.GetMaxMapScrollDelta();

    // Scroll the map when the mouse is on the left/right side of the screen
#if !defined(PLATFORM_IOS)
	if(m_InputX < 64)                  sign = -1.0f;
	if(m_InputX > g_DisplayWidth - 64) sign = 1.0f;
#else
    
    if(m_GameMode == GAMEMODE_IDLE)
    {
        // Touch swipe
        if(click == 1)
        {
            // Reset
            lastTapX = -1;
        }
    
        if(clickdown == 1)
        {
            if(lastTapX == -1)
            {
                lastTapX = inputX;
            }
            else
            {
                m_TargetMapScrollDelta -= (inputX - lastTapX);
                lastTapX = inputX;
            }
        }
    }

#endif
    
	if(m_WindowConsole.isVisible() == 0 && m_Key == HEDKEY_LEFT)  sign = -5.0f;
	if(m_WindowConsole.isVisible() == 0 && m_Key == HEDKEY_RIGHT) sign =  5.0f;

	// Don't allow scrolling while in EMP attack mode
	if(m_GameMode == GAMEMODE_ATTACK_EMP) sign = 0.0f;

	m_TargetMapScrollDelta += sign * g_FrameTime * 500.0f;
	CLAMP(m_TargetMapScrollDelta,-maxScrollDelta,maxScrollDelta);

	if(abs((int)(m_TargetMapScrollDelta - m_TargetMapScrollDeltaLast)) >= 100/*70*/)
	{
		m_TargetMapScrollDeltaLast = m_TargetMapScrollDelta;
		g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-serverover.wav");
	}

	// Pass input info to all servers (only when the game is idle)
	if(m_GameMode == GAMEMODE_IDLE && m_Quit == 0)
	{
		for(i = 0; i < m_GameLevel.m_ServerCount; i++)
		{
			// Check if we must force a lock on the localhost (space key)
            // We also force a lock on the localhost if the currently locked server is invisible
            if(key == ' ' || (m_GameLevel.m_ServerList[m_ServerLocked].m_LevelIntegrity == 0 && m_WindowConsole.isVisible() == 0))
			{
				inputX = m_GameLevel.m_ServerList[m_ServerLocal].getScreenX();
				inputY = m_GameLevel.m_ServerList[m_ServerLocal].getScreenY();
				click  = 1;

				// UPDATE 1
				// Center map on SPACE key
				m_TargetMapScrollDelta     = 0.0f;
				m_TargetMapScrollDeltaLast = 0.0f;
			}

			if(m_GameLevel.m_ServerList[i].ProcessInput(inputX,inputY, click) == 1)
			{
				// If a new server LOCK happened, clear the previous one
				if(m_ServerLocked > - 1)
					m_GameLevel.m_ServerList[m_ServerLocked].ClearLock();

				m_ServerLocked = i;

                // Start the trace
                // UPDATE 1 - Trace starts later for hacked servers

				//m_TimeUntilTrace      = calculatePlayerTraceTime();
                //m_LastTraceAlertDigit = m_TimeUntilTrace + 1; // 11

				m_TimeUntilTrace      = calculatePlayerTraceTime();
				if(m_GameLevel.m_ServerList[m_ServerLocked].isFullyHacked( m_GameLevel.m_ServerList[m_GameLevel.m_ServerLocal].m_ServerKeys[m_ServerLocked] ) == 1) m_TimeUntilTrace *= 3;
                m_LastTraceAlertDigit = m_TimeUntilTrace + 1; // 11
				
				// HINT
				if(calculateBounceCPU() >= m_GameLevel.m_ServerList[m_ServerLocked].m_LevelFirewall)
					m_WindowToolbar.PrintTextN("Press the FIREWALL button to hack this server's firewall.");
				else
				{
					if(m_GameLevel.m_ServerList[m_ServerLocal].m_ToolDOSLevel == 75.0f) m_WindowToolbar.PrintTextN("Not enough CPU power. Use the EMP to attack this server.");
					if(m_GameLevel.m_ServerList[m_ServerLocal].m_ToolDOSLevel == 75.0f) m_WindowToolbar.PrintTextN("Not enough CPU power. Use the DOS to attack this server.");
				}
				if(m_GameLevel.m_ServerList[m_ServerLocked].m_AIControlled == 1)
					m_WindowToolbar.PrintTextN("[WARNING] AI controlled server.");

				// Log
				sysActivityLogPrint("%d locked %s",(int)m_SecondsPlayed,m_GameLevel.m_ServerList[m_ServerLocked].m_HostName);
			}
			else
			{
				// Empty click. Display a nice animation
				if(inputY > DISPLAY_TOP_OFFSET && inputY < g_DisplayHeight - 100 && click == 1)
				{
					m_ClickCircleX = inputX;
					m_ClickCircleY = inputY;
					g_InterfaceManager.skinClickCircle->m_AnimScale      = 0.0f;
					g_InterfaceManager.skinClickCircle->m_AnimAlpha      = 1.0f;
					g_InterfaceManager.skinClickCircle->m_AnimAlphaSpeed = -ANIM_SPEED_ALPHA;
				}
			}
		}
	}

	// Clear
	// Moved to the top of this function m_Click = 0;

	// Keyboard shortcuts
	if(key == HEDKEY_F2) m_WindowLevelIntro.ToggleVisibility();

	// Update server coordinates here
	for(i = 0; i < m_GameLevel.m_ServerCount; i++)
		m_GameLevel.m_ServerList[i].SetCoordinateDeltas(- m_TargetMapScrollDelta + (g_DisplayWidth - m_GameLevel.m_TargetMapImageWidth) / 2, DISPLAY_TOP_OFFSET);
}

//=======================================================================================================================================
// Render the game
void hedGame::RenderGame()
{
	int x, y;
	int i;

	m_FramesRendered++;

#ifdef BUILD_STEAM
	g_GameAchievements.STEAM_RunCallbacks();
#endif

	//===================================================================================================================================
	// Render the targetmap
	m_GameLevel.RenderTargetMap(m_TargetMapScrollDelta);

    //===================================================================================================================================
	// Background pattern at the bottom
	for(i = 0; i < (g_DisplayWidth / 128) + 1; i++)
		g_InterfaceManager.skinBGPattern->Render(i * 128, g_DisplayHeight - 100);

	// Render fade bar
	g_InterfaceManager.skinFadeBar->Render(-32,DISPLAY_TOP_OFFSET,-1,g_DisplayHeight - DISPLAY_TOP_OFFSET - 100);

	// Draw world map elements
	// Horizontal map scroll bar
	g_InterfaceManager.skinTargetMapScrollBar->Render(35,g_DisplayHeight - 125);
	g_RenderEngine.RenderText(3,g_DisplayHeight - 125, 0.5f,0.5f,0.5f, HED_FONT_NORMAL,"-200");
	g_RenderEngine.RenderText(5 + 400 + 35,g_DisplayHeight - 125, 0.5f,0.5f,0.5f, HED_FONT_NORMAL,"200");

	// Map tracker
	int tracker_x = m_TargetMapScrollDelta / 3;
	g_RenderEngine.RenderText(tracker_x + 10 + 200 + 35,g_DisplayHeight - 125 + 14, 0.5f,0.5f,0.5f, HED_FONT_NORMAL,"%3.3f/tracking",m_TargetMapScrollDelta);
	g_InterfaceManager.skinTargetMapScrollBarCursor->Render(tracker_x + 200 + 35 - 5,g_DisplayHeight - 125 + 10 + 5);

	// Render link between servers
	if(m_BounceCount > 0)
	{
		g_RenderEngine.DrawLineAA(m_GameLevel.m_ServerList[m_ServerLocal].getScreenX(), m_GameLevel.m_ServerList[m_ServerLocal].getScreenY(),
								  m_GameLevel.m_ServerList[m_BounceList[0]].getScreenX(), m_GameLevel.m_ServerList[m_BounceList[0]].getScreenY(),
								  1.0f, 0.39f, 0.09f,/*0.67f,0.87,0.003f,*/3);

		for(i = 0; i < m_BounceCount - 1; i++)
		{
			g_RenderEngine.DrawLineAA(m_GameLevel.m_ServerList[m_BounceList[i]].getScreenX(), m_GameLevel.m_ServerList[m_BounceList[i]].getScreenY(),
									  m_GameLevel.m_ServerList[m_BounceList[i + 1]].getScreenX(), m_GameLevel.m_ServerList[m_BounceList[i + 1]].getScreenY(),
								     1.0f, 0.39f, 0.09f,/*0.67f,0.87,0.003f,*/3);
		}
	}

	if(m_GameMode == GAMEMODE_ATTACK_FIREWALL || m_GameMode == GAMEMODE_ATTACK_DOS || m_GameMode == GAMEMODE_ATTACK_EMP || m_GameMode == GAMEMODE_ATTACK_VPTOOL || 
       m_GameMode == GAMEMODE_ATTACK_RETINA || m_GameMode == GAMEMODE_ATTACK_KEYCRACK ||
       m_GameMode == GAMEMODE_INTERFACE)
	{
		if(m_BounceCount == 0)
			g_RenderEngine.DrawLineAA(m_GameLevel.m_ServerList[m_ServerLocal].getScreenX(), m_GameLevel.m_ServerList[m_ServerLocal].getScreenY(),
									  m_GameLevel.m_ServerList[m_ServerLocked].getScreenX(), m_GameLevel.m_ServerList[m_ServerLocked].getScreenY() ,
									  0.8,0.0,0.0,3);
		else
			g_RenderEngine.DrawLineAA(m_GameLevel.m_ServerList[m_BounceList[m_BounceCount - 1]].getScreenX(), m_GameLevel.m_ServerList[m_BounceList[m_BounceCount - 1]].getScreenY(),
									  m_GameLevel.m_ServerList[m_ServerLocked].getScreenX(), m_GameLevel.m_ServerList[m_ServerLocked].getScreenY(),
									  0.8,0.0,0.0,3);
	}

	// Draw ongoing AI attacks
	for(i = 0; i < m_GameLevel.m_ServerCount; i++)
	{
		// AI attack in progress? Draw it...
		if(m_GameLevel.m_ServerList[i].m_AIAttackProgress > 0.0f)
		{
			float x1, y1, x2, y2, dx, dy, length;
			
			x1 = m_GameLevel.m_ServerList[i].getScreenX();
			y1 = m_GameLevel.m_ServerList[i].getScreenY();

			x2 = m_GameLevel.m_ServerList[m_GameLevel.m_ServerList[i].m_AIAttackDestination].getScreenX();
			y2 = m_GameLevel.m_ServerList[m_GameLevel.m_ServerList[i].m_AIAttackDestination].getScreenY();

			dx = (x2 - x1) / 100.0f;
			dy = (y2 - y1) / 100.0f;

			g_RenderEngine.DrawLineAA(x1,y1,x2,y2,0.7,0.7,0.7,2);
			g_RenderEngine.DrawLineAA(x1,y1,x1 + dx * m_GameLevel.m_ServerList[i].m_AIAttackProgress,y1 + dy * m_GameLevel.m_ServerList[i].m_AIAttackProgress,0.8,0.0,0.0,2);

		}
	}

	// Draw servers
	for(i = 0; i < m_GameLevel.m_ServerCount; i++)
        m_GameLevel.m_ServerList[i].Draw(m_GameLevel.m_ServerList[0].m_ServerKeys[i]);

	//===================================================================================================================================
	// MISC TEXTS THAT ARE DRAWN OVER THE MAP
	int overlayTextX, overlayTextY;

	overlayTextX = 10;
	overlayTextY = g_DisplayHeight - 170;

    /*
	g_RenderEngine.RenderText(overlayTextX,overlayTextY, 0.35f,0.35f,0.35f, HED_FONT_NORMAL,"Succesful hacks: %d",m_GameLevel.m_ServerList[m_ServerLocal].statHackCount); overlayTextY += 12;
	g_RenderEngine.RenderText(overlayTextX,overlayTextY, 0.35f,0.35f,0.35f, HED_FONT_NORMAL,"DOS's used:      %d",m_GameLevel.m_ServerList[m_ServerLocal].statDOSCount);  overlayTextY += 12;
	g_RenderEngine.RenderText(overlayTextX,overlayTextY, 0.35f,0.35f,0.35f, HED_FONT_NORMAL,"EMP's used:      %d",m_GameLevel.m_ServerList[m_ServerLocal].statEMPCount);       
    */
    g_RenderEngine.RenderText(overlayTextX,overlayTextY + 15, 0.35f,0.35f,0.35f, HED_FONT_NORMAL,"[%s]",g_GameModeNames[m_GameMode]);
    g_RenderEngine.RenderText(overlayTextX,overlayTextY + 30, 0.35f,0.35f,0.35f, HED_FONT_NORMAL,"FPS: %3.0f (Avg: %2.1f)", 1.0f / g_FrameTime, (double)m_FramesRendered / (double)m_GameLevel.m_LevelTime);

	// Seconds played
	g_RenderEngine.RenderText(g_DisplayWidth - 70,g_DisplayHeight - 115, 0.5f,0.5f,0.5f, HED_FONT_BIG10,"%s",utilFormatTime(m_SecondsPlayed));

	// FPS history chart, on top of the scroll bar
	y   = g_DisplayHeight - 125;
	int min;

	// Update FPS history, every 3rd frame
	if(m_FramesRendered % 3 == 0)
	{
		for(i = 0; i < 199; i++)
			m_FPSHistory[i] = m_FPSHistory[i + 1];

		m_FPSHistory[199] = (1.0f / g_FrameTime);
		CAPMAX(m_FPSHistory[199],60);
		CAPMIN(m_FPSHistory[199],0);
	}

    min = m_FPSHistory[0];
	for(i = 1; i < 199; i++)
		if(m_FPSHistory[i] < min) min = m_FPSHistory[i];

	for(i = 0; i < 200; i++)
		g_RenderEngine.DrawLineAlpha(237 + i, y, 237 + i, y - m_FPSHistory[i] + min,0.2,0.2,0.2,0.35);

	// Click animation
	if(m_GameMode == GAMEMODE_IDLE)
		g_InterfaceManager.skinClickCircle->Render(m_ClickCircleX - 16, m_ClickCircleY - 16);

	//=============================================================================================================================================================================================
	// ** Toolbar top (BEGIN)
	g_InterfaceManager.skinToolbarTop->Render((g_DisplayWidth - g_InterfaceManager.skinToolbarTop->m_pWidth) / 2, 0);
	m_WindowAIChat.Render();
	
    // ** Latitude / longitude for the selected server
	x  = 5 + 768 + (g_DisplayWidth - 1024) / 2;
	if(m_ServerLocked > -1)
		g_RenderEngine.RenderText(x,  5, 0.5f,0.5f,0.5f, HED_FONT_BIG10,"lat: %3.2f | long: %3.2f",m_GameLevel.m_ServerList[m_ServerLocked].m_latAnimated,m_GameLevel.m_ServerList[m_ServerLocked].m_longAnimated);

	// ** Bounce link info
	x  = 5 + 0 + (g_DisplayWidth - 1024) / 2;
	if(m_BounceCount > 0)
		g_RenderEngine.RenderText(x, 5, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Bounced link: %d | CPU: %d",m_BounceCount,calculateBounceCPU());
	else
		g_RenderEngine.RenderText(x, 5, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Bounced link: none",0);

	// ** Trace timer
    // No trace in progress
    if(m_ServerLocked < 1)
		g_RenderEngine.RenderText(x,20, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Your tracetime: %d s",calculatePlayerTraceTime());

    // Trace countdown in progress
    if(m_ServerLocked > 0 && m_TimeUntilTrace > 0.0f )
        g_RenderEngine.RenderText(x,20, 0.8f,0.0f,0.0f, HED_FONT_BIG10,"Trace will start in %2.2fs",m_TimeUntilTrace);

    // Trace countdown in progress
    if(m_ServerLocked > 0 && m_TimeUntilTrace == 0.0f && m_FlashTrace == 0)
		g_RenderEngine.RenderText(x,20, 0.8f,0.0f,0.0f, HED_FONT_BIG10,"You are being traced!");

	// ** Player's score and money
	x  = 5 + 512 + (g_DisplayWidth - 1024) / 2;
    if(m_GameLevel.m_OpenWorld == 1)
        g_RenderEngine.RenderText(x,  5, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Score: %d",m_GameLevel.m_ServerList[m_ServerLocal].m_AIStatScore);
    else
        g_RenderEngine.RenderText(x,  5, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Score: %d",m_GameLevel.m_ServerList[m_ServerLocal]._statScore);

	g_RenderEngine.RenderText(x, 20, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Money: $%d",m_GameLevel.m_ServerList[m_ServerLocal].m_Money);
	
	// ** Player's trace level and time left
	x  = 5 + 256 + (g_DisplayWidth - 1024) / 2;
	if(m_GameLevel.GetTimedObjective() >= 60.0f || m_GameLevel.GetTimedObjective() == -1)
		g_RenderEngine.RenderText(x, 5, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Time left: %s",utilFormatTime(m_GameLevel.GetTimedObjective()));
	else
	{
		if((int)(m_SecondsPlayed * 10) % 10 != 0)
			g_RenderEngine.RenderText(x, 5, 0.8f,0.0f,0.0f, HED_FONT_BIG10,"Time left: %3s",utilFormatTime(m_GameLevel.GetTimedObjective()));
	}
	if(m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel < 75.0f)
		g_RenderEngine.RenderText(x, 20, 0.67f,0.87f,0.003f, HED_FONT_BIG10,"Trace level: %3.2f %%",m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel);

	if(m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel >= 75.0f  && m_FlashTrace == 0)
		g_RenderEngine.RenderText(x, 20, 0.8f,0.0f,0.0f, HED_FONT_BIG10,"Trace level: %3.2f %%",m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel);

	// ** Toolbar top (END)

	//===================================================================================================================================
	// ** Toolbar
	m_ToolBar.Render();
	m_WindowToolbar.Render();

	// EMP and DOS charge level
	int d_index = 0,e_index = 0;
	if(m_GameLevel.m_ServerList[m_ServerLocal].m_ToolDOSLevel == 75.0f) d_index = 1;
	if(m_GameLevel.m_ServerList[m_ServerLocal].m_ToolEMPLevel == 75.0f) e_index = 1;
	for(i = 0; i < m_GameLevel.m_ServerList[m_ServerLocal].m_ToolDOSLevel; i++)
		g_InterfaceManager.skinButtonEMPDOSFill[d_index]->Render(m_ToolBar.m_DisplayX + m_ToolBar.attachedImageX[2] + 13 + i,m_ToolBar.m_DisplayY + m_ToolBar.attachedImageY[2] + 19);

	for(i = 0; i < m_GameLevel.m_ServerList[m_ServerLocal].m_ToolEMPLevel; i++)
		g_InterfaceManager.skinButtonEMPDOSFill[e_index]->Render(m_ToolBar.m_DisplayX + m_ToolBar.attachedImageX[4] + 13 + i,m_ToolBar.m_DisplayY + m_ToolBar.attachedImageY[4] + 19);

	//===================================================================================================================================
	// POPUP WINDOWS (console, objectives)
	// ** Attack mode: EMP
	if(m_GameMode == GAMEMODE_ATTACK_FIREWALL && m_WindowFirewallAttack.isVisible()) {run_FirewallAttack();} else {m_WindowFirewallAttack.Render();}
	if(m_GameMode == GAMEMODE_ATTACK_VPTOOL && m_WindowVoicePrintTool.isVisible()) {run_VoicePrintPasswordTool();} else {m_WindowVoicePrintTool.Render();}
	if(m_GameMode == GAMEMODE_ATTACK_DOS && m_WindowDOSTool.isVisible()) {run_DOSTool();} else {m_WindowDOSTool.Render();}
	if(m_GameMode == GAMEMODE_ATTACK_EMP) run_EMPTool();
	if(m_GameMode == GAMEMODE_ATTACK_KEYCRACK && m_WindowKeyCrackTool.isVisible())  {run_KeyCrackTool();} else {m_WindowKeyCrackTool.Render();}
	if(m_GameMode == GAMEMODE_ATTACK_RETINA && m_WindowRetinalScanTool.isVisible()) {run_RetinalScanTool();} else {m_WindowRetinalScanTool.Render();}
	if(m_GameMode == GAMEMODE_INTERFACE && m_WindowInterfaceTool.isVisible())       {run_InterfaceTool();}   else {m_WindowInterfaceTool.Render();}
	if(m_GameMode == GAMEMODE_MESSAGES  && m_WindowMessages.isVisible())            {run_Messages();}        else {m_WindowMessages.Render();}
	if(m_GameMode == GAMEMODE_HARDWARE  && m_WindowHardware.isVisible())            {run_HardwareManager();} else {m_WindowHardware.Render();}

	m_WindowConsole.Render();
	m_WindowConsole.RenderText(20,345,0.67f,0.87f,0.003f,HED_FONT_NORMAL,m_CommandConsole.GetCommandLine());

 	m_WindowHelp.Render();	
	m_WindowAchievements.Render();
	g_GameAchievements.Render(&m_WindowAchievements, &m_WindowToolbar);

    // Objectives and game statistics
	m_WindowObjectives.Render();
	for(i = 0; i < m_GameLevel.m_ObjectiveCount; i++)
	{
        m_WindowObjectives.RenderText(70,3 + 45 + i * 40, 0.8, 0.8, 0.8,HED_FONT_NORMAL,m_GameLevel.m_ObjectiveList[i].description[0]);
        m_WindowObjectives.RenderText(70,3 + 15 + 45 + i * 40, 0.8, 0.8, 0.8,HED_FONT_NORMAL,m_GameLevel.m_ObjectiveList[i].description[1]);
    }
    m_GameLevel.ShowOpenWorldStatics(&m_WindowObjectives);

	//===================================================================================================================================
	// ** Message boxes
	// ** System window messages
	m_WindowSystemMessage.Clear();
	//if(m_GameMode == GAMEMODE_CONFIRM_MSG)
	//	m_WindowSystemMessage.PrintText("\n\n  New message received.\n  Would you like to read it?");
	if(m_GameMode == GAMEMODE_CONFIRM_EXIT)
		m_WindowSystemMessage.PrintText("\n\n  Exit to main menu?");
	if(m_GameMode == GAMEMODE_CONFIRM_EXIT_2)
		m_WindowSystemMessage.PrintText("\n  This level is completed.\n\n  YES - advance to the next level.\n  NO - return to the main menu.");
	if(m_GameMode == GAMEMODE_LEVEL_FAILED)
		m_WindowSystemMessage.PrintText("\n\n  Level failed.\n  Return to the main menu?");
	if(m_GameMode == GAMEMODE_LEVEL_COMPLETED)
		m_WindowSystemMessage.PrintText("\n  Level completed.\n  Advance to the next level?\n\n  You can press ESCAPE later\n  to advance to the next level.");

	m_WindowSystemMessage.Render();

	// ** Level failed confirmation
	m_WindowSystemConfirm.Clear();
	if(m_GameMode == GAMEMODE_CONFIRM_FAILED)
    {
        if(m_GameLevel.m_ServerList[0].m_TraceLevel == 100.0f)
		    m_WindowSystemConfirm.PrintText("\n\n  You have been traced.\n  You have lost the game.");
        else
		    m_WindowSystemConfirm.PrintText("\n\n  Your integrity level is 0.\n  You have lost the game.");
    }

	m_WindowSystemConfirm.Render();

	// ** Other windows
	m_WindowLevelIntro.Render();
	m_WindowLevelIntro.RenderText(20,20,0.67f,0.87f,0.003f,HED_FONT_BIG12,m_GameLevel.m_LevelDescription[m_GameLevel.m_LevelIndex]);

    m_WindowNewMessageNotification.Render();
    
	//===================================================================================================================================
	// TABLET PC GUI elements
	// if(g_IsTabletPC == 1)
	{
		// Left and right arrows 
		g_InterfaceManager.skinTabletArrow[0]->Render(0,50 - 70 + (g_DisplayHeight - 150) / 2);
		g_InterfaceManager.skinTabletArrow[1]->Render(g_DisplayWidth - 71,50 - 70 + (g_DisplayHeight - 150) / 2);

		// Slow pulse/fade animation
		for(i = 0; i < 2; i++)
		if(g_InterfaceManager.skinTabletArrow[i]->m_AnimAlpha == 1.0f || g_InterfaceManager.skinTabletArrow[i]->m_AnimAlpha == 0.0f)
			g_InterfaceManager.skinTabletArrow[i]->m_AnimAlphaSpeed = -g_InterfaceManager.skinTabletArrow[i]->m_AnimAlphaSpeed;

		// Tablet ESC button
		
	}

    // Game is paused
    if(m_GameMode == GAMEMODE_PAUSED)
        g_RenderEngine.DrawRectangleFill(0,50,g_DisplayWidth,g_DisplayHeight - 100,0.0f,0.0f,0.0f,0.9f);
    m_WindowPaused.Render();

	// Tests
}

//=======================================================================================================================================
// Display an error message in the toolbar
void hedGame::_DisplayErrorMsg(char* msg, ...)
{
    hedString msgLine;
	va_list	  ap;

	va_start(ap, msg);									
    vsprintf(msgLine, msg, ap);						
	va_end(ap);											

    g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-error.wav");
    m_WindowToolbar.PrintTextN("/c2[ERROR] %s",msgLine);
}

//=======================================================================================================================================
// Calculates the player's tracetime
int hedGame::calculatePlayerTraceTime()
{
    int i,traceTime;

    traceTime = m_GameLevel.m_ServerList[m_ServerLocal].m_LevelFirewall * HED_TRACETIME_UNIT;

    for(i = 0; i < m_BounceCount; i++)
        traceTime += HED_TRACETIME_UNIT * m_GameLevel.m_ServerList[m_BounceList[i]].m_LevelCPUPower;

	// Robert - more tracetime in the first 2 levels (and even more in the demo)
#ifdef DEMO
	if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"level-0-tutorial") == 0) traceTime *= 4;
	if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"level-1-money") == 0)    traceTime *= 4;	
#else
	if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"level-0-tutorial") == 0) traceTime *= 3;
	if(strcmp(m_GameLevel.m_LevelList[m_GameLevel.m_LevelIndex],"level-1-money") == 0)    traceTime *= 3;
#endif

    return traceTime;
}

//=======================================================================================================================================
// Calculates the amount of power received through bouncing
int hedGame::calculateBounceCPU()
{
    int i,cpu = 0;


    for(i = 0; i < m_BounceCount; i++)
        cpu += m_GameLevel.m_ServerList[m_BounceList[i]].m_LevelCPUPower;

    cpu += m_GameLevel.m_ServerList[m_ServerLocal].m_LevelCPUPower;

    return cpu;
}

//=======================================================================================================================================
// Checks is any game action is in progress
int hedGame::checkActionInProgress()                         
{
	if(m_GameMode != GAMEMODE_IDLE)
	{
		_DisplayErrorMsg("Please wait for the current action to complete...");
		return 1;
	}
	return 0;
}

//=======================================================================================================================================
// Check if a given server is in the bounced link
int hedGame::checkServerInBouncedLink(int serverIndex)
{
    int i;

    for(i = 0; i < m_BounceCount; i++)
        if(m_BounceList[i] == serverIndex) 
        {
            _DisplayErrorMsg("Please remove the server from the bounced link (double-click)");
            return 1;
        }
    return 0;
}

//=======================================================================================================================================
// Bounced link management
void hedGame::bouncedLink_Add(int serverIndex)
{
	// Add to bounce link list
	m_BounceList[m_BounceCount] = serverIndex;
	m_BounceCount++;

	// Log
	sysActivityLogPrint("%d bounce-add %s",(int)m_SecondsPlayed,m_GameLevel.m_ServerList[serverIndex].m_HostName);

	// HINT
	m_WindowToolbar.PrintTextN("Your CPU power has now increased.");
	g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");
}

//=======================================================================================================================================
void hedGame::bouncedLink_Remove(int serverIndex)
{
	int i;
    int index = -1;
    int traceDecrease;

    // Find server
	for(i = 0; i < m_BounceCount; i++)
        if(m_BounceList[i] == serverIndex) index = i;
    
    if(index == -1)
    {
        sysLogPrint("[ERROR] Trying to remove a server that is not in the bouncelink?");
        return;
    }
    traceDecrease = m_GameLevel.m_ServerList[m_BounceList[index]].m_LevelCPUPower;

	// Remove from bounce link list
	for(i = index; i < m_BounceCount - 1; i++)
		m_BounceList[i] = m_BounceList[i + 1];
	m_BounceCount--;

    // Decrease player's trace time
    DECREMENT_WITH_CAP(m_TimeUntilTrace,HED_TRACETIME_UNIT * traceDecrease,0);

	// Log
	sysActivityLogPrint("%d bounce-remove %s",(int)m_SecondsPlayed,m_GameLevel.m_ServerList[m_BounceList[index]].m_HostName);

	// HINT
	m_WindowToolbar.PrintTextN("Your CPU power has now decreased.");
	g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");
}

//=======================================================================================================================================
int hedGame::bouncedLink_Present(int serverIndex)
{
	int i,found = -1;

	for(i = 0; i < m_BounceCount; i++)
		if(m_BounceList[i] == serverIndex) found = i;

	return found;
}

//=======================================================================================================================================
// ** Firewall button handler
void hedGame::buttonFuncFirewallAttack()
{
	// Toggle with the same button
	if(m_GameMode == GAMEMODE_ATTACK_FIREWALL)
	{
		m_WindowFirewallAttack.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}

	// Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;
    if(checkServerInBouncedLink(m_ServerLocked)) return;

	// Make sure we have selected a server
	if(m_ServerLocked == -1 || m_ServerLocked == m_ServerLocal)
	{
		_DisplayErrorMsg("Please select a target. (Click a server on the map)");
		return;
	}

	// Make sure the server isn't hacked already
	if(m_GameLevel.m_ServerList[m_ServerLocked].m_ServerKey == m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked])
	{
		_DisplayErrorMsg("Server already hacked.");
		return;
	}

	// Do we have enough resources to hack this server?
	if(calculateBounceCPU() < m_GameLevel.m_ServerList[m_ServerLocked].m_LevelFirewall)
	{				
		_DisplayErrorMsg("You don't have enough CPU power to hack this server.");
		return;
	}

    m_WindowFirewallAttack.ToggleVisibility();

	// Hack server
	g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-engaging.wav");

    // Initialize variables
	m_GameMode               = GAMEMODE_ATTACK_FIREWALL;
    m_FirewallAttackCleared  = 0;
    m_FirewallAttackTimeLeft = m_GameLevel.m_ServerList[m_ServerLocked].m_LevelFirewall * HED_TRACETIME_UNIT;
    DECREMENT_WITH_CAP(m_FirewallAttackTimeLeft,calculateBounceCPU() * HED_TRACETIME_UNIT,HED_TRACETIME_UNIT * 2); // Reduce time to half, if we are bouncing

    m_FirewallAttackDuration =  m_FirewallAttackTimeLeft;
    for(int i = 0; i < 20; i++)
        for(int j = 0; j < 20; j++)
            m_FirewallAttackGrid[i][j] = 1;  
}

//=======================================================================================================================================
// EMP attack button handler
void hedGame::buttonFuncEMPAttack()
{
	// HINT
	m_WindowToolbar.PrintTextN("EMP attack: Firewall and integrity will decrease by 2 units.");

	// Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;
    if(checkServerInBouncedLink(m_ServerLocked)) return;

	// Make sure we have selected a server
	if(m_ServerLocked == -1 || m_ServerLocked == m_ServerLocal)
	{
		_DisplayErrorMsg("Please select a target. (Click a server on the map)");
		return;
	}

	// Is the EMP charged?
	if(m_GameLevel.m_ServerList[m_ServerLocal].m_ToolEMPLevel < 75.0f)
	{
		_DisplayErrorMsg("EMP not yet ready.");
		return;
	}
	
	// Launch EMP attack
	g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-engaging.wav");

	// Calculate satellite alignment angle
	hedVector v1(0,-1);
	hedVector v2 = hedVector(m_GameLevel.m_ServerList[m_ServerLocked].getScreenX() - (satelliteX + 64), m_GameLevel.m_ServerList[m_ServerLocked].getScreenY() - (satelliteY + 64)).Normalized();	
	
	satelliteAngle = atan2(v1.Cross(v2), v1.Dot(v2)) * 180.0f / HED_PI;

	// Start animations
	g_InterfaceManager.skinSatellite->m_AnimScale      = 0.0f;
	g_InterfaceManager.skinSatellite->m_AnimAlpha      = 0.0f;

	m_GameMode           = GAMEMODE_ATTACK_EMP;
	m_GameActionProgress = 0.0f;
	m_GameActionSpeed    = GAME_SPEED_EMP;

	// Log
	sysActivityLogPrint("%d emp %s",(int)m_SecondsPlayed,m_GameLevel.m_ServerList[m_ServerLocked].m_HostName);

	// ** New code
	satelliteAlignStart = 0;
	satelliteAligned    = 0;

	// Randomize satellite angle
	do
	{
		g_InterfaceManager.skinSatellite->m_AnimAngle = rand() % 180;
	}while(abs(g_InterfaceManager.skinSatellite->m_AnimAngle - satelliteAngle) < 45);
	
	g_InterfaceManager.skinSatelliteAligned->m_AnimAngle      = g_InterfaceManager.skinSatellite->m_AnimAngle;
	g_InterfaceManager.skinSatellite->m_AnimAngleSpeed        = 0.001f;
	g_InterfaceManager.skinSatelliteAligned->m_AnimAngleSpeed = 0.001f;
}

//=======================================================================================================================================
// DOS attack button handler
void hedGame::buttonFuncDOSAttack()
{
	// Toggle with the same button
	if(m_GameMode == GAMEMODE_ATTACK_DOS)
	{
		m_WindowDOSTool.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}
	// HINT
	m_WindowToolbar.PrintTextN("DOS attack: Firewall and integrity will decrease by 1 unit.");

	// Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;
    if(checkServerInBouncedLink(m_ServerLocked)) return;

	// Make sure we have selected a server
	if(m_ServerLocked == -1 || m_ServerLocked == m_ServerLocal)
	{
		_DisplayErrorMsg("Please select a target. (Click a server on the map)");
		return;
	}

	// Is the DOS ready?
	if(m_GameLevel.m_ServerList[m_ServerLocal].m_ToolDOSLevel < 75.0f)
	{				
		_DisplayErrorMsg("DOS not yet ready.");
		return;
	}

	// Initialize
	m_WindowDOSTool.CreateSlider(2,300, rand() % 300);
	m_WindowDOSTool.CreateSlider(3,300, rand() % 300);
	m_WindowDOSTool.CreateSlider(4,300, rand() % 300);
	m_WindowDOSTool.CreateSlider(5,300, rand() % 300);
	
    // Randomize port bandwidths
	int i, index = 0;

    for(i = 0; i < 4; i++)
    {
        do
        {
            m_DOSToolBW[i] = 100 + sysRandomNumber(100) + sysRandomNumber(100);
        }while(abs(m_DOSToolBW[i] - m_WindowDOSTool.sliderPosition[i + 2]) < 30);
    }

    m_DOSToolChart[0] = 150 + sysRandomNumber(150);

    // Chart
    m_DOSToolChart[0] = 40 + rand() % 30;
	for(i = 1; i < 300; i++)
	{
		if(rand() % 2 == 0)
			m_DOSToolChart[i] = m_DOSToolChart[i - 1] + 1;
		else
			m_DOSToolChart[i] = m_DOSToolChart[i - 1] - 1;
		CAPMAX(m_DOSToolChart[i],70);
	}
	m_DOSToolChartIndex = 0.0f;
	m_DOSToolReady      = 0;

	// Launch DOS attack
	g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-engaging.wav");
	m_GameMode = GAMEMODE_ATTACK_DOS;
	m_WindowDOSTool.ToggleVisibility();
}

//=======================================================================================================================================
// Voiceprint password tool
void hedGame::buttonFuncVoiceprint()
{
	// Toggle with the same button
	if(m_GameMode == GAMEMODE_ATTACK_VPTOOL)
	{
		m_WindowVoicePrintTool.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}

    // Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;
    if(checkServerInBouncedLink(m_ServerLocked)) return;

	// Make sure we have selected a server
	if(m_ServerLocked == -1 || m_ServerLocked == m_ServerLocal)
	{
    	_DisplayErrorMsg("Please select a target. (Click a server on the map)");
		return;
	}
	
	// Verify if the server has a voiceprint password
	if(m_GameLevel.m_ServerList[m_ServerLocked].m_VoiceprintPassword == 0)
	{
		_DisplayErrorMsg("[Voiceprint Password Authentication] not present.");
		return;
	}

	// Verify if the firewall is hacked, before allowing to crack the voiceprint password
	if(m_GameLevel.m_ServerList[m_ServerLocked].m_ServerKey != m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked])
	{
		_DisplayErrorMsg("Access denied. Firewall needs to be hacked first.");
		return;
	}

	// Set game mode
	m_WindowVoicePrintTool.ToggleVisibility();
	m_GameMode = GAMEMODE_ATTACK_VPTOOL;
	g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-piy.wav");

	// Randomize the sliders
	m_WindowVoicePrintTool.CreateSlider(4,300, (rand() % 100) * 3);
	m_WindowVoicePrintTool.CreateSlider(5,300, rand() % 300);
    m_WindowVoicePrintTool.CreateSlider(6,300, rand() % 300);
}

//=======================================================================================================================================
// Retina scan password tool
void hedGame::buttonFuncRetina()
{
#ifdef DEMO
	_DisplayErrorMsg("Unavailable in the DEMO.");
	return;
#endif

	// Toggle with the same button
	if(m_GameMode == GAMEMODE_ATTACK_RETINA)
	{
		m_WindowRetinalScanTool.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}

	// Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;
    if(checkServerInBouncedLink(m_ServerLocked)) return;

	// Make sure we have selected a server
	if(m_ServerLocked == -1 || m_ServerLocked == m_ServerLocal)
	{
		_DisplayErrorMsg("Please select a target. (Click a server on the map)");
		return;
	}

    // Verify if the server has an encryption key
    if(m_GameLevel.m_ServerList[m_ServerLocked].m_RetinaPassword == 0)
	{
		_DisplayErrorMsg("[Retinal scan authentication] not present.");
		return;
	}

	// Server needs to be hacked
    if(m_GameLevel.m_ServerList[m_ServerLocked].m_ServerKey != m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked])
	{
		_DisplayErrorMsg("Access denied. Firewall needs to be hacked first.");
		return;
	}

  	g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-engaging.wav");

    // Initialize key matrix
    
    int i,j,x,y,temp;
    int index = 0;

    for(i = 0; i < 5; i++)
        for(j = 0; j < 5; j++)
            m_RetinalScanTool_Matrix[i][j] = index++;

    // Shuffle it
    index = 0;
    for(i = 0; i < 5; i++)
        for(j = 0; j < 5; j++)
        {
            x = sysRandomNumber(5);
            y = sysRandomNumber(5);
            temp = m_RetinalScanTool_Matrix[i][j]; m_RetinalScanTool_Matrix[i][j] = m_RetinalScanTool_Matrix[x][y]; m_RetinalScanTool_Matrix[x][y] = temp;
            index ++;
        }
    

    // All OK. Let's run the tool
    m_RetinalScanTool_Index  = 0;
    m_RetinalScanTool_Timer  = 0.0f;
    m_RetinalScanTool_Failed = 0;
    m_RetinalScanTool_Next   = 24;
	m_GameMode               = GAMEMODE_ATTACK_RETINA;
	m_WindowRetinalScanTool.ToggleVisibility();
}

//=======================================================================================================================================
// Key crack tool
void hedGame::buttonFuncKeyCrack()
{
#ifdef DEMO
	_DisplayErrorMsg("Unavailable in the DEMO.");
	return;
#endif
	// Toggle with the same button
	if(m_GameMode == GAMEMODE_ATTACK_KEYCRACK)
	{
		m_WindowKeyCrackTool.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}

	// Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;
    if(checkServerInBouncedLink(m_ServerLocked)) return;

	// Make sure we have selected a server
	if(m_ServerLocked == -1 || m_ServerLocked == m_ServerLocal)
	{
		_DisplayErrorMsg("Please select a target. (Click a server on the map)");
		return;
	}

    // Verify if the server has an encryption key
	if(m_GameLevel.m_ServerList[m_ServerLocked].m_EncryptionKey == 0)
	{
		_DisplayErrorMsg("[Encryption Key] not present.");
		return;
	}

	// Server needs to be hacked
    if(m_GameLevel.m_ServerList[m_ServerLocked].m_ServerKey != m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked])
	{
		_DisplayErrorMsg("Access denied. Firewall needs to be hacked first.");
		return;
	}

  	g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-engaging.wav");

    // Initialize key matrix
    int i,j,x,y,temp;
    int index = 0;

    for(i = 0; i < 8; i++)
        for(j = 0; j < 8; j++)
            m_KeyCrackTool_Matrix[i][j] = index++;

    // Shuffle it
    index = 0;
    for(i = 0; i < 8; i++)
        for(j = 0; j < 8; j++)
        {
            x = sysRandomNumber(8);
            y = sysRandomNumber(8);
            temp = m_KeyCrackTool_Matrix[i][j]; m_KeyCrackTool_Matrix[i][j] = m_KeyCrackTool_Matrix[x][y]; m_KeyCrackTool_Matrix[x][y] = temp;

            m_KeyCrackTool_Key[index] = m_KeyCrackTool_Matrix[i][j];
            index ++;
        }


    // All OK. Let's run the tool
    m_KeyCrackTool_Next      = 63;
    m_KeyCrackTool_Failed    = 0;
    m_KeyCrackTool_Timer     = 0.0f;
    m_KeyCrackTool_TimerHelp = 0.0f; 
	m_GameMode            = GAMEMODE_ATTACK_KEYCRACK;
	m_WindowKeyCrackTool.ToggleVisibility();
}


//=======================================================================================================================================
// Server interface
void hedGame::buttonFuncInterface()
{
	// Toggle with the same button
	if(m_GameMode == GAMEMODE_INTERFACE)
	{
		m_WindowInterfaceTool.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}

	// Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;
    if(checkServerInBouncedLink(m_ServerLocked)) return;

	// Make sure we have selected a server
	if(m_ServerLocked == -1)
	{
		_DisplayErrorMsg("Please select a target. (Click a server on the map)");
		return;
	}

	// Server needs to be hacked
	if(m_GameLevel.m_ServerList[m_ServerLocked].isFullyHacked(m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked]) == 0 && m_ServerLocked != m_ServerLocal)
	{
		_DisplayErrorMsg("Access denied. You need to hack this server first.");
		return;
	}

	// Set values in the interface window toggles
	int i;
	for(i = 0; i < 3; i++)
	{
		m_WindowInterfaceTool.AttachImage(INTERFACE_TOGGLE_1 + i * 2 + 0,g_InterfaceManager.skinToggle[0],20,285 + i * 30,0);
		m_WindowInterfaceTool.AttachImage(INTERFACE_TOGGLE_1 + i * 2 + 1,g_InterfaceManager.skinToggle[1],20,285 + i * 30,0);

		if(m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchType[i] > 0)  
			m_WindowInterfaceTool.CreateToggle(INTERFACE_TOGGLE_1 + i * 2, m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchState[i]); 
	}

	m_InterfaceAction          = INTERFACE_ACTION_NONE; // Any action in progress?
	m_InterfaceActionProgress  = 0.0f;                  // Progres..
	m_InterfaceLFSelected      = -1;                    // Selected local file
	m_InterfaceRFSelected      = -1;                    // Selected remote file
	m_InterfaceFWLastSelection = -1;                    // Which file we last selected (for viewing) (remote or local)
	m_InterfaceFWActive        =  0;                    // Fileviewer active?

	// Camera view
	m_WindowInterfaceTool.AttachImage(INTERFACE_CAMERAVIEW,0,429,89,0); // clear previous camera image
	if(m_GameLevel.m_ServerList[m_ServerLocked].m_CameraImagePresent)
		m_WindowInterfaceTool.AttachImage(INTERFACE_CAMERAVIEW,m_GameLevel.m_ServerList[m_ServerLocked].m_CameraImage,429,89,m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchState[0]);

	m_GameMode = GAMEMODE_INTERFACE;
	m_WindowInterfaceTool.ToggleVisibility();
}

//=======================================================================================================================================
// Console button handler
void hedGame::buttonFuncConsole()
{
	/*
#ifdef DEMO
	_DisplayErrorMsg("Unavailable in the DEMO.");
	return;
#endif
	*/

	// Every time the console is toggled, repopulate the command dictionary with the visible server names
	int i;

	m_CommandConsole.Dictionary_Reset();
	for(i = 0; i < m_GameLevel.m_ServerCount; i++)
		if(m_GameLevel.m_ServerList[i].m_Visible == 1) 
			m_CommandConsole.Dictionary_Add(m_GameLevel.m_ServerList[i].m_HostName);

	m_WindowConsole.ToggleVisibility();
}

//=======================================================================================================================================
// Hardware manager
void hedGame::buttonFuncHardware()
{
	// Toggle with the same button
	if(m_GameMode == GAMEMODE_HARDWARE)
	{
		m_WindowHardware.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}

    // Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;

    sprintf(m_HardwareNotification,"-");
	m_GameMode = GAMEMODE_HARDWARE;
	m_WindowHardware.ToggleVisibility();
}

//=======================================================================================================================================
// Messages
void hedGame::buttonFuncMessages()
{
    // Toggle with the same button
	if(m_GameMode == GAMEMODE_MESSAGES)
	{
		m_WindowMessages.ToggleVisibility(); 
		m_GameMode = GAMEMODE_IDLE; 
		return;
	}

    // Make sure there is no action in progress or server is in the bounced link
	if(checkActionInProgress()) return;

	m_GameMode = GAMEMODE_MESSAGES;
	m_WindowMessages.ToggleVisibility();

	// Dismiss the new message notifier
	if(m_WindowNewMessageNotification.isVisible() == 1)
		m_WindowNewMessageNotification.ToggleVisibility();
}

//=======================================================================================================================================
// Objectives button handler
void hedGame::buttonFuncObjectives()
{
	m_WindowObjectives.ToggleVisibility();
}

//=======================================================================================================================================
// Achievements button handler
void hedGame::buttonFuncAchievements()
{
	m_WindowAchievements.ToggleVisibility();
}

//=======================================================================================================================================
// Help button handler
void hedGame::buttonFuncHelp()
{
	m_WindowHelp.ToggleVisibility();
}

//=======================================================================================================================================
// Voice print password tool function 
void hedGame::run_VoicePrintPasswordTool()
{
	int i;
	int vpt_dx1, vpt_dx2, vpt_dy;
	int vpt_length;

	// Get values from the sliders
	int m_VoicePrintTool_Speed     = m_WindowVoicePrintTool.sliderPosition[4] / 3;
	int m_VoicePrintTool_Length    = m_WindowVoicePrintTool.sliderPosition[5];
	int m_VoicePrintTool_Amplitude = (2 * m_WindowVoicePrintTool.sliderPosition[6] - 300) / 5;
	hedClassServer *target = &m_GameLevel.m_ServerList[m_ServerLocked];

	// Snap function. This is to allow easier matching of the values, specifically on touchscreen devices
	if(abs(m_VoicePrintTool_Speed * 3 - target->m_VoicePrintPassword_Speed * 3) < g_TabletPCPrecision) m_WindowVoicePrintTool.sliderPosition[4] =  target->m_VoicePrintPassword_Speed * 3;
	if(abs(m_VoicePrintTool_Length - target->m_VoicePrintPassword_Length * 10) < g_TabletPCPrecision) m_WindowVoicePrintTool.sliderPosition[5] =  target->m_VoicePrintPassword_Length * 10;

    // Amplitude snap
    float _tmp;
    if(target->m_VoicePrintPassword_Amplitude > 0)
        _tmp = ((target->m_VoicePrintPassword_Amplitude * 5) + 300 + 1) / 2;
    else
        _tmp = ((target->m_VoicePrintPassword_Amplitude * 5) + 300 - 1) / 2;

    if(abs(_tmp - m_WindowVoicePrintTool.sliderPosition[6]) < g_TabletPCPrecision) 
		m_WindowVoicePrintTool.sliderPosition[6] =  _tmp;

    // SNAP end
    
	m_WindowVoicePrintTool.Render();
	if(m_WindowVoicePrintTool.isVisible() == 0) return;

	CAPMIN(m_VoicePrintTool_Speed,1);
	CAPMIN(m_VoicePrintTool_Length,1);

	// Calculate amplitude
	for(int i = 0; i < 11; i++)
	{		
		m_VoicePrintPassword[i] = target->m_VoicePrintPassword_Pattern[i] + target->m_VoicePrintPassword_Amplitude - m_VoicePrintTool_Amplitude;
		CAPMIN(m_VoicePrintPassword[i],0);
		CAPMAX(m_VoicePrintPassword[i],60);
	}

	// Gray background pattern
	for(i = 0; i < 300; i++)
	{
		vpt_dy = (float)(target->m_VoicePrintPassword_Pattern[i / 30] * (30 - i % 30)) + (float)(target->m_VoicePrintPassword_Pattern[i / 30 + 1] * (i % 30));
		vpt_dy /= 60;

		m_WindowVoicePrintTool.DrawLine(110 + i, 95 - vpt_dy, 110 + i, 95 + vpt_dy, 0.2,0.2,0.2,1);
	}

	// Draw voice patterns
	vpt_dx1    = (300 - m_VoicePrintTool_Length) / 2;
	vpt_dx2    = (300 - target->m_VoicePrintPassword_Length * 10) / 2;
	vpt_length = ((float)m_VoicePrintTool_Length / 300.0f) * 30.0f;

	for(i = 0; i < 10; i ++)
	{
		m_WindowVoicePrintTool.DrawLine(vpt_dx2 + 110 + i * target->m_VoicePrintPassword_Length, 135 - target->m_VoicePrintPassword_Pattern[i],
			                            vpt_dx2 + 110 + (i + 1) * target->m_VoicePrintPassword_Length, 135 - target->m_VoicePrintPassword_Pattern[i + 1], 
										0.8,0.8,0.8,2);
		m_WindowVoicePrintTool.DrawLine(vpt_dx1 + 110 + i * vpt_length,135 - m_VoicePrintPassword[i], 
			                            vpt_dx1 + 110 + (i + 1) * vpt_length,135 - m_VoicePrintPassword[i + 1], 
										0.8,0.0,0.0,2);
	}
	
	for(i = 0; i < 11; i ++)
	{
		m_WindowVoicePrintTool.DrawLine(vpt_dx2 + 110 + i * target->m_VoicePrintPassword_Length, 136 - target->m_VoicePrintPassword_Pattern[i],
			                            vpt_dx2 + 110 + i * target->m_VoicePrintPassword_Length, 134 - target->m_VoicePrintPassword_Pattern[i], 1,1,1,3);
		m_WindowVoicePrintTool.DrawLine(vpt_dx1 + 110 + i * vpt_length,136 - m_VoicePrintPassword[i],vpt_dx1 + 110 + i * vpt_length,134 - m_VoicePrintPassword[i], 1,0,0,3);
	}

	// Speed(s)
	m_VoicePrintTool_Position1 += (g_FrameTime * (float)target->m_VoicePrintPassword_Speed);
	m_VoicePrintTool_Position2 += (g_FrameTime * (float)m_VoicePrintTool_Speed);

	if(m_VoicePrintTool_Position1 >= 300 || m_VoicePrintTool_Position2 >= 300)
	{
		m_VoicePrintTool_Position1 = 0;
		m_VoicePrintTool_Position2 = 0;
	}

	m_WindowVoicePrintTool.DrawLine(110 + m_VoicePrintTool_Position1,135,110 + m_VoicePrintTool_Position1,65, 0.9,0.9,0.9,1);
	m_WindowVoicePrintTool.DrawLine(110 + m_VoicePrintTool_Position2,135,110 + m_VoicePrintTool_Position2,65, 0.9,0,0,1);

	// Texts
	if(m_VoicePrintTool_Speed == target->m_VoicePrintPassword_Speed) 
		m_WindowVoicePrintTool.RenderText(430,178,0.9,0,0,HED_FONT_BIG10,"MATCH: %d / %d",m_VoicePrintTool_Speed,target->m_VoicePrintPassword_Speed);
	else
		m_WindowVoicePrintTool.RenderText(430,178,0.9,0.9,0.9,HED_FONT_BIG10,"MATCH: %d / %d",m_VoicePrintTool_Speed,target->m_VoicePrintPassword_Speed);

	if(m_VoicePrintTool_Length == target->m_VoicePrintPassword_Length * 10)
		m_WindowVoicePrintTool.RenderText(430,208,0.9,0,0,HED_FONT_BIG10,"MATCH: %3.1f / %d",(float)m_VoicePrintTool_Length / 10.0f,target->m_VoicePrintPassword_Length);
	else
		m_WindowVoicePrintTool.RenderText(430,208,0.9,0.9,0.9,HED_FONT_BIG10,"MATCH: %3.1f / %d",(float)m_VoicePrintTool_Length / 10.0f,target->m_VoicePrintPassword_Length);
	
	if(utilArraysEqual(m_VoicePrintPassword,target->m_VoicePrintPassword_Pattern,11))
		m_WindowVoicePrintTool.RenderText(430,238,0.9,0,0,HED_FONT_BIG10,"MATCH: %d / %d",m_VoicePrintTool_Amplitude,target->m_VoicePrintPassword_Amplitude);
	else
		m_WindowVoicePrintTool.RenderText(430,238,0.9,0.9,0.9,HED_FONT_BIG10,"MATCH: %d / %d",m_VoicePrintTool_Amplitude,target->m_VoicePrintPassword_Amplitude);

	// Handle input
	if(m_WindowVoicePrintTool.ButtonClicked(VPTOOL_BUTTON_CANCEL))
	{
		m_GameMode = GAMEMODE_IDLE;
		m_WindowVoicePrintTool.ToggleVisibility();
	}
	
	if(m_WindowVoicePrintTool.ButtonClicked(VPTOOL_BUTTON_LOGIN))
	{
		if(m_VoicePrintTool_Speed == target->m_VoicePrintPassword_Speed && m_VoicePrintTool_Length == target->m_VoicePrintPassword_Length * 10 &&
		   m_VoicePrintPassword[0] == target->m_VoicePrintPassword_Pattern[0])
		{
			// Login is ok
			g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-accepted.wav");
			m_GameMode = GAMEMODE_IDLE;
			m_WindowVoicePrintTool.ToggleVisibility();
			target->m_VoiceprintPassword = 0;

			// Update achievements
            if(m_GameLevel.m_ServerList[m_ServerLocked].isFullyHacked(m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked]) == 1) g_GameAchievements.achievementValue[HED_ACHIEVEMENT_SERVERSHACKED]++;
            g_GameAchievements.achievementValue[HED_ACHIEVEMENT_VOICEPRINT]++;

            // Update score
            m_GameLevel.m_ServerList[m_ServerLocal].statScore += SCORE_VOICEPRINT;
		}
		else
		{
			g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-access-denied.wav");
		}
	}
}

//=======================================================================================================================================
// DOS tool function 
void hedGame::run_DOSTool()
{
	int i;
	int total = 0,ok = 1;
	
	// ** 
	m_WindowDOSTool.Render();
	if(m_WindowDOSTool.isVisible() == 0) return;

	// Snap function. This is to allow easier matching of the values, specifically on touchscreen devices
	for(i = 0; i < 4; i++)
	{
		if(abs(m_DOSToolBW[i] - m_WindowDOSTool.sliderPosition[2 + i]) < g_TabletPCPrecision)
			m_WindowDOSTool.sliderPosition[2 + i] = m_DOSToolBW[i];
	}

	// 
	if(m_DOSToolReady == 1)
		INCREMENT_WITH_CAP(m_DOSToolChartIndex,g_FrameTime * 30.0f,330.0f);

	// Gray background pattern
	for(i = 0; i < 300; i++)
	{
		m_WindowDOSTool.DrawLine(111 + i, 134 - m_DOSToolChart[i], 111 + i, 134, 0.2,0.2,0.2,1);
		if(m_DOSToolReady == 1 && m_DOSToolChartIndex > i )
			m_WindowDOSTool.DrawLine(111 + i, 134 - m_DOSToolChart[i], 111 + i, 134, 0.9,0.0,0.0,1);
	}

	// Texts
	for(i = 0; i < 4; i++)
	{
        if(m_WindowDOSTool.sliderPosition[2 + i] == m_DOSToolBW[i])
		    m_WindowDOSTool.RenderText(430,178 + i * 30,0.9,0.0,0.0,HED_FONT_NORMAL,"MATCHED! (%3d kbps)",m_WindowDOSTool.sliderPosition[2 + i]);
        else
        {
		    m_WindowDOSTool.RenderText(430,178 + i * 30,0.8,0.8,0.8,HED_FONT_NORMAL,"%3d kbps",m_WindowDOSTool.sliderPosition[2 + i]);
		    m_WindowDOSTool.RenderText(490,178 + i * 30,0.9,0.0,0.0,HED_FONT_NORMAL,"|Needed: %d kbps",m_DOSToolBW[i]);
        }
		total += m_WindowDOSTool.sliderPosition[2 + i];

		if(m_DOSToolBW[i] != m_WindowDOSTool.sliderPosition[2 + i]) ok = 0;
	}

	// Bandwidth adjusted, launch attack
    if(ok == 1 && m_DOSToolReady == 0)
        g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-please-hold.wav");
	if(ok == 1) m_DOSToolReady = 1;

	if(m_DOSToolReady == 1)
    {
		m_WindowDOSTool.RenderText(430,125,0.9,0.0,0.0,HED_FONT_NORMAL,"Please hold...");
		m_WindowDOSTool.RenderText(430,145,0.9,0.0,0.0,HED_FONT_NORMAL,"Running attack ... %3d",330 - (int)m_DOSToolChartIndex);
    }
    else
    {
		m_WindowDOSTool.RenderText(430,125,0.8,0.8,0.8,HED_FONT_NORMAL,"Status: Setting BW");
    	if(total <= 1024)
	    	m_WindowDOSTool.RenderText(430,145,0.8,0.8,0.8,HED_FONT_NORMAL,"Total bw used: %4d kbps",total);
	    else
		    m_WindowDOSTool.RenderText(430,145,0.9,0.0,0.0,HED_FONT_NORMAL,"Bandwidth exceeded!");
    }
	
	//** Dismiss the window 5 seconds after the attack
	if(m_DOSToolChartIndex >= 330)
	{
		// Update servers
		DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocked].m_LevelIntegrity,1,0);
		DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocked].m_LevelFirewall,1,0);
		m_GameLevel.m_ServerList[m_ServerLocal].m_ToolDOSLevel = 0.0f;
		m_GameLevel.m_ServerList[m_ServerLocal].statDOSCount++;
        m_GameLevel.m_ServerList[m_ServerLocal].m_AIStatDOS++;

		sysActivityLogPrint("%d DOS %s",(int)m_SecondsPlayed,m_GameLevel.m_ServerList[m_ServerLocked].m_HostName);

		// Return to the game
		m_GameMode = GAMEMODE_IDLE;
		m_WindowDOSTool.ToggleVisibility();
		m_DOSToolReady = 0;

		g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-complete.wav");

		// Update AI
		m_GameLevel.m_ServerList[m_ServerLocked].AI_IncreaseHostileLevel(m_ServerLocal, AI_HOSTILE_INCREASE_DOS + pda_AI_HOSTILE_INCREASE * g_PlayerDifficulty);

        // Update achievements
        g_GameAchievements.achievementValue[HED_ACHIEVEMENT_DOS]++;

        // Update score
        m_GameLevel.m_ServerList[m_ServerLocal].statScore += SCORE_DOS;
	}

	//** Handle input - CANCEL
	if(m_WindowDOSTool.ButtonClicked(BUTTON_DEFAULT))
	{
		m_GameMode = GAMEMODE_IDLE;
		m_WindowDOSTool.ToggleVisibility();
	}
}

//=======================================================================================================================================
// EMP tool function 
void hedGame::run_EMPTool()
{
	if(g_InterfaceManager.skinSatellite->m_AnimAngle > 180.0f) g_InterfaceManager.skinSatellite->m_AnimAngle -= 360.0f;

	g_RenderEngine.DrawLineAA(satelliteX + 64,satelliteY + 64, m_GameLevel.m_ServerList[m_ServerLocked].getScreenX(), m_GameLevel.m_ServerList[m_ServerLocked].getScreenY(),0.5,0.5,0.5,2);

	if(satelliteAlignStart == 1)
	{
		g_RenderEngine.RenderText(satelliteX + 5,satelliteY - 36, 0.65f,0.65f,0.65f, HED_FONT_NORMAL,"[click] to STOP when the satellite is aligned.");
		g_RenderEngine.RenderText(satelliteX + 5,satelliteY - 24, 0.65f,0.65f,0.65f, HED_FONT_NORMAL,"alignment: %3.2f / %3.2f",g_InterfaceManager.skinSatellite->m_AnimAngle,satelliteAngle);
	}
	else
	{
		g_RenderEngine.RenderText(satelliteX + 5,satelliteY - 36, 0.65f,0.65f,0.65f, HED_FONT_NORMAL,"[click] to START aligning the satellite.");
		g_RenderEngine.RenderText(satelliteX + 5,satelliteY - 24, 0.65f,0.65f,0.65f, HED_FONT_NORMAL,"alignment: %3.2f / %3.2f",g_InterfaceManager.skinSatellite->m_AnimAngle,satelliteAngle);
	}

	if(abs(satelliteAngle - g_InterfaceManager.skinSatellite->m_AnimAngle) < 5.0f)
	{
		g_InterfaceManager.skinSatellite->m_AnimAngle = g_InterfaceManager.skinSatelliteAligned->m_AnimAngle;
		g_InterfaceManager.skinSatelliteAligned->Render(satelliteX,satelliteY);
		
		if(satelliteAlignStart == 1)
			g_RenderEngine.RenderText(satelliteX + 5,satelliteY - 12, 0.9f,0.0f,0.0f, HED_FONT_NORMAL,"satellite aligned.",g_InterfaceManager.skinSatellite->m_AnimAngle,satelliteAngle);
		satelliteAligned = 1;
	}
	else
	{
		g_InterfaceManager.skinSatellite->Render(satelliteX,satelliteY);
		g_InterfaceManager.skinSatelliteAligned->m_AnimAngle = g_InterfaceManager.skinSatellite->m_AnimAngle;
		g_RenderEngine.RenderText(satelliteX + 5,satelliteY - 12, 0.95f,0.95f,0.95f, HED_FONT_NORMAL,"satellite NOT aligned.",g_InterfaceManager.skinSatellite->m_AnimAngle,satelliteAngle);
		satelliteAligned = 0;
	}

	// Satellite has been correctly aligned, launch EMP attack
	if(satelliteAligned == 1 && satelliteAlignStart == 0)
	{
		// Start EMP blast animation
		if(g_InterfaceManager.skinServerEMPBlast->m_AnimScale == 1.0f)
		{
			g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-emp.wav");
			g_InterfaceManager.skinServerEMPBlast->m_AnimScaleSpeed = 0.3f;
			g_InterfaceManager.skinServerEMPBlast->m_AnimScale = 0.0f;
		}
		else
		{
			g_InterfaceManager.skinServerEMPBlast->Render(m_GameLevel.m_ServerList[m_ServerLocked].getScreenX() - 128,m_GameLevel.m_ServerList[m_ServerLocked].getScreenY() - 128);
			g_RenderEngine.RenderText(satelliteX + 5,satelliteY - 12, 0.65f,0.65f,0.65f, HED_FONT_NORMAL,"EMP attack progress: %3.2f%%",g_InterfaceManager.skinServerEMPBlast->m_AnimScale * 100.0f);
		}

		// EMP completed
		if(g_InterfaceManager.skinServerEMPBlast->m_AnimScale == 1.0f)
		{
			DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocked].m_LevelIntegrity,2,0);
			DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocked].m_LevelFirewall,2,0);		

			// Discharge EMP attack tool
			m_GameLevel.m_ServerList[m_ServerLocal].m_ToolEMPLevel = 0.0f;

			// Update stats
			m_GameLevel.m_ServerList[m_ServerLocal].statEMPCount++;
            m_GameLevel.m_ServerList[m_ServerLocal].m_AIStatEMP++;

			// Update AI
			m_GameLevel.m_ServerList[m_ServerLocked].AI_IncreaseHostileLevel(m_ServerLocal, AI_HOSTILE_INCREASE_EMP + pda_AI_HOSTILE_INCREASE * g_PlayerDifficulty);

			m_GameMode = GAMEMODE_IDLE;

            // Update achievements
            g_GameAchievements.achievementValue[HED_ACHIEVEMENT_EMP]++;

            // Update score
            m_GameLevel.m_ServerList[m_ServerLocal].statScore += SCORE_EMP;
		}
	}
}

//=======================================================================================================================================
// Interface tool function 
void hedGame::run_InterfaceTool()
{
	int i;
	int delta = 0;
	
	// File viewer is active on top of the interface
	m_WindowFileView.Render();
	if(m_InterfaceFWActive == 1)
	{
		if(m_InterfaceFWLastSelection == 0)
			m_WindowFileView.RenderText(20,333, 0.65f,0.65f,0.65f, HED_FONT_BIG10, "Remote file: [%s]",m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[m_InterfaceRFSelected]);
		else
			m_WindowFileView.RenderText(20,333, 0.65f,0.65f,0.65f, HED_FONT_BIG10, "Local file: [%s]",m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[m_InterfaceLFSelected]);

		if(m_WindowFileView.ButtonClicked(BUTTON_DEFAULT) || m_Key == HEDKEY_ESCAPE)
		{
			m_WindowFileView.ToggleVisibility();
			m_InterfaceFWActive = 0;
		}
		return;
	}

	// Wait until it becomes fully visible
	m_WindowInterfaceTool.Render();
	if(m_WindowInterfaceTool.isVisible() == 0) return;

	// Render window content
	// Toggle's
	for(i = 0; i < 3; i++)
		if(m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchType[i] != 0)
			m_WindowInterfaceTool.RenderText(90,287 + i * 30,0.8,0.8,0.8,HED_FONT_BIG10,m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchName[i]);

	// Money
	if(m_InterfaceAction == INTERFACE_ACTION_TRANSFER_1) delta = (m_GameLevel.m_ServerList[m_ServerLocked].m_Money * m_InterfaceActionProgress) / -100;
	if(m_InterfaceAction == INTERFACE_ACTION_TRANSFER_2) delta = (m_GameLevel.m_ServerList[m_ServerLocal].m_Money * m_InterfaceActionProgress) / 100;

	if(m_GameLevel.m_ServerList[m_ServerLocked].m_Money + delta > 0)
		m_WindowInterfaceTool.RenderText(435,248,0.9,0.0,0.0,HED_FONT_NORMAL,"$%d",m_GameLevel.m_ServerList[m_ServerLocked].m_Money + delta);
	else
		m_WindowInterfaceTool.RenderText(435,248,0.9,0.0,0.0,HED_FONT_NORMAL,"no money");

	// File lists
	for(i = 0; i < m_GameLevel.m_ServerList[m_ServerLocked].m_FileCount; i++)
		m_WindowInterfaceTool.RenderText(30,76 + i * 15,0.65f,0.65f,0.65f,HED_FONT_NORMAL,m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[i]);
	for(i = 0; i < m_GameLevel.m_ServerList[m_ServerLocal].m_FileCount; i++)
		m_WindowInterfaceTool.RenderText(230,76 + i * 15,0.65f,0.65f,0.65f,HED_FONT_NORMAL,m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[i]);

	// Action progress
	for(i = 0; i < m_InterfaceActionProgress * 2; i++)
	{
		m_WindowInterfaceTool.DrawLine(410 + i, 15, 410 + i, 25, 0.9,0.0,0.0,1);
	}

	// ** Handle input

	// File selection
	int mx = m_WindowInterfaceTool.mouseToWindowX(m_InputX);
	int my = m_WindowInterfaceTool.mouseToWindowY(m_InputY);
	int dx,dy;
	int rf_selected = -1, lf_selected = -1;

	// Remote file
	if(mx >= 20 && mx < 220) rf_selected = (my - 76) / 15;
	if(rf_selected >= 0 && rf_selected < m_GameLevel.m_ServerList[m_ServerLocked].m_FileCount) {} else rf_selected = -1;
	if(mx >= 220 && mx <= 420) lf_selected = (my - 76) / 15;
	if(lf_selected >= 0 && lf_selected < m_GameLevel.m_ServerList[m_ServerLocal].m_FileCount) {} else lf_selected = -1;

    if(m_InterfaceAction == INTERFACE_ACTION_NONE)
    {
        // Only allow file selection, when "idle"
	    if(m_Click == 1 && rf_selected != -1) {m_InterfaceRFSelected = rf_selected; m_InterfaceFWLastSelection = 0;}
	    if(m_Click == 1 && lf_selected != -1) {m_InterfaceLFSelected = lf_selected; m_InterfaceFWLastSelection = 1;}
    }

	// Draw file highlight
	dx = m_WindowInterfaceTool.m_DisplayX + 21;
	dy = m_WindowInterfaceTool.m_DisplayY + 75;
	if(rf_selected != -1) g_InterfaceManager.skinFileMarker[0]->Render(dx,dy + rf_selected * 15);
	if(lf_selected != -1) g_InterfaceManager.skinFileMarker[0]->Render(dx + 200,dy + lf_selected * 15);

	if(m_InterfaceRFSelected != -1)
	{
		m_WindowInterfaceTool.RenderText(30,60,0.9f,0.0f,0.0f,HED_FONT_NORMAL,"%s",m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[m_InterfaceRFSelected]);
		g_InterfaceManager.skinFileMarker[1]->Render(dx,dy + m_InterfaceRFSelected * 15);
	}

	if(m_InterfaceLFSelected != -1)
	{
		m_WindowInterfaceTool.RenderText(230,60,0.9f,0.0f,0.0f,HED_FONT_NORMAL,"%s",m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[m_InterfaceLFSelected]);
		g_InterfaceManager.skinFileMarker[1]->Render(dx + 200,dy + m_InterfaceLFSelected * 15);
	}

	// Update switches
    int _initialState;
	for(i = 0; i < 3; i++)
		if(m_WindowInterfaceTool.toggleStatus[INTERFACE_TOGGLE_1 + i * 2] != -1)
        {
            _initialState = m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchState[i];
			m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchState[i] = m_WindowInterfaceTool.toggleStatus[INTERFACE_TOGGLE_1 + i * 2];

            // Verify trap switch
            if(_initialState != m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchState[i] && m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchType[i] == SWITCH_TRAP_1)
            {
                INCREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel,SWITCH_TRAP_HIT,100);
                m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchType[i] = SWITCH_NORMAL;
                m_WindowToolbar.PrintTextN("[WARNING] Unauthorized switch use detected. Tracelevel +%d %s",SWITCH_TRAP_HIT,"%%");
                g_SoundManager.PlaySoundFX("hed-audio/hed-audiofx-impact.wav");
            }

        }
	// Force camera view refresh
	if(m_GameLevel.m_ServerList[m_ServerLocked].m_CameraImagePresent)
		m_WindowInterfaceTool.AttachImage(INTERFACE_CAMERAVIEW,m_GameLevel.m_ServerList[m_ServerLocked].m_CameraImage,429,89,m_GameLevel.m_ServerList[m_ServerLocked].m_SwitchState[0]);

	// Update action
	if(m_InterfaceAction != INTERFACE_ACTION_NONE)
	{
		m_InterfaceActionProgress += g_FrameTime * 10.0f;     // Action takes 10 seconds to complete
		CAPMAX(m_InterfaceActionProgress,100.0f);

		// Completed?
		if(m_InterfaceActionProgress == 100.0f)
		{
			// Money transfer from server
			if(m_InterfaceAction == INTERFACE_ACTION_TRANSFER_1)
			{
                // If we transfered money to an AI controlled server, increase it's hostility
                if(m_GameLevel.m_ServerList[m_ServerLocked].m_AIControlled == 1)
                    m_GameLevel.m_ServerList[m_ServerLocked].AI_IncreaseHostileLevel(0,(m_GameLevel.m_ServerList[m_ServerLocked].m_Money / AI_HOSTILE_INCREASE_MONEY)); 

                m_WindowToolbar.PrintTextN("$%d has been transfered to your account.",m_GameLevel.m_ServerList[m_ServerLocked].m_Money);
				m_GameLevel.m_ServerList[m_ServerLocal].m_Money += m_GameLevel.m_ServerList[m_ServerLocked].m_Money;
				m_GameLevel.m_ServerList[m_ServerLocked].m_Money = 0;

                // If we transfered money to an AI controlled server, increase it's hostility
			}

			// Money transfer to server
			if(m_InterfaceAction == INTERFACE_ACTION_TRANSFER_2)
			{
                // If we transfered money to an AI controlled server, decrease it's hostility
                if(m_GameLevel.m_ServerList[m_ServerLocked].m_AIControlled == 1)
                    m_GameLevel.m_ServerList[m_ServerLocked].AI_IncreaseHostileLevel(0,-(m_GameLevel.m_ServerList[m_ServerLocal].m_Money / AI_HOSTILE_INCREASE_MONEY)); 

                m_WindowToolbar.PrintTextN("$%d has been transfered to [%s].",m_GameLevel.m_ServerList[m_ServerLocal].m_Money,m_GameLevel.m_ServerList[m_ServerLocked].m_HostName);
				m_GameLevel.m_ServerList[m_ServerLocked].m_Money += m_GameLevel.m_ServerList[m_ServerLocal].m_Money;
				m_GameLevel.m_ServerList[m_ServerLocal].m_Money = 0;
			}

			// Download
			if(m_InterfaceAction == INTERFACE_ACTION_DOWNLOAD)
			{
				m_WindowToolbar.PrintTextN("%s has been downloaded to your server.",m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[m_InterfaceRFSelected]);
				m_GameLevel.m_ServerList[m_ServerLocal].fileAdd(m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[m_InterfaceRFSelected],m_GameLevel.m_ServerList[m_ServerLocked].m_FilePath[m_InterfaceRFSelected]);
			}

			// Upload
			if(m_InterfaceAction == INTERFACE_ACTION_UPLOAD)
			{
				m_WindowToolbar.PrintTextN("%s has been uploaded to the server.",m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[m_InterfaceLFSelected]);
				m_GameLevel.m_ServerList[m_ServerLocked].fileAdd(m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[m_InterfaceLFSelected],m_GameLevel.m_ServerList[m_ServerLocal].m_FilePath[m_InterfaceLFSelected]);
			}

			// Reset values
			m_InterfaceAction         = INTERFACE_ACTION_NONE;
			m_InterfaceActionProgress = 0.0f;
		}
	}

	// Skip processing input while an action is in progress
	if(m_InterfaceAction != INTERFACE_ACTION_NONE)
    {
        // Clear clicks
        m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_VIEW);
        m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_LOGOUT);
        m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_TRANSFER);
        m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_DOWNLOAD);
        m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_UPLOAD);
        m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_DELETE);

		return;
    }

	// Button: VIEW
	if(m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_VIEW))
	{
		if(m_InterfaceFWLastSelection == -1)
			_DisplayErrorMsg("Please select a file to view...");
		else
		{
			m_InterfaceFWActive = 1;
			m_WindowFileView.ToggleVisibility();

			// Display file
			m_WindowFileView.Clear();
			if(m_InterfaceFWLastSelection == 0)
				m_WindowFileView.DisplayFile(m_GameLevel.m_ServerList[m_ServerLocked].m_FilePath[m_InterfaceRFSelected]);
			else
				m_WindowFileView.DisplayFile(m_GameLevel.m_ServerList[m_ServerLocal].m_FilePath[m_InterfaceLFSelected]);
		}
	}

	// Button: DELETE
	if(m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_DELETE))
	{
		if(m_InterfaceFWLastSelection == -1)
			_DisplayErrorMsg("Please select a file to delete...");
		else
		{
			if(m_InterfaceFWLastSelection == 0)
			{
				// Delete REMOTE file
				for(i = m_InterfaceRFSelected; i < m_GameLevel.m_ServerList[m_ServerLocked].m_FileCount - 1; i++)
				{
					strcpy(m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[i],m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[i + 1]);
					strcpy(m_GameLevel.m_ServerList[m_ServerLocked].m_FilePath[i],m_GameLevel.m_ServerList[m_ServerLocked].m_FilePath[i + 1]);
				}				
				m_GameLevel.m_ServerList[m_ServerLocked].m_FileCount--;
			}
			else
			{
				for(i = m_InterfaceLFSelected; i < m_GameLevel.m_ServerList[m_ServerLocal].m_FileCount - 1; i++)
				{
					strcpy(m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[i],m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[i + 1]);
					strcpy(m_GameLevel.m_ServerList[m_ServerLocal].m_FilePath[i],m_GameLevel.m_ServerList[m_ServerLocal].m_FilePath[i + 1]);
				}				
				m_GameLevel.m_ServerList[m_ServerLocal].m_FileCount--;
			}

			// Reset file lists
			rf_selected = -1;
			lf_selected = -1;
			m_InterfaceLFSelected = -1;
			m_InterfaceRFSelected = -1;
			m_InterfaceFWLastSelection = -1;
		}
	}

	// Button: LOGOUT
	if(m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_LOGOUT))
	{
		m_GameMode = GAMEMODE_IDLE;
		m_WindowInterfaceTool.ToggleVisibility();
	}

    // The buttons below, are disabled for the localhost
    if(m_ServerLocked == m_ServerLocal)
    {
        if(m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_TRANSFER) || m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_DOWNLOAD) || m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_UPLOAD))
        {
			_DisplayErrorMsg("This function is not available on your localhost.");
            return;
        }
    }

	// Button: TRANSFER
	if(m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_TRANSFER))
	{
		// Transfer money from the server
		if(m_GameLevel.m_ServerList[m_ServerLocked].m_Money > 0)
			m_InterfaceAction = INTERFACE_ACTION_TRANSFER_1;
		else
			if(m_GameLevel.m_ServerList[m_ServerLocal].m_Money > 0)
				m_InterfaceAction = INTERFACE_ACTION_TRANSFER_2;

		if(m_GameLevel.m_ServerList[m_ServerLocked].m_Money == 0 && m_GameLevel.m_ServerList[m_ServerLocal].m_Money == 0)
			_DisplayErrorMsg("There is no money available for transfer.");
		else
			m_WindowToolbar.PrintTextN("Money transfer in progress.");

	}
	
	// Button: DOWNLOAD
	if(m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_DOWNLOAD))
	{
		int ok = 1;
		if(m_InterfaceRFSelected == -1)                                          {ok = 0; _DisplayErrorMsg("Please select a file to download.");};
		if(m_GameLevel.m_ServerList[m_ServerLocal].m_FileCount >= GAME_MAXFILES) {ok = 0; _DisplayErrorMsg("No space availabe on your server, to download this file.");};
		if(m_GameLevel.m_ServerList[m_ServerLocal].filePresent(m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[m_InterfaceRFSelected],m_GameLevel.m_ServerList[m_ServerLocked].m_FilePath[m_InterfaceRFSelected]))
	    {ok = 0; _DisplayErrorMsg("This file is already present on your server.");};

		if(ok == 1)
		{
			m_WindowToolbar.PrintTextN("Downloading %s...",m_GameLevel.m_ServerList[m_ServerLocked].m_FileName[m_InterfaceRFSelected]);
			m_InterfaceAction = INTERFACE_ACTION_DOWNLOAD;
		}
	}

	// Button: UPLOAD
	if(m_WindowInterfaceTool.ButtonClicked(INTERFACE_BUTTON_UPLOAD))
	{
		int ok = 1;
		if(m_InterfaceLFSelected == -1)                                           {ok = 0; _DisplayErrorMsg("Please select a file to upload.");};
		if(m_GameLevel.m_ServerList[m_ServerLocked].m_FileCount >= GAME_MAXFILES) {ok = 0; _DisplayErrorMsg("No space availabe on the server, to upload this file.");};
		if(m_GameLevel.m_ServerList[m_ServerLocked].filePresent(m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[m_InterfaceLFSelected],m_GameLevel.m_ServerList[m_ServerLocal].m_FilePath[m_InterfaceLFSelected]))
	    {ok = 0; _DisplayErrorMsg("This file is already present on the server.");};

		if(ok == 1)
		{
			m_WindowToolbar.PrintTextN("Uploading %s...",m_GameLevel.m_ServerList[m_ServerLocal].m_FileName[m_InterfaceLFSelected]);
			m_InterfaceAction = INTERFACE_ACTION_UPLOAD;
		}
	}
}

//=======================================================================================================================================
// Message handler
void hedGame::run_Messages()
{
	int i;

	// render the window, but wait until it becomes fully visible
	m_WindowMessages.Render();
	if(m_WindowMessages.isVisible() == 0) return;

	// Render message list
	for(i = 0; i < m_GameLevel.m_RMessageCount; i++)
		m_WindowMessages.RenderText(25,45 + i * 15,0.65f,0.65f,0.65f,HED_FONT_NORMAL,m_GameLevel.m_MessageList[m_GameLevel.m_RMessageList[i]].subject);
	if(m_GameLevel.m_RMessageCount == 0)
		m_WindowMessages.RenderText(25,45,0.65f,0.65f,0.65f,HED_FONT_NORMAL,"You have no messages.");

	// ** Handle input
	// Message selection
	int mx = m_WindowMessages.mouseToWindowX(m_InputX);
	int my = m_WindowMessages.mouseToWindowY(m_InputY);
	int dx,dy;
	int m_selected = -1;

	if(mx >= 20 && mx < 220) m_selected = (my - 45) / 15;
	if(m_selected >= 0 && m_selected < m_GameLevel.m_RMessageCount) {} else m_selected = -1;

	if(m_Click == 1 && m_selected != -1) m_GameLevel.m_RMessageIndex = m_selected;

	// Draw file highlight
	dx = m_WindowMessages.m_DisplayX + 20;
	dy = m_WindowMessages.m_DisplayY + 45;
	if(m_selected != -1) g_InterfaceManager.skinFileMarker[0]->Render(dx,dy + m_selected * 15);

	if(m_GameLevel.m_RMessageIndex != -1)
		g_InterfaceManager.skinFileMarker[1]->Render(dx,dy + m_GameLevel.m_RMessageIndex * 15);

	// Display message body
	if(m_GameLevel.m_RMessageIndex != -1)
	{	
		m_WindowMessageBody.Clear();
		m_WindowMessageBody.DisplayFile(m_GameLevel.m_MessageList[m_GameLevel.m_RMessageList[m_GameLevel.m_RMessageIndex]].fileName);
		m_WindowMessageBody.Render();
	}

    // Process input
    if(m_WindowMessages.ButtonClicked(BUTTON_DEFAULT))
    {
        m_GameMode = GAMEMODE_IDLE;
        m_WindowMessages.ToggleVisibility();
    }
}

//=======================================================================================================================================
// Firewall attack handler
void hedGame::run_FirewallAttack()
{
    int i,j;

	// ** Wait until the window becomes fully visible
	m_WindowFirewallAttack.Render();
	if(m_WindowFirewallAttack.isVisible() == 0) return;

    // ** Update grid
    int _cleared;
    
    DECREMENT_WITH_CAP(m_FirewallAttackTimeLeft,g_FrameTime,0.0f);
    _cleared = (int)((400.0f / m_FirewallAttackDuration) * (m_FirewallAttackDuration - m_FirewallAttackTimeLeft)); // How many grid elements should have been cleared so far?
    CAPMAX(_cleared, 400);

    if(m_FirewallAttackTimeLeft == 0.0f) _cleared = 400;

    if(m_FirewallAttackCleared < _cleared)
    {
       do
       {
           i = rand() % 20;
           j = rand() % 20;
       }while(m_FirewallAttackGrid[i][j] == 0);

       m_FirewallAttackCleared++;
       m_FirewallAttackGrid[i][j] = 0;
    }

    // ** Render window content
    for(i = 0; i < 20; i++)
        for(j = 0; j < 20; j++)
            if(m_FirewallAttackGrid[i][j] == 1)
                m_WindowFirewallAttack.DrawRectangleFill(1 + 116 + i * 20,1 + 76 + j * 10,116 + i * 20 + 18,76 + j * 10 + 8,0.9,0.0,0.0,1.0);

    if(m_FirewallAttackCleared < 400)
        m_WindowFirewallAttack.RenderText(115,50,0.9,0.0,0.0,HED_FONT_BIG10,"Cells left to bypass: %d",400 - m_FirewallAttackCleared);
    else
        m_WindowFirewallAttack.RenderText(115,50,0.9,0.0,0.0,HED_FONT_BIG10,"Firewall attack completed!");

    // ** Process input
    if(m_WindowFirewallAttack.ButtonClicked(INTERFACE_BUTTON_LOGOUT))
    {
        m_WindowFirewallAttack.ToggleVisibility();
        m_GameMode = GAMEMODE_IDLE;

        // Was this a success?
        if(m_FirewallAttackCleared == 400)
        {
        	// Update server state
            DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocked].m_LevelIntegrity,1,0);
            m_GameLevel.m_ServerList[m_ServerLocked].m_ServerKeyRegenerationTime = SERVER_KEYREGEN_TIME;
	        m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked] = m_GameLevel.m_ServerList[m_ServerLocked].m_ServerKey;

	        // Update stats
	        m_GameLevel.m_ServerList[m_ServerLocal].statHackCount++;
            m_GameLevel.m_ServerList[m_ServerLocal].m_AIStatFirewall++;

	        // Log
	        sysActivityLogPrint("%d hacked %s",(int)m_SecondsPlayed,m_GameLevel.m_ServerList[m_ServerLocked].m_HostName);

	        // HINT
	        m_WindowToolbar.PrintTextN("Bounce through this server to increase your CPU power.");
	        m_WindowToolbar.PrintTextN("Double-click on the server to bounce through it.");

	        // Update AI
	        m_GameLevel.m_ServerList[m_ServerLocked].AI_IncreaseHostileLevel(m_ServerLocal, AI_HOSTILE_INCREASE_FIREWALL + pda_AI_HOSTILE_INCREASE * g_PlayerDifficulty);

	        // Update achievements
	        if(m_GameLevel.m_ServerList[m_ServerLocked].isFullyHacked(m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked]) == 1) g_GameAchievements.achievementValue[HED_ACHIEVEMENT_SERVERSHACKED]++;
            g_GameAchievements.achievementValue[HED_ACHIEVEMENT_FIREWALL]++;

            // Update score
            m_GameLevel.m_ServerList[m_ServerLocal].statScore += SCORE_FIREWALL;

        }
    }
}

//=======================================================================================================================================
// Hardware manager handler
void hedGame::run_HardwareManager()
{
    int       i;

	// ** Wait until the window becomes fully visible
	m_WindowHardware.Render();
	if(m_WindowHardware.isVisible() == 0) return;

    // ** Draw player's hardware
    for(i = 0; i < 10; i++)
    {
        if(m_GameLevel.m_ServerList[m_ServerLocal].m_LevelFirewall > i)
            g_InterfaceManager.skinHardwareIcon[0]->Render(i * 50 + 20 + m_WindowHardware.m_DisplayX,60 + m_WindowHardware.m_DisplayY);

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_LevelCPUPower > i)
            g_InterfaceManager.skinHardwareIcon[1]->Render(i * 50 + 20 + m_WindowHardware.m_DisplayX,130 + m_WindowHardware.m_DisplayY);

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_LevelIntegrity > i)
            g_InterfaceManager.skinHardwareIcon[2]->Render(i * 50 + 20 + m_WindowHardware.m_DisplayX,200 + m_WindowHardware.m_DisplayY);
    }

    m_WindowHardware.RenderText(20,45,0.9,0.9,0.9,HED_FONT_NORMAL,"Firewall level: %d | Upgrade price: $%d / unit",m_GameLevel.m_ServerList[m_ServerLocal].m_LevelFirewall,UPGRADE_PRICE_FIREWALL);
    m_WindowHardware.RenderText(20,115,0.9,0.9,0.9,HED_FONT_NORMAL,"CPU level: %d | Upgrade price: $%d / unit",m_GameLevel.m_ServerList[m_ServerLocal].m_LevelCPUPower,UPGRADE_PRICE_CPU);
    m_WindowHardware.RenderText(20,185,0.9,0.9,0.9,HED_FONT_NORMAL,"Integrity level: %d | Upgrade price: $%d (+10%s trace level reduction)",m_GameLevel.m_ServerList[m_ServerLocal].m_LevelIntegrity,UPGRADE_PRICE_INTEGRITY,"%%");

    // ** Process input
    if(m_WindowHardware.ButtonClicked(HARDWARE_BUTTON_FIREWALL))
    {
        if(m_GameLevel.m_ServerList[m_ServerLocal].m_LevelFirewall >= 10)
            sprintf(m_HardwareNotification,"Upgrade limit reached.");

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_Money < UPGRADE_PRICE_FIREWALL)
            sprintf(m_HardwareNotification,"You don't have enough money to purchase this upgrade.");

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_Money >= UPGRADE_PRICE_FIREWALL && m_GameLevel.m_ServerList[m_ServerLocal].m_LevelFirewall < 10)
        {
            m_GameLevel.m_ServerList[m_ServerLocal].m_LevelFirewall++;
            m_GameLevel.m_ServerList[m_ServerLocal].m_Money -= UPGRADE_PRICE_FIREWALL;
            sprintf(m_HardwareNotification,"Firewall upgraded one level at a cost of $%d",UPGRADE_PRICE_FIREWALL);
            g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");

            // (OPEN WORLD)
            if(m_GameLevel.m_OpenWorld == 1)
            {
                if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_Money += UPGRADE_PRICE_FIREWALL;            // Move the money to HackerEvolutionDuality.com
                if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_LevelFirewall += 1;
                if(g_AISetting_Deathmatch == 1 && g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_LevelIntegrity++;
            }
        }
    }

    if(m_WindowHardware.ButtonClicked(HARDWARE_BUTTON_CPU))
    {
        if(m_GameLevel.m_ServerList[m_ServerLocal].m_LevelCPUPower >= 10)
            sprintf(m_HardwareNotification,"Upgrade limit reached.");

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_Money < UPGRADE_PRICE_CPU)
            sprintf(m_HardwareNotification,"You don't have enough money to purchase this upgrade.");

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_Money >= UPGRADE_PRICE_CPU && m_GameLevel.m_ServerList[m_ServerLocal].m_LevelCPUPower < 10)
        {
            m_GameLevel.m_ServerList[m_ServerLocal].m_LevelCPUPower++;
            m_GameLevel.m_ServerList[m_ServerLocal].m_Money -= UPGRADE_PRICE_CPU;
            sprintf(m_HardwareNotification,"CPU upgraded one level at a cost of $%d",UPGRADE_PRICE_CPU);
            g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");

            // (OPEN WORLD)
            if(m_GameLevel.m_OpenWorld == 1)
            {
                if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_Money += UPGRADE_PRICE_CPU;            // Move the money to HackerEvolutionDuality.com
                if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_LevelFirewall += 1;
                if(g_AISetting_Deathmatch == 1 && g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_LevelIntegrity++;
            }

        }
    }

    if(m_WindowHardware.ButtonClicked(HARDWARE_BUTTON_INTEGRITY))
    {
        if(m_GameLevel.m_ServerList[m_ServerLocal].m_LevelIntegrity >= 10)
            sprintf(m_HardwareNotification,"Integrity level is at it's maximum value.");

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_Money < UPGRADE_PRICE_INTEGRITY)
            sprintf(m_HardwareNotification,"You don't have enough money to enhance your integrity.");

        if(m_GameLevel.m_ServerList[m_ServerLocal].m_Money >= UPGRADE_PRICE_INTEGRITY && m_GameLevel.m_ServerList[m_ServerLocal].m_LevelIntegrity < 10)
        {
            m_GameLevel.m_ServerList[m_ServerLocal].m_LevelIntegrity++;
            DECREMENT_WITH_CAP(m_GameLevel.m_ServerList[m_ServerLocal].m_TraceLevel, UPGRADE_TRACE_DECREASE, 0);
            m_GameLevel.m_ServerList[m_ServerLocal].m_Money -= UPGRADE_PRICE_INTEGRITY;
            sprintf(m_HardwareNotification,"Integrity enhanced by 1. Trace level reduced by 10 percent.");
            g_SoundManager.PlaySoundFX("hed-audio/hed-soundfx-click.wav");

            // (OPEN WORLD)
            if(m_GameLevel.m_OpenWorld == 1)
            {
                if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_Money += UPGRADE_PRICE_INTEGRITY;            // Move the money to HackerEvolutionDuality.com
                if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_LevelFirewall += 1;
                if(g_AISetting_Deathmatch == 1 && g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_LevelIntegrity++;

                // Reset hacked info
		        if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_ServerKeyRegenerationTime = -1.0f;
		        if(g_AISetting_MoneyDest != -1) m_GameLevel.m_ServerList[g_AISetting_MoneyDest].m_ServerKey = (rand() % 99) * (rand() % 99);
            }
        }
    }
    m_WindowHardware.RenderText(20,250,0.9,0.9,0.9,HED_FONT_NORMAL,m_HardwareNotification);
}

//=======================================================================================================================================
// Encryption key cracking tool
void hedGame::run_KeyCrackTool()                              
{
    int i,j;
    int x,y;
    int showHint = -1,hx,hy;

	// ** Wait until the window becomes fully visible
	m_WindowKeyCrackTool.Render(); if(m_WindowKeyCrackTool.isVisible() == 0) return;

    // ** Timers
    m_KeyCrackTool_Timer     += g_FrameTime;
    m_KeyCrackTool_TimerHelp += g_FrameTime;

    if(m_KeyCrackTool_TimerHelp > 4 + 3 * g_PlayerDifficulty)
        showHint = 1;

    // ** Render
    for(i = 0; i < 8; i++)
        for(j = 0; j < 8; j++)
        {
            if(m_KeyCrackTool_Matrix[i][j] >= 0 && m_KeyCrackTool_Next != -1)
                m_WindowKeyCrackTool.RenderText(75 + 18 + i * 60,60 + 10 + j * 30,0.8,0,0,HED_FONT_BIG12,"%2d",m_KeyCrackTool_Matrix[i][j]);
            else
                m_WindowKeyCrackTool.RenderText(75 + 18 + i * 60,60 + 10 + j * 30,0.2,0.2,0.2,HED_FONT_BIG12,"%2d",-m_KeyCrackTool_Matrix[i][j]);

            if(m_KeyCrackTool_Matrix[i][j] == m_KeyCrackTool_Next)
            {
                hx = i; hy = j;
            }
        }

    if(m_KeyCrackTool_Failed == 0)
    {
        if(m_KeyCrackTool_Next != -1)
            m_WindowKeyCrackTool.RenderText(75,310,0.9,0.9,0.9,HED_FONT_BIG10,"Next cell to clear: %2d [0x%2X]",m_KeyCrackTool_Next,m_KeyCrackTool_Next);
        else
            m_WindowKeyCrackTool.RenderText(75,310,0.9,0.9,0.9,HED_FONT_BIG10,"Key decrypted. Press LOGIN to proceed.");
    }
    else
    {
        m_WindowKeyCrackTool.RenderText(75,310,0.9,0.0,0.0,HED_FONT_BIG10,"Encryption key crack failed.");
    }
    // ** Cell highlight
    x = m_WindowKeyCrackTool.mouseToWindowX(m_InputX);
    y = m_WindowKeyCrackTool.mouseToWindowY(m_InputY);

    x = (x - 75) / 60;
    y = (y - 60) / 30;

    if(x >= 0 && x < 8 && y >= 0 && y < 8) m_WindowKeyCrackTool.DrawRectangleFill(77 + x * 60, 62 + y * 30,76 + 58 + x * 60, 62 + 28 + y * 30,1.0f,1.0f,1.0f,0.2);
    if(showHint == 1) m_WindowKeyCrackTool.DrawRectangleFill(77 + hx * 60, 62 + hy * 30,76 + 58 + hx * 60, 62 + 28 + hy * 30,1.0f,1.0f,1.0f,0.5);

    // ** Timers
    m_WindowKeyCrackTool.RenderText(20,60,0.9,0.0,0.0,HED_FONT_NORMAL,utilFormatTime(m_KeyCrackTool_Timer));

    // ** Process input
    if(m_Click == 1 && x >= 0 && x < 8 && y >= 0 && y < 8 && m_KeyCrackTool_Failed == 0 && m_KeyCrackTool_Matrix[x][y] >= 0)
    {
        if(m_KeyCrackTool_Matrix[x][y] == m_KeyCrackTool_Next)
        {
            m_KeyCrackTool_Next--;
            m_KeyCrackTool_Matrix[x][y] = -m_KeyCrackTool_Matrix[x][y];
            m_KeyCrackTool_TimerHelp = 0.0f;
        }
        else
        {
			// UPDATE 1
			// On EASY and MEDIUM mode, allow the player to continue using the tool even if he failed to check the correct number.
			if(g_PlayerDifficulty > 1)
				m_KeyCrackTool_Failed = 1;
			else
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-error.wav");

            _DisplayErrorMsg("Encryption key crack failed.");
        }
    }
    
    if(m_WindowKeyCrackTool.ButtonClicked(TOOL_BUTTON_LOGIN) && m_KeyCrackTool_Next == -1)
    {
        m_GameLevel.m_ServerList[m_ServerLocked].m_EncryptionKey = 0;
        m_WindowKeyCrackTool.ToggleVisibility();
        m_GameMode = GAMEMODE_IDLE;

        // Update achievements
        if(m_GameLevel.m_ServerList[m_ServerLocked].isFullyHacked(m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked]) == 1) g_GameAchievements.achievementValue[HED_ACHIEVEMENT_SERVERSHACKED]++;
        g_GameAchievements.achievementValue[HED_ACHIEVEMENT_KEYCRACK]++;
    }

    if(m_WindowKeyCrackTool.ButtonClicked(TOOL_BUTTON_CANCEL))
    {
        m_WindowKeyCrackTool.ToggleVisibility();
        m_GameMode = GAMEMODE_IDLE;
    }
}

//=======================================================================================================================================
// Retinal scan tool
void hedGame::run_RetinalScanTool()                              
{
    int x,y,i,j;

	// ** Wait until the window becomes fully visible
	m_WindowRetinalScanTool.Render(); if(m_WindowRetinalScanTool.isVisible() == 0) return;

    // ** Timers

    // ** Render
    for(i = 0; i < 5; i++)
        for(j = 0; j < 5; j++)
        {
            if(m_RetinalScanTool_Matrix[i][j] >= 0 && m_RetinalScanTool_Next != -1)
                m_WindowRetinalScanTool.RenderText(-30 + 75 + 18 + (i + 2) * 60,-15 + 50 + 10 + (j + 2)* 30,0.0,0.0,0.0,HED_FONT_BIG12,"%2d",m_RetinalScanTool_Matrix[i][j]);
            else
            {
                m_WindowRetinalScanTool.RenderText(-30 + 75 + 18 + (i + 2) * 60,-15 + 50 + 10 + (j + 2) * 30,0.2,0.2,0.2,HED_FONT_BIG12,"%2d",-m_RetinalScanTool_Matrix[i][j]);
                
                
                int _x, _y, border = 10;

                _x = (75 + (i + 2) * 60);
                _y = (50 + (j + 2) * 30);

                m_WindowRetinalScanTool.DrawLine(75,_y,555,_y,0.0f,0.0f,0.0f,2);
                m_WindowRetinalScanTool.DrawLine(_x,50,_x,290,0.0f,0.0f,0.0f,2);
            }
        }

   for(i = 0; i < 5; i++)
        for(j = 0; j < 5; j++)
            if(m_RetinalScanTool_Matrix[i][j] >= 0 && m_RetinalScanTool_Next != -1)
                m_WindowRetinalScanTool.RenderText(-30 + 75 + 18 + (i + 2) * 60,-15 + 50 + 10 + (j + 2)* 30,0.98,0.64f,0.07f,HED_FONT_BIG12,"%2d",m_RetinalScanTool_Matrix[i][j]);


    if(m_RetinalScanTool_Failed == 0)
    {
        if(m_RetinalScanTool_Next != -1)
            m_WindowRetinalScanTool.RenderText(75,310,0.9,0.9,0.9,HED_FONT_BIG10,"Next control point to lock: %2d [click on %d]",m_RetinalScanTool_Next,m_RetinalScanTool_Next);
        else
            m_WindowRetinalScanTool.RenderText(75,310,0.9,0.9,0.9,HED_FONT_BIG10,"Retinal scan bypassed. Press LOGIN to proceed.");
    }
    else
    {
        m_WindowRetinalScanTool.RenderText(75,310,0.9,0.0,0.0,HED_FONT_BIG10,"Retinal scan bypass failed.");
    }

    // ** Cell highlight
    x = m_WindowRetinalScanTool.mouseToWindowX(m_InputX);
    y = m_WindowRetinalScanTool.mouseToWindowY(m_InputY);

    x = (x - 45) / 60;
    y = (y - 35) / 30;

    if(x >= 2 && x <= 6 && y >= 2 && y <= 6) 
    {   
        int _x, _y, border = 10;


        _x =  (45 + x * 60) + border;
        _y =  (35 + y * 30) + border;

        m_WindowRetinalScanTool.DrawRectangleFill(_x,_y - 1,_x + 61 - border * 2, _y + 32 - border * 2,1.0f,1.0f,1.0f,0.2);
    }

    // ** Timers
    //m_WindowKeyCrackTool.RenderText(20,60,0.9,0.0,0.0,HED_FONT_NORMAL,utilFormatTime(m_KeyCrackTool_Timer));

    // ** Process input
    if(m_Click == 1 && x >= 2 && x <= 6 && y >= 2 && y <= 6 && m_RetinalScanTool_Failed == 0 && m_RetinalScanTool_Matrix[x - 2][y - 2] >= 0)
    {
        if(m_RetinalScanTool_Matrix[x - 2][y - 2] == m_RetinalScanTool_Next)
        {
            m_RetinalScanTool_Next--;
            m_RetinalScanTool_Matrix[x - 2][y - 2] = -m_RetinalScanTool_Matrix[x - 2][y - 2];
        }
        else
        {
            _DisplayErrorMsg("Retinal scan bypass failed.");

			// UPDATE 1
			// On EASY and MEDIUM mode, allow the player to continue using the tool even if he failed to check the correct number.
			if(g_PlayerDifficulty > 1)
				m_RetinalScanTool_Failed = 1;
			else
				g_SoundManager.PlaySoundFX("hed-audio/soundfx-voice-error.wav");
        }
    }
    if(m_WindowRetinalScanTool.ButtonClicked(TOOL_BUTTON_LOGIN) && m_RetinalScanTool_Next == -1)
    {
        m_GameLevel.m_ServerList[m_ServerLocked].m_RetinaPassword = 0;
        m_WindowRetinalScanTool.ToggleVisibility();
        m_GameMode = GAMEMODE_IDLE;

        // Update achievements
        if(m_GameLevel.m_ServerList[m_ServerLocked].isFullyHacked(m_GameLevel.m_ServerList[m_ServerLocal].m_ServerKeys[m_ServerLocked]) == 1) g_GameAchievements.achievementValue[HED_ACHIEVEMENT_SERVERSHACKED]++;
        g_GameAchievements.achievementValue[HED_ACHIEVEMENT_RETINA]++;
    }

    if(m_WindowRetinalScanTool.ButtonClicked(TOOL_BUTTON_CANCEL))
    {
        m_WindowRetinalScanTool.ToggleVisibility();
        m_GameMode = GAMEMODE_IDLE;
    }
}
