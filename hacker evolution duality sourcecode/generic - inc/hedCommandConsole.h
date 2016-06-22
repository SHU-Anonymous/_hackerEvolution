/*
Name: hedCommandConsole.h
Desc: Command console management class

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#ifndef _hedCommandConsole_h_
#define _hedCommandConsole_h_

#include "HackerEvolutionDualityDefines.h"
#include "hedGameLevel.h"
#include "hedGameEngine-ClassTextWindow.h"

#define CURSOR_BLINK_RATE 0.5f // Cursor blink rate in seconds

// ====================================================================================================================================================================================================
// Command constants
#define COMMAND_NONE     -1
#define COMMAND_INVALID   0
#define COMMAND_HELP      1
#define COMMAND_SPOILER   2
#define COMMAND_LS        3
#define COMMAND_CAT       4
#define COMMAND_NEXTLEVEL 5
#define COMMAND_SYSINFO   6
#define COMMAND_CLS       7
#define COMMAND_CLOSE     8

#define COMMAND_SCAN      9
#define COMMAND_BYPASS    10
#define COMMAND_DELETE    11
#define COMMAND_DNS       12
#define COMMAND_PING      13
#define COMMAND_KILLTRACE 14
#define COMMAND_CRACK     15

// Cheats
#define COMMAND_CHEAT_NEXTLEVEL   99
#define COMMAND_CHEAT_SHOWALL     100
#define COMMAND_CHEAT_SHOW        101

// ====================================================================================================================================================================================================
class hedCommandConsole
{
public:
	hedCommandConsole();    // Class constructor
	~hedCommandConsole();   // Class destructor

	void  Init(hedClassTextWindow* window);                                // Initialize the console
	void  AddCharacter(int character);                                     // Passes a character from the input, to the console
	char* GetCommandLine();                                                // Returns the content of the current command line
	int   GetCommand();                                                    // Returns the executed command

	void  ProcessCommandLine();                                            // Processes the commands in the command line (when ENTER is pressed)
	void  ExecCommand(int command, hedGameLevel* level, int serverLocked); // Execute a command

    // ** Command autocompletion dictionary
    void  Dictionary_Add(char* word);
    char* Dictionary_Match(char* word, int index = 0);
	void  Dictionary_Reset();                                              // Reset dictionary on level load (clear server names)

	int       commandCount;
	hedString commandArgs[5];
	int       commandType;

protected:
	hedClassTextWindow* consoleWindow;           // Window to display console command results

	char commandLine[1024];                      // Contents of the current command line
	char commandLineDisplay[1024];               // Formated command line (this is displayed in the game)
	int  cursorPos;

	float blinkTimer;                            // Cursor blink timer

	// ==========================================
    // Dictionary
    hedString Dictionary[1024];
    int       dictionaryCount;

    hedString dictionarySearch;
    char*     dictionaryMatch;
    int       lastIndex;
    int       tabIndex;

	int       baseDictionaryCount;               // This is the word count for the words that don't change from level to level (commands, parameters, etc).

    // Command history
    hedString commandHistory[1024];
    int       commandHistoryCount;
    int       commandHistoryIndex;
};

#endif