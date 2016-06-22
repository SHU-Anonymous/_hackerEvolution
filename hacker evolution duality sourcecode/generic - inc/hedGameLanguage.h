/*
Name: hedGameLanguage.h
Desc: Language localization support

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#ifndef _hedGameLanguage_h_
#define _hedGameLanguage_h_

#include "HackerEvolutionDualityDefines.h"

#define HED_MAX_LANGUAGES 10
#define GAMETEXT(x)       g_GameLanguage.GetLanguageTranslation(x)

typedef struct
{
	hedString* Translations;
}heLanguageString;

//=======================================================================================================================================
// Class definition

class  hedGameLanguage;
extern hedGameLanguage g_GameLanguage;                   

class hedGameLanguage
{
public:
	hedGameLanguage();
	~hedGameLanguage();

	void  LoadLanguageFile(char* languageFileName);
	void  SetCurrentLanguage(char* language);
	void  SetCurrentLanguageNext();
	char* GetCurrentLanguage();
	char* GetLanguageTranslation(const char* string);

	int      languageCount;
	hedString languageList[HED_MAX_LANGUAGES];

protected:

	int              stringCount;
	heLanguageString stringList[256];
	int              currentLanguage;
};

#endif