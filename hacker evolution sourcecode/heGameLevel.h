// heGameLevel.h
// 
// Game level management class
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _heGameLevel_h_
#define _heGameLevel_h_

#ifndef HE_EDITOR_BUILD
#include "moduleRender.h"
#include "heGameWindow.h"
#include "hePlayerInfo.h"
#endif
#include "HackerEvolutionDefines.h"

// ===============================================================
// Services running on a host
typedef struct
{
	int  port;
	char name[512];

	int  hasPassword;
	char password[64];
}hostService;

// ===============================================================
// File availability rules
// These rules restrict access to certain files, depending on the port number on which the connection occurs
typedef struct
{
	int  port;
	char fileName[512];
}fileRule;

// ===============================================================
// Host management class
class heHost
{
public:
	void LoadFromFile(char* pHostName, 
		              char* pGameModName, char* pGameLevelName); // load host info from file
	int  isService(int port);                                    // returns the index of a service running on the give port, if any
	int  getRulePort(char* name, int port);                      // check if there is a rule for a certain file
	int  getFileIndex(char* fileName);                           // return the index of a certain file

	//* misc functions
	int  getOpenPortCount();

	// * returns 1, if all services and the encryption key, have been hacked
	int  hackedCompletely();

public:

	char hostName[1024];      // host name (ex : server.com)
	int  mapx, mapy;          // target map coordinates

	int          serviceCount; // services running on the host
	hostService *serviceList;

	gameFile fileList[MAX_SERVER_FILES];  // Files on server
	int      fileCount;

	fileRule ruleList[MAX_SERVER_FILES];  // File access rules
	int      ruleCount;

	// * Misc host variables
	int mMoney;         // Amount of money on server
	int mEncryptionKey; // Encryption key for the host

	// * Member variables
	int mVisible;      // Host visibility on the targetmap
	int mBounceCount;  // Number of times the player can bounce through this host

protected:
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Conditions wich must be met, for a message to be displayed
#define MESSAGE_CONDITION_TIMED            1
#define MESSAGE_CONDITION_HOSTHACKED       2
#define MESSAGE_CONDITION_HOSTVISIBLE      3
#define MESSAGE_CONDITION_MONEY            4
#define MESSAGE_CONDITION_PLAYERFILE       5
#define MESSAGE_CONDITION_HOSTFILE         6
#define MESSAGE_CONDITION_HOSTFILEDELETED  7

class GameLevelMessage
{
public:
	char mMessageContentPath[1024];       // Path to file, containing message text
	
	int  mMessageCondition;               // Condition for displaying the message
	int  mMinute,mSecond;                 // Minute and second, when message is displayed
	char mFilename[128];                  // Filename for condition
	char mHostName[128];                  // Hostname for condition

	int  mReceived;                       // Message has been received by the player
protected:
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Level objectives
// * Objective types
#define HE_MAX_LEVELOBJECTIVES    10
#define HE_LEVELOBJECTIVE_GETFILE      1
#define HE_LEVELOBJECTIVE_MONEY        2
#define HE_LEVELOBJECTIVE_HACKHOST     3
#define HE_LEVELOBJECTIVE_PUTFILE      4
#define HE_LEVELOBJECTIVE_HOSTVISIBLE  5
#define HE_LEVELOBJECTIVE_TRACELEVEL   6
#define HE_LEVELOBJECTIVE_DELETEFILE   7

class heLevelObjective
{
public:
	char mHostName[1024];
	char mFileName[1024];
	int  mType;

	char mDescription1[1024];
	char mDescription2[1024];

	int  mCompleted;
protected:
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc defines
#define HE_TARGETMAP_XOFFS 10
#define HE_TARGETMAP_YOFFS 30


// ===============================================================
// Level management class
class heGameLevel
{
public:
	// misc
	int  getHostIndex(char* hostName);

	// functions
#ifndef HE_EDITOR_BUILD
	void drawTargetMap(int x, int y);
	void drawHosts(int x,int y);
	void drawHostToolTip(int pMouseX, int pMouseY);
	void drawTraceProgress(int pTracePercent, int pHostIndex, int x, int y,int pLastBounceHost);

	int  mouseOverHost(int pMouseX, int pMouseY);
	void drawBouncedLink(int x, int y, int pCount, int pList[]);
#endif
	int     hostCount;              // number of hosts in the current level
	heHost* hostList;               // list of hosts in the level


	// Connected to a host?
	int connected;
	int con_hostindex;
	int con_port;

	// misc
	char statusPanelMessage[1024]; // status panel message

	int  woffset_x;                // horizontal window offset
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level information
	void  SetLevelInfo(char* pMmodName, char* pLevelName);            // Set mod and level name
	void  GetLevelTimeText(char* pString);                            // Returns the elapsed level time, in text format
	long  GetLevelTimeSeconds();                                      // Returns the elapsed level time, in seconds

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Level manipulation
	void LoadFromFile();                                              // Load level from file
	void AddLevelTime(long pDeltaTime);                               // Update game level time

	//  * These methods are skipped, when this file is linked in the game editor
#ifndef HE_EDITOR_BUILD
	void UpdateMessages(heImage *pImage);                             // Updates in-level messages
	void DisplayMessage(heGameWindow* pWindow, int pMessageIndex);    // Display a message in a window
	void NextMessage();                                               // Jump to the next active message
	void PrevMessage();                                               // Jump to the previous active message
	void AttachToolTipImage(heImage* pImage);

	int  UpdateLevelObjectives(heGameWindow* pWindow);                // Update and display level objectives

	int  getLastCompletedObjective();                                 // Returns the last completed objective
	int  mLastCompletedObjective;
	int  mLastCompletedObjectiveList[HE_MAX_LEVELOBJECTIVES];
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// * Target map interaction
	int mTargetMapX, mTargetMapY;
	int clickInTargetMap(int pX, int pY);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Game messages
	GameLevelMessage mMessageList[200];       // This holds all messages from a game level
	GameLevelMessage mActiveMessageList[200]; // Messages that are active (have been received by player)
	int              mMessageCount;           // Number of messages in level
	int              mActiveMessageCount;     // Number of messages that are active
	int              mActiveMessageIndex;     // Index of the currently active message

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Current level info
	char             mGameModName[1024];   // Current game mod name
	char             mGameLevelName[1024]; // Current game level name
	char             mLevelPath[2048];     // The path to the current level files	
	long             mGameLevelTime;       // Total time elapsed since the level was started (in milliseconds)

	// * public variables
#ifndef HE_EDITOR_BUILD
	hePlayerInfo*    mPlayerInfo;          // player info class
#endif

protected:

	// * member variables
#ifndef HE_EDITOR_BUILD
	heImage  mTargetmapImage;              // targetmap image
	heImage* mToolTipImage;                // target tooltip image
	int      mLastToolTipX,mLastToolTipY;  // tooltip coordinates
	int      mLastToolTipHost;             // host for wich we display the tooltip
#endif

	int     player_mapx, player_mapy;      // player's server coordinates on the world map

	// * level objectives
	heLevelObjective mLevelObjectiveList[HE_MAX_LEVELOBJECTIVES];
	int              mLevelObjectiveCount;
};


#endif