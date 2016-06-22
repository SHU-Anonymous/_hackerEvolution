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
#include "bshAudioDirectX.h"
#include "HackerEvolutionUntold_Defines.h"
#include "HackerEvolutionDefines.h"
#include "heutGameLanguage.h"

heCommand*    commandList = 0;
heGameWindow* commandWindow = 0;

// Global status variables

int commandInProgress     = -1;
int currentCommand        = -1;

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
		commandWindow->printfn(GAMETEXT(" Unknown command: [%s](param: %s)"),commandName,param);
		commandWindow->printfn(GAMETEXT(" Type HELP, to see a list of available commands"));
		commandWindow->printf("\n");
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
	commandWindow->displayFile("heuntold-data/console-help.txt");
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
		commandWindow->printf("%s\n %s\n %s\n",GAMETEXT(" Syntax error. You haven't specified the host you wish to scan."),GAMETEXT(" Usage: scan server"),GAMETEXT(" Example: scan server.com"));
		return;
	}

	// * localhost
	if(strcmp("localhost",param) == 0)
	{
		commandWindow->printf("%s\n",GAMETEXT(" Error: You can't use this command on the localhost"));
		return;
	}

	index = gGameLevel.getHostIndex(param);
	
	if(index == -1)
	{
		commandWindow->printf(GAMETEXT(" Server unreachable: %s"),param);
		commandWindow->printf("\n");
		return;
	}
	else
	{
		commandWindow->printf(GAMETEXT(" scanning: %s, open ports:"),param);
		for(i = 0; i < gGameLevel.hostList[index].serviceCount; i++)
		{			
			if(gGameLevel.hostList[index].serviceList[i].hasPassword == 0)
				commandWindow->printfn(GAMETEXT(" port %4d (unprotected), running: %s"),gGameLevel.hostList[index].serviceList[i].port,gGameLevel.hostList[index].serviceList[i].name);
			else
				commandWindow->printfn(GAMETEXT(" port %4d (password protected: %2d characters), running: %s"),gGameLevel.hostList[index].serviceList[i].port,strlen(gGameLevel.hostList[index].serviceList[i].password),gGameLevel.hostList[index].serviceList[i].name);
			
		}

		// * check encryption key
		if(gGameLevel.hostList[index].mEncryptionKey != 0)
			commandWindow->printfn(GAMETEXT(" encryption key: %d bits"),gGameLevel.hostList[index].mEncryptionKey);
		else
			commandWindow->printfn(GAMETEXT(" encryption key: none"));

		commandWindow->printfn(GAMETEXT(" number of times you can bounce through this server: %d"),gGameLevel.hostList[index].mBounceCount);
		commandWindow->printfn(GAMETEXT(" scan complete."));

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
		commandWindow->printf("%s\n %s\n %s\n",GAMETEXT(" Syntax error. You haven't specified the host you wish to scan."),GAMETEXT(" Usage: scan server"),GAMETEXT(" Example: scan server.com"));
		return;
	}

	// * localhost
	if(strcmp("localhost",param) == 0)
	{
		commandWindow->printf("%s\n",GAMETEXT(" Error: You can't use this command on the localhost"));
		return;
	}

	index = gGameLevel.getHostIndex(param);
	
	if(index == -1)
	{
		commandWindow->printf(GAMETEXT(" Server unreachable: %s"),param);
		commandWindow->printf("\n");
		return;
	}
	else
	{
		commandWindow->printf(GAMETEXT(" scanning: %s, open ports:"),param);
		for(i = 0; i < gGameLevel.hostList[index].serviceCount; i++)
		{			
			if(gGameLevel.hostList[index].serviceList[i].hasPassword == 0)
				commandWindow->printfn(GAMETEXT(" port %4d (unprotected), running: %s"),gGameLevel.hostList[index].serviceList[i].port,gGameLevel.hostList[index].serviceList[i].name);
			else
				commandWindow->printfn(GAMETEXT(" port %4d (password protected: %2d characters), running: %s"),gGameLevel.hostList[index].serviceList[i].port,strlen(gGameLevel.hostList[index].serviceList[i].password),gGameLevel.hostList[index].serviceList[i].name);
			
		}

		// * check encryption key
		if(gGameLevel.hostList[index].mEncryptionKey != 0)
			commandWindow->printfn(GAMETEXT(" encryption key: %d bits"),gGameLevel.hostList[index].mEncryptionKey);
		else
			commandWindow->printfn(GAMETEXT(" encryption key: none"));

		commandWindow->printfn(GAMETEXT(" number of times you can bounce through this server: %d"),gGameLevel.hostList[index].mBounceCount);
		commandWindow->printfn(GAMETEXT(" scan complete."));


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

	// * Add a small delay
	audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
	system_Delay(2);

	// Disconnect, if we are connected to a server
	command_Logout("inline");

	commandWindow->printf("\n");

	// check if we have a parameter
	if(strlen(param) == 0)
	{
		commandWindow->printf(GAMETEXT(" Syntax error. You haven't specified the host you are connecting to."));
		commandWindow->printf("\n");
		return;
	}

	// This is to default to port 80, if no port is specified
	strcat(param," 80");

	sscanf(param,"%s %s",hostname,cport);
	
	port = atoi(cport);

	// * Connecting to localhost?
	if(strcmp(hostname,"localhost") == 0)
	{
		commandWindow->printf(GAMETEXT(" Error: You are already connected to your localhost."));
		commandWindow->printf("\n");
		return;
	}

	index = gGameLevel.getHostIndex(hostname);
	
	if(index == -1)
	{
		commandWindow->printf(GAMETEXT(" Server unreachable: [%s:%d]"),hostname,port);
		commandWindow->printf("\n");
		return;
	}

	if(gGameLevel.hostList[index].mEncryptionKey != 0)
	{
        // * Sound fx
        audio_PlaySoundFX("heuntold-soundfx/voice-access-denied.wav");

		commandWindow->printf(GAMETEXT(" Unable to connect. Server has an encryption key"));
		commandWindow->printfn(GAMETEXT(" You must decrypt the key, using the DECRYPT command"));
		commandWindow->printf("\n");
		return;
	}

    // * Make sure we don't have a crack/decrypt in progress
    if(decrypt_InProgress == 1)
    {
		commandWindow->printf(GAMETEXT(" Unable to connect while the key decrypter is running."));
		commandWindow->printf("\n");
		return;
    }
    if(crack_InProgress == 1)
    {
		commandWindow->printf(GAMETEXT(" Unable to connect while the password cracker is running."));
		commandWindow->printf("\n");
		return;
    }

	// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
	if(gGameLevel.hostList[index].mVisible == 0)
		gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);
	gGameLevel.hostList[index].mVisible = 1;

	// * make sure the host isn't in the bounced link list
	if(bouncedLink_IsHost(index) == 1)
	{
		commandWindow->printf(GAMETEXT(" Unable to connect. Server is added to the bounced link."));
		commandWindow->printfn(GAMETEXT(" Please remove the server from the bounced link, by clicking on it."));
		commandWindow->printf("\n");
		return;
	}
	else
	{
		if(bouncedLink_GetTraceMultiplier() != 0)
		{
			commandWindow->printf(GAMETEXT(" Bouncing link through %d host(s)..."), bouncedLink_GetTraceMultiplier() - 1);
			commandWindow->printf("\n");
		}
	}
	// * done

	commandWindow->printf(GAMETEXT(" connecting to: %s[%d]"),hostname,port);

	// Check if there is a service running on the selected port
	serviceindex = gGameLevel.hostList[index].isService(port);
	if(serviceindex == -1)
	{
		// If the player has entered an invalid port number, find the first
		// port for which the password is hacked and connect through it.
		commandWindow->printfn(GAMETEXT("/ Unable to connect on port %d. There is no service running on that port."),port);
		commandWindow->printfn(GAMETEXT("/ Type: scan %s, to see what services are running on this server."),hostname);
	}
	else
	{
		// Check if password is needed
		if(gGameLevel.hostList[index].serviceList[serviceindex].hasPassword == 1)
		{
            // * Sound fx
            audio_PlaySoundFX("heuntold-soundfx/voice-access-denied.wav");
			commandWindow->printfn(GAMETEXT(" Unable to connect. Access to this service is password protected."));
		}
		else				
		{
            // * Sound fx
            audio_PlaySoundFX("heuntold-soundfx/voice-connecting.wav");
			commandWindow->printf("\n %s\n",GAMETEXT("connected."));
			commandWindow->printf("/%s\n",GAMETEXT(" Type LS to view a list of files on this server/service."));
			commandWindow->printf("/%s",GAMETEXT(" Type CAT FILENAME to see the contents of a file."));
			commandWindow->printfn("");

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

	//if(strcmp(param,"inline") != 0) commandWindow->printf("\n");

	if(gGameLevel.connected == 1)
	{
		commandWindow->printfn(GAMETEXT(" Disconnecting from %s..."),gGameLevel.hostList[gGameLevel.con_hostindex].hostName);
		gGameLevel.connected = 0;

		// Check if there was anything going on
		if(commandInProgress != COMMAND_NONE)
		{
			// Cancel download
			if(currentCommand == COMMAND_DOWNLOAD)
			{
				download_InProgress = 0;			
				commandWindow->printf(GAMETEXT(" Download (%s) canceled."),download_FilePointer->fileName);

				currentCommand = COMMAND_NONE;
			}

			// Cancel upload
			if(currentCommand == COMMAND_UPLOAD)
			{
				upload_InProgress = 0;			
				commandWindow->printfn(GAMETEXT(" Upload (%s) canceled."),download_FilePointer->fileName);

				currentCommand = COMMAND_NONE;
			}

			// * Cancel money transfer
			if(currentCommand == COMMAND_TRANSFER)
			{
				transfer_InProgress = 0;			
				commandWindow->printfn(GAMETEXT(" Money transfer cancelled"));

				currentCommand = COMMAND_NONE;
			}

            // * Sound fx
            audio_PlaySoundFX("heuntold-soundfx/voice-disconnecting.wav");
		}

		// * If there is a trace in progress, stop it
		heCommandEngine_StopTrace();
	}
	else
	{
		// This is to avoid displaying the message, when connecting from the localhost
		if(strcmp(param,"inline") != 0)
			commandWindow->printfn(GAMETEXT(" You are not connected to any server."));
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
	commandWindow->printf("\n\n");
	commandWindow->printf(GAMETEXT(" Total: %d file(s)"),actualFileCount);

	if(gGameLevel.connected == 0)
		commandWindow->printfn(GAMETEXT(" Storage space: %d Mb (Used: %d Mb | Free: %d Mb)"),hePlayer.getMemSize(),hePlayer.getMemSizeUsed(),hePlayer.getMemSize() - hePlayer.getMemSizeUsed());

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
			commandWindow->printf(GAMETEXT(" Error: No file name specified"));
			commandWindow->printf("\n%s\n",GAMETEXT(" Usage: cat filename (example: cat connection.log)"));
			return;
		}

		if(fileIndex == - 1)
		{
			commandWindow->printf(GAMETEXT(" Error. No such file: [%s]"),param);
			commandWindow->printfn(GAMETEXT(" Usage: cat filename (example: cat connection.log)"));
		}
		else
		{
			// Check if file is not restricted
			rulePort = gGameLevel.hostList[gGameLevel.con_hostindex].getRulePort(param,gGameLevel.con_port);
			if(rulePort != 1)
			{
				commandWindow->printf(GAMETEXT(" Access to [%s] is restricted on port %d"),param,gGameLevel.con_port);
				commandWindow->printfn(GAMETEXT(" Please connect on port %d, to access the file."),gGameLevel.con_port);

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
			commandWindow->printf(GAMETEXT(" Error: No file name specified"));
			commandWindow->printfn(GAMETEXT(" Usage: cat filename (example: cat connection.log)"));
			commandWindow->printf("\n");
			return;
		}

		fileIndex = hePlayer.getFileIndex(param);
		if(fileIndex == -1)
		{
			commandWindow->printf(GAMETEXT(" Error. No such file: [%s]"),param);
			commandWindow->printfn(GAMETEXT(" Usage: cat filename (example: cat connection.log)"));
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
		commandWindow->printf(GAMETEXT("WARNING: download aborted (%s)..."),download_FilePointer->fileName);
		commandWindow->printf("\n");
	}

	// make sure we are connected
	if(gGameLevel.connected == 1)
	{
		fileIndex = gGameLevel.hostList[gGameLevel.con_hostindex].getFileIndex(fileName);
		if(fileIndex == - 1)
			commandWindow->printf(GAMETEXT(" Error: No such file: %s"),fileName);
		else
		{
			file = &gGameLevel.hostList[gGameLevel.con_hostindex].fileList[fileIndex];

			// Check if file is not restricted
			rulePort = gGameLevel.hostList[gGameLevel.con_hostindex].getRulePort(fileName,gGameLevel.con_port);
			if(rulePort != 1)
			{
				commandWindow->printf(GAMETEXT(" Access to [%s] is restricted on port %d"),fileName,gGameLevel.con_port);
				commandWindow->printfn(GAMETEXT(" Please connect on port %d, to access the file."),gGameLevel.con_port);
			}
			else
			{
				// * Check if the file doesn't already exist on the localhost
				if(hePlayer.getFileIndex(file->fileName) != -1)
				{
					commandWindow->printf("%s\n",GAMETEXT("Error: file already exists on your localhost"));
					return;
				}

				// * Make sure there is plenty of space
				if(file->size > hePlayer.getMemSizeFree())
				{
					commandWindow->printf(GAMETEXT("Error: There is not enough space to store the file."));
					commandWindow->printfn("%s\n",GAMETEXT("Please upgrade your system memory or delete unnecesary files from your localhost."));
					return;
				}
				
				commandWindow->printf(GAMETEXT(" Downloading %s (%d Mbytes)... please wait."),fileName,file->size);
				commandWindow->printfn(GAMETEXT(" You can type ABORT anytime, to abort the download."));
				
				// Setup and start download progress
				commandInProgress = 1;
				currentCommand    = COMMAND_DOWNLOAD;

				download_InProgress  = 1;
				download_PercentDone = 0;
				download_StartTime   = system_TimeMilliSeconds();
				download_FilePointer = file;

				download_Speed       = HE_SPEED_DOWNLOAD * hePlayer.getModemMultiplier();

                // * Sound fx
                audio_PlaySoundFX("heuntold-soundfx/voice-downloading.wav");
			}
				
		}
	}
	else
	{
			commandWindow->printf(" %s",GAMETEXT("Error: You are not connected to a server."));
			commandWindow->printfn(" %s",GAMETEXT("You must be connected to a server to be able to download files."));
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
		commandWindow->printf(GAMETEXT("WARNING: upload aborted (%s)..."),upload_FilePointer->fileName);
		commandWindow->printf("\n");
	}

	// * make sure we are connected
	if(gGameLevel.connected == 1)
	{
		fileIndex = hePlayer.getFileIndex(fileName);

		if(fileIndex == - 1)
			commandWindow->printf(GAMETEXT(" Error: No such file: %s"),fileName);
		else
		{
			file = &hePlayer.fileList[fileIndex]; 

			// * Check if the file doesn't already exist on the remote host
			if(gGameLevel.hostList[gGameLevel.con_hostindex].getFileIndex(file->fileName) != -1)
			{
				commandWindow->printf("%s\n",GAMETEXT("Error: file already exists on remote host. Unable to upload."));
				return;
			}
	
			commandWindow->printf(GAMETEXT(" Uploading %s (%d Mbytes)... please wait."),fileName,file->size);
			commandWindow->printfn(GAMETEXT(" You can type ABORT anytime, to abort the upload."));
				
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
		commandWindow->printf(" %s\n %s",GAMETEXT("Error: You are not connected to a server."),GAMETEXT("You must be connected to a server to be able to upload files."));
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
		commandWindow->printf("%s\n",GAMETEXT(" Error. Please disconnect before executing an exploit."));
		return;
	}

	// Check if syntax is valid, there must be 2 parameters
	if(eu_wordcount(param) != 2)
	{
		commandWindow->printf(" %s\n %s\n",GAMETEXT("Syntax error"),GAMETEXT("Usage: exploit filename.exploit server"));
		return;
	}
	else
	{
	}

	gApplicationLog.printf("heCommandEngine::command_Exec() - extracting parameters...");
	sscanf(param,"%s %s",scriptname,servername);
	commandWindow->printf(GAMETEXT(" executing [%s][%s]"),scriptname, servername);

	// Check if it's really an exploit that we are executing
	// exploit have a .exploit extension
	if(eu_endswith(scriptname,".exploit") == 0)
	{
		commandWindow->printfn(GAMETEXT(" Error: [%s] is not a valid exploit file."),scriptname);
		commandWindow->printf("\n");
		return;
	}

	// Check if the exploit exists
	int index;
	index = hePlayer.getFileIndex(scriptname);

	if(index == -1)
	{
		commandWindow->printfn(GAMETEXT(" Error. Exploit file doesn't exist on your localhost: [%s]"),scriptname);
		commandWindow->printf("\n");
		return;
	}

	// * Localhost?
	if(strcmp(servername,"localhost") == 0)
	{
		commandWindow->printf("\n %s\n",GAMETEXT("Error: can't execute exploits on localhost"));
		return;
	}

	// Check if the host exists
	if(gGameLevel.getHostIndex(servername) == -1)
	{
		commandWindow->printfn(GAMETEXT(" Error. Server unreachable : [%s]"),servername);
		commandWindow->printf("\n");
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

	commandWindow->printfn(GAMETEXT(" Exploit loaded (%s/%d)"),e_service,e_port);
	commandWindow->printfn(GAMETEXT(" Launching exploit on [%s], port: %d"),servername, e_port);

	// Check if the target runs anything on the respective port
	index = gGameLevel.getHostIndex(servername);
	serviceIndex = gGameLevel.hostList[index].isService(e_port);
	if(serviceIndex == -1)
	{
		commandWindow->printfn(GAMETEXT(" Error. Port %d is closed on [%s]"),e_port,servername);
		commandWindow->printf("\n");
		return;
	}

	// Check if the exploit is for the respective service
	if(strcmp(gGameLevel.hostList[index].serviceList[serviceIndex].name,e_service) != 0)
	{
		commandWindow->printf("\n\n %s\n %s\n",GAMETEXT("Failed to execute exploit."),GAMETEXT("The exploit doesn't work on this host/service"));
		return;
	}

	// Everything is ok, execute exploit
	gGameLevel.hostList[index].serviceList[serviceIndex].hasPassword = 0;
	commandWindow->printf("\n ---");
	commandWindow->printfn(GAMETEXT(" Success! Service password has been disabled."));
	commandWindow->printfn(GAMETEXT(" You may now connect to %s on port %d"),servername,e_port);

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
		commandWindow->printf(GAMETEXT(" Error: Unable to start cracker. Another command is in progress...(%d)"),commandInProgress);
		commandWindow->printf("\n");
		return;
	}

	// * Make sure we are not connected
	if(gGameLevel.connected == 1)
	{
		commandWindow->printf("%s\n",GAMETEXT(" Error: Unable to start cracker. Please LOGOUT first..."));
		return;
	}
	
	// check if we have a parameter
	if(strlen(param) == 0)
	{
		commandWindow->printf(" %s\n %s\n %s\n",GAMETEXT("Syntax error. You haven't specified a host and portname to crack."),GAMETEXT("Usage: crack server port"),GAMETEXT("Example: crack server.com 110"));
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
		commandWindow->printf(" %s\n %s\n",GAMETEXT("Unable to connect. Server is added to the bounced link."),GAMETEXT("Please remove the server from the bounced link, by clicking on it."));
		return;
	}
	else
	{
		if(bouncedLink_GetTraceMultiplier() != 0)
		{
			commandWindow->printf(GAMETEXT(" Bouncing link through %d host(s)..."), bouncedLink_GetTraceMultiplier() - 1);
			commandWindow->printf("\n");
		}
	}
	// * done
	gApplicationLog.printf("\ncommand_Crack - initiating...");

	if(index == -1)
		commandWindow->printf(GAMETEXT(" Error: Server unreachable: [%s:%d]"),hostname,port);
	else
	{
		// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
		if(gGameLevel.hostList[index].mVisible == 0)
			gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);
		gGameLevel.hostList[index].mVisible = 1;

		commandWindow->printf(GAMETEXT(" cracking: %s[port : %d]"),hostname,port);

		// Check if there is a service running on the selected port
		serviceindex = gGameLevel.hostList[index].isService(port);
		if(serviceindex == -1)
		{
			commandWindow->printfn(GAMETEXT(" Unable to crack service on port %d. There is no service running on that port."),port);
			commandWindow->printfn(GAMETEXT(" Type: scan %s, to see what services are running on this server."),hostname);
		}
		else
		{
			// Check if password is needed
			if(gGameLevel.hostList[index].serviceList[serviceindex].hasPassword == 0)
			{
				commandWindow->printfn(GAMETEXT(" Service is unprotected. You don't need to crack it."));
			}
			else				
			{
				commandWindow->printfn(GAMETEXT(" launching."));
				commandWindow->printfn(GAMETEXT(" You can type ABORT anytime, to abort the crack."));

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
		commandWindow->printf(GAMETEXT(" Error: Unable to start decrypt. Another command is in progress...(%d)"),commandInProgress);
		commandWindow->printf("\n");
		return;
	}
 
	// * Make sure we are not connected
	if(gGameLevel.connected == 1)
	{
		commandWindow->printf("%s\n",GAMETEXT(" Error: Unable to start decrypt. Please LOGOUT first..."));
		return;
	}

	// check if we have a parameter
	if(strlen(param) == 0)
	{
		commandWindow->printf(" %s\n %s\n %s\n",GAMETEXT("Syntax error. You haven't specified a host to decrypt it's key."),GAMETEXT("Usage: decrypt server"),GAMETEXT("Example: decrypt server.com"));
		return;
	}

	sscanf(param,"%s",hostname);	
	index = gGameLevel.getHostIndex(hostname);

	// * make sure the host isn't in the bounced link list
	if(bouncedLink_IsHost(index) == 1)
	{
		commandWindow->printf(" %s\n %s\n",GAMETEXT("Unable to connect. Server is added to the bounced link."),GAMETEXT("Please remove the server from the bounced link, by clicking on it."));
		return;
	}
	else
	{
		if(bouncedLink_GetTraceMultiplier() != 0)
		{
			commandWindow->printf(GAMETEXT(" Bouncing link through %d host(s)..."), bouncedLink_GetTraceMultiplier() - 1);
			commandWindow->printf("\n");
		}
	}
	// * done
	
	if(index == -1)
		commandWindow->printf(GAMETEXT(" Error: Server unreachable: [%s]"),hostname);
	else
	{
		// * reveal host on targetmap, if it wasn't visible, and add to completion dictionary
		if(gGameLevel.hostList[index].mVisible == 0)
			gDictionaryHostNames.AddWord(gGameLevel.hostList[index].hostName);
	
        gGameLevel.hostList[index].mVisible = 1;

		commandWindow->printf(GAMETEXT(" decrypting: %s"),hostname);

		if(gGameLevel.hostList[index].mEncryptionKey == 0)
			commandWindow->printfn(GAMETEXT(" Server has no encryption key. You don't need to decrypt it."));
		else				
		{
			commandWindow->printfn(GAMETEXT(" launching."));
			commandWindow->printfn(GAMETEXT(" You can type ABORT anytime, to abort the decrypt."));

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

	commandWindow->printf(GAMETEXT(" Trying to transfer $%d"),transferAmount);

	// * Check all conditions for a transfer
	if(transfer_InProgress == 1)
	{
		transferOK = 0;
		commandWindow->printfn(GAMETEXT(" Error: Transfer already in progress. Please wait until it's completed."));
	} else
	if(gGameLevel.connected == 0)
	{
		transferOK = 0;
		commandWindow->printfn(GAMETEXT(" Error: You must be connected to a server, to transfer money."));
	} else
	if(gGameLevel.hostList[gGameLevel.con_hostindex].getOpenPortCount() != gGameLevel.hostList[gGameLevel.con_hostindex].serviceCount)
	{
		transferOK = 0;
		commandWindow->printfn(GAMETEXT(" Error: All services on the server must be hacked, before being able to transfer money."));
	} else
	if(gGameLevel.hostList[gGameLevel.con_hostindex].mMoney < transferAmount)
	{
		transferOK = 0;
		if(gGameLevel.hostList[gGameLevel.con_hostindex].mMoney == 0)
			commandWindow->printfn(GAMETEXT(" Error: There is no money on the server."));
		else
			commandWindow->printfn(GAMETEXT(" Error: You can only transfer a maximum of $%d."),gGameLevel.hostList[gGameLevel.con_hostindex].mMoney);
	}
	if(transferAmount == 0)
	{
		transferOK = 0;
		commandWindow->printfn(GAMETEXT(" Error: Please enter a positive amount of money to transfer."));
	}

	// * All conditions ok, start the transfer
	if(transferOK == 1)
	{
			commandWindow->printfn(GAMETEXT(" You can type ABORT anytime, to abort the transfer."));
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

	if(strcmp(param,"inline") != 0) commandWindow->printf("\n ");
	if(currentCommand == COMMAND_TRANSFER)
	{
		commandWindow->printf(GAMETEXT("Money transfer aborted..."));
		transfer_InProgress = 0;
		abort               = 1;
	}
	if(currentCommand == COMMAND_DOWNLOAD)
	{
		commandWindow->printf(GAMETEXT("File download aborted..."));
		download_InProgress = 0;
		abort               = 1;
	}
	if(currentCommand == COMMAND_UPLOAD)
	{
		commandWindow->printf(GAMETEXT("File upload aborted..."));
		upload_InProgress   = 0;
		abort               = 1;
	}
	if(currentCommand == COMMAND_CRACK)
	{
		commandWindow->printf(GAMETEXT("Password crack aborted..."));
		crack_InProgress = 0;
		abort            = 1;
	}
	if(currentCommand == COMMAND_DECRYPT)
	{
		commandWindow->printf(GAMETEXT("Key decryption aborted..."));
		decrypt_InProgress = 0;
		abort              = 1;
	}

	// * Nothing to abort?
	if(abort == 0)
	{
        if(strcmp(param,"inline") != 0)
		    commandWindow->printf(GAMETEXT("Nothing to abort..."));
	}

	if(strcmp(param,"inline") != 0)
		commandWindow->printf("\n");

	// * Cancel tracer
	trace_InProgress = 0;
	strcpy(trace_Msg,"-");

	currentCommand    = -1;
	commandInProgress = -1;

	strcpy(gGameLevel.statusPanelMessage,GAMETEXT("idle"));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Display player's hardware configuration
void command_Config(char* param)
{
	commandWindow->printfn(GAMETEXT(" Your current hardware configuration:"));
	commandWindow->printf("\n ------------------------------------- ");
		commandWindow->printfn(GAMETEXT(" CPU Slot 1    : %s"),HE_NAME_CPU[hePlayer.cpu1]);

	if(hePlayer.cpu2 != -1)
		commandWindow->printfn(GAMETEXT(" CPU Slot 2    : %s"),HE_NAME_CPU[hePlayer.cpu2]);
	else
		commandWindow->printfn(GAMETEXT(" CPU Slot 2    : (none)"));

		commandWindow->printfn(GAMETEXT(" Memory Slot 1 : %s"),HE_NAME_MEM[hePlayer.mem1]);

	if(hePlayer.mem2 != -1)
		commandWindow->printfn(GAMETEXT(" Memory Slot 2 : %s"),HE_NAME_MEM[hePlayer.mem2]);
	else
		commandWindow->printfn(GAMETEXT(" Memory Slot 2 : (none)"));

	    commandWindow->printfn(GAMETEXT(" Modem         : %s"),HE_NAME_MODEM[hePlayer.modem]);
		commandWindow->printfn(GAMETEXT(" Firewall      : %s"),HE_NAME_FIREWALL[hePlayer.firewall]);

	commandWindow->printf("\n ------------------------------------- ");
	commandWindow->printfn(GAMETEXT(" Storage space: %d Mb (Used: %d Mb | Free: %d Mb)"),hePlayer.getMemSize(),hePlayer.getMemSizeUsed(),hePlayer.getMemSize() - hePlayer.getMemSizeUsed());
	commandWindow->printf("\n ------------------------------------- ");
	
	// * Print multipliers
	commandWindow->printfn(GAMETEXT(" Tracetime decrease factor : %dx (firewall)"),hePlayer.getFirewallMultiplier());
	commandWindow->printfn(GAMETEXT(" Transfer increase factor  : %dx (modem)"),hePlayer.getModemMultiplier());
	commandWindow->printfn(GAMETEXT(" Speed increase factor     : %dx (CPU)"),hePlayer.getCPUMultiplier());

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
		commandWindow->printf(" %s\n %s\n",GAMETEXT("Syntax error"),GAMETEXT("Usage: login server password"));
		return;
	}

	// * Extract hostname and password
	strcat(param," @");
	sscanf(param,"%s %s",hostname,password);

	// * Verify parameters
	if(strlen(hostname) == 0)
	{
		commandWindow->printf(" %s\n %s\n",GAMETEXT("Syntax error"),GAMETEXT("Usage: login server password"));
		return;
	}
	if(strlen(password) == 0 || strcmp(password,"@") == 0)
	{
		commandWindow->printf(" %s\n %s\n",GAMETEXT("Syntax error"),GAMETEXT("Usage: login server password"));
		return;
	}

	hostindex = gGameLevel.getHostIndex(hostname);
	if(hostindex == -1)
	{
		commandWindow->printf(GAMETEXT(" Error: server is unreachable: [%s]"),hostname);
		commandWindow->printf("\n");
		return;
	}

	// * Try password on all ports
	commandWindow->printf(GAMETEXT(" Trying password ... [%s]"),password);
	for(i = 0; i < gGameLevel.hostList[hostindex].serviceCount; i++)
	{
		commandWindow->printfn(GAMETEXT(" Trying: port %4d, service: %20s -"),gGameLevel.hostList[hostindex].serviceList[i].port,gGameLevel.hostList[hostindex].serviceList[i].name);
		if(gGameLevel.hostList[hostindex].serviceList[i].hasPassword == 1)
		{
			if(strcmp(gGameLevel.hostList[hostindex].serviceList[i].password,password) == 0)
			{
				commandWindow->printf(GAMETEXT(" success. Password disabled."));
				gGameLevel.hostList[hostindex].serviceList[i].hasPassword = 0;
			}
			else
				commandWindow->printf(GAMETEXT(" failed."));
		}
		else
			commandWindow->printf(GAMETEXT(" skipped. Port is not password protected."));
	}

	// * Add trace
	hePlayer.mGlobalTrace += HE_TRACE_LOGIN;
	gGameLevel.hostList[hostindex].mVisible = 1;
	if(gDictionaryHostNames.isWordContained(gGameLevel.hostList[hostindex].hostName) == 0)
		gDictionaryHostNames.AddWord(gGameLevel.hostList[hostindex].hostName);

	// * Done
	commandWindow->printf("\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Upgrade player hardware
void command_Upgrade(char* param)
{
	int i, min,max;

	// * Add a small delay
	audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_tick2.wav");
	system_DelayMS(500);

	// * no paramater passed, display usage
	if(strlen(param) == 0)
	{
		commandWindow->printfn(GAMETEXT(" The following upgrade codes are available:"));
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
		commandWindow->printf("\n%s  ",GAMETEXT("/ CPU:"));
		if(min == HE_MAX_CPU - 1) commandWindow->printf(GAMETEXT("none"));
		for(i = min + 1; i < HE_MAX_CPU; i++)
			commandWindow->printfn(GAMETEXT(" - %s, price: $%5d, Upgrade CODE: CPU%d"),HE_NAME_CPU[i],HE_CPU_PRICE[i],i);

		//////////////////////////////////////////////////////////////////////////////////////////
		// * display available memory upgrades
		min = hePlayer.mem2;
		max = hePlayer.mem1;
		if(hePlayer.mem1 < hePlayer.mem2)
		{
			min = hePlayer.mem1;
			max = hePlayer.mem2;
		}		
		commandWindow->printfn(GAMETEXT("/ MEMORY:"));
		if(min == HE_MAX_MEM - 1) commandWindow->printf(GAMETEXT("none"));
		for(i = min + 1; i < HE_MAX_MEM; i++)
			commandWindow->printfn(GAMETEXT(" - %s, price: $%5d, Upgrade CODE: MEM%d"),HE_NAME_MEM[i],HE_MEM_PRICE[i],i);
		
		//////////////////////////////////////////////////////////////////////////////////////////
		// * display available firewall upgrades
		commandWindow->printfn(GAMETEXT("/ FIREWALL:"));
		if(hePlayer.firewall == HE_MAX_FIREWALL - 1) commandWindow->printf(GAMETEXT("none"));
		for(i = hePlayer.firewall + 1; i < HE_MAX_FIREWALL; i++)
			commandWindow->printfn(GAMETEXT(" - %s, price: $%5d, Upgrade CODE: FWL%d"),HE_NAME_FIREWALL[i],HE_FIREWALL_PRICE[i],i);

		//////////////////////////////////////////////////////////////////////////////////////////
		// * display available modem upgrades
		commandWindow->printfn(GAMETEXT("/ MODEM:"));
		if(hePlayer.modem == HE_MAX_MODEM - 1) commandWindow->printf(GAMETEXT("none"));
		for(i = hePlayer.modem + 1; i < HE_MAX_MODEM; i++)
			commandWindow->printfn(GAMETEXT(" - %s, price: $%5d, Upgrade CODE: MDM%d"),HE_NAME_MODEM[i],HE_MODEM_PRICE[i],i);

		//////////////////////////////////////////////////////////////////////////////////////////
		// * neural network adapter
		commandWindow->printfn(GAMETEXT("/ Neural Network Adapter:"));
		if(hePlayer.nnadapter == 0) 
			commandWindow->printf(GAMETEXT("none"));
		else
			commandWindow->printfn(GAMETEXT(" - Neural Network Adapter, price: $50000, Upgrade CODE: NNA"));

		/////////////////////////////////////////////////////////////////////////////////////////
		commandWindow->printf("\n ------------------------------------------- ");
		commandWindow->printfn(GAMETEXT(" Type: UPGRADE CODE, to perform an upgrade."));
		commandWindow->printfn(GAMETEXT(" Note: upgrade codes are ok in lowercase."));
	}
	// * upgrade hardware
	else
	{
		int      upgradeType =  0;
		int      upgradeCode = -1;
		hestring upgradeId;

		commandWindow->printfn(GAMETEXT(" Upgrading configuration..."));

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
			commandWindow->printfn(GAMETEXT(" ERROR: Invalid upgrade code entered: [%s]"),param);
			commandWindow->printfn("%s\n",GAMETEXT(" Type: UPGRADE, to see the list of available upgrade codes."));
			return;
		}

		// * Check upgrade option
		upgradeId[0] = param[3];
		upgradeId[1] = 0;
		upgradeCode = atoi(upgradeId);

		// * Special case

		if((upgradeId[0] < '0' || upgradeId[0] > '3') && (upgradeType != 5))
		{
			commandWindow->printfn(GAMETEXT(" ERROR: Invalid upgrade code entered: [%s]"),param);
			commandWindow->printfn("%s\n",GAMETEXT(" Type: UPGRADE, to see the list of available upgrade codes."));
			return;
		}

		commandWindow->printfn(GAMETEXT(" Upgrade parameter: %d/%d"),upgradeType,upgradeCode);

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
			commandWindow->printfn(GAMETEXT(" ERROR: Unable to perform the selected upgrade."));
			commandWindow->printfn(GAMETEXT(" You don't have enough money for the selected upgrade."));
			commandWindow->printfn(GAMETEXT(" You need at least $%d."),moneyOk);
			commandWindow->printf("\n");
			return;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// CPU upgrade
		if(upgradeType == 1)
		{
			// * Make sure we are not "downgrading"
			if(upgradeCode <= hePlayer.cpu1 && upgradeCode <= hePlayer.cpu2)
			{
				commandWindow->printfn(GAMETEXT(" ERROR: You are trying to update to a lower or same performance CPU."));
				commandWindow->printfn("%s\n",GAMETEXT(" Type: UPGRADE, to see the list of available upgrade codes."));
				return;
			}

			// * Choose slot to update
			commandWindow->printfn(GAMETEXT(" Upgrading CPU..."));
			if(hePlayer.cpu1 < hePlayer.cpu2)
			{
				hePlayer.cpu1 = upgradeCode;
				commandWindow->printfn(GAMETEXT(" slot 1. Done!"));
			}
			else
			{
				hePlayer.cpu2 = upgradeCode;
				commandWindow->printfn(GAMETEXT(" slot 2. Done!"));
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
				commandWindow->printfn(GAMETEXT(" ERROR: You are trying to update to a smaller size memory."));
				commandWindow->printfn("%s\n",GAMETEXT(" Type: UPGRADE, to see the list of available upgrade codes."));
				return;
			}

			// * Choose slot to update
			commandWindow->printfn(GAMETEXT(" Upgrading MEMORY..."));
			if(hePlayer.mem1 < hePlayer.mem2)
			{
				hePlayer.mem1 = upgradeCode;
				commandWindow->printfn(GAMETEXT(" slot 1. Done!"));
			}
			else
			{
				hePlayer.mem2 = upgradeCode;
				commandWindow->printfn(GAMETEXT(" slot 2. Done!"));
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
				commandWindow->printfn(GAMETEXT(" ERROR: You are trying to update to a smaller version firewall."));
				commandWindow->printfn("%s\n",GAMETEXT(" Type: UPGRADE, to see the list of available upgrade codes."));
				return;
			}

			// * Choose slot to update
			commandWindow->printfn(GAMETEXT(" Upgrading FIREWALL..."));
			hePlayer.firewall = upgradeCode;
			commandWindow->printfn(GAMETEXT(" done!"));
			hePlayer.mMoney -= moneyAmount;				
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MODEM upgrade
		if(upgradeType == 4)
		{
			// * Make sure we are not "downgrading"
			if(upgradeCode <= hePlayer.modem)
			{
				commandWindow->printfn(GAMETEXT(" ERROR: You are trying to update to a slower modem."));
				commandWindow->printfn("%s\n",GAMETEXT(" Type: UPGRADE, to see the list of available upgrade codes."));
				return;
			}

			// * Choose slot to update
			commandWindow->printfn(GAMETEXT(" Upgrading MODEM..."));
			hePlayer.modem = upgradeCode;
			commandWindow->printfn(GAMETEXT(" done!"));
			hePlayer.mMoney -= moneyAmount;				
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Neural adapter upgrade
		if(upgradeType == 5)
		{
			// * Make sure we are not "downgrading"

			commandWindow->printfn(GAMETEXT(" Installing Neural Network Adapter..."));
			hePlayer.nnadapter = 0;
			commandWindow->printfn(GAMETEXT(" done!"));
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
	commandWindow->displayFile("heuntold-data/bounce-help.txt");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Displays information on the bounced link
void command_BounceInfo(char* param)
{
	int i;

	commandWindow->printfn(GAMETEXT(" === Bounced link status:"));	
	if(gBouncedLinkCount == 0)
		commandWindow->printfn(GAMETEXT("     - no bounced link setup"));	
	else
	{
		commandWindow->printfn(GAMETEXT("    %d host(s) in the bounced link"),gBouncedLinkCount);	
		for(i = 0; i < gBouncedLinkCount; i++)
			commandWindow->printfn(GAMETEXT("     - [%25s] - %d bounces left"), gGameLevel.hostList[gBouncedLinkHostList[i]].hostName,gGameLevel.hostList[gBouncedLinkHostList[i]].mBounceCount);	
	}

	// * Display list of hosts through wich the player can bounce
	commandWindow->printfn(GAMETEXT(" === The following servers can be used to bounce through:"));	
	for(i = 0; i < gGameLevel.hostCount; i++)
	{
		if(gGameLevel.hostList[i].mVisible == 1 && gGameLevel.hostList[i].mBounceCount > 0)
			if(gGameLevel.hostList[i].hackedCompletely() == 1)
				commandWindow->printfn(GAMETEXT("     - [%25s] - %d bounces left"), gGameLevel.hostList[i].hostName,gGameLevel.hostList[i].mBounceCount);	
			else
				commandWindow->printfn(GAMETEXT("     - [%25s] - %d bounces left (server needs to be hacked first)"), gGameLevel.hostList[i].hostName,gGameLevel.hostList[i].mBounceCount);	
	}

	commandWindow->printf("\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Reduces the global trace level
void command_KillTrace(char* param)
{
	if(hePlayer.mGlobalTrace <= 0)
	{
		commandWindow->printfn(GAMETEXT(" Your trace level is 0%."));
		commandWindow->printfn(GAMETEXT(" There is no need to use killtrace at this time."));
		commandWindow->printf("\n");
		return;
	}

	if(hePlayer.mMoney < HE_KILLTRACE_COST)
	{
		commandWindow->printfn(GAMETEXT(" You don't have enough money to reduce your trace level."));
		commandWindow->printfn(GAMETEXT(" You need at least $%d to reduce your tracetime by 10%%."),HE_KILLTRACE_COST);
		commandWindow->printf("\n");
		return;
	}

	// * Everything is ok, reduce the tracetime
	int newTraceLevel;

	newTraceLevel = hePlayer.mGlobalTrace - 10;
	if(newTraceLevel < 0) newTraceLevel = 0;

	commandWindow->printf("\n%s",GAMETEXT(" Your trace level will be reduced by 10% by aquiring a new IP address."));
	commandWindow->printf("\n%s",GAMETEXT(" Your account will be charged $500 for this service, by your ISP."));

	hePlayer.mGlobalTrace = newTraceLevel;
	hePlayer.mMoney      -= HE_KILLTRACE_COST;
	consoleTextAnimation.PlayTextAnimation(commandWindow,"heuntold-data/textanim-killtrace.txt");
	
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
			commandWindow->printf(GAMETEXT(" Error: File not found: %s"),param);
		}
		else
		{
			// * Make sure the file can accesed on the current port
			if(gGameLevel.hostList[gGameLevel.con_hostindex].getRulePort(param,gGameLevel.con_port) != 1)
			{
				commandWindow->printf(GAMETEXT(" Error: File not found: %s"),param);
				commandWindow->printf(GAMETEXT(" This file can't be accessed on the current port: %d"),param,gGameLevel.con_port);
			}
			else
			{
				// * Delete file
				for(i = index; i < gGameLevel.hostList[gGameLevel.con_hostindex].fileCount - 1; i++)
					gGameLevel.hostList[gGameLevel.con_hostindex].fileList[i] = gGameLevel.hostList[gGameLevel.con_hostindex].fileList[i + 1];
				gGameLevel.hostList[gGameLevel.con_hostindex].fileCount--;
				commandWindow->printf(GAMETEXT(" File deleted: %s"),param);
			}
		}
	}
	else
	{
		index = hePlayer.getFileIndex(param);
		if(index == -1)
		{
			commandWindow->printf(GAMETEXT(" Error: File not found: %s"),param);
		}
		else
		{
			// * Delete file
			for(i = index; i < hePlayer.fileCount - 1; i++)
				hePlayer.fileList[i] = hePlayer.fileList[i + 1];
			hePlayer.fileCount--;
			commandWindow->printf(GAMETEXT(" File deleted: %s"),param);
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

	    commandWindow->printf(GAMETEXT(" Selling all hardware..."));
     
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
            commandWindow->printfn(GAMETEXT(" You only have the basic hardware configuration installed."));
            money = 0;
        }


        if(money == 0)
            commandWindow->printfn(GAMETEXT(" You don't have any hardware to sell."));
        else
        {   
            // * Substract basic hardware cost
			int basicCost = 0;
			if(hePlayer.cpu1     == 0) basicCost += HE_CPU_PRICE[0];
			if(hePlayer.firewall == 0) basicCost += HE_FIREWALL_PRICE[0];
			if(hePlayer.modem    == 0) basicCost += HE_MODEM_PRICE[0];
			if(hePlayer.mem1     == 0) basicCost += HE_MEM_PRICE[0];
			
			//money -= 6000;
			money -= basicCost;

            commandWindow->printfn(GAMETEXT(" Your hardware values: $%d"),money);
            money = money * 0.9;
            commandWindow->printfn(GAMETEXT(" You have received back 90%% of it's value: %d"), money);

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
	    commandWindow->printf(GAMETEXT(" You can use this commmand to sell all your hardware, and receive back 90%% of it's value."));
	    commandWindow->printfn(GAMETEXT(" Type:"));
		commandWindow->printf("\n/ %s \n%s","DOWNGRADE ALL",GAMETEXT(" to sell all your hardware."));
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
		commandWindow->printf(GAMETEXT(" Error. You must specify a hostname to add/remove from/to the bounced link."));
		commandWindow->printfn(GAMETEXT(" Usage: bounce server"));
    }
    else
    {
        int newHostIndex;
        int i,found;

        newHostIndex = gGameLevel.getHostIndex(param);
        found        = -1;

        if(newHostIndex == -1)
        {
            commandWindow->printf(GAMETEXT(" Error. [%s] doesn't exist."),param);
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
					audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
					commandWindow->printf(GAMETEXT(" Host added to bounced link: [%s]"),gGameLevel.hostList[newHostIndex].hostName);
				    
					char tmpmsg[1024];
					sprintf(tmpmsg,"%s \n%s",GAMETEXT("To learn about bounced links"),GAMETEXT("type BOUNCEHELP"));
					setHelpToolTip(tmpmsg);
				}

				// * Check if we can still bounce through this host
				if(gGameLevel.hostList[newHostIndex].mBounceCount == 0)
				{
					commandWindow->printfn(GAMETEXT(" Error: You can't bounce anymore through [%s]"),gGameLevel.hostList[newHostIndex].hostName);
				}

				if(gGameLevel.hostList[newHostIndex].hackedCompletely() != 1)
				{
					commandWindow->printfn(GAMETEXT(" Error: All services on [%s] must be hacked,"),gGameLevel.hostList[newHostIndex].hostName);
					commandWindow->printfn(GAMETEXT("in order to be able to bounce through it."));
					commandWindow->printfn("%s\n",GAMETEXT(" Type: BOUNCEHELP in the console to learn more about bounced links."));
				}
			}
			else
			{
				for(i = found; i < gBouncedLinkCount - 1; i++)
					gBouncedLinkHostList[i] = gBouncedLinkHostList[i + 1];
				gBouncedLinkCount--;
				audio_PlaySoundFX("heuntold-soundfx/hesfx-tick.wav");
				commandWindow->printf(GAMETEXT(" Host removed from bounced link: [%s]"),gGameLevel.hostList[newHostIndex].hostName);
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
	commandWindow->printf("\n %s\n",GAMETEXT("Warning : cheat code has been used. Your score has been reset to 0."));
	hePlayer.mScore = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Money cheat. Add $10000 to player's balance
void command_Cheat_Money(char* param)
{
	hePlayer.mMoney += 10000;

	// * Reset score, when using cheats
	commandWindow->printf("\n %s\n",GAMETEXT("Warning : cheat code has been used. Your score has been reset to 0."));
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
	commandWindow->printf("\n %s\n",GAMETEXT("Warning : cheat code has been used. Your score has been reset to 0."));
	hePlayer.mScore = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Easter egg. Gotta have one in the game :-)
void command_EasterEgg_exosyphen(char* param)
{
	commandWindow->printf(LANG_TEXT_CMD_EXOSYPHEN);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Submit hiscore
void command_SubmitHiscore(char* param)
{
	// Killed in the 2014 update
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Deletes the tracing longs from the server
void command_DeleteLogs(char* param)
{
	int i;

	if(gGameLevel.connected == 1)
	{
		audio_PlaySoundFX("heuntold-soundfx/hesfx_untold_searching.wav");
		commandWindow->printfn(GAMETEXT(" Attempting to lower your global trace level by %d %%"),gGameLevel.hostList[gGameLevel.con_hostindex].mTraceAdded / HE_FACTOR_DELETELOGS);
		hePlayer.mGlobalTrace -= (gGameLevel.hostList[gGameLevel.con_hostindex].mTraceAdded / HE_FACTOR_DELETELOGS);
		if(hePlayer.mGlobalTrace < 0) hePlayer.mGlobalTrace = 0;
		gGameLevel.hostList[gGameLevel.con_hostindex].mTraceAdded = 0;
		consoleTextAnimation.PlayTextAnimation(commandWindow,"heuntold-data/textanim-deletelogs.txt");
	}
	else
	{
		commandWindow->printfn(GAMETEXT(" Error: You are not connected to any server."));
		commandWindow->printfn(GAMETEXT(" You must be connected to a server to use this command."));
		commandWindow->printf("\n");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Jumps to the next level
void command_NextLevel(char* param)
{
	gNextLevel = 1;

	commandWindow->printfn(GAMETEXT(" Jumping to next level..."));
	commandWindow->printf("\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Display tutorial for level
void command_Tutorial(char* param)
{
	FILE* file;

	commandWindow->printf("\n");
	file = fopen(createModLevelFilePath("tutorial.txt"),"rt");

	if(file != NULL)
	{
		fclose(file);
		commandWindow->displayFile(createModLevelFilePath("tutorial.txt"));
	}
	else
	{
		commandWindow->printf(GAMETEXT(" There is no tutorial available for the current level."));
		commandWindow->printf("\n");
	}
}
