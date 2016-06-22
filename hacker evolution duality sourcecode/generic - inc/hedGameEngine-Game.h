/*
Name: hedGameEngine-Game.h
Desc: Hacker Evolution Duality - Gameplay management

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#ifndef _hedGameEngine_Game_h_
#define _hedGameEngine_Game_h_

#include "HackerEvolutionDualityDefines.h"
#include "HackerEvolutionDualityUtil.h"

#include "hedGameWindow.h"
#include "hedGraphicsImage.h"
#include "hedGameEngine-ClassServer.h"
#include "hedGameEngine-ClassTextWindow.h"
#include "hedGameEngine-ClassInterfaceManager.h"

#include "hedGameLevel.h"
#include "hedCommandConsole.h"

class  hedGame;
extern hedGame g_Game;                  // The GAME

//=======================================================================================================================================
// Defines and constants
// Game action speeds
#define GAME_SPEED_EMP          1.5f // Seconds needed for the action to be completed

// Level objective data types
#define OBJECTIVE_TIME          1  // Objective is to finish the level in a given time
#define OBJECTIVE_HACK          2  // Objective is to hack a given server
#define OBJECTIVE_DESTROY       3  // Destroy the structural integrity of a given server

#define OBJECTIVE_STATUS_UNCOMPLETED 1 // Objective status
#define OBJECTIVE_STATUS_COMPLETED   2
#define OBJECTIVE_STATUS_FAILED      3

// Animation constants
#define TIME_BACKGROUNDBAR          30 // Interval in seconds, at which the background bar slides over the map

// ====================================================================================================
// ** Buttons
#define BUTTON_DEFAULT               0

// ** Toolbar buttons
#define TOOLBAR_BUTTON_FIREWALL      0
#define TOOLBAR_BUTTON_DOS           2
#define TOOLBAR_BUTTON_EMP           4
#define TOOLBAR_BUTTON_VOICEPRINT    6
#define TOOLBAR_BUTTON_RETINA        8
#define TOOLBAR_BUTTON_KEYCRACK     10
#define TOOLBAR_BUTTON_INTERFACE    12
#define TOOLBAR_BUTTON_CONSOLE      14

#define TOOLBAR_BUTTON_HARDWARE     16
#define TOOLBAR_BUTTON_MESSAGES     18
#define TOOLBAR_BUTTON_OBJECTIVES   20
#define TOOLBAR_BUTTON_ACHIEVEMENTS 22
#define TOOLBAR_BUTTON_HELP         24
#define TOOLBAR_BUTTON_ESC          26

// ** Voiceprint password tool buttons
#define VPTOOL_BUTTON_LOGIN       0
#define VPTOOL_BUTTON_CANCEL      2

// ** Hardware manager buttons
#define HARDWARE_BUTTON_FIREWALL  0
#define HARDWARE_BUTTON_CPU       2
#define HARDWARE_BUTTON_INTEGRITY 4

#define HARDWARE_ICON             6

// ** Keycrack tool button
#define TOOL_BUTTON_LOGIN         0
#define TOOL_BUTTON_CANCEL        2

// ** System message window button
#define SMWINDOW_BUTTON_YES         0
#define SMWINDOW_BUTTON_NO          2

#define SMCONFIRM_BUTTON_OK         0

// ** Interface tool buttons
#define INTERFACE_BUTTON_LOGOUT      0
#define INTERFACE_BUTTON_TRANSFER    2
#define INTERFACE_BUTTON_UPLOAD      4
#define INTERFACE_BUTTON_DOWNLOAD    6
#define INTERFACE_BUTTON_VIEW        8
#define INTERFACE_TOGGLE_1          10
#define INTERFACE_TOGGLE_2          12
#define INTERFACE_TOGGLE_3          14
#define INTERFACE_CAMERAVIEW        16
#define INTERFACE_BUTTON_DELETE     17

#define INTERFACE_FILE_REMOTE       18
#define INTERFACE_FILE_LOCAL        INTERFACE_FILE_REMOTE + 10 * 2

#define INTERFACE_ACTION_NONE       0
#define INTERFACE_ACTION_TRANSFER_1 1
#define INTERFACE_ACTION_TRANSFER_2 2
#define INTERFACE_ACTION_UPLOAD     3
#define INTERFACE_ACTION_DOWNLOAD   4

class hedGame
{
public:
	// Constructor / Destructor
	hedGame();
	~hedGame();

	// Functions
	void RenderLevelIntro(int dismiss);                                           // Render the level intro
	void RenderLevelEnd(int dismiss);                                             // Render level end screen
	void InitGameElements();                                                      // Initialize permanent elements (ie: not level specific)
	void InitGame();                                                              // Initialize non-permanent elements (ie: level specific)
	int  UpdateGame(float frameTime);                                             // Update game state
	void ProcessInput(int inputX, int inputY, int click, int clickdown, int key); // Process input
	void RenderGame();                                                            // Render the game
    void _DisplayErrorMsg(char* msg,...);                                         // Display an error message in the toolbar

	// Handler functions for the interface buttons
    int  calculatePlayerTraceTime();                      // Calculates the player's tracetime
    int  calculateBounceCPU();                            // Calculates the amount of power received through bouncing
	int  checkActionInProgress();                         // Checks is any game action is in progress
    int  checkServerInBouncedLink(int serverIndex);       // Check if a given server is in the bounced link
	void bouncedLink_Add(int serverIndex);                // Bounced link management
	void bouncedLink_Remove(int serverIndex);
	int  bouncedLink_Present(int serverIndex);
	
	void buttonFuncFirewallAttack();                      // DOS attack button handler
	void buttonFuncDOSAttack();                           // DOS attack button handler
	void buttonFuncEMPAttack();                           // EMP attack button handler
	void buttonFuncVoiceprint();                          // Voiceprint password tool
	void buttonFuncRetina();                              // Retina scan password tool
	void buttonFuncKeyCrack();                            // Key crack tool
	void buttonFuncInterface();                           // Server interface
	void buttonFuncConsole();                             // Console button handler

	void buttonFuncHardware();                            // Hardware manager
	void buttonFuncMessages();                            // Messages
	void buttonFuncObjectives();                          // Objectives button handler

	void buttonFuncAchievements();                        // Achievements button handler
	void buttonFuncHelp();                                // Help button handler

	// In-game actions
	void run_DOSTool();                                   // DOS tool function 
	void run_EMPTool();                                   // EMP tool function
	void run_VoicePrintPasswordTool();                    // Voice print password tool function 
	void run_InterfaceTool();                             // Interface tool
	void run_Messages();                                  // Message handler
    void run_FirewallAttack();                            // Firewall attack handler
    void run_HardwareManager();                           // Hardware manager handler
    void run_KeyCrackTool();                              // Encryption key cracking tool
    void run_RetinalScanTool();                              // Encryption key cracking tool

	// Public variables
	hedGameLevel      m_GameLevel;
	hedCommandConsole m_CommandConsole;

//protected:

	// misc
	float         m_FrameTime;                                   // frame time
	int           m_LeveIntroWindowCreated;                      // Level intro window created?
    int           m_IntegrityWarningLevel;                       // Player's integrity level warning

	// input
	int           m_InputX, m_InputY, m_Click, m_Key;            // Input state

	// world map elements
	float         m_TargetMapScrollDelta;                    
	float         m_TargetMapScrollDeltaLast;

	// gameplay (level) elements
	// =========== LEVEL DATA ============================================================================
	int            m_ServerLocal;                                // Player's server index	
	int            m_LevelFailed;                                // Level failed?

	// =========== ========== ============================================================================
	float          m_SecondsPlayed;                              // Total seconds of gameplay
	int            m_FramesRendered;                             // Total frames rendered

	int            m_ServerLocked;                               // Which server is locked on?

	int            m_GameMode;                                   // Current game mode
	float          m_GameActionProgress;                         // Attack, hack, etc progress...
	float          m_GameActionSpeed;                            // Attack, hack, etc speed...

	int            m_BounceList[GAME_MAXSERVERS];                // Bounced link list
	int            m_BounceCount;                                // Number of servers in the bouncedlink

	int            satelliteX;                                   // Satellite coordinates 
	int	           satelliteY;

    // ** Trace times
    float          m_TimeUntilTrace;                             // Time until the trace starts

	// Text windows
	hedClassTextWindow  m_WindowLevelIntro;                      // Level intro window
	hedImage            m_ImageLevelScreenshot;                  // Level screenshot, for the intro window

	hedClassTextWindow m_WindowConsole;                          // In-game console
	hedClassTextWindow m_WindowObjectives;
	hedClassTextWindow m_WindowToolbar;                          // Small 3 line window in the toolbar
	hedClassTextWindow m_WindowHelp;                             // Help window
	hedClassTextWindow m_WindowAchievements;                     // Achievements window

    hedClassTextWindow m_WindowAIChat;                           // AI Chat window

	// ===========================================================================================================================
	// Game interface elements
	// ** Toolbar at the bottom of the screen
	hedClassTextWindow m_ToolBar;                                // Toolbar at the bottom of the window

	// ** DOS attack tool
	hedClassTextWindow m_WindowDOSTool;                          // DOS attack tool
	int                m_DOSToolBW[4];                           // Bandwidth needed to attack each port
	int                m_DOSToolChart[300];                      // DOS bandwidth chart
	float              m_DOSToolChartIndex;                      // DOS bandwidth chart index
	int                m_DOSToolReady;                           // DOS attack ready and running

	// ** Voiceprint password tool
	hedClassTextWindow m_WindowVoicePrintTool;                   // Voiceprint password hacking tool
	int           	   m_VoicePrintPassword[11];
	float              m_VoicePrintTool_Position1;
	float              m_VoicePrintTool_Position2;

	// ** Interface tool
	hedClassTextWindow m_WindowInterfaceTool;                    // Interface tool
	hedClassTextWindow m_WindowFileView;                         // Fileview window
	int                m_InterfaceAction;                        // Any action in progress?
	float              m_InterfaceActionProgress;                // Progres..
	int                m_InterfaceLFSelected;                    // Selected local file
	int                m_InterfaceRFSelected;                    // Selected remote file

	int                m_InterfaceFWLastSelection;               // Which file we last selected (for viewing) (remote or local)
	int                m_InterfaceFWActive;                      // Fileviewer active?

    // ** Firewall attack tool
    hedClassTextWindow m_WindowFirewallAttack;                   // Firewall attack window
    int                m_FirewallAttackGrid[20][20];             // Firewall attack animation grid
    int                m_FirewallAttackCleared;                  // Number of grid elements cleared
    float              m_FirewallAttackDuration;                 // Attack duration
    float              m_FirewallAttackTimeLeft;                 // Time left until the attack is completed

    // ** Hardware manager tool
    hedClassTextWindow m_WindowHardware;                         // Hardware manager window
    hedString          m_HardwareNotification;

    // ** Keycrack tool
    hedClassTextWindow m_WindowKeyCrackTool;
    int                m_KeyCrackTool_Matrix[8][8];
    int                m_KeyCrackTool_Key[64];
    int                m_KeyCrackTool_Next;
    int                m_KeyCrackTool_Failed;
    float              m_KeyCrackTool_Timer;
    float              m_KeyCrackTool_TimerHelp;

    // ** Retinal scan tool
    hedClassTextWindow m_WindowRetinalScanTool;
    int                m_RetinalScanTool_Matrix[5][5];
    float              m_RetinalScanTool_Timer;
    int                m_RetinalScanTool_Index;
    int                m_RetinalScanTool_Failed;
    int                m_RetinalScanTool_Next;

	// ** Messages window
	hedClassTextWindow m_WindowMessages;                         // Messages window
	hedClassTextWindow m_WindowMessageBody;                      // Message content window

	// ** WINDOW: System message
	hedClassTextWindow m_WindowSystemMessage;                    // System message window
	hedClassTextWindow m_WindowSystemConfirm;                    // Confirm window (with OK button)
    hedClassTextWindow m_WindowNewMessageNotification;           // New message notification window

    hedClassTextWindow m_WindowPaused;                           // PAUSE window
    int                m_GameModeBeforePause;                    // Saving the game mode

	// =========== Interface animations ==================================================================
	int                m_FPSHistory[200];                        // FPS history chart
	int                m_ClickCircleX, m_ClickCircleY;           // Last empty click coordinates (for drawing an animated circle)
	int                m_FlashTrace;                             // Flash the trace level text
	int                m_LastTraceTick;                          // Last trace percentage at which the "tick" sound was played 

	// =========== Various clases ========================================================================
	hedTimedEvents     m_GameEvents;                             // Timed events inside the game
	int                m_Quit;                                   // Set the one when we must quit the game
    int                m_LastTraceAlertDigit;                    // Last digit for which a trace alert has been issued
};

#endif