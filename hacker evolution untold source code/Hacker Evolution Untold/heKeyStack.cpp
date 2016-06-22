// heKeyStack.cpp
// 
// Key stack management class
// This handles all keyboard input from the console
//
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "heKeyStack.h"
#include "bshInput.h"
#include "heSystem.h"
#include "heLog.h"
#include "exoUtil.h"
#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Global dictionaries
heDictionary gDictionaryCommands;
heDictionary gDictionaryHostNames;
heDictionary gDictionaryFileNames;

heDictionary gDictionaryTemp;

// =============================================================================================================================
// Class constructor
heKeyStack::heKeyStack()
{
	this->reset();

	// * initialize command history buffer
    mCommandHistoryCount = 0;
    mCommandHistoryIndex = 0;

	mLastKey = 0;
	mLastMatchIndex = 0;
	strcpy(mLastPartialWord,"");
}

// =============================================================================================================================
// Class destructor
heKeyStack::~heKeyStack()
{
}

// =============================================================================================================================
// Add a key to the buffer
void heKeyStack::addKey(int key, heGameWindow* pWindow)
{
	int isChar = 1;

	// * HEKEY_UP, scroll up in the command history
	if(key == HEKEY_UP)
	{
		isChar = 0;

		// * save current command for later
		if(mCommandHistoryIndex == mCommandHistoryCount  && mCommandHistoryCount != 0)
			strcpy(mCommandTemp,m_keybuffer);

		// * scroll up in the command history		
		if(mCommandHistoryIndex >= 1) mCommandHistoryIndex--;

		gApplicationLog.printf("\nKEY_UP : %d",mCommandHistoryIndex);
		if((mCommandHistoryCount > 0))
		{
			strcpy(m_keybuffer,mCommandHistory[mCommandHistoryIndex]);
			m_keypos = strlen(m_keybuffer);			
		}
	}

	// * HEKEY_DOWN, scroll down in the command history
	if(key == HEKEY_DOWN)
	{
		isChar = 0;

		// * scroll down in the command history		
		if(mCommandHistoryIndex < mCommandHistoryCount - 1 && mCommandHistoryCount != 0)
		{
			mCommandHistoryIndex++;
			
			gApplicationLog.printf("\nKEY_DOWN : %d",mCommandHistoryIndex);
			if((mCommandHistoryCount >= 0))
			{
				strcpy(m_keybuffer,mCommandHistory[mCommandHistoryIndex]);
				m_keypos = strlen(m_keybuffer);			
			}
		}
		else
		{
			// * back to the originally typed command
			strcpy(m_keybuffer,mCommandTemp);
			m_keypos = strlen(m_keybuffer);
			gApplicationLog.printf("\nrestore original command line [%s]",m_keybuffer);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// * HEKEY_TAB, command autocompletion
	if(key == HEKEY_TAB)
	{		
		char     *mMatch;
		hestring word;
		hestring newCmdBuffer;
		int      startPos;
		int      newCmdPos;
		int      i,j;
		int      wordIndex;

		//* Special case, TAB is pressed repeatedly, to scroll through several options

		//* Don't allow command completion in the middle of a word
		if((m_keybuffer[m_keypos] != ' ') && (m_keybuffer[m_keypos] != 0))
		{
		}
		else
		{
			// find the word the player is currently typing in
			i = m_keypos - 1;
			while((m_keybuffer[i] != ' ') && (i > 0))i--;

			// See where the word is extracted from
			wordIndex = i;

			// skip space
			if(m_keybuffer[i] == ' ') i++;
			startPos = i;

			// extract word;
			for(j = 0;i < m_keypos;)
			{
				word[j] = m_keybuffer[i];
				i++;
				j++;
			}
			word[j] = 0;

			// * Try to match word in all dictionaries
			int found = 0;
			mMatch = 0;

			// * Save word, for repeated TAB presses
			if(mLastKey != HEKEY_TAB)
			{
				strcpy(mLastPartialWord,word);
				mLastMatchIndex = 0;

				// * Create a temp dictionary
				int i;
				gDictionaryTemp.Reset();

				// Do not add commands if the player has typed a word(command),
				// followed by a space
				if(strstr(m_keybuffer," ") != 0)
				{
				}
				else
				{
					for(i = 0; i < gDictionaryCommands.mDictionaryCount; i++)
						gDictionaryTemp.AddWord(gDictionaryCommands.mDictionary[i]);
				}

				for(i = 0; i < gDictionaryHostNames.mDictionaryCount; i++)
					gDictionaryTemp.AddWord(gDictionaryHostNames.mDictionary[i]);
				for(i = 0; i < gDictionaryFileNames.mDictionaryCount; i++)
					gDictionaryTemp.AddWord(gDictionaryFileNames.mDictionary[i]);			
			}
			else
			{
				mLastMatchIndex++;
			}

			// * Only look in the command dictionary for the first word
			/*
			if(mMatch == 0)
			if(wordIndex == 0 || mLastKey == HEKEY_TAB) 
			{
				mMatch = gDictionaryCommands.GetMatch(mLastPartialWord,mLastMatchIndex);
			}

			if(mMatch == 0) mMatch = gDictionaryHostNames.GetMatch(mLastPartialWord,mLastMatchIndex);
			if(mMatch == 0) mMatch = gDictionaryFileNames.GetMatch(mLastPartialWord,mLastMatchIndex);
			*/
			if(mMatch == 0) mMatch = gDictionaryTemp.GetMatch(mLastPartialWord,mLastMatchIndex);
			gApplicationLog.printf("\nTAB : [%s][%d]->[%s]",mLastPartialWord,mLastMatchIndex,mMatch);
			
			// * Reset index, to scroll through matching words
			// mLastMatchIndex = mLastMatchIndex % (gDictionaryTemp.GetMatchCount(mLastPartialWord) - 1);
			// if(mLastMatchIndex = gDictionaryTemp.GetMatchCount(mLastPartialWord)) mLastMatchIndex = -1;
			//if(mMatch == 0 && mLastMatchIndex > 0)
			//	mLastMatchIndex = -1;

			if(mMatch != 0)
			{
				//
				if(mLastMatchIndex != 0)
				{
					startPos--;
					startPos--;
					while(m_keybuffer[startPos] != ' ' && startPos > 0) startPos--;

					if(startPos != 0) startPos++;
				}

				// Fill in matching word;
				for(i = 0; i < startPos; i++)
					newCmdBuffer[i] = m_keybuffer[i];

				for(j = 0; j < strlen(mMatch);)
				{
					newCmdBuffer[i] = mMatch[j];
					i++;
					j++;
				}

				// Append a space at the end of the completed word, if there isn't one
				if(m_keybuffer[m_keypos] != ' ') 
				{
					newCmdBuffer[i] = ' '; 
					i++;
				}

				newCmdPos = i; // move cursor to the end of the completed word

				for(j = m_keypos; j < strlen(m_keybuffer);)
				{
					newCmdBuffer[i] = m_keybuffer[j];
					i++;
					j++;
				}
				newCmdBuffer[i] = 0;
				strcpy(m_keybuffer,newCmdBuffer);
				m_keypos        = newCmdPos;
			}
		}

	}

	// * KEY_PGUP, scroll up window
	if(key == HEKEY_PGUP)
	{
		isChar = 0;
		pWindow->scrollUp();
	}

	// * KEY_PGDN
	if(key == HEKEY_PGDN)
	{
		isChar = 0;
		pWindow->scrollDown();
	}

	// KEY_HOME
	if(key == HEKEY_HOME)
	{
		isChar = 0;
		m_keypos = 0;
	}

	// KEY_END
	if(key == HEKEY_END)
	{
		isChar = 0;
		m_keypos = strlen(m_keybuffer);
	}

	// KEY_LEFT
	if(key == HEKEY_LEFT)
	{
		isChar = 0;
		if(m_keypos > 0) m_keypos--;
	}

	// KEY_RIGHT
	if(key == HEKEY_RIGHT)
	{
		isChar = 0;
		if(m_keypos < strlen(m_keybuffer)) m_keypos++;
	}

	// KEY_ENTER
	if(key == HEKEY_ENTER)
	{
		isChar = 0;

		// Disregard, if there is no command entered
		if(strlen(m_keybuffer) > 0)
			m_commandCompleted = 1;
	}

	// KEY_DEL
	if(key == HEKEY_DEL)
	{
		isChar = 0;
		for(int i = m_keypos; i < MAX_COMMANDLINE_CHARS - 1; i++)
			m_keybuffer[i] = m_keybuffer[i + 1];
		m_keybuffer[MAX_COMMANDLINE_CHARS - 1] = 0;
	}

	// KEY_BACKSPACE
	if(key == HEKEY_BACKSPACE)
	{
		isChar = 0;
		
		if(m_keypos > 0)
		{
			m_keybuffer[m_keypos - 1] = 0;
			m_keypos--;

			// shift all chars left
			for(int i = m_keypos; i < MAX_COMMANDLINE_CHARS - 1; i++) m_keybuffer[i] = m_keybuffer[i + 1];
		}
	}

	// * Make sure it's not some other special character
	if(key < 0) isChar = 0;

	// If it's a character, add it to the buffer
	if(isChar == 1 && m_keypos < MAX_COMMANDLINE_CHARS) 
	{
		// * reset scroll
		pWindow->scrollReset();

		// shift all chars right
		for(int i = MAX_COMMANDLINE_CHARS - 1; i >= m_keypos; i--) m_keybuffer[i] = m_keybuffer[i - 1];

		m_keybuffer[m_keypos] = key;
		m_keypos++;
	}

	//* Save key, to be able to handle repeated TAB presses
	mLastKey = key;

}

// =============================================================================================================================
// Get buffer content
char* heKeyStack::getBuffer()
{
	return m_keybuffer;
}

// =============================================================================================================================
// Get blinking cursor
char* heKeyStack::getDisplayCommand()
{
	//strcpy(m_cursor,"");
	///if(system_TimeMilliSeconds() % 1000 > 500) strcpy(m_cursor,"|");
	int i;
	for(i = 0; i < MAX_COMMANDLINE_CHARS + 1; i++) m_keybufferDisplay[i] = 0;

	for(i = 0; i < m_keypos; i++) m_keybufferDisplay[i] = m_keybuffer[i];

	if(system_TimeMilliSeconds() % 1000 > 500)
		m_keybufferDisplay[i] = '|';
	else
		m_keybufferDisplay[i] = ' ';
	for(i = m_keypos; i < MAX_COMMANDLINE_CHARS; i++) m_keybufferDisplay[i + 1] = m_keybuffer[i];

	return m_keybufferDisplay;
}

// =============================================================================================================================
// Return command (line) if any
char* heKeyStack::getCommand()
{
	if(m_commandCompleted == 1)
		return m_keybuffer;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Reset command history
void heKeyStack::reset()
{
	int i;


	// * save the command, in the command history
	if(mCommandHistoryCount < HE_MAX_COMMANDHISTORY)
	{
		strcpy(mCommandHistory[mCommandHistoryCount],m_keybuffer);
		mCommandHistoryCount++;
	}
	else
	{
		for(int i = 0; i < HE_MAX_COMMANDHISTORY; i++)
			strcpy(mCommandHistory[i],mCommandHistory[i + 1]);

		strcpy(mCommandHistory[HE_MAX_COMMANDHISTORY - 1],m_keybuffer);
		mCommandHistoryCount = HE_MAX_COMMANDHISTORY;
	}
	mCommandHistoryIndex = mCommandHistoryCount;

	// * reset key stack
	m_keypos           = 0;
	m_commandCompleted = 0;

	for(i = 0; i < MAX_COMMANDLINE_CHARS; i++)
		m_keybuffer[i] = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Clear command history
void heKeyStack::clearCommandHistory()
{
	strcpy(m_keybuffer,"");
	m_keypos = strlen(m_keybuffer);			

    mCommandHistoryCount = 0;
    mCommandHistoryIndex = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Add a word to the command autocompletion dictionary
void heDictionary::AddWord(char* pWord)
{
	if(mDictionaryCount < HE_MAX_DICTIONARY)
	{
		strcpy(mDictionary[mDictionaryCount],pWord);
		mDictionaryCount++;
	}
	else
		gApplicationLog.printf("\nERROR : Command dictionary full (%s)",pWord);

	//gApplicationLog.printf("\nheDictionary::AddWord() - %s",pWord);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Find a match in the dictionary, for a partial word
char* heDictionary::GetMatch(char* pWord, int index)
{
	int i;
	int matchIndex = 0;
	int lastIndex  = -1;

	if(strlen(pWord) == 0) return 0;

	for(i = 0; i < mDictionaryCount; i++)
	{
		if(strncmp(mDictionary[i],pWord,strlen(pWord)) == 0)
		{
			if(matchIndex == index)
			{
				return mDictionary[i];
			}
			else
			{
				lastIndex = i;
				matchIndex++;
			}
		}

	}

	if(lastIndex == -1)
		return 0;
	else
		//return 0;
		return mDictionary[lastIndex];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Find the number of matches in the dictionary, for a partial word
int heDictionary::GetMatchCount(char* pWord)
{
	int count = 0;
	int i;
	int matchIndex = 0;
	int lastIndex  = -1;

	if(strlen(pWord) == 0) return 0;

	for(i = 0; i < mDictionaryCount; i++)
		if(strncmp(mDictionary[i],pWord,strlen(pWord)) == 0)
			count++;

	return count;
}


