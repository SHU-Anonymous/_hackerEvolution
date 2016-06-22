/*
Name: hedGameLevel.h
Desc: Game level management class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#ifndef _hedGameLevel_h_
#define _hedGameLevel_h_

#include "HackerEvolutionDualityDefines.h"
#include "HackerEvolutionDualityUtil.h"
#include "hedGameEngine-ClassServer.h"
#include "hedGameEngine-ClassTextWindow.h"

// ===================================================================================================================================================================================================
// Constants

// ===================================================================================================================================================================================================
// Level objective data types
#define OBJECTIVE_TIME          1      // Objective is to finish the level in a given time
#define OBJECTIVE_HACK          2      // Objective is to hack a given server
#define OBJECTIVE_DESTROY       3      // Destroy the structural integrity of a given server
#define OBJECTIVE_TRACE         4      // Keep your trace level below a certain threshold
#define OBJECTIVE_MONEY         5      // Obtain a certain amount of money
#define OBJECTIVE_UPLOAD        6      // Upload a file to a server
#define OBJECTIVE_DOWNLOAD      7      // Download a file
#define OBJECTIVE_SWITCH_ON     8      // Turn a switch on
#define OBJECTIVE_SWITCH_OFF    9      // Turn a switch off
#define OBJECTIVE_MONEY_2      10      // Upload a certain amount of money to a server
#define OBJECTIVE_MONEY_3      11      // Obtain a certain amount of money (but you can't spend it)
// UPDATE 1
#define OBJECTIVE_DELETE_FILE  12      // Delete a file from a server

#define OBJECTIVE_OW_KILLALL   101     // Eliminate all servers from the map


#define OBJECTIVE_STATUS_UNCOMPLETED 1 // Objective status
#define OBJECTIVE_STATUS_COMPLETED   2
#define OBJECTIVE_STATUS_FAILED      3

// Switches
#define SWITCH_NONE             0
#define SWITCH_NORMAL           1
#define SWITCH_CAMERA           2
#define SWITCH_TRAP_1           3       // This is a "trap" switch. It increases the player's trace level by SWITCH_TRAP_HIT
    #define SWITCH_TRAP_HIT    10

// Level objective type	  
typedef struct
{
	int       type;                     // Objective type
	char      hostName[256];            // Server hostname
	char      itemName[256];            // Item name
	float     value;                    // Value (ex: timed levels)
	hedString description[2];           // Objective description text

	//--
	int   status;                   // Objective status (uncompleted, completed, failed)  
}hedLevelObjective;

// ===================================================================================================================================================================================================
// Messages 
#define MESSAGE_TIME             1       // Timed messages (will be displayed after a certain amount of time has passed)
#define MESSAGE_SERVER_HACKED    2       // Message is displayed when a server is fully hacked
#define MESSAGE_MONEY            3       // Message is displayed when the player has a certain amount of money
#define MESSAGE_DOWNLOAD         4       // Message is displayed when the player has downloaded a certain file
#define MESSAGE_SWITCH_ON        5       // Message is displayed when a certain switch is ON
#define MESSAGE_UPLOAD           6       // Message is displayed when the player has uploaded a file to a server
#define MESSAGE_SWITCH_OFF       7       // Message is displayed when a certain switch is OFF
// UPDATE 1
#define MESSAGE_DELETE_FILE      8       // Message is displayed when a file has been deleted
#define MESSAGE_SERVER_DESTROYED 9       // Message is displayed when a server has been destroyed

#define MESSAGE_COUNT_1         40      // Number of message condition types. Everything above 40 will show servers on the map instead of messages, based on the same conditions

#define MESSAGE_TYPE_REVEAL_SERVER 40   // If the message type is (40 + MESSAGE_TIME) then a server is revealed instead of a message
#define MESSAGE_TYPE_ADJUSTTRACE   80   // If the message type is (80 + MESSAGE_TIME) then the trace level is adjusted instead of a message (increase OR decrease)

#define MESSAGE_STATUS_NOTRECEIVED 0
#define MESSAGE_STATUS_RECEIVED    1

// Message data type
typedef struct
{
	int       type;                     // Objective type
	char      hostName[256];            // Server hostname
	char      itemName[256];            // Item name
    char      itemName2[256];           // Second item name
	float     value;                    // Value (ex: timed messages)
	hedString fileName;                 // File containing the message

	//--
	int       status;                   // Message status (received/not received)  
	hedString subject;                  // Message subject
}hedLevelMessage;

// ===================================================================================================================================================================================================
// Level management class
class hedGameLevel
{
public:
	hedGameLevel();                                                          // Class constructor
 	~hedGameLevel();                                                         // Class destructor

	void Init();                                                             // Initialize the class (cleans up all variables)
	void SetGameMod(char* modName, int openworld);                           // Sets the current game mod
	void LoadLevel(int levelIndex);                                          // Load a specified level
	void Update(hedClassTextWindow* windowObjectives, 
		        hedClassTextWindow* windowNotification, float frameTimeSec); // Called once every frame, to process all needed updates

	void RenderTargetMap(float targetMapScrollDelta);                        // Render the targetmap

	char* GetFilePath(char* file);                                           // Obtains the path of a file from a level
	float GetMaxMapScrollDelta();                                            // Horizontal map scroll length
	float GetTimedObjective();                                               // Returns the time left to finish the level (if the level has such a feature)
	int   GetServerIndex(char* hostName);                                    // Returns the index of a server
	int   GetServerDNSIndex(char* hostName);                                 // Returns the global DNS index for a server
	int   ObjectivesCompleted();                                             // Returns true if all objectives are completed
	int   ObjectivesFailed();                                                // Returns true if any of the objectives is failed

    // ** Anti cheat
    int   _levelchecksum;

    // ** Final level statistics
    void  UpdateFinalStatistics();                                           // Update final level statistics
    int   m_Stat_FlawlessLevel;                                    

    // ** Display OPEN WORLD statistics
    void  ShowOpenWorldStatics(hedClassTextWindow* window);

    int   m_Stat_DiscoveredAllServers;
    float m_Stat_LevelTime;

	// Statistics
	float m_LevelTime;                                                       // Time since the level has started
	int   m_NewMsgReceived;                                                  // New message notifier6

	// Public variables
	hedImage m_TargetMapImage[2];                                            // Targetmap image (2 layers)

//protected:

	// ===============================================================================================================================================================================================
	// These are common variables with the editor
	// Mod information
	hedString         m_ModName;                                     // Current game mod name
	hedString         m_ModDescription;                              // Mod descriptions string
	hedString         m_LevelList[GAME_MAXLEVELS];                   // Level names (path)
	hedString         m_LevelDescription[GAME_MAXLEVELS];            // Level description
	int               m_LevelCount;                                  // Level count
	int               m_LevelIndex;                                  // Current level index
    int               m_OpenWorld;                                   // Open world game?

	// Servers
	hedClassServer    m_ServerList[GAME_MAXSERVERS];                 // Servers in the game
	int               m_ServerCount;                                 // Number of servers in the current level
	int               m_ServerLocal;                                 // Player's server index
	
	// Objectives
	hedLevelObjective m_ObjectiveList[GAME_MAXOBJECTIVES];           // Level objectives list
	int               m_ObjectiveCount;                              // Level objective count
	int               m_LevelFailed;                                 // Level failed?

	// Messages
	hedLevelMessage   m_MessageList[GAME_MAXMESSAGES];               // Level objectives list
	int               m_MessageCount;                                // Level objective count

	int               m_RMessageList[GAME_MAXMESSAGES];              // Received messages
	int               m_RMessageCount;
	int               m_RMessageIndex;

    // AI settings for this level
    int              m_AISetting_AIAttackTarget;                     // AI attack target: -1 everything in the level. Otherwise it's the target index (0 = localhost, 1 = some other server, so on)
    int              m_AISetting_MoneyDest;                          // Destination (server) where the money goes, when AI players spend it
    int              m_AISetting_MoneySpend;                         // Should the AI controlled servers spend the money the have?
    int              m_AISetting_Deathmatch;                         // Deathmatch mode

	// These variables are not loaded with the level
	int               m_TargetMapImageWidth, m_TargetMapImageHeight; // Targetmap image size

    // Target map fade animation
    int               _targetMapScrollDelta;
    int               mapFadeX[10];
    float             mapFadeAlpha[10];

	int            m_LevelCompleted;                                 // True when level is completed, but player is still playing the current level
    int            m_Quit;                                           // Console is forcing a quit (or advance to the next level)


	// Internal (game) use
	char m_FilePath[1024];
};

#endif