/*
Name: hedGameEngine-ClassServer.h
Desc: Hacker Evolution Duality - Gameplay: server class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#ifndef _hedGameEngine_ClassServer_h_
#define _hedGameEngine_ClassServer_h_

#include "HackerEvolutionDualityDefines.h"
#include "hedGameEngine-ClassInterfaceManager.h"
#include "hedGameEngine-ClassTextWindow.h"

//=======================================================================================================================================
// Constants and defines
#define SERVER_LOCK_TIME       0.8f   // seconds needed until server lock animation is completed

//=======================================================================================================================================
// Class definition
class hedClassServer;
class hedClassServer
{
public:
	// Constructor / Destructor
	hedClassServer();
	~hedClassServer();

	void Init();																		   // Initialize
	void SetCoordinateDeltas(int deltaX, int deltaY);									   // Set the coordinate deltas
	int  IsNamed(char* hostName);														   // Check server name
	void ClearLock();																	   // Clear the lock from this server
	void Update(float frameTime, hedClassTextWindow* window, int gameMode);				   // Update server state
	int  ProcessInput(int inputX, int inputY, int click);								   // Process input (input is adjusted to reflect the mouse position on the map), return 1 if a LOCK just happened
	void Draw(int playerKey);    														   // Draw the server on the map
	
	int  isFullyHacked(int serverKey);   												   // Returns true if this server is fully hacked

	void SetCoordinates(int x, int y){ m_MapX = x; m_MapY = y;}							   // Set server coordinates
	int  getScreenX(){ return (m_MapX + m_DeltaX);}										   // Get absolute screen coordinates
	int  getScreenY(){ return (m_MapY + m_DeltaY);}										   // Get absolute screen coordinates
	int  filePresent(char* fileName, char* filePath);                                      // Check if a file is present on this server
	int  fileIndex(char* fileName);                                                        // Returns the index of a file on the server
	void fileAdd(char* fileName, char* filePath);                                          // Add a file to this server

	// ** AI functions
	void AI_IncreaseHostileLevel(int index, float delta);	       // Increase AI hostile level
	int  AI_SendAttack(int selfIndex, hedClassServer* serverList, int count, hedClassTextWindow* chat, int AIPaused); // This is called every frame to initiate attacks from this server to another server

	// Predefined values (these are loaded from the game level files)
	// protected: int   m_MapX, m_MapY;                // Server coordinates on the map
	char  m_HostName[128];               // Server hostname
    int   m_Orientation;                 // In which direction is the server info rendered on the map
	int   m_AIControlled;                // Is this server AI controled?
	int   m_Visible;                     // Server visible on the target map?
	int   m_LevelIntegrity;              // Server integrity level
	int   m_LevelFirewall;               // Server firewall level
	int   m_LevelCPUPower;               // Server cpu power
	int   m_VoiceprintPassword;          // Voiceprint password present?
	int   m_RetinaPassword;              // Retina scan password?
	int   m_EncryptionKey;               // Encryption key present?
	int   m_Money;                       // Amount of money on the server
	int   m_SwitchType[3];               // Switch states
	int   m_SwitchState[3];              // Switch states
	hedString m_SwitchName[3];           // Switch names
	int   m_FileCount;                   // Number of files on the server
	hedString m_FileName[GAME_MAXFILES]; // File names
	hedString m_FilePath[GAME_MAXFILES]; // Realtive path to each file

	int       m_CameraImagePresent;      // Camera image present?
	hedImage* m_CameraImage;             // Camera image

    int       m_OpenWorld;               // Open world game?

	char   m_BypassVoicePrint[128];      // Voiceprint authentification bypass password
	char   m_BypassRetinaScan[128];      // Retina scan authentification bypass password
	char   m_BypassEncryptionKey[128];   // Encryption key authentification bypass password

	// AI variables
	float m_AIHostile;                   // Hostile level (0 to 100)
	float m_AIHostileCountdown;          // Countdown until the next attack

	float     m_AIAttackProgress;                       // AI attack progress percentage (1 .. 100)                            
	int       m_AIAttackDestination;                    // AI attack destination
    hedString m_AINickname;

    int       m_AIGotMoney;              // True, after the first money transfer
    float     m_AIWaitSpendMoney;        //
    float     m_AIWaitRespawn;          

    int       m_AIStatFirewall;          // Attack statistics
    int       m_AIStatDOS;
    int       m_AIStatEMP;
    int       m_AIStatScore;

    int       m_AIStatSpeed;             // These are calculated during the game
    int       m_AIStatAttacks;

	// Dynamic variables (these are created/adjusted during gameplay)
	// ** Firewall encryption key
	int   m_ServerKey;                   // Key for this server's firewall
	int   m_ServerKeys[GAME_MAXSERVERS]; // Keys for other server's firewalls (obtained when a firewall is cracked)
	float m_ServerKeyRegenerationTime;   // Time until the server key is regenerated (after a hack)

	// ** VoicePrint password
	int   m_VoicePrintPassword_Pattern[11]; // Voiceprint password pattern
	int   m_VoicePrintPassword_Speed;       // Voiceprint password speed
	int   m_VoicePrintPassword_Length;      // Voiceprint password length
	int   m_VoicePrintPassword_Amplitude;   // Voiceprint password amplitude

	// Public variables
	long  m_DoubleClick;                 // 1 if there was a doubleclick on this server
	float m_lat,m_long;                  // Geographical latitude and longitude
	float m_latAnimated,m_longAnimated;  // Geographical latitude and longitude (animated text)


	float m_ToolEMPLevel;              // EMP level (1..100)
	float m_ToolDOSLevel;              // DOS level (1..100)

	float m_TraceLevel;                // Trace level
	int   m_IsLocalHost;               // Is this the player's localhost?
    
    hedString m_HostNameAnimated;      // Animated characters in the hostname
    float     m_HostNameAnimationTime;
    
	// Statistics
	int   statHackCount;               // Statistics (Hack/EMP/DOS count, etc)
	int   statEMPCount;
	int   statDOSCount;
    int   statScore;             
    int   _statScore;

	// Server state (other)
	int   m_Lock;                      // Do we have a lock on this server?

	int   m_MapX, m_MapY;              // Server coordinates on the map
protected:

	float m_FrameTime;
	int   m_LockJustHappened;          // Do we have a lock on this server (this frame)?
	long  m_ClickTime[2];              // Time at which the last 2 clicks occured (to detect double-clicks)

	// Animation and effects
	float m_FXHighlightSize;           // Mouse over highlight size
	int   m_MouseOverSound;            // Mouse over sound being played

	// ** Rendering related
	int   m_DeltaX, m_DeltaY;          // Offset on screen, when rendering
};

#endif