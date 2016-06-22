// heutGameLanguage.h
// 
// Language and localization management class
// Code : Robert Muresan (mrobert@exosyphen.com, exosyphen studios)
//
#ifndef _heutLanguage_h_
#define _heutLanguage_h_

#include "HackerEvolutionUntold_Defines.h"
#include "heLog.h"

#define HE_MAX_LANGUAGES 32
#define HE_MAX_STRINGS   512

#define GAMETEXT(x) gameLanguage->GetLanguageTranslation(x)

// * Hardware names for UPGRADE COMMAND
static char* HE_NAME_CPU[]      = {"1 Ghz CPU","2 Ghz CPU","4 Ghz CPU","8 Ghz CPU"};
static char* HE_NAME_MODEM[]    = {"1 Mbps Modem","2 Mbps Modem","4 Mbps Modem","8 Mbps Modem"};
static char* HE_NAME_FIREWALL[] = {"(none)","class 1 firewall","class 2 firewall","class 3 firewall"};
static char* HE_NAME_MEM[]      = {"1 Gb QRam","2 Gb QRam","4 Gb QRam","8 Gb QRam"};

// * exosyphen command (easter-egg)
#define LANG_TEXT_CMD_EXOSYPHEN "\nexosyphen studios made this game :-) (www.exosyphen.com, support@exosyphenstudios.com)\n"

#define LANG_TEXT_CREDITS_1 "Hacker Evolution: Untold"
#define LANG_TEXT_CREDITS_2 "developed by exosyphen studios"
#define LANG_TEXT_CREDITS_DEV1 "- Programming and design: Robert Muresan"
#define LANG_TEXT_CREDITS_DEV2 "- Additional content: Dan McIvor"
#define LANG_TEXT_CREDITS_DEV3 "- French translation: Milazzo Christopher"
#define LANG_TEXT_CREDITS_DEV4 "- Soundtrack: DJ Velocity (www.dj-velocity.com) / Tony Bogardi"
#define LANG_TEXT_CREDITS_4 "website: http://www.exosyphen.com/"
#define LANG_TEXT_CREDITS_5 "e-mail:  support@exosyphenstudios.com"
#define LANG_TEXT_CREDITS_6 "Copyright © 2007-2008, exosyphen studios. All rights reserved."

#define LANG_TEXT_MODWARNING "Main game mod has been changed.\nPlease reinstall Hacker Evolution: Untold.\nOnline hiscore submission has been disabled."

typedef struct
{
	heString* Translations;
}heLanguageString;

class heGameLanguage
{
public:
	heGameLanguage();
	~heGameLanguage();

	void  LoadLanguageFile(char* languageFileName);
	void  SetCurrentLanguage(char* language);
	void  SetCurrentLanguageNext();
	char* GetCurrentLanguage();
	char* GetLanguageTranslation(char* string);

	int      languageCount;
	heString languageList[HE_MAX_LANGUAGES];

protected:
	heLog *languageLog;

	int              stringCount;
	heLanguageString stringList[HE_MAX_STRINGS];

	int              currentLanguage;
};

extern heGameLanguage* gameLanguage;

#endif