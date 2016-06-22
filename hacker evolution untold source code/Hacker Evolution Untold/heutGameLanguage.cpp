// heutGameLanguage.cpp
// 
// Language and localization management class
// Code : Robert Muresan (mrobert@exosyphen.com, exosyphen studios)
//

#include "heutGameLanguage.h"
#include "HackerEvolutionDefines.h"
#include <stdio.h>
#include <string.h>

#if defined (PLATFORM_WINDOWS)
#include "stdafx.h"
#endif

heGameLanguage* gameLanguage;

//define TRIM_EOL(x) {if (strlen(x) > 0)(x[strlen(x) - 1] = 0);}
//2014 update
#define TRIM_EOL(x) stripNewLine(x)

const char no_translation[] = "/color 255 0 0 GAME ERROR: Unable to find language localization text. Please contact exosyphen studios.";

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Class contructor
heGameLanguage::heGameLanguage()
{
	// Initializations
	languageCount   = 0;
	languageLog     = new heLog("language.log");
	stringCount     = 0;
	currentLanguage = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Class destructor
heGameLanguage::~heGameLanguage()
{
	delete languageLog;

	for(int i = 0; i < stringCount; i++)
		delete stringList[i].Translations;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Load and parse the language file
void heGameLanguage::LoadLanguageFile(char* languageFileName)
{
	FILE*    file;
	heString line;
	int      i,count;

	languageLog->printf("\nLoading language file: %s",languageFileName);

	// Load language list
	int   languagesLoaded = 0;
	char* eof;

	file = fopen(languageFileName,"rt");
	while(fgets(line,1024,file) && languagesLoaded == 0)
	{
		TRIM_EOL(line);
		if(strcmp(line,"[LANGUAGES]") == 0)
		{
			do
			{
				eof = fgets(line,1024,file);
				TRIM_EOL(line);
				if(strcmp(line,"[/LANGUAGES]") == 0)
					languagesLoaded = 1;
				else
				{
					strcpy(this->languageList[this->languageCount],line);
					this->languageCount++;

					languageLog->printf("\nFound new language: %s",line);
				}
			}while(languagesLoaded == 0 && eof != 0);
		}
	}

	if(languagesLoaded == 0)
		languageLog->printf("\nERROR: Failed to load language list.");
	languageLog->printf("\n%d languages loaded",languageCount);

	// Load string translations
	do{
		eof = fgets(line,1024,file);
		TRIM_EOL(line);
		
		// [STRING] tag found?
		if(strcmp(line,"[STRING]") == 0)
		{
			stringList[stringCount].Translations = new heString[languageCount];
			count = 0;

			// Load translations
			for(i = 0; i < languageCount; i++)
			{
				eof = fgets(line,1024,file);
				TRIM_EOL(line);

				// DEBUG languageLog->printf("\n]] %s",line);
				strcpy(stringList[stringCount].Translations[count],line);
				if(eof == 0 || strcmp(line,"[/STRING]") == 0)
				{
					languageLog->printf("\nERROR: While loading string:%s",stringList[stringCount].Translations[0]);
					eof = 0; // force exit
				}
				else
					count++;
			}
			stringCount++;
		}
	}while(eof != 0);

	languageLog->printf("\nFinished loading language file. %d strings loaded",stringCount);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Set the currently active language
void heGameLanguage::SetCurrentLanguage(char* language)
{
	int i,ok = 0;

	for(i = 0; i < languageCount; i++)
		if(strcmp(languageList[i],language) == 0)
		{
			currentLanguage = i;
			ok = 1;
			languageLog->printf("\nLanguage set to: %s",language);
		}

	if(ok == 0)
		languageLog->printf("\nERROR: Failed to set language to: %s",language);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Advance to the next language
void heGameLanguage::SetCurrentLanguageNext()
{
	if(languageCount > 0)
	{
		currentLanguage++;
		if(currentLanguage == languageCount) currentLanguage = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Retrieve currently active language
char* heGameLanguage::GetCurrentLanguage()
{
	if (languageCount == 0) 
		return 0;
	else 
		return languageList[currentLanguage];
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Retrieve translation for a certain string
char* heGameLanguage::GetLanguageTranslation(char* string)
{
	int   i;
	char* translation = 0;

	for(i = 0; i < stringCount; i++)
		if(strcmp(stringList[i].Translations[0],string) == 0)
		{
			return stringList[i].Translations[currentLanguage];
		}
	
	languageLog->printf("\nERROR: Failed to find translation for: (%s)",string);

	return (char*)&no_translation[0];
}
