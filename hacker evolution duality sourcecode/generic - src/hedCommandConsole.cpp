/*
Name: hedCommandConsole.cpp
Desc: Command console management class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedCommandConsole.h"
#include "../generic - inc/hedSystem.h"
#include <string.h>
#include <stdio.h>

// ====================================================================================================================================================================================================
// Class constructor
hedCommandConsole::hedCommandConsole()
{
	strcpy(commandLine,"");
	cursorPos   = 0;
	blinkTimer  = 0.0f;
	commandType = COMMAND_NONE;

	strcpy(commandArgs[0],"");
	strcpy(commandArgs[1],"");
	strcpy(commandArgs[2],"");

    // Initialize dictionary
    dictionaryCount = 0;
    Dictionary_Add("help");
    Dictionary_Add("spoiler");
    Dictionary_Add("ls");
    Dictionary_Add("cat");
    Dictionary_Add("nextlevel");
    Dictionary_Add("sysinfo");
    Dictionary_Add("cls");
    Dictionary_Add("close");

	// New commands in UPDATE 1
	Dictionary_Add("scan");
	Dictionary_Add("bypass"); Dictionary_Add("voiceprint"); Dictionary_Add("retinalscan"); Dictionary_Add("encryptionkey");
	Dictionary_Add("delete");
	Dictionary_Add("dns");
	Dictionary_Add("ping");
	Dictionary_Add("killtrace");
	Dictionary_Add("crack");

	// command parameters
	Dictionary_Add("voiceprint");
	Dictionary_Add("retinalscan");
	Dictionary_Add("encryptionkey");

	baseDictionaryCount = dictionaryCount;
}

// ====================================================================================================================================================================================================
// Class destructor
hedCommandConsole::~hedCommandConsole()
{
}

// ====================================================================================================================================================================================================
// Initialize the console
void hedCommandConsole::Init(hedClassTextWindow* window)      
{
	consoleWindow           = window;
    tabIndex                = 0;
	commandHistoryCount     = 0;
    commandHistoryIndex     = -1;
}

// ====================================================================================================================================================================================================
// Passes a character from the input, to the console
void hedCommandConsole::AddCharacter(int character)
{
	int i, length, charOk = 0;

	// Verify if the character is used in the command line
	if(character >= 'a' && character <= 'z') charOk = 1; 
	if(character >= 'A' && character <= 'Z') {charOk = 1; character += 32;}
	if(character >= '0' && character <= '9') charOk = 1; 

	if(character == '.') charOk = 1; 
	if(character == '-') charOk = 1;
	if(character == ' ') charOk = 1;

    // Repeating tab
    if(character != HEDKEY_TAB && character != 0) tabIndex = 0;

	// If the character is ok, add it to the command line
	length = strlen(commandLine);

    if(strlen(commandLine) > 70) charOk = 0;

	if(charOk == 1)
	{
		// Insert an empty character
		for(i = length; i > cursorPos; i--)
			commandLine[i] = commandLine[i - 1];

		commandLine[cursorPos] = character;
		cursorPos++;

		commandLine[length + 1] = 0;
	}

	// Process special characters
	length = strlen(commandLine);
	if(character == HEDKEY_BACKSPACE && cursorPos > 0)
	{
		// Delete character
		commandLine[cursorPos - 1] = 0;

		if(cursorPos > 0)
			for(i = cursorPos - 1; i < length - 1; i++)
				commandLine[i] = commandLine[i + 1];

		commandLine[length - 1] = 0;
		cursorPos--;
	}

    // DEL KEY
	if(character == HEDKEY_DEL)
	{
		// Delete character
		for(i = cursorPos; i < length; i++)
			commandLine[i] = commandLine[i + 1];

		commandLine[length] = 0;
	}
	if(character == HEDKEY_LEFT)  DECREMENT_WITH_CAP(cursorPos,1,0);
	if(character == HEDKEY_RIGHT) INCREMENT_WITH_CAP(cursorPos,1,length);

	if(character == HEDKEY_HOME)  cursorPos = 0;
	if(character == HEDKEY_END) cursorPos = strlen(commandLine);

	// UP and DOWN keys ... scroll through history
	if(character == HEDKEY_UP)
	{
		// start
		if(commandHistoryIndex == -1)
		{
			commandHistoryIndex = commandHistoryCount - 1;
			strcpy(commandHistory[commandHistoryCount],commandLine);
		}
		else
		{
			if(commandHistoryIndex > 0)
				commandHistoryIndex--;
		}

		strcpy(commandLine,commandHistory[commandHistoryIndex]);
	}

	if(character == HEDKEY_DOWN)
	{
		if(commandHistoryIndex != -1 && commandHistoryIndex < commandHistoryCount)
		{
			commandHistoryIndex++;
			strcpy(commandLine,commandHistory[commandHistoryIndex]);
		}
	}

    // TAB - autocompletion
    if(character == HEDKEY_TAB && (cursorPos == strlen(commandLine)))
    {
        if(tabIndex == 0)
        {
            lastIndex = cursorPos;
            while(commandLine[lastIndex] != ' ' && lastIndex != 0)
			{
                lastIndex--;
			}

			if(commandLine[lastIndex] == ' ') lastIndex++;

            strcpy(dictionarySearch,(char*)&commandLine[lastIndex]);
        }

        dictionaryMatch = Dictionary_Match(dictionarySearch,tabIndex);

        if(dictionaryMatch != 0)
        {
            strcpy((char*)&commandLine[lastIndex],dictionaryMatch);
            cursorPos = strlen(commandLine);
            tabIndex++;
        }
    }

	// Enter pressed, process command
	if(character == HEDKEY_ENTER)
		ProcessCommandLine();
}

// ====================================================================================================================================================================================================
// Returns the content of the current command line
char* hedCommandConsole::GetCommandLine()
{
	int i, index, blink;

	// Update cursor blinking
	blink = 1;

	blinkTimer += g_FrameTime;
	if(blinkTimer >= CURSOR_BLINK_RATE) blink = 0;
	if(blinkTimer >= 2 * CURSOR_BLINK_RATE) blinkTimer -= 2 * CURSOR_BLINK_RATE;

	// Format the command console for display
	strcpy(commandLineDisplay,"");
	sprintf(commandLineDisplay,"console/> ",cursorPos,commandLine);
	index = strlen(commandLineDisplay);

	// Command text until the cursor position
	for(i = 0; i < cursorPos; i++)
		commandLineDisplay[i + index] = commandLine[i];
	
	// Cursor
	if(blink == 1)
		commandLineDisplay[i + index] = '|';
	else
		commandLineDisplay[i + index] = ' ';

	// Text after the cursor
	for(i = cursorPos; i < strlen(commandLine); i++)
		commandLineDisplay[i + index + 1] = commandLine[i];
	
	// End of line
	commandLineDisplay[i + index + 1] = 0;

	return commandLineDisplay;
}

// ====================================================================================================================================================================================================
// Returns the executed command
int hedCommandConsole::GetCommand()
{
	int retval = commandType;

	commandType = COMMAND_NONE;
	return retval;
}

// ====================================================================================================================================================================================================
// Processes the commands in the command line (when ENTER is pressed)
void hedCommandConsole::ProcessCommandLine()
{
	int index = 0, index2 = 0;
	commandCount = 0;

	strcpy(commandArgs[0],"");
	strcpy(commandArgs[1],"");
	strcpy(commandArgs[2],"");
	strcpy(commandArgs[3],"");
	strcpy(commandArgs[4],"");

	if(strlen(commandLine) == 0) return;

	do
	{
		// Skip spaces
		while(commandLine[index] == ' ' && commandLine[index] != 0) index++;

		// Extract argument
		while(commandLine[index] != ' ' && commandLine[index] != 0) 
		{
			commandArgs[commandCount][index2] = commandLine[index];
			index++;
			index2++;
		}

		commandArgs[commandCount][index2] = 0;
		commandCount++;
		index2                            = 0;
	}while(commandCount < 5 && commandLine[index] != 0);

	// Identify command
	commandType = COMMAND_INVALID;
	if(strcmp(commandArgs[0],"help") == 0)      commandType = COMMAND_HELP;
	if(strcmp(commandArgs[0],"spoiler") == 0)   commandType = COMMAND_SPOILER;
	if(strcmp(commandArgs[0],"ls") == 0)        commandType = COMMAND_LS;
	if(strcmp(commandArgs[0],"cat") == 0)       commandType = COMMAND_CAT;
	if(strcmp(commandArgs[0],"nextlevel") == 0) commandType = COMMAND_NEXTLEVEL;
	if(strcmp(commandArgs[0],"sysinfo") == 0)   commandType = COMMAND_SYSINFO;
	if(strcmp(commandArgs[0],"cls") == 0)       commandType = COMMAND_CLS;
	if(strcmp(commandArgs[0],"close") == 0)     commandType = COMMAND_CLOSE;

	// * new commands in UPDATE 1
	if(strcmp(commandArgs[0],"scan")   == 0)      commandType = COMMAND_SCAN;
	if(strcmp(commandArgs[0],"bypass") == 0)      commandType = COMMAND_BYPASS;
	if(strcmp(commandArgs[0],"delete") == 0)      commandType = COMMAND_DELETE;
	if(strcmp(commandArgs[0],"dns")    == 0)      commandType = COMMAND_DNS;
	if(strcmp(commandArgs[0],"ping")   == 0)      commandType = COMMAND_PING;
	if(strcmp(commandArgs[0],"killtrace")  == 0)  commandType = COMMAND_KILLTRACE;
	if(strcmp(commandArgs[0],"crack")  == 0)      commandType = COMMAND_CRACK;

    // Cheats
#ifndef BUILD_STEAM
	if(strcmp(commandArgs[0],"briannext") == 0)      commandType = COMMAND_CHEAT_NEXTLEVEL;
	if(strcmp(commandArgs[0],"brianshowall") == 0)   commandType = COMMAND_CHEAT_SHOWALL;
	if(strcmp(commandArgs[0],"brianshow") == 0)      commandType = COMMAND_CHEAT_SHOW;
#endif
    consoleWindow->Anim_DisplayText("/c2> %s\n",commandLine);

    // Save command to history, if valid
    if(commandType != COMMAND_INVALID)
    {
        strcpy(commandHistory[commandHistoryCount],commandLine);
        if(commandHistoryCount < 1024) 
			commandHistoryCount++;
		else
		{
			// command history is full
			for(int i = 0; i < 1023; i++)
				strcpy(commandHistory[i],commandHistory[i + 1]);
		}

		commandHistoryIndex = -1;
    }

    strcpy(commandLine,"");
	cursorPos = 0;
}

// ====================================================================================================================================================================================================
// Execute a command
void hedCommandConsole::ExecCommand(int command, hedGameLevel* level, int serverLocked)
{
	int i;

	// ================================================================================================================================================================================================
	// INVALID COMMAND
	if(command == COMMAND_INVALID)
	{
		consoleWindow->Anim_DisplayText("ERROR. Unknown command. Type HELP to see all available commands.", commandArgs[0]);
		consoleWindow->Anim_DisplayText("\n---\n");
	}

	// ================================================================================================================================================================================================
	// HELP
	if(command == COMMAND_HELP)
	{
		consoleWindow->Anim_DisplayFile("hed-text/english/help-console.txt");
		//consoleWindow->Anim_DisplayText("---\n");
	}

	// ================================================================================================================================================================================================
	// SPOILER
	if(command == COMMAND_SPOILER)
	{
		consoleWindow->Anim_DisplayFile(level->GetFilePath("level_spoiler.txt"));
		consoleWindow->Anim_DisplayText("---\n");
	}

	// ================================================================================================================================================================================================
	// LS
	if(command == COMMAND_LS)
	{
		consoleWindow->Anim_DisplayText("/c1Files on (localhost):\n");
		for(i = 0; i < level->m_ServerList[0].m_FileCount; i++)
			consoleWindow->Anim_DisplayText("> %s\n",level->m_ServerList[0].m_FileName[i]);

		// If we are locked on a hacked server, also display the files on it.
		if(level->m_ServerList[serverLocked].isFullyHacked(level->m_ServerList[level->m_ServerLocal].m_ServerKeys[serverLocked]) == 1)
		{
			consoleWindow->Anim_DisplayText("/c1Files on locked server (%s): \n",level->m_ServerList[serverLocked].m_HostName);

			for(i = 0; i < level->m_ServerList[serverLocked].m_FileCount; i++)
				consoleWindow->Anim_DisplayText("> %s\n",level->m_ServerList[serverLocked].m_FileName[i]);
		}
	}

	// ================================================================================================================================================================================================
	// LS
	if(command == COMMAND_CAT)
	{
		int index = level->m_ServerList[0].fileIndex(commandArgs[1]);

		if(index != -1)
		{
			consoleWindow->Anim_DisplayText("/c1Listing contents of [%s]\n",commandArgs[1]);
			consoleWindow->Anim_DisplayFile(level->m_ServerList[0].m_FilePath[index]);
		}
		else
		{
			consoleWindow->Anim_DisplayText("File not found on localhost: [%s]\n",commandArgs[1]);

			// Is the file available on the currently locked server?
			index = level->m_ServerList[serverLocked].fileIndex(commandArgs[1]);
			
			if(index != -1)
			{
				consoleWindow->Anim_DisplayText("File found on remote server (%s)\n",level->m_ServerList[serverLocked].m_HostName);
				consoleWindow->Anim_DisplayText("/c1Listing contents of [%s]\n",commandArgs[1]);
				consoleWindow->Anim_DisplayFile(level->m_ServerList[serverLocked].m_FilePath[index]);
			}
		}

		consoleWindow->Anim_DisplayText("\n");
	}
	
    // ================================================================================================================================================================================================
	// NEXTLEVEL
	if(command == COMMAND_NEXTLEVEL)
	{
        if(level->m_LevelCompleted == 1)
		{
			consoleWindow->Anim_DisplayText("Advancing to the next game level...\n");
            level->m_Quit = 2;
		}
		else
			consoleWindow->Anim_DisplayText("Unable to advance to the next level. Level not yet completed.\n");

		consoleWindow->Anim_DisplayText("---\n");
	}

    // ================================================================================================================================================================================================
	// SYSINFO
	if(command == COMMAND_SYSINFO)
	{
		consoleWindow->Anim_DisplayText("System information [localhost]\n");
        consoleWindow->Anim_DisplayText("DOS charge time: %ds (default: %ds)\n",(int)(SERVER_CHARGETIME_DOS - level->m_ServerList[0].m_LevelCPUPower * 5), (int)(SERVER_CHARGETIME_DOS - 10.0f));
        consoleWindow->Anim_DisplayText("EMP charge time: %ds (default: %ds)\n",(int)(SERVER_CHARGETIME_EMP - level->m_ServerList[0].m_LevelCPUPower * 5), (int)(SERVER_CHARGETIME_EMP - 10.0f));

		consoleWindow->Anim_DisplayText("---\n");
	}

    // ================================================================================================================================================================================================
	// CLS
	if(command == COMMAND_CLS)
	{
		consoleWindow->Clear();
	}

    // ================================================================================================================================================================================================
	// COMMAND : SCAN
	if(command == COMMAND_SCAN)
	{
        int index;

		if(strlen(commandArgs[1]) == 0)
		{
			consoleWindow->Anim_DisplayText("Error: invalid argument\n");
			consoleWindow->Anim_DisplayText("USAGE: SCAN servername\n");
		}
		else
		{
			index = level->GetServerIndex(commandArgs[1]);
			consoleWindow->Anim_DisplayText("Scanning for [%s]...\n",commandArgs[1]);
			if(index > -1)
			{
				consoleWindow->Anim_DisplayText("Server found...\n");
				level->m_ServerList[index].m_Visible = 1;
				consoleWindow->Anim_DisplayText("Server revealed on targetmap\n");
			}
			else
			{
				consoleWindow->Anim_DisplayText("Server NOT found: [%s]\n",commandArgs[1]);
			}
		}
	}
    
	// ================================================================================================================================================================================================
	// COMMAND : BYPASS
	if(command == COMMAND_BYPASS)
	{
		int index = level->GetServerIndex(commandArgs[2]);
		int type  = -1;

		consoleWindow->Anim_DisplayText("Authentication bypass initiated...\n");

		// Find server
		if(index == - 1)
		{
			consoleWindow->Anim_DisplayText("/c2Error: server not found (%s)\n",commandArgs[2]);
			consoleWindow->Anim_DisplayText("USAGE: BYPASS voiceprint SERVER password\n");
			consoleWindow->Anim_DisplayText("USAGE: BYPASS retinalscan SERVER password\n");
			consoleWindow->Anim_DisplayText("USAGE: BYPASS encryptionkey SERVER password\n");

			commandType = COMMAND_NONE;
			return;
		}

		if(strcmp(commandArgs[1],"voiceprint") == 0)    type = 1;
		if(strcmp(commandArgs[1],"retinalscan") == 0)   type = 2;
		if(strcmp(commandArgs[1],"encryptionkey") == 0) type = 3;

		if(type == - 1)
		{
			consoleWindow->Anim_DisplayText("/c2Error: invalid parameter (%s)\n",commandArgs[1]);
			consoleWindow->Anim_DisplayText("USAGE: BYPASS voiceprint SERVER password\n");
			consoleWindow->Anim_DisplayText("USAGE: BYPASS retinalscan SERVER password\n");
			consoleWindow->Anim_DisplayText("USAGE: BYPASS encryptionkey SERVER password\n");

			commandType = COMMAND_NONE;
			return;
		}
				
		// Bypass voiceprint authentication 
		if(type == 1)
		{
			// Authentication type present?
			if(level->m_ServerList[index].m_VoiceprintPassword == 0)
			{
				consoleWindow->Anim_DisplayText("/c2Error: Voiceprint authentication not present on this server.\n");
				commandType = COMMAND_NONE;
				return;
			}

			// Can it be bypassed with a password?
			if(strlen(level->m_ServerList[index].m_BypassVoicePrint) < 2)
			{
				consoleWindow->Anim_DisplayText("/c2Voiceprint authentication can't be bypassed with a password on this server.\n");
				commandType = COMMAND_NONE;
				return;
			}


			// Password check
			if(strcmp(commandArgs[3],level->m_ServerList[index].m_BypassVoicePrint) != 0)
			{
				consoleWindow->Anim_DisplayText("/c2Error: Invalid password.\n");
				commandType = COMMAND_NONE;
				return;
			}

			consoleWindow->Anim_DisplayText("Bypassing voiceprint authentication @ %s ...\n",commandArgs[2]);
			consoleWindow->Anim_DisplayText("%s ...\n",commandArgs[3]);
			consoleWindow->Anim_DisplayText("Successfully completed.\n");
			level->m_ServerList[index].m_VoiceprintPassword = 0;
		}

		// Bypass retinal scan authentication 
		if(type == 2)
		{
			// Authentication type present?
			if(level->m_ServerList[index].m_RetinaPassword == 0)
			{
				consoleWindow->Anim_DisplayText("/c2Error: Retinal scan authentication not present on this server.\n");
				commandType = COMMAND_NONE;
				return;
			}

			// Can it be bypassed with a password?
			if(strlen(level->m_ServerList[index].m_BypassRetinaScan) < 2)
			{
				consoleWindow->Anim_DisplayText("/c2Retinal scan authentication can't be bypassed with a password on this server.\n");
				commandType = COMMAND_NONE;
				return;
			}

			// Password check
			if(strcmp(commandArgs[3],level->m_ServerList[index].m_BypassRetinaScan) != 0)
			{
				consoleWindow->Anim_DisplayText("/c2Error: Invalid password.\n");
				commandType = COMMAND_NONE;
				return;
			}

			consoleWindow->Anim_DisplayText("Bypassing retina scan authentication @ %s ...\n",commandArgs[2]);
			consoleWindow->Anim_DisplayText("%s ...\n",commandArgs[3]);
			consoleWindow->Anim_DisplayText("Successfully completed.\n");
			level->m_ServerList[index].m_RetinaPassword = 0;
		}

		// Bypass encryption key authentication 
		if(type == 3)
		{
			// Authentication type present?
			if(level->m_ServerList[index].m_EncryptionKey == 0)
			{
				consoleWindow->Anim_DisplayText("/c2Error: Encryption key authentication not present on this server.\n");
				commandType = COMMAND_NONE;
				return;
			}

			// Can it be bypassed with a password?
			if(strlen(level->m_ServerList[index].m_BypassEncryptionKey) < 2)
			{
				consoleWindow->Anim_DisplayText("/c2Encryption key authentication can't be bypassed with a password on this server.\n");
				commandType = COMMAND_NONE;
				return;
			}

			// Password check
			if(strcmp(commandArgs[3],level->m_ServerList[index].m_BypassEncryptionKey) != 0)
			{
				consoleWindow->Anim_DisplayText("/c2Error: Invalid password.\n");
				commandType = COMMAND_NONE;
				return;
			}

			consoleWindow->Anim_DisplayText("Bypassing encryption key authentication @ %s ...\n",commandArgs[2]);
			consoleWindow->Anim_DisplayText("%s ...\n",commandArgs[3]);
			consoleWindow->Anim_DisplayText("Successfully completed.\n");
			level->m_ServerList[index].m_EncryptionKey = 0;
		}
	}

	// ================================================================================================================================================================================================
	// COMMAND : DELETE
	if(command == COMMAND_DELETE)
	{
		int i,index;

		// Verify syntax
		if(strlen(commandArgs[1]) == 0)
		{
			consoleWindow->Anim_DisplayText("/c2Error. No filename specified. Usage: DELETE FILENAME\n");
			commandType = COMMAND_NONE;
			return;
		}

		consoleWindow->Anim_DisplayText("Checking permissions on %s...(%d/%d)\n",level->m_ServerList[serverLocked].m_HostName,serverLocked,level->m_ServerLocal);
		
		// Is server hacked?
		if(level->m_ServerList[serverLocked].isFullyHacked(level->m_ServerList[level->m_ServerLocal].m_ServerKeys[serverLocked] == 0) && serverLocked != level->m_ServerLocal)
		{
			// Not hacked
			consoleWindow->Anim_DisplayText("/c2Access denied. You need to hack this server first.\n");
		}
		else
		{
			// Hacked. Delete the selected file
			consoleWindow->Anim_DisplayText("Permissions OK.\n");

			index = level->m_ServerList[serverLocked].fileIndex(commandArgs[1]);

			if(index == -1)
			{
				consoleWindow->Anim_DisplayText("/c2Error. File not found: %s.\n",commandArgs[1]);
				consoleWindow->Anim_DisplayText("Type LS to view a list of available files.\n");
			}
			else
			{
				for(i = index; i < level->m_ServerList[serverLocked].m_FileCount - 1; i++)
				{
					strcpy(level->m_ServerList[serverLocked].m_FileName[i],level->m_ServerList[serverLocked].m_FileName[i + 1]);
					strcpy(level->m_ServerList[serverLocked].m_FilePath[i],level->m_ServerList[serverLocked].m_FilePath[i + 1]);
				}

				level->m_ServerList[serverLocked].m_FileCount--;
				consoleWindow->Anim_DisplayText("/c2File DELETED: %s.\n",commandArgs[1]);
			}
		}
	}

	// ================================================================================================================================================================================================
	// COMMAND : DNS
	if(command == COMMAND_DNS)
	{
		int i;
		int resolved = 0;

		if(strlen(commandArgs[1]) == 0)
		{
			consoleWindow->Anim_DisplayText("/c2Error. No hostname or ip address specified. Usage: DNS hostname/ip_address\n");
			commandType = COMMAND_NONE;
			return;
		}

		consoleWindow->Anim_DisplayText("Attempting to resolve %s.\n",commandArgs[1]);
		
		for(i = 0; i < hedDNSCount; i++)
		{
			if(strcmp(hedDNS[i].hostName, commandArgs[1]) == 0) 
			{
				consoleWindow->Anim_DisplayText("Resolved [%s] to [%s].\n",commandArgs[1], hedDNS[i].hostIP);
				resolved = 1;
			}

			if(strcmp(hedDNS[i].hostIP, commandArgs[1]) == 0)
			{
				consoleWindow->Anim_DisplayText("Resolved [%s] to [%s].\n",commandArgs[1], hedDNS[i].hostName);
				resolved = 1;
			}
		}

		if(resolved == 0) consoleWindow->Anim_DisplayText("Unable to resolve %s.\n",commandArgs[1]);
	}
	
	// ================================================================================================================================================================================================
	// COMMAND : PING
	if(command == COMMAND_PING)
	{
		int index,dns_index;

		index = level->GetServerIndex(commandArgs[1]);

		if(strlen(commandArgs[1]) == 0)
		{
			consoleWindow->Anim_DisplayText("/c2Error. No hostname specified. Usage: PING hostname\n");
			commandType = COMMAND_NONE;
			return;
		}

		if(strlen(commandArgs[1]) == 0)
		{
			consoleWindow->Anim_DisplayText("/c2Error. Server not found: %s\n",commandArgs[1]);
			commandType = COMMAND_NONE;
			return;
		}

		// Ping
		dns_index = level->GetServerDNSIndex(commandArgs[1]);
		if(dns_index == -1)
		{
			consoleWindow->Anim_DisplayText("/c2Error. Unable to resolve %s.\n",commandArgs[1]);
			commandType = COMMAND_NONE;
			return;
		}
		
		consoleWindow->Anim_DisplayText("PING %s (%s) 64 bytes of data.\n",commandArgs[1],hedDNS[dns_index].hostIP);

		consoleWindow->Anim_DisplayText("64 bytes from (%s) icmp_req=1 ttl=128 time=%2.1f ms\n",hedDNS[dns_index].hostIP,(float)sysRandomNumber(99) / 10.0f);
		consoleWindow->Anim_DisplayText("64 bytes from (%s) icmp_req=1 ttl=128 time=%2.1f ms\n",hedDNS[dns_index].hostIP,(float)sysRandomNumber(99) / 10.0f);
		consoleWindow->Anim_DisplayText("64 bytes from (%s) icmp_req=1 ttl=128 time=%2.1f ms\n",hedDNS[dns_index].hostIP,(float)sysRandomNumber(99) / 10.0f);

		consoleWindow->Anim_DisplayText("completed.\n");
	}

	// ================================================================================================================================================================================================
	// COMMAND : KILLTRACE
	if(command == COMMAND_KILLTRACE)
	{
		// Is there any trace to kill?
		if(level->m_ServerList[level->m_ServerLocal].m_TraceLevel == 0.0f)
		{
			consoleWindow->Anim_DisplayText("Your trace level is 0. There is no need to use KILLTRACE.\n");
			commandType = COMMAND_NONE;
			return;
		}

		// Player has enough money?
		if(level->m_ServerList[level->m_ServerLocal].m_Money < 1000.0f)
		{
			consoleWindow->Anim_DisplayText("/c2Error. Not enough money to use KILLTRACE.\n");
			consoleWindow->Anim_DisplayText("You need at least $1000 to reduce your trace level by 10%%.\n");
			commandType = COMMAND_NONE;
			return;
		}
		
		// Execute killtrace
		level->m_ServerList[level->m_ServerLocal].m_Money      -= 1000;
		DECREMENT_WITH_CAP(level->m_ServerList[level->m_ServerLocal].m_TraceLevel,10,0);

		consoleWindow->Anim_DisplayText("Killtrace in progress .......... done!\n");
		consoleWindow->Anim_DisplayText("Trace level reduced to %3.2.\n",level->m_ServerList[level->m_ServerLocal].m_TraceLevel);		
	}
	// ================================================================================================================================================================================================
	// COMMAND : CRACK
	if(command == COMMAND_CRACK)
	{
		int index = level->GetServerIndex(commandArgs[1]);

		// Is there a parameter?
		if(index == -1)
		{
			consoleWindow->Anim_DisplayText("/c2Error: server not found (%s)\n",commandArgs[1]);
			consoleWindow->Anim_DisplayText("USAGE: CRACK SERVER \n");
			consoleWindow->Anim_DisplayText("\n");
			consoleWindow->Anim_DisplayText("CRACK will reduce the Firewall and Integrity level of a server by 1 point.\n");
			consoleWindow->Anim_DisplayText("Your trace level will increase by 5 percent when using the CRACK tool.\n");

			commandType = COMMAND_NONE;
			return;
		}
		
		// Make sure firewall > 1
		if(level->m_ServerList[index].m_LevelFirewall == 0)
		{
			consoleWindow->Anim_DisplayText("Warning: server firewall level is already 0.\n");
		}

		// Exec crack (this should last about 20 seconds. Why the text length).
		consoleWindow->Anim_DisplayText("Running CRACK tool on %s\n",commandArgs[1]);
		for(i = 0; i < 20; i++)
			consoleWindow->Anim_DisplayText("[depth:%2d]............................................................\n",i);
		consoleWindow->Anim_DisplayText("Completed. Your trace level has increased by 5 percent.\n");

		// Adjust variables
		DECREMENT_WITH_CAP(level->m_ServerList[index].m_LevelFirewall,1,0);
		DECREMENT_WITH_CAP(level->m_ServerList[index].m_LevelIntegrity,1,0);
		INCREMENT_WITH_CAP(level->m_ServerList[level->m_ServerLocal].m_TraceLevel,5,100);

		commandType = COMMAND_NONE;
		return;
	}
	// ================================================================================================================================================================================================
	// CHEAT : NEXTLEVEL
	if(command == COMMAND_CHEAT_NEXTLEVEL)
	{
        level->m_Quit = 2;
	}

    // ================================================================================================================================================================================================
	// CHEAT : SHOWALL
	if(command == COMMAND_CHEAT_SHOWALL)
	{
        int i;
        for(i = 0; i < level->m_ServerCount; i++)
            level->m_ServerList[i].m_Visible = 1;
	}
    
    // ================================================================================================================================================================================================
	// CHEAT : SHOW
	if(command == COMMAND_CHEAT_SHOW)
	{
        int index;

        index = level->GetServerIndex(commandArgs[1]);
        if(index > -1) level->m_ServerList[index].m_Visible = 1;
	}

	// Done
	commandType = COMMAND_NONE;
}

// ====================================================================================================================================================================================================
// Add a command to the dictionary
void hedCommandConsole::Dictionary_Add(char* word)
{
    strcpy(Dictionary[dictionaryCount],word);
    dictionaryCount++;
}
// ====================================================================================================================================================================================================
// Add a command to the dictionary
char* hedCommandConsole::Dictionary_Match(char* word, int index)
{
    int i;
    int _index = 0;

    sysLogPrintDebug("hedCommandConsole::Dictionary_Match(%s,%d)",word,index);

    for(i = 0; i < dictionaryCount; i++)
    {
        if(strncmp(word,Dictionary[i],strlen(word)) == 0)
        {
            if(index == _index)
                return Dictionary[i];
            else
                _index++;
        }
    }
    sysLogPrintDebug("hedCommandConsole::Dictionary_Match(%s) - NO MATCH.",word);

    return 0;
}
// ====================================================================================================================================================================================================
// Reset dictionary on level load (clear server names)
void hedCommandConsole::Dictionary_Reset()
{
	dictionaryCount = baseDictionaryCount;
}