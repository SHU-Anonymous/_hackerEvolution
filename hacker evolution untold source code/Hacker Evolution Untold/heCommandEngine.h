// heCommandEngine.h
// 
// Command management class
//
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _heCommandEngine_h_
#define _heCommandEngine_h_

#include "heGameWindow.h"
#include "heGameLevel.h"
#include "hePlayerInfo.h"
#include "heKeyStack.h"

#include "heEngine.h"

class heCommand;
extern heCommand*    commandList;
extern heGameWindow* commandWindow;
extern heGameLevel   gGameLevel; // this references the class in heEngine.cpp
extern hePlayerInfo  hePlayer;   // this references the class in heEngine.cpp

// Global status variables
#define COMMAND_NONE     -1
#define COMMAND_DOWNLOAD 1
#define COMMAND_CRACK    2
#define COMMAND_DECRYPT  3
#define COMMAND_TRANSFER 4
#define COMMAND_UPLOAD   5

extern int commandInProgress;
extern int currentCommand;

// ==========================================================================
// Download progress
extern int       download_InProgress;
extern int       download_PercentDone;
extern long      download_StartTime;
extern int       download_Speed;
extern gameFile* download_FilePointer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Upload progress 
extern int       upload_InProgress;
extern int       upload_PercentDone;
extern long      upload_StartTime;
extern int       upload_Speed;
extern gameFile* upload_FilePointer;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Crack progress
extern int       crack_InProgress;
extern int       crack_PercentDone;
extern long      crack_StartTime;
extern int       crack_Speed;
extern hestring  crack_Password;
extern hestring  crack_Password2;
extern int       crack_HostIndex;
extern int       crack_ServiceIndex;
extern int       crack_Tick;
extern int       crack_LastTick;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Decrypt progress
extern int       decrypt_InProgress;
extern int       decrypt_PercentDone;
extern long      decrypt_StartTime;
extern int       decrypt_Speed;
extern int       decrypt_KeySize;
extern int       decrypt_HostIndex;
extern int       decrypt_Tick;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Money transfer progress
extern int       transfer_InProgress;
extern int       transfer_PercentDone;
extern long      transfer_StartTime;
extern int       transfer_Speed;
extern int       transfer_Amount;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Trace
extern int       trace_InProgress;
extern long      trace_StartTime;
extern int       trace_Length;
extern int       trace_HostIndex;
extern hestring  trace_Msg;

class heCommand
{
public:
	heCommand* nextCommand;
	void       (*commandHandler)(char* param);
	char       commandName[256];
protected:
};

// ============================================================================================================================
// Command management functions
void       reset_CommandEngine();
void       register_Command(char* commandName, void(*commandHandler)(char* param));
heCommand* getHandler_Command(char* commandName);
int        exec_Command(char* commandName, char* param);

// ============================================================================================================================
// Commands
void command_Help(char* param);
void command_Scan(char* param);
void command_Scan2(char* param);
void command_Connect(char* param);
void command_Logout(char* param);
void command_LS(char* param);
void command_Cat(char* param);
void command_Download(char* param);
void command_Upload(char* param);
void command_Exec(char* param);
void command_Clear(char* param);
void command_Crack(char* param);
void command_Decrypt(char* param);
void command_Transfer(char* param);
void command_Abort(char* param);
void command_Config(char* param);
void command_Login(char* param);
void command_Upgrade(char* param);
void command_BounceHelp(char* param);
void command_BounceInfo(char* param);
void command_KillTrace(char* param);
void command_Delete(char* param);
void command_Downgrade(char* param);
void command_Bounce(char* param);

void command_Cheat_SkipLevel(char* param);
void command_Cheat_Money(char* param);
void command_Cheat_ShowHosts(char* param);

void command_EasterEgg_exosyphen(char* param);

void command_SubmitHiscore(char* param);

// New commands introduced in Hacker Evolution Untold
void command_DeleteLogs(char* param);
void command_NextLevel(char* param);
void command_Tutorial(char* param);

#endif