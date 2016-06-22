/*
Name: hedGameLanguage.cpp
Desc: Language localization support

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#include "../generic - inc/hedGameLanguage.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedSystem.h"
#include <stdio.h>
#include <string.h>

const char no_translation[] = "UNDEFINED: ";

hedGameLanguage g_GameLanguage;                   

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Class contructor
hedGameLanguage::hedGameLanguage()
{
	// Initializations
	languageCount   = 0;
	stringCount     = 0;
	currentLanguage = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Class destructor
hedGameLanguage::~hedGameLanguage()
{
	for(int i = 0; i < stringCount; i++)
		delete stringList[i].Translations;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Load and parse the language file
void hedGameLanguage::LoadLanguageFile(char* languageFileName)
{
	FILE*     file;
	hedString line;
	int       i,count;

    sysLogPrint("hedGameLanguage::LoadLanguageFile(%s) - Loading language localizations...",languageFileName);

	// Load language list
	int   languagesLoaded = 0;
	char* eof;

	file = fopen(languageFileName,"rt");
	while(fgets(line,1024,file) && languagesLoaded == 0)
	{
		utilStripNewLine(line);
		if(strcmp(line,"[LANGUAGES]") == 0)
		{
			do
			{
				eof = fgets(line,1024,file);
				utilStripNewLine(line);
				if(strcmp(line,"[/LANGUAGES]") == 0)
					languagesLoaded = 1;
				else
				{
					strcpy(this->languageList[this->languageCount],line);
					this->languageCount++;

                    sysLogPrint("hedGameLanguage::LoadLanguageFile(%s) - Found new language: %s",languageFileName,line);
				}
			}while(languagesLoaded == 0 && eof != 0);
		}
	}

	if(languagesLoaded == 0)
        sysLogPrint("hedGameLanguage::LoadLanguageFile(%s) - ERROR. Failed to load language list",languageFileName);
    else
        sysLogPrint("hedGameLanguage::LoadLanguageFile(%s) - Language list loaded.",languageFileName);

	// Load string translations
	do{
		eof = fgets(line,1024,file);
		utilStripNewLine(line);
		
		// [STRING] tag found?
		if(strcmp(line,"[STRING]") == 0)
		{
			stringList[stringCount].Translations = new hedString[languageCount];
			count = 0;

			// Load translations
			for(i = 0; i < languageCount; i++)
			{
				eof = fgets(line,1024,file);
				utilStripNewLine(line);

				strcpy(stringList[stringCount].Translations[count],line);
				if(eof == 0 || strcmp(line,"[/STRING]") == 0)
				{
                    sysLogPrint("hedGameLanguage::LoadLanguageFile(%s) - ERROR: While loading string: [%s]",languageFileName,stringList[stringCount].Translations[0]);
					eof = 0; // force exit
				}
				else
					count++;
			}
            sysLogPrintDebug("hedGameLanguage::LoadLanguageFile(%s) - [%d][%s][%s]",languageFileName,stringCount,stringList[stringCount].Translations[0],stringList[stringCount].Translations[1]);
			stringCount++;
		}
	}while(eof != 0);

    sysLogPrint("hedGameLanguage::LoadLanguageFile(%s) - Finished loading language file. %d strings loaded",languageFileName,stringCount);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// * Set the currently active language
void hedGameLanguage::SetCurrentLanguage(char* language)
{
	int i,ok = 0;

	for(i = 0; i < languageCount; i++)
		if(strcmp(languageList[i],language) == 0)
		{
			currentLanguage = i;
			ok = 1;
            sysLogPrint("hedGameLanguage::SetCurrentLanguage(%s) - Language set to: %s",language,language);
		}

	if(ok == 0)
        sysLogPrint("hedGameLanguage::SetCurrentLanguage(%s) - ERROR. Failed to set language set to: %s",language,language);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Advance to the next language
void hedGameLanguage::SetCurrentLanguageNext()
{
	if(languageCount > 0)
	{
		currentLanguage++;
		if(currentLanguage == languageCount) currentLanguage = 0;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Retrieve currently active language
char* hedGameLanguage::GetCurrentLanguage()
{
	if (languageCount == 0) 
		return 0;
	else 
		return languageList[currentLanguage];
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Retrieve translation for a certain string
char* hedGameLanguage::GetLanguageTranslation(const char* string)
{
	int   i;
	char* translation = 0;

	for(i = 0; i < stringCount; i++)
		if(strcmp(stringList[i].Translations[0],string) == 0)
		{
			return stringList[i].Translations[currentLanguage];
		}
	
    //sysLogPrint("hedGameLanguage::GetLanguageTranslation(%s) - Failed to find translation",string);
    //printf("hedGameLanguage::GetLanguageTranslation(%s) - Failed to find translation",string);

	return (char*)&no_translation[0];
}

