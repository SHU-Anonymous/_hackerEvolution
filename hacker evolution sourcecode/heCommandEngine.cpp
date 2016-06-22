// heCommandEngine.cpp
// 
// Command management class
//
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "heCommandEngine.h"
#include "heSystem.h"
#include "heLog.h"
#include "exoUtil.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bshAudioDirectX.h"

heCommand*    commandList = 0;
heGameWindow* commandWindow = 0;

// Global status variables

int commandInProgress = -1;
int currentCommand    = -1;

// ==========================================================================
// Download progress
int       download_InProgress  = -1;
int       download_PercentDone = 0;
long      download_StartTime   = 0;
gameFile* download_FilePointer = 0;
int       download_Speed;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Upload progress 
int       upload_InProgress;
int       upload_PercentDone;
long      upload_StartTime;
int       upload_Speed;
gameFile* upload_FilePointer;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Crack progress
int       crack_InProgress  = -1;
int       crack_PercentDone = 0;
long      crack_StartTime   = 0;
int       crack_Speed;
hestring  crack_Password;
hestring  crack_Password2;
int       crack_HostIndex   = -1;
int       crack_ServiceIndex;
int       crack_Tick = 0;
int       crack_LastTick = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Decrypt progress
int       decrypt_InProgress;
int       decrypt_PercentDone;
long      decrypt_StartTime;
int       decrypt_Speed;
int       decrypt_KeySize;
int       decrypt_HostIndex;
int       decrypt_Tick;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Trace
int       trace_InProgress = -1;
long      trace_StartTime  = 0;
int       trace_Length     = 0;
int       trace_HostIndex;
hestring  trace_Msg;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Money transfer progress
int       transfer_InProgress;
int       transfer_PercentDone;
long      transfer_StartTime;
int       transfer_Speed;
int       transfer_Amount;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc. variables
int   gSkipLevel = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Bounced links 
extern int gBouncedLinkCount;
extern int gBouncedLinkHostList[HE_MAX_BOUNCEDLINK];


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Functions for local use
// * Stop ongoing trace
void heCommandEngine_StopTrace()
{
	trace_InProgress = 0;
	strcpy(trace_Msg,"-");
}

void heCommandEngine_StartTrace(long pLength, int pHostIndex)
{
	// * Start tracer
	trace_InProgress   = 1;
	trace_StartTime    = system_TimeMilliSeconds();
	trace_Length       = pLength;
	trace_HostIndex    = pHostIndex;

	// * Adjut speed
	trace_Length = trace_Length * hePlayer.getFirewallMultiplier();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Resets the command engine
void reset_CommandEngine()
{
	commandInProgress = -1;
	currentCommand    = -1;

	download_InProgress = -1;
	crack_InProgress    = -1;
	decrypt_InProgress  = -1;
	trace_InProgress    = -1;

	strcpy(trace_Msg,"-");
}
// ============================================================================================================================
// Register a command
void register_Command(char* commandName, void(*commandHandler)(char* param))
{
	heCommand* newCommand;

	newCommand = new heCommand;
	strcpy(newCommand->commandName,commandName);
	newCommand->nextCommand = commandList;
	newCommand->commandHandler = commandHandler;

	commandList = newCommand;

	// * Add command name to dictionary
	gDictionaryCommands.AddWord(commandName);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Locate command handler
heCommand* getHandler_Command(char* commandName)
{
	heCommand* head;

	head = commandList;

	// Iterate through the list of commands
	while(head)
	{
		if(strcmp(commandName,head->commandName) == 0)
		{
			return head;
		}

		head = head->nextCommand;
	}	

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Execute a command
int exec_Command(char* commandName, char* param)
{
	heCommand* head;
	int        executed = 0;
	int        i = 0;

	head = commandList;

	// Check if we have a window attached
	if(commandWindow == 0)
	{
		gApplicationLog.printf("\n[CRITICAL ERROR] exec_Command() - no display window attached.");
		return 0;
	}

	// * Remove any spaces from the end of the command
	i = strlen(param); i--;
	while(param[i] == ' ')
	{
		param[i] = 0;
		i--;
	}


	// Iterate through the list of commands
	while(head)
	{
		if(strcmp(commandName,head->commandName) == 0)
		{
			executed = 1;
			head->commandHandler(param);
		}

		head = head->nextCommand;
	}

	if(executed == 0)
	{
		commandWindow->printf(LANG_TEXT_CMD_ERR_1,commandName,param);
		commandWindow->printf(LANG_TEXT_CMD_ERR_2);
		return 0;
	}
	return 1;
}

// ============================================================================================================================
// Commands

// ============================================================================================================================
// HELP
void command_Help(char* param)
{
	commandWindow->printf("\n");
	commandWindow->displayFile("he-content/console-help.txt");
}

// ============================================================================================================================
// SCAN
void command_Scan(char* param)
{
	int index,i;

	commandWindow->printf("\n");

	// * No parameter specified
	if(strlen(param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_8);
		commandWindow->printf("\n");
		return;
	}

	// * localhost
	if(strcmp("localhost",param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_9);
		return;
	}

	index = gGameLevel.getHostIndex(param);
	
	if(index == -1)
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_10,param);
		commandWindow->printf("\n");
		return;
	}
	else
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_1,param);
		for(i = 0; i < gGameLevel.hostList[index].serviceCount; i++)
		{			
			if(gGameLevel.hostList[index].serviceList[i].hasPassword == 0)
				commandWindow->printf(LANG_TEXT_CMDSCAN_2,gGameLevel.hostList[index].serviceList[i].port,gGameLevel.hostList[index].serviceList[i].name);
			else
				commandWindow->printf(LANG_TEXT_CMDSCAN_3,gGameLevel.hostList[index].serviceList[i].port,strlen(gGameLevel.hostList[index].serviceList[i].password),gGameLevel.hostList[index].serviceList[i].name);
			
		}

		// * check encryption key
		if(gGameLevel.hostList[index].mEncryptionKey != 0)
			commandWindow->printf(LANG_TEXT_CMDSCAN_4,gGameLevel.hostList[index].mEncryptionKey);
		else
			commandWindow->printf(LANG_TEXT_CMDSCAN_5);

		commandWindow->printf(LANG_TEXT_CMDSCAN_6,gGameLevel.hostList[index].mBounceCount);
		commandWindow->printf(LANG_TEXT_CMDSCAN_7);

        // * :-)
        if(gDinamicDifficulty == 1)
        {
            if(gGameLevel.hostList[index].mVisible == 0)
                hePlayer.mGlobalTrace += 1 * hePlayer.getDinamicPerformanceMultiplier();
        }
        else
        {
            if(gGameLevel.hostList[index].mVisible == 0)
                hePlayer.mGlobalTrace += 1;
        }


		// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
		if(gGameLevel.hostList[index].mVisible == 0)
			gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);

		gGameLevel.hostList[index].mVisible = 1;		
	}
	commandWindow->printf("\n");
}

// ============================================================================================================================
// SCAN
void command_Scan2(char* param)
{
	int index,i;

	commandWindow->printf("\n");

	// * No parameter specified
	if(strlen(param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_8);
		commandWindow->printf("\n");
		return;
	}

	// * localhost
	if(strcmp("localhost",param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_9);
		return;
	}

	index = gGameLevel.getHostIndex(param);
	
	if(index == -1)
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_10,param);
		commandWindow->printf("\n");
		return;
	}
	else
	{
		commandWindow->printf(LANG_TEXT_CMDSCAN_1,param);
		for(i = 0; i < gGameLevel.hostList[index].serviceCount; i++)
		{			
			if(gGameLevel.hostList[index].serviceList[i].hasPassword == 0)
				commandWindow->printf(LANG_TEXT_CMDSCAN_2,gGameLevel.hostList[index].serviceList[i].port,gGameLevel.hostList[index].serviceList[i].name);
			else
				commandWindow->printf(LANG_TEXT_CMDSCAN_3,gGameLevel.hostList[index].serviceList[i].port,strlen(gGameLevel.hostList[index].serviceList[i].password),gGameLevel.hostList[index].serviceList[i].name);
			
		}

		// * check encryption key
		if(gGameLevel.hostList[index].mEncryptionKey != 0)
			commandWindow->printf(LANG_TEXT_CMDSCAN_4,gGameLevel.hostList[index].mEncryptionKey);
		else
			commandWindow->printf(LANG_TEXT_CMDSCAN_5);

		commandWindow->printf(LANG_TEXT_CMDSCAN_6,gGameLevel.hostList[index].mBounceCount);
		commandWindow->printf(LANG_TEXT_CMDSCAN_7);

		// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
		if(gGameLevel.hostList[index].mVisible == 0)
			gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);

		gGameLevel.hostList[index].mVisible = 1;		
	}
	commandWindow->printf("\n");
}
// ============================================================================================================================
// CONNECT
void command_Connect(char* param)
{
	int  index,serviceindex;
	char hostname[256],cport[256],filepath[1024];
	int  port;
	int  i;

	// Disconnect, if we are connected to a server
	command_Logout("inline");

	commandWindow->printf("\n");

	// check if we have a parameter
	if(strlen(param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMD_CON_13);
		return;
	}

	// This is to default to port 80, if no port is specified
	strcat(param," 80");

	sscanf(param,"%s %s",hostname,cport);
	
	port = atoi(cport);

	// * Connecting to localhost?
	if(strcmp(hostname,"localhost") == 0)
	{
		commandWindow->printf(LANG_TEXT_CMD_CON_10);
		return;
	}

	index = gGameLevel.getHostIndex(hostname);
	
	if(index == -1)
	{
		commandWindow->printf(LANG_TEXT_CMD_CON_15,hostname,port);
		commandWindow->printf("\n");
		return;
	}

	if(gGameLevel.hostList[index].mEncryptionKey != 0)
	{
        // * Sound fx
        directXAudio_playSoundFX("he-soundfx/voice-access-denied.wav");

		commandWindow->printf(LANG_TEXT_CMD_CON_7);
		commandWindow->printf(LANG_TEXT_CMD_CON_8);
		return;
	}

    // * Make sure we don't have a crack/decrypt in progress
    if(decrypt_InProgress == 1)
    {
		commandWindow->printf(LANG_TEXT_CMD_CON_14);
		return;
    }
    if(crack_InProgress == 1)
    {
		commandWindow->printf(LANG_TEXT_CMD_CON_11);
		return;
    }

	// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
	if(gGameLevel.hostList[index].mVisible == 0)
		gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);
	gGameLevel.hostList[index].mVisible = 1;

	// * make sure the host isn't in the bounced link list
	if(bouncedLink_IsHost(index) == 1)
	{
		commandWindow->printf(LANG_TEXT_CMD_CON_1);
		commandWindow->printf(LANG_TEXT_CMD_CON_2);
		commandWindow->printf("\n");
		return;
	}
	else
	{
		if(bouncedLink_GetTraceMultiplier() != 0)
			commandWindow->printf(LANG_TEXT_CMD_CON_3, bouncedLink_GetTraceMultiplier() - 1);
	}
	// * done

	commandWindow->printf(LANG_TEXT_CMD_CON_4,hostname,port);

	// Check if there is a service running on the selected port
	serviceindex = gGameLevel.hostList[index].isService(port);
	if(serviceindex == -1)
	{
		commandWindow->printf(LANG_TEXT_CMD_CON_5,port);
		commandWindow->printf(LANG_TEXT_CMD_CON_6,hostname);
	}
	else
	{
		// Check if password is needed
		if(gGameLevel.hostList[index].serviceList[serviceindex].hasPassword == 1)
		{
            // * Sound fx
            directXAudio_playSoundFX("he-soundfx/voice-access-denied.wav");
			commandWindow->printf(LANG_TEXT_CMD_CON_9);
		}
		else				
		{
            // * Sound fx
            directXAudio_playSoundFX("he-soundfx/voice-connecting.wav");
			commandWindow->printf(LANG_TEXT_CMD_CON_12);

			// Display MOTD for this service, if any.
			sprintf(filepath,"%s%s/motd/%d",createModLevelFilePath(""),gGameLevel.hostList[index].hostName,port);
			commandWindow->printf("\n");
			commandWindow->displayFile(filepath);

			gGameLevel.connected = 1;
			gGameLevel.con_hostindex = index;
			gGameLevel.con_port = port;

			// * Fill in the filenames dictionary
			for(i = 0; i < gGameLevel.hostList[index].fileCount; i++)
            {
                if(gGameLevel.hostList[index].getRulePort(gGameLevel.hostList[index].fileList[i].fileName,port) == 1)
				    gDictionaryFileNames.AddWord(gGameLevel.hostList[index].fileList[i].fileName);
            }
		}		
	}
	commandWindow->printf("\n");
}
// ============================================================================================================================
// LOGOUT
void command_Logout(char* param)
{
	int i;

	if(strcmp(param,"inline") != 0) commandWindow->printf("\n");

	if(gGameLevel.connected == 1)
	{
		commandWindow->printf(LANG_TEXT_CMD_LOGOUT_1,gGameLevel.hostList[gGameLevel.con_hostindex].hostName);
		gGameLevel.connected = 0;

		// Check if there was anything going on
		if(commandInProgress != COMMAND_NONE)
		{
			// Cancel download
			if(currentCommand == COMMAND_DOWNLOAD)
			{
				download_InProgress = 0;			
				commandWindow->printf(LANG_TEXT_CMD_LOGOUT_5,download_FilePointer->fileName);

				currentCommand = COMMAND_NONE;
			}

			// Cancel upload
			if(currentCommand == COMMAND_UPLOAD)
			{
				upload_InProgress = 0;			
				commandWindow->printf(LANG_TEXT_CMD_LOGOUT_4,download_FilePointer->fileName);

				currentCommand = COMMAND_NONE;
			}

			// * Cancel money transfer
			if(currentCommand == COMMAND_TRANSFER)
			{
				transfer_InProgress = 0;			
				commandWindow->printf(LANG_TEXT_CMD_LOGOUT_3);

				currentCommand = COMMAND_NONE;
			}

            // * Sound fx
            directXAudio_playSoundFX("he-soundfx/voice-disconnecting.wav");
		}

		// * If there is a trace in progress, stop it
		heCommandEngine_StopTrace();
	}
	else
	{
		// This is to avoid displaying the message, when connecting from the localhost
		if(strcmp(param,"inline") != 0)
			commandWindow->printf(LANG_TEXT_CMD_LOGOUT_2);
	}
	if(strcmp(param,"inline") != 0) commandWindow->printf("\n");

	// * Reset the filenames dictionary and fill in with the filenames from the localhost
	gDictionaryFileNames.Reset();
	for(i = 0; i < hePlayer.fileCount; i++)
		gDictionaryFileNames.AddWord(hePlayer.fileList[i].fileName);
}
// ============================================================================================================================
// LS
void command_LS(char* param)
{
	gameFile* fileList;
	int       fileCount,i,actualFileCount = 0, rulePort;
	int       local;

	commandWindow->printf("\n");

	if(gGameLevel.connected == 1)
	{
		fileList  = gGameLevel.hostList[gGameLevel.con_hostindex].fileList;			
		fileCount = gGameLevel.hostList[gGameLevel.con_hostindex].fileCount;
		local     = 0;
	}
	else
	{
		fileList  = hePlayer.fileList;
		fileCount = hePlayer.fileCount;
		local     = 1;
	}

	// Display listing of files
	commandWindow->printf(" .");
	commandWindow->printf("\n ..");

	for(i = 0; i < fileCount; i++)
	{
		rulePort = gGameLevel.hostList[gGameLevel.con_hostindex].getRulePort(fileList[i].fileName,gGameLevel.con_port);

		// File rules don't exist on localhost
		if(local == 1) rulePort = 1;

		if(rulePort == 1)
		{
				actualFileCount++;
				commandWindow->printf("\n %-40s | %d Mbytes",fileList[i].fileName,fileList[i].size);
		}
	}
	commandWindow->printf(LANG_TEXT_CMD_LS_1,actualFileCount);
	if(gGameLevel.connected == 0)
	{
		int freeMem = hePlayer.getMemSize() - hePlayer.getMemSizeUsed();
		if(freeMem < 0) freeMem = 0;

		commandWindow->printf(LANG_TEXT_CMD_LS_2,hePlayer.getMemSize(),hePlayer.getMemSizeUsed(),freeMem);
	}
	commandWindow->printf("\n");
}

// ============================================================================================================================
// CAT - displays the content of a file
void command_Cat(char* param)
{
	int fileIndex,rulePort;

	commandWindow->printf("\n");

	// if connected to a host, we display a remote file
	if(gGameLevel.connected == 1)
	{
		fileIndex = gGameLevel.hostList[gGameLevel.con_hostindex].getFileIndex(param);

		// No file?
		if(strlen(param) == 0)
		{
			commandWindow->printf(LANG_TEXT_CMD_CAT_1);
			commandWindow->printf(LANG_TEXT_CMD_CAT_2);
			commandWindow->printf("\n");
			return;
		}

		if(fileIndex == - 1)
		{
			commandWindow->printf(LANG_TEXT_CMD_CAT_3,param);
			commandWindow->printf(LANG_TEXT_CMD_CAT_2);
		}
		else
		{
			// Check if file is not restricted
			rulePort = gGameLevel.hostList[gGameLevel.con_hostindex].getRulePort(param,gGameLevel.con_port);
			if(rulePort != 1)
			{
				commandWindow->printf(LANG_TEXT_CMD_CAT_4,param,gGameLevel.con_port);
				commandWindow->printf(LANG_TEXT_CMD_CAT_5,gGameLevel.con_port);

			}
			else
			{
				commandWindow->printf("\n");
				commandWindow->displayFile(gGameLevel.hostList[gGameLevel.con_hostindex].fileList[fileIndex].filePath);
			}
				
		}
	}
	else
	{
		// No file?
		if(strlen(param) == 0)
		{
			commandWindow->printf(LANG_TEXT_CMD_CAT_1);
			commandWindow->printf(LANG_TEXT_CMD_CAT_2);
			commandWindow->printf("\n");
			return;
		}

		fileIndex = hePlayer.getFileIndex(param);
		if(fileIndex == -1)
		{
			commandWindow->printf(LANG_TEXT_CMD_CAT_3,param);
			commandWindow->printf(LANG_TEXT_CMD_CAT_2);
		}
		else
		{
			commandWindow->printf("\n");
			commandWindow->displayFile(hePlayer.fileList[fileIndex].filePath);
		}
	}
	commandWindow->printf("\n");
}
// ============================================================================================================================
// DOWNLOAD - downloads a file
void command_Download(char* param)
{
	int       fileIndex,rulePort;
	gameFile* file;
	hestring  fileName;

	commandWindow->printf("\n");
    command_Abort("inline");     // * abort any upload/download/etc

	stripSpaces(fileName,param);

	// * Warn player that an ongoing download has been cancelled.
	if(download_InProgress == 1)
	{
		commandWindow->printf(LANG_TEXT_CMDDL_3,download_FilePointer->fileName);
	}

	// make sure we are connected
	if(gGameLevel.connected == 1)
	{
		fileIndex = gGameLevel.hostList[gGameLevel.con_hostindex].getFileIndex(fileName);
		if(fileIndex == - 1)
			commandWindow->printf(LANG_TEXT_MISC_NOSUCHFILE,fileName);
		else
		{
			file = &gGameLevel.hostList[gGameLevel.con_hostindex].fileList[fileIndex];

			// Check if file is not restricted
			rulePort = gGameLevel.hostList[gGameLevel.con_hostindex].getRulePort(fileName,gGameLevel.con_port);
			if(rulePort != 1)
			{
				commandWindow->printf(LANG_TEXT_CMD_CAT_4,fileName,gGameLevel.con_port);
				commandWindow->printf(LANG_TEXT_CMD_CAT_5,gGameLevel.con_port);
			}
			else
			{
				// * Check if the file doesn't already exist on the localhost
				if(hePlayer.getFileIndex(file->fileName) != -1)
				{
					commandWindow->printf(LANG_TEXT_CMDDL_8);
					return;
				}

				// * Make sure there is plenty of space
				if(file->size > hePlayer.getMemSizeFree())
				{
					commandWindow->printf(LANG_TEXT_CMDDL_6);
					commandWindow->printf(LANG_TEXT_CMDDL_7);
					return;
				}
				
				commandWindow->printf(LANG_TEXT_CMDDL_2,fileName,file->size);
				commandWindow->printf(LANG_TEXT_CMDDL_1);
				
				// Setup and start download progress
				commandInProgress = 1;
				currentCommand    = COMMAND_DOWNLOAD;

				download_InProgress  = 1;
				download_PercentDone = 0;
				download_StartTime   = system_TimeMilliSeconds();
				download_FilePointer = file;

				download_Speed       = HE_SPEED_DOWNLOAD * hePlayer.getModemMultiplier();

                // * Sound fx
                directXAudio_playSoundFX("he-soundfx/voice-downloading.wav");
			}
				
		}
	}
	else
	{
			commandWindow->printf(LANG_TEXT_CMDDL_4);
			commandWindow->printf(LANG_TEXT_CMDDL_5);
	}
	commandWindow->printf("\n");
}
// ============================================================================================================================
// UPLOAD - uploads a file
void command_Upload(char* param)
{
	int       fileIndex;
	gameFile* file;
	hestring  fileName;

	commandWindow->printf("\n");
    command_Abort("inline");     // * abort any upload/download/etc

	stripSpaces(fileName,param);

	// * Warn player that an ongoing download has been cancelled.
	if(upload_InProgress == 1)
	{
		commandWindow->printf(LANG_TEXT_CMDUPLOAD_5,upload_FilePointer->fileName);
	}

	// * make sure we are connected
	if(gGameLevel.connected == 1)
	{
		fileIndex = hePlayer.getFileIndex(fileName);

		if(fileIndex == - 1)
			commandWindow->printf(LANG_TEXT_MISC_NOSUCHFILE,fileName);
		else
		{
			file = &hePlayer.fileList[fileIndex]; 

			// * Check if the file doesn't already exist on the remote host
			if(gGameLevel.hostList[gGameLevel.con_hostindex].getFileIndex(file->fileName) != -1)
			{
				commandWindow->printf(LANG_TEXT_CMDUPLOAD_3);
				commandWindow->printf("\n");
				return;
			}
	
			commandWindow->printf(LANG_TEXT_CMDUPLOAD_4,fileName,file->size);
			commandWindow->printf(LANG_TEXT_CMDUPLOAD_1);
				
			// * Setup and start upload progress
			commandInProgress = 1;
			currentCommand    = COMMAND_UPLOAD;

			upload_InProgress  = 1;
			upload_PercentDone = 0;
			upload_StartTime   = system_TimeMilliSeconds();
			upload_FilePointer = file;

			upload_Speed       = HE_SPEED_DOWNLOAD * hePlayer.getModemMultiplier();
		}
				
	}
	else
	{
		commandWindow->printf(LANG_TEXT_CMDUPLOAD_2);
	}
	commandWindow->printf("\n");
}
// ============================================================================================================================
// EXEC
void command_Exec(char* param)
{
	char scriptname[1024];
	char servername[1024];

	commandWindow->printf("\n");
				
	// * Don't allow exploit execution, while connected
	if(gGameLevel.connected == 1)
	{
		commandWindow->printf(LANG_TEXT_CMDEXEC_9);
		return;
	}

	// Check if syntax is valid, there must be 2 parameters
	if(eu_wordcount(param) != 2)
	{
		commandWindow->printf(LANG_TEXT_CMDEXEC_6);
		return;
	}
	else
	{
	}

	gApplicationLog.printf("heCommandEngine::command_Exec() - extracting parameters...");
	sscanf(param,"%s %s",scriptname,servername);
	commandWindow->printf(LANG_TEXT_CMDEXEC_1,scriptname, servername);

	// Check if it's really an exploit that we are executing
	// exploit have a .exploit extension
	if(eu_endswith(scriptname,".exploit") == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDEXEC_13,scriptname);
		return;
	}

	// Check if the exploit exists
	int index;
	index = hePlayer.getFileIndex(scriptname);

	if(index == -1)
	{
		commandWindow->printf(LANG_TEXT_CMDEXEC_8,scriptname);
		return;
	}

	// * Localhost?
	if(strcmp(servername,"localhost") == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDEXEC_12);
		return;
	}

	// Check if the host exists
	if(gGameLevel.getHostIndex(servername) == -1)
	{
		commandWindow->printf("\n Error. Server unreachable : [%s]\n",servername);
		return;
	}

	// Exploit exists, let's execute it
	FILE* file;
	int   e_port;
	char  e_service[1024];
	int   serviceIndex;

	// Read exploit info from file
	file = fopen(hePlayer.fileList[index].filePath,"r");
	fscanf(file,"%d %s",&e_port,e_service);
	fclose(file);

	commandWindow->printf(LANG_TEXT_CMDEXEC_2,e_service,e_port);
	commandWindow->printf(LANG_TEXT_CMDEXEC_3,servername, e_port);

	// Check if the target runs anything on the respective port
	index = gGameLevel.getHostIndex(servername);
	serviceIndex = gGameLevel.hostList[index].isService(e_port);
	if(serviceIndex == -1)
	{
		commandWindow->printf(LANG_TEXT_CMDEXEC_7,e_port,servername);
		return;
	}

	// Check if the exploit is for the respective service
	if(strcmp(gGameLevel.hostList[index].serviceList[serviceIndex].name,e_service) != 0)
	{
		commandWindow->printf(LANG_TEXT_CMDEXEC_10);
		return;
	}

	// Everything is ok, execute exploit
	gGameLevel.hostList[index].serviceList[serviceIndex].hasPassword = 0;
	commandWindow->printf("\n ---");
	commandWindow->printf(LANG_TEXT_CMDEXEC_4);
	commandWindow->printf(LANG_TEXT_CMDEXEC_5,servername,e_port);

	commandWindow->printf("\n");

	// * Update player info
	hePlayer.mScore       += HE_SCORE_EXEC;
	hePlayer.mGlobalTrace += HE_TRACE_EXEC;
	hePlayer.mHackCount++;
}
// ============================================================================================================================
// CLEAR - clears the command console
void command_Clear(char* param)
{
	commandWindow->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRACK
void command_Crack(char* param)
{
	int  index,serviceindex;
	char hostname[256],cport[256];
	int  port;

	commandWindow->printf("\n");
	
	// * Make sure we are not running any commands
	if(commandInProgress != -1)
	{
		commandWindow->printf(LANG_TEXT_CMDCRACK_2,commandInProgress);
		commandWindow->printf("\n");
		return;
	}

	// * Make sure we are not connected
	if(gGameLevel.connected == 1)
	{
		commandWindow->printf(LANG_TEXT_CMDCRACK_3);
		commandWindow->printf("\n");
		return;
	}
	
	// check if we have a parameter
	if(strlen(param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDCRACK_10);
		return;
	}

	// This is to default to port 80, if no port is specified
	strcat(param," 80");

	sscanf(param,"%s %s",hostname,cport);
	
	port = atoi(cport);
	index = gGameLevel.getHostIndex(hostname);
	
	// * make sure the host isn't in the bounced link list
	if(bouncedLink_IsHost(index) == 1)
	{
		commandWindow->printf(LANG_TEXT_CMDDECRYPT_9);
		return;
	}
	else
	{
		if(bouncedLink_GetTraceMultiplier() != 0)
			commandWindow->printf(LANG_TEXT_CMDCRACK_5, bouncedLink_GetTraceMultiplier() - 1);
	}
	// * done
	gApplicationLog.printf("\ncommand_Crack - initiating...");

	if(index == -1)
		commandWindow->printf(LANG_TEXT_CMDCRACK_4,hostname,port);
	else
	{
		// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
		if(gGameLevel.hostList[index].mVisible == 0)
			gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);
		gGameLevel.hostList[index].mVisible = 1;

		commandWindow->printf(LANG_TEXT_CMDCRACK_6,hostname,port);

		// Check if there is a service running on the selected port
		serviceindex = gGameLevel.hostList[index].isService(port);
		if(serviceindex == -1)
		{
			commandWindow->printf(LANG_TEXT_CMDCRACK_7,port);
			commandWindow->printf(LANG_TEXT_CMDCRACK_8,hostname);
		}
		else
		{
			// Check if password is needed
			if(gGameLevel.hostList[index].serviceList[serviceindex].hasPassword == 0)
			{
				commandWindow->printf(LANG_TEXT_CMDCRACK_9);
			}
			else				
			{
				commandWindow->printf(LANG_TEXT_MISC_LAUNCHING);
				commandWindow->printf(LANG_TEXT_CMDCRACK_1);

				// * Start the cracking process
				commandInProgress = 1;
				currentCommand    = COMMAND_CRACK;

				crack_InProgress   = 1;
				crack_PercentDone  = 0;
				crack_StartTime    = system_TimeMilliSeconds();
				crack_HostIndex    = index;
			    crack_ServiceIndex = serviceindex;
				crack_Tick         = 0;
				crack_LastTick     = 0;
				strcpy(crack_Password,gGameLevel.hostList[index].serviceList[serviceindex].password);
				strcpy(crack_Password2,gGameLevel.hostList[index].serviceList[serviceindex].password);

				crack_Speed        = HE_SPEED_CRACK / hePlayer.getCPUMultiplier();

				// * Start tracer
				heCommandEngine_StartTrace(HE_SPEED_TRACE * bouncedLink_GetTraceMultiplier(),index);
				bouncedLink_UpdateHosts();
			}
		}
	}
	commandWindow->printf("\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * DECRYPT - decrypt host encryption key
void command_Decrypt(char* param)
{
	int  index;
	char hostname[256];

	commandWindow->printf("\n");
	
	// * Make sure we are not running any commands
	if(commandInProgress != -1)
	{
		commandWindow->printf(LANG_TEXT_CMDDECRYPT_2,commandInProgress);
		commandWindow->printf("\n");
		return;
	}
 
	// * Make sure we are not connected
	if(gGameLevel.connected == 1)
	{
		commandWindow->printf(LANG_TEXT_CMDDECRYPT_3);
		commandWindow->printf("\n");
		return;
	}

	// check if we have a parameter
	if(strlen(param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDDECRYPT_4);
		commandWindow->printf("\n");
		return;
	}

	sscanf(param,"%s",hostname);	
	index = gGameLevel.getHostIndex(hostname);

	// * make sure the host isn't in the bounced link list
	if(bouncedLink_IsHost(index) == 1)
	{
		commandWindow->printf(LANG_TEXT_CMDDECRYPT_9);
		return;
	}
	else
	{
		if(bouncedLink_GetTraceMultiplier() != 0)
			commandWindow->printf(LANG_TEXT_CMDDECRYPT_5, bouncedLink_GetTraceMultiplier() - 1);
	}
	// * done
	
	if(index == -1)
		commandWindow->printf(LANG_TEXT_CMDDECRYPT_6,hostname);
	else
	{
		// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
		if(gGameLevel.hostList[index].mVisible == 0)
			gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);
	
        gGameLevel.hostList[index].mVisible = 1;

		commandWindow->printf(LANG_TEXT_CMDDECRYPT_7,hostname);

		if(gGameLevel.hostList[index].mEncryptionKey == 0)
			commandWindow->printf(LANG_TEXT_CMDDECRYPT_8);
		else				
		{
			commandWindow->printf(LANG_TEXT_MISC_LAUNCHING);
			commandWindow->printf(LANG_TEXT_CMDDECRYPT_1);

			// * Start the decrypt process
			commandInProgress = 1;
			currentCommand    = COMMAND_DECRYPT;

			decrypt_InProgress   = 1;
			decrypt_PercentDone  = 0;
			decrypt_StartTime    = system_TimeMilliSeconds();
			decrypt_HostIndex    = index;
			decrypt_Tick         = 0;
			decrypt_KeySize      = gGameLevel.hostList[index].mEncryptionKey;
			decrypt_Speed        = HE_SPEED_DECRYPT / hePlayer.getCPUMultiplier();

			// * Start tracer
			heCommandEngine_StartTrace(HE_SPEED_TRACE * bouncedLink_GetTraceMultiplier(),index);
			bouncedLink_UpdateHosts();
		}
	}
	commandWindow->printf("\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * TRANSFER - transfer money from a host
void command_Transfer(char* param)
{
	int       transferOK = 1;
	int       transferAmount;

	commandWindow->printf("\n");
    command_Abort("inline");     // * abort any upload/download/etc

	if(strlen(param) == 0) 
		transferAmount = 0;
	else
		transferAmount = atoi(param);

	commandWindow->printf(LANG_TEXT_CMDXFER_1,transferAmount);

	// * Check all conditions for a transfer
	if(transfer_InProgress == 1)
	{
		transferOK = 0;
		commandWindow->printf(LANG_TEXT_CMDXFER_4);
	} else
	if(gGameLevel.connected == 0)
	{
		transferOK = 0;
		commandWindow->printf(LANG_TEXT_CMDXFER_5);
	} else
	if(gGameLevel.hostList[gGameLevel.con_hostindex].getOpenPortCount() != gGameLevel.hostList[gGameLevel.con_hostindex].serviceCount)
	{
		transferOK = 0;
		commandWindow->printf(LANG_TEXT_CMDXFER_8);
	} else
	if(gGameLevel.hostList[gGameLevel.con_hostindex].mMoney < transferAmount)
	{
		transferOK = 0;
		if(gGameLevel.hostList[gGameLevel.con_hostindex].mMoney == 0)
			commandWindow->printf(LANG_TEXT_CMDXFER_2);
		else
			commandWindow->printf(LANG_TEXT_CMDXFER_7,gGameLevel.hostList[gGameLevel.con_hostindex].mMoney);
	}
	if(transferAmount == 0)
	{
		transferOK = 0;
		commandWindow->printf(LANG_TEXT_CMDXFER_6);
	}

	// * All conditions ok, start the transfer
	if(transferOK == 1)
	{
			commandWindow->printf(LANG_TEXT_CMDXFER_3);
			commandInProgress = 1;
			currentCommand    = COMMAND_TRANSFER;

			transfer_InProgress  = 1;
			transfer_PercentDone = 0;
			transfer_StartTime   = system_TimeMilliSeconds();
			transfer_Speed       = HE_SPEED_TRANSFER * hePlayer.getModemMultiplier();
			transfer_Amount      = transferAmount;

			// * Start tracer
			heCommandEngine_StartTrace(HE_SPEED_TRACE * bouncedLink_GetTraceMultiplier(),gGameLevel.con_hostindex);
			bouncedLink_UpdateHosts();
	}

	commandWindow->printf("\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * ABORT - aborts any download, upload, money transfer, etc
void command_Abort(char* param)
{
	int abort = 0;

	if(strcmp(param,"inline") != 0) commandWindow->printf("\n");
	if(currentCommand == COMMAND_TRANSFER)
	{
		commandWindow->printf(LANG_TEXT_CMD_ABORT_2);
		transfer_InProgress = 0;
		abort               = 1;
	}
	if(currentCommand == COMMAND_DOWNLOAD)
	{
		commandWindow->printf(LANG_TEXT_CMD_ABORT_3);
		download_InProgress = 0;
		abort               = 1;
	}
	if(currentCommand == COMMAND_UPLOAD)
	{
		commandWindow->printf(LANG_TEXT_CMD_ABORT_4);
		upload_InProgress   = 0;
		abort               = 1;
	}
	if(currentCommand == COMMAND_CRACK)
	{
		commandWindow->printf(LANG_TEXT_CMD_ABORT_5);
		crack_InProgress = 0;
		abort            = 1;
	}
	if(currentCommand == COMMAND_DECRYPT)
	{
		commandWindow->printf(LANG_TEXT_CMD_ABORT_6);
		decrypt_InProgress = 0;
		abort              = 1;
	}

	// * Nothing to abort?
	if(abort == 0)
	{
        if(strcmp(param,"inline") != 0)
		    commandWindow->printf(LANG_TEXT_CMD_ABORT_1);
	}
		
	// * Cancel tracer
	trace_InProgress = 0;
	strcpy(trace_Msg,"-");

	currentCommand    = -1;
	commandInProgress = -1;

	strcpy(gGameLevel.statusPanelMessage,"idle");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Display player's hardware configuration
void command_Config(char* param)
{
	int freeMem = hePlayer.getMemSize() - hePlayer.getMemSizeUsed();
	if(freeMem < 0) freeMem = 0;

	commandWindow->printf(LANG_TEXT_CMD_CONFIG_1);
	commandWindow->printf("\n ------------------------------------- ");
		commandWindow->printf(LANG_TEXT_CMD_CONFIG_6,HE_NAME_CPU[hePlayer.cpu1]);

	if(hePlayer.cpu2 != -1)
		commandWindow->printf(LANG_TEXT_CMD_CONFIG_7,HE_NAME_CPU[hePlayer.cpu2]);
	else
		commandWindow->printf(LANG_TEXT_CMD_CONFIG_8);

		commandWindow->printf(LANG_TEXT_CMD_CONFIG_9,HE_NAME_MEM[hePlayer.mem1]);

	if(hePlayer.mem2 != -1)
		commandWindow->printf(LANG_TEXT_CMD_CONFIG_10,HE_NAME_MEM[hePlayer.mem2]);
	else
		commandWindow->printf(LANG_TEXT_CMD_CONFIG_11);

	    commandWindow->printf(LANG_TEXT_CMD_CONFIG_12,HE_NAME_MODEM[hePlayer.modem]);
	    commandWindow->printf(LANG_TEXT_CMD_CONFIG_13,HE_NAME_FIREWALL[hePlayer.firewall]);

	commandWindow->printf("\n ------------------------------------- ");
	commandWindow->printf(LANG_TEXT_CMD_CONFIG_2,hePlayer.getMemSize(),hePlayer.getMemSizeUsed(),freeMem);
	commandWindow->printf("\n ------------------------------------- ");
	// * Print multipliers
	commandWindow->printf(LANG_TEXT_CMD_CONFIG_3,hePlayer.getFirewallMultiplier());
	commandWindow->printf(LANG_TEXT_CMD_CONFIG_4,hePlayer.getModemMultiplier());
	commandWindow->printf(LANG_TEXT_CMD_CONFIG_5,hePlayer.getCPUMultiplier());

	commandWindow->printf("\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Attempts to login into a host, with a given password
void command_Login(char* param)
{
	hestring hostname,password;
	int      hostindex;
	int      i;

	commandWindow->printf("\n");

	if(strlen(param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDLOGIN_4);
		return;
	}

	// * Extract hostname and password
	strcat(param," @");
	sscanf(param,"%s %s",hostname,password);

	// * Verify parameters
	if(strlen(hostname) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDLOGIN_4);
		return;
	}
	if(strlen(password) == 0 || strcmp(password,"@") == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDLOGIN_5);
		return;
	}

	hostindex = gGameLevel.getHostIndex(hostname);
	if(hostindex == -1)
	{
		commandWindow->printf(LANG_TEXT_CMDLOGIN_8,hostname);
		return;
	}

	// * Try password on all ports
	commandWindow->printf(LANG_TEXT_CMDLOGIN_1,password);
	for(i = 0; i < gGameLevel.hostList[hostindex].serviceCount; i++)
	{
		commandWindow->printf(LANG_TEXT_CMDLOGIN_2,gGameLevel.hostList[hostindex].serviceList[i].port,gGameLevel.hostList[hostindex].serviceList[i].name);
		if(gGameLevel.hostList[hostindex].serviceList[i].hasPassword == 1)
		{
			if(strcmp(gGameLevel.hostList[hostindex].serviceList[i].password,password) == 0)
			{
				commandWindow->printf(LANG_TEXT_CMDLOGIN_3);
				gGameLevel.hostList[hostindex].serviceList[i].hasPassword = 0;
			}
			else
				commandWindow->printf(LANG_TEXT_CMDLOGIN_7);
		}
		else
			commandWindow->printf(LANG_TEXT_CMDLOGIN_6);
	}

	// * Add trace
	hePlayer.mGlobalTrace += HE_TRACE_LOGIN;
	gGameLevel.hostList[hostindex].mVisible = 1;		

	// * Done
	commandWindow->printf("\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Upgrade player hardware
void command_Upgrade(char* param)
{
	int i, min,max;

	// * no paramater passed, display usage
	if(strlen(param) == 0)
	{
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_1);
		commandWindow->printf("\n ------------------------------------------- ");

		//////////////////////////////////////////////////////////////////////////////////////////
		// * display available cpu upgrades
		min = hePlayer.cpu2;
		max = hePlayer.cpu1;
		if(hePlayer.cpu1 < hePlayer.cpu2)
		{
			min = hePlayer.cpu1;
			max = hePlayer.cpu2;
		}		
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_2);
		if(min == HE_MAX_CPU - 1) commandWindow->printf(LANG_TEXT_CMDUPGRADE_NONE);
		for(i = min + 1; i < HE_MAX_CPU; i++)
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_3,HE_NAME_CPU[i],HE_CPU_PRICE[i],i);

		//////////////////////////////////////////////////////////////////////////////////////////
		// * display available memory upgrades
		min = hePlayer.mem2;
		max = hePlayer.mem1;
		if(hePlayer.mem1 < hePlayer.mem2)
		{
			min = hePlayer.mem1;
			max = hePlayer.mem2;
		}		
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_4);
		if(min == HE_MAX_MEM - 1) commandWindow->printf(LANG_TEXT_CMDUPGRADE_NONE);
		for(i = min + 1; i < HE_MAX_MEM; i++)
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_5,HE_NAME_MEM[i],HE_MEM_PRICE[i],i);
		
		//////////////////////////////////////////////////////////////////////////////////////////
		// * display available firewall upgrades
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_6);
		if(hePlayer.firewall == HE_MAX_FIREWALL - 1) commandWindow->printf(LANG_TEXT_CMDUPGRADE_NONE);
		for(i = hePlayer.firewall + 1; i < HE_MAX_FIREWALL; i++)
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_7,HE_NAME_FIREWALL[i],HE_FIREWALL_PRICE[i],i);

		//////////////////////////////////////////////////////////////////////////////////////////
		// * display available modem upgrades
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_8);
		if(hePlayer.modem == HE_MAX_MODEM - 1) commandWindow->printf(LANG_TEXT_CMDUPGRADE_NONE);
		for(i = hePlayer.modem + 1; i < HE_MAX_MODEM; i++)
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_9,HE_NAME_MODEM[i],HE_MODEM_PRICE[i],i);

		//////////////////////////////////////////////////////////////////////////////////////////
		// * neural network adapter
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_10);
		if(hePlayer.nnadapter == 0) 
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_NONE);
		else
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_11);

		/////////////////////////////////////////////////////////////////////////////////////////
		commandWindow->printf("\n ------------------------------------------- ");
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_12);
		commandWindow->printf(LANG_TEXT_CMDUPGRADE_13);
	}
	// * upgrade hardware
	else
	{
		int      upgradeType =  0;
		int      upgradeCode = -1;
		hestring upgradeId;

		commandWindow->printf(LANG_TEXT_CMDUPGRADE_14);

		stripSpaces(param,param);

		// * check update code		
		if(strncmp(param,"cpu",3) == 0) upgradeType = 1;
		if(strncmp(param,"mem",3) == 0) upgradeType = 2;
		if(strncmp(param,"fwl",3) == 0) upgradeType = 3;
		if(strncmp(param,"mdm",3) == 0) upgradeType = 4;
		if(strncmp(param,"nna",3) == 0) upgradeType = 5;

		// * Invalid code entered?
		if(upgradeType == 0)
		{
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_27,param);
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_28);
			return;
		}

		// * Check upgrade option
		upgradeId[0] = param[3];
		upgradeId[1] = 0;
		upgradeCode = atoi(upgradeId);

		// * Special case

		if((upgradeId[0] < '0' || upgradeId[0] > '3') && (upgradeType != 5))
		{
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_27,param);
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_28);
			return;
		}

		commandWindow->printf(LANG_TEXT_CMDUPGRADE_15,upgradeType,upgradeCode);

		// * upgrade code is ok.
		// * check if player has enough money
		int moneyOk = 0;
		int moneyAmount;
		if(upgradeType == 1 && HE_CPU_PRICE[upgradeCode] > hePlayer.mMoney) moneyOk = HE_CPU_PRICE[upgradeCode];
		if(upgradeType == 2 && HE_MEM_PRICE[upgradeCode] > hePlayer.mMoney) moneyOk = HE_MEM_PRICE[upgradeCode];
		if(upgradeType == 3 && HE_FIREWALL_PRICE[upgradeCode] > hePlayer.mMoney) moneyOk = HE_FIREWALL_PRICE[upgradeCode];
		if(upgradeType == 4 && HE_MODEM_PRICE[upgradeCode] > hePlayer.mMoney) moneyOk = HE_MODEM_PRICE[upgradeCode];
		if(upgradeType == 5 && 50000 > hePlayer.mMoney) moneyOk = 50000;

		if(upgradeType == 1) moneyAmount = HE_CPU_PRICE[upgradeCode];
		if(upgradeType == 2) moneyAmount = HE_MEM_PRICE[upgradeCode];
		if(upgradeType == 3) moneyAmount = HE_FIREWALL_PRICE[upgradeCode];
		if(upgradeType == 4) moneyAmount = HE_MODEM_PRICE[upgradeCode];
		if(upgradeType == 5) moneyAmount = 50000;

		if(moneyOk > 0)
		{
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_24);
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_25);
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_26,moneyOk);
			return;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// CPU upgrade
		if(upgradeType == 1)
		{
			// * Make sure we are not "downgrading"
			if(upgradeCode <= hePlayer.cpu1 && upgradeCode <= hePlayer.cpu2)
			{
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_32);
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_28);
				return;
			}

			// * Choose slot to update
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_16);
			if(hePlayer.cpu1 < hePlayer.cpu2)
			{
				hePlayer.cpu1 = upgradeCode;
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_17);
			}
			else
			{
				hePlayer.cpu2 = upgradeCode;
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_18);
			}
			hePlayer.mMoney -= moneyAmount;				
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MEM upgrade
		if(upgradeType == 2)
		{
			// * Make sure we are not "downgrading"
			if(upgradeCode <= hePlayer.mem1 && upgradeCode <= hePlayer.mem2)
			{
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_31);
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_28);
				return;
			}

			// * Choose slot to update
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_19);
			if(hePlayer.mem1 < hePlayer.mem2)
			{
				hePlayer.mem1 = upgradeCode;
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_17);
			}
			else
			{
				hePlayer.mem2 = upgradeCode;
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_18);
			}
			hePlayer.mMoney -= moneyAmount;				
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// FIREWALL upgrade
		if(upgradeType == 3)
		{
			// * Make sure we are not "downgrading"
			if(upgradeCode <= hePlayer.firewall)
			{
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_29);
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_28);
				return;
			}

			// * Choose slot to update
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_20);
			hePlayer.firewall = upgradeCode;
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_21);
			hePlayer.mMoney -= moneyAmount;				
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MODEM upgrade
		if(upgradeType == 4)
		{
			// * Make sure we are not "downgrading"
			if(upgradeCode <= hePlayer.modem)
			{
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_30);
				commandWindow->printf(LANG_TEXT_CMDUPGRADE_28);
				return;
			}

			// * Choose slot to update
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_22);
			hePlayer.modem = upgradeCode;
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_21);
			hePlayer.mMoney -= moneyAmount;				
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Neural adapter upgrade
		if(upgradeType == 5)
		{
			// * Make sure we are not "downgrading"

			commandWindow->printf(LANG_TEXT_CMDUPGRADE_23);
			hePlayer.nnadapter = 0;
			commandWindow->printf(LANG_TEXT_CMDUPGRADE_21);
			hePlayer.mMoney -= moneyAmount;				
		}
	}

	commandWindow->printf("\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Help listing for bounced links
void command_BounceHelp(char* param)
{
	commandWindow->printf("\n");
	commandWindow->displayFile("he-content/bounce-help.txt");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Displays information on the bounced link
void command_BounceInfo(char* param)
{
	int i;

	commandWindow->printf(LANG_TEXT_CMD_BINFO_1);	
	if(gBouncedLinkCount == 0)
		commandWindow->printf(LANG_TEXT_CMD_BINFO_2);	
	else
	{
		commandWindow->printf(LANG_TEXT_CMD_BINFO_6,gBouncedLinkCount);	
		for(i = 0; i < gBouncedLinkCount; i++)
			commandWindow->printf(LANG_TEXT_CMD_BINFO_4, gGameLevel.hostList[gBouncedLinkHostList[i]].hostName,gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount);	
	}

	// * Display list of hosts through wich the player can bounce
	commandWindow->printf(LANG_TEXT_CMD_BINFO_3);	
	for(i = 0; i < gGameLevel.hostCount; i++)
	{
		if(gGameLevel.hostList[i].mVisible == 1 && gGameLevel.hostList[i].mBounceCount > 0)
			if(gGameLevel.hostList[i].hackedCompletely() == 1)
				commandWindow->printf(LANG_TEXT_CMD_BINFO_4, gGameLevel.hostList[i].hostName,gGameLevel.hostList[i].mBounceCount);	
			else
				commandWindow->printf(LANG_TEXT_CMD_BINFO_5, gGameLevel.hostList[i].hostName,gGameLevel.hostList[i].mBounceCount);	
	}

	commandWindow->printf("\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Reduces the global trace level
void command_KillTrace(char* param)
{
	if(hePlayer.mGlobalTrace <= 0)
	{
		commandWindow->printf(LANG_TEXT_CMDKILL_9);
		commandWindow->printf(LANG_TEXT_CMDKILL_10);
		return;
	}

	if(hePlayer.mMoney < HE_KILLTRACE_COST)
	{
		commandWindow->printf(LANG_TEXT_CMDKILL_1);
		commandWindow->printf(LANG_TEXT_CMDKILL_2,HE_KILLTRACE_COST);
		return;
	}

	// * Everything is ok, reduce the tracetime
	int newTraceLevel;

	newTraceLevel = hePlayer.mGlobalTrace - 10;
	if(newTraceLevel < 0) newTraceLevel = 0;

   	commandWindow->printf(LANG_TEXT_CMDKILL_3);
   	commandWindow->printf(LANG_TEXT_CMDKILL_4);
   	commandWindow->printf(LANG_TEXT_CMDKILL_5);
   	commandWindow->printf(LANG_TEXT_CMDKILL_6);

	commandWindow->printf(LANG_TEXT_CMDKILL_7,hePlayer.mGlobalTrace,newTraceLevel);
	commandWindow->printf(LANG_TEXT_CMDKILL_8,HE_KILLTRACE_COST);

	hePlayer.mGlobalTrace = newTraceLevel;
	hePlayer.mMoney      -= HE_KILLTRACE_COST;

	commandWindow->printf("\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Delete a file
void command_Delete(char* param)
{
	int i,index;

	commandWindow->printf("\n");
	stripSpaces(param,param);

	if(gGameLevel.connected == 1)
	{
		index = gGameLevel.hostList[gGameLevel.con_hostindex].getFileIndex(param);
		if(index == -1)
		{
			commandWindow->printf(LANG_TEXT_CMDDEL_1,param);
		}
		else
		{
			// * Make sure the file can accesed on the current port
			if(gGameLevel.hostList[gGameLevel.con_hostindex].getRulePort(param,gGameLevel.con_port) != 1)
			{
				commandWindow->printf(LANG_TEXT_CMDDEL_1,param);
				commandWindow->printf(LANG_TEXT_CMDDEL_3,param,gGameLevel.con_port);
			}
			else
			{
				// * Delete file
				for(i = index; i < gGameLevel.hostList[gGameLevel.con_hostindex].fileCount - 1; i++)
					gGameLevel.hostList[gGameLevel.con_hostindex].fileList[i] = gGameLevel.hostList[gGameLevel.con_hostindex].fileList[i + 1];
				gGameLevel.hostList[gGameLevel.con_hostindex].fileCount--;
				commandWindow->printf(LANG_TEXT_CMDDEL_2,param);
			}
		}
	}
	else
	{
		index = hePlayer.getFileIndex(param);
		if(index == -1)
		{
			commandWindow->printf(LANG_TEXT_CMDDEL_1,param);
		}
		else
		{
			// * Delete file
			for(i = index; i < hePlayer.fileCount - 1; i++)
				hePlayer.fileList[i] = hePlayer.fileList[i + 1];
			hePlayer.fileCount--;
			commandWindow->printf(LANG_TEXT_CMDDEL_2,param);
		}
	}

	commandWindow->printf("\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Downgrade (player can sell all his upgrades and recover the money)
void command_Downgrade(char* param)
{
	commandWindow->printf("\n");

    if(strcmp(param,"all") == 0)
    {
        int money = 0;

	    commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_4);
     
        money += HE_CPU_PRICE[hePlayer.cpu1];
        money += HE_MEM_PRICE[hePlayer.mem1];
        money += HE_FIREWALL_PRICE[hePlayer.firewall];
        money += HE_MODEM_PRICE[hePlayer.modem];
        if(hePlayer.cpu2 != -1) money += HE_CPU_PRICE[hePlayer.cpu2];
        if(hePlayer.mem2 != -1) money += HE_MEM_PRICE[hePlayer.mem2];
        if(hePlayer.nnadapter != -1) money += 50000;

        // * Check if player has the basic configuration
        if(hePlayer.cpu1     == 0 && 
           hePlayer.cpu2     == -1 && 
           hePlayer.mem1     == 0 && 
           hePlayer.mem2     == -1 && 
           hePlayer.modem    == 0 &&
           hePlayer.firewall == 0 &&
           hePlayer.nnadapter == -1)
        {
            commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_7);
            money = 0;
        }


        if(money == 0)
            commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_8);
        else
        {   
            // * Substract basic hardware cost
            money -= 6000;

            commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_5,money);
            money = money * 0.9;
            commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_6, money);

        	hePlayer.cpu1      = 0; 
	        hePlayer.cpu2      = -1;
	        hePlayer.mem1      = 0;
	        hePlayer.mem2      = -1;
	        hePlayer.modem     = 0;
	        hePlayer.firewall  = 0;
	        hePlayer.nnadapter = -1;

            hePlayer.mMoney += money;

        }

    }
    else
    {
	    commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_1);
	    commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_2);
	    commandWindow->printf(LANG_TEXT_CMDDOWNGRADE_3);
    }

	commandWindow->printf("\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Add / Remove a host from the bounced link
void command_Bounce(char* param)
{
	commandWindow->printf("\n");
    if(strlen(param) == 0)
    {
		commandWindow->printf(LANG_TEXT_CMD_BOUNCE_1);
		commandWindow->printf(LANG_TEXT_CMD_BOUNCE_2);
    }
    else
    {
        int newHostIndex;
        int i,found;

        newHostIndex = gGameLevel.getHostIndex(param);
        found        = -1;

        if(newHostIndex == -1)
        {
            commandWindow->printf(LANG_TEXT_BOUNCE_3,param);
        }
        else
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
					directXAudio_playSoundFX("he-soundfx/hesfx-tick.wav");
					commandWindow->printf(LANG_TEXT_CMD_BOUNCE_3,gGameLevel.hostList[newHostIndex].hostName);
					setHelpToolTip(LANG_TEXT_TIPBOUNCE);
				}

				// * Check if we can still bounce through this host
				if(gGameLevel.hostList[newHostIndex].mBounceCount == 0)
				{
					commandWindow->printf(LANG_TEXT_BOUNCE_4,gGameLevel.hostList[newHostIndex].hostName);
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
				directXAudio_playSoundFX("he-soundfx/hesfx-tick.wav");
				commandWindow->printf(LANG_TEXT_CMD_BOUNCE_4,gGameLevel.hostList[newHostIndex].hostName);
			}
        }
    }
	commandWindow->printf("\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Skip current level (mark it as completed)
void command_Cheat_SkipLevel(char* param)
{
	gSkipLevel = 1;

	// * Reset score, when using cheats
	commandWindow->printf(LANG_TEXT_MISC_CHEAT);
	hePlayer.mScore = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Money cheat. Add $10000 to player's balance
void command_Cheat_Money(char* param)
{
	hePlayer.mMoney += 10000;

	// * Reset score, when using cheats
	commandWindow->printf(LANG_TEXT_MISC_CHEAT);
	hePlayer.mScore = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Shows all hosts on the targetmap
void command_Cheat_ShowHosts(char* param)
{
	int i;

	for(i = 0; i < gGameLevel.hostCount; i++)
		gGameLevel.hostList[i].mVisible = 1;

	// * Reset score, when using cheats
	commandWindow->printf(LANG_TEXT_MISC_CHEAT);
	hePlayer.mScore = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Easter egg. Gotta have one in the game :-)
void command_EasterEgg_exosyphen(char* param)
{
	commandWindow->printf(LANG_TEXT_CMD_EXOSYPHEN);
}
