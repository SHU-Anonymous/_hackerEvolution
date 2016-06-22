// HackerEvolutionDefines.cpp
// 
// Global game defines
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "HackerEvolutionDefines.h"
#include "HackerEvolutionUntold_Defines.h"
#include "moduleRender.h"
#include "heLog.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <io.h>
#endif

// * Global variables
int gFocusLost = 0;
int gModHacked = 0;
int g_screen_width;
int g_screen_height;

int quit = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Global variables
double     gFrameTime;

hestring   gSerialNumber;
int        gDinamicDifficulty;
int        gFullScreen;
int        gDemo;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Current mod info
// * mod info
hestring gModPath;
hestring gModFolder;
hestring gModName;
hestring gModDescription;
hestring gModLevelList[HE_MAX_MOD_LEVELS];
int      gModLevelCount;
int      gModLevelCurrent;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Current skin info
hestring   gSkinName;
gSkinColor gSkinColorHostHacked;
gSkinColor gSkinColorHost;
gSkinColor gSkinColorTargetMap;
gSkinColor gSkinColorProgress;
gSkinColor gSkinColorBouncedLink;
gSkinColor gSkinColorCommandPrompt;
gSkinColor gSkinColorWindowText;
gSkinColor gSkinColorHostNameBase;
gSkinColor gSkinColorProgressBar;
gSkinColor gSkinColorConsoleHighlight;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc variables
int  gDirectXLost = 0;     // * This is set to one, when DirectX lost focus
long gSoundVolume = 1;     // * Global volume level
long gSoundFXVolume = 1;   // * Global fx volume level

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load current skin colors
void loadSkinColors()
{
	FILE* file;
	char temp[2048];

	file = fopen(createSkinPath("colors.txt"),"rt");
    gApplicationLog.printf("\nloadSkinColors() - [%s]",createSkinPath("colors.txt"));
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorHostHacked.r,&gSkinColorHostHacked.g,&gSkinColorHostHacked.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorHost.r,&gSkinColorHost.g,&gSkinColorHost.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorTargetMap.r,&gSkinColorTargetMap.g,&gSkinColorTargetMap.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorProgress.r,&gSkinColorProgress.g,&gSkinColorProgress.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorBouncedLink.r,&gSkinColorBouncedLink.g,&gSkinColorBouncedLink.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorCommandPrompt.r,&gSkinColorCommandPrompt.g,&gSkinColorCommandPrompt.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorWindowText.r,&gSkinColorWindowText.g,&gSkinColorWindowText.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorHostNameBase.r,&gSkinColorHostNameBase.g,&gSkinColorHostNameBase.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorProgressBar.r,&gSkinColorProgressBar.g,&gSkinColorProgressBar.b);
	fgets(temp,2048,file); sscanf(temp,"%d %d %d",&gSkinColorConsoleHighlight.r,&gSkinColorConsoleHighlight.g,&gSkinColorConsoleHighlight.b);
	fclose(file);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc utility functions
void stripNewLine(char* pString)
{
	if(strlen(pString) == 0)
	{
		printf("\nutilStripNewLine 0\n");
		return;
	}

	for(int i = 0; i < strlen(pString); i++)
		if(pString[i] == '\n' || pString[i] == '\r') pString[i] = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Strip spaces from the begining and end of a string
void stripSpaces(char* pDest, char *pSrc)
{
	char* tmp;
	char* tmp2;

	tmp  = pSrc;
	tmp2 = pDest;

	while(*tmp == ' ') tmp++;
	while(*tmp != ' ' && *tmp != 0 && *tmp != '\n') {*tmp2 = *tmp; tmp++; tmp2++;}
	*tmp2 = 0;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Counts the words in a string
int  wordCount(char* pSrc)
{
	char* tmp;
	int   count = 0;

	if(strlen(pSrc) == 0) return 0;
	
	tmp = pSrc;
	
	while(*tmp != 0 && *tmp != '\n')
	{
		if(*tmp == ' ') count++;
		tmp++;
	}
	count++;

	return count;
}
// * Create mod file path
void createModPath(char* pPath, char* pFileName)
{
	sprintf(pPath,"%s/%s/%s",gModPath,gModFolder,pFileName);
}
// * Check if a point is inside a rectangle
int  PointInRect(int pX, int pY, int pLeft, int pTop, int pWidth, int pHeight)
{

#ifdef HE_MOUSE_AREA_HIGHLIGHT
	render_DrawRectangle_Empty(pLeft,pTop,pWidth,pHeight,255,255,255);
#endif

	if(pX > pLeft && pX < (pLeft + pWidth) && pY > pTop && pY < (pTop + pHeight))
		return 1;
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Power of function
int  power_of(int pBase, int pPower)
{
	int i,result = 1;

	if(pBase < 0) return 0;
	if(pPower < 0) return 0;

	for(i = 0; i < pPower; i++) result *= pBase;

	return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Finds wich power of 2 is closest to pNumber
int  _nearestPowOf2(int pNumber)
{
    int pow     = 1;
    int nearest = 1;

    while(pow < pNumber)
    {
        pow *= 2;
        nearest++;
    }

    nearest--;
    if(nearest <= 1) nearest = 1;

    return nearest;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Create mod file path
char* createModFilePath(char* pFileName)
{
	static hestring path;

	sprintf(path,"%s/%s",gModFolder,pFileName);
	return path;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Create level file path
char* createModLevelFilePath(char* pFileName)
{
	static hestring path;

	sprintf(path,"%s/%s/%s",gModFolder,gModLevelList[gModLevelCurrent],pFileName);
	return path;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Create skin path
char* createSkinPath(char* pFileName)
{
	static hestring path;

              sprintf(path,"heuntold-skins/%s/%s",gSkinName,pFileName);
	return path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Check if a file exists, and has the specified size
int heCheckFile(char* fileName, int size)
{
    FILE* file;
    unsigned long _checksum = 0;
    int chr = 0;
    int count = 0;

    file = fopen(fileName,"rb");

    while(chr != EOF)
    {
        chr = fgetc(file);
        count++;
    }

    fclose(file);

    if(count == size)
    	return 1;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * password encryption and decryption
void encryptPassword(char* password)
{
	char temp[128],temp2[128];
	int  i;
	
	strcpy(temp,password);

	temp2[0] = 'X';
	temp2[1] = 'E';
	temp2[2] = 'C';

	for(i = 0; i < strlen(temp); i++)
	{
		temp2[i + 3] = 'z' - (temp[i] - 'a');
	}
	
	temp2[i + 3] = 0;

    gApplicationLog.printf("PWD : %s",temp2);
}
void decryptPassword(char* password)
{
	char temp[128];
	int  i;

	// Check if this is an encrypted password
	if(password[0] == 'X' && password[1] == 'E' && password[2] == 'C')
	{
		for(i = 0; i < strlen(password) - 3; i++)
			temp[i] = 'z' - (password[i + 3] - 'a');

		temp[i] = 0;

		strcpy(password,temp);
        //gApplicationLog.printf("\nDECRYPTED : %s",password);
	}

    //gApplicationLog.printf("\nDECRYPTED : %s",password);
}

unsigned long fileCheckSum(char* filename)
{
    FILE* file;
    unsigned long _checksum = 0;
    int chr = 0;
    int count = 0;

    file = fopen(filename,"rb");

    while(chr != EOF)
    {
        chr = fgetc(file);
        count++;
        if(count == 10) count = 1;

        _checksum += (chr * count);
    }

    fclose(file);

    return _checksum;
}

unsigned long fileSizeBytes(char* filename)
{
    FILE* file;
    unsigned long _checksum = 0;
    int chr = 0;
    int count = 0;

    file = fopen(filename,"rb");

    while(chr != EOF)
    {
        chr = fgetc(file);
        count++;
    }

    fclose(file);

    return count;
}
