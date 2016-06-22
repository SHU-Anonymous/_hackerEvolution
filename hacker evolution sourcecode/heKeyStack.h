// heKeyStack.h
// 
// Key stack management class
// This handles all keyboard input from the console
//
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _heKeyStack_h_
#define _heKeyStack_h_

#include "HackerEvolutionDefines.h"
#include "heGameWindow.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Word completion dictionary
class heDictionary
{
public:

	// * Member functions
	heDictionary()
	{
		Reset();
	}

	void  AddWord(char* pWord);                 // Add a word to the command autocompletion dictionary
	char* GetMatch(char* pWord,int index = 0);  // Find a match in the dictionary, for a partial word
	void  Reset()                               // Reset dictionary
	{
		mDictionaryCount = 0;
	}

protected:

	// * command autocompletion dictionary
	hestring mDictionary[HE_MAX_DICTIONARY];
	int      mDictionaryCount;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Global dictionaries
extern heDictionary gDictionaryCommands;
extern heDictionary gDictionaryHostNames;
extern heDictionary gDictionaryFileNames;


class heKeyStack
{
public:
	
	heKeyStack();
	~heKeyStack();

	void  addKey(int key, heGameWindow* pWindow);
	char* getBuffer();
	char* getDisplayCommand();
	char* getCommand();
	void  reset();
	void  clearCommandHistory();

protected:

	hestring m_keybuffer;
	hestring m_keybufferDisplay;
	int  m_keypos;
	int  m_commandCompleted;
	unsigned char m_cursor[16];

	//* command history
	hestring mCommandHistory[HE_MAX_COMMANDHISTORY];
	int      mCommandHistoryCount;
	int      mCommandHistoryIndex;
	hestring mCommandTemp;

	int      mLastKey;
	int      mLastMatchIndex;
	hestring mLastPartialWord;
};

#endif
