/*
Name: hedSystem.cpp
Desc: Operating system specific code (Win32)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#include "../generic - inc/hedSystem.h"
#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedPlayerProfile.h"
#include "../generic - inc/hedGameSettings.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_IOS)
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
timeval startTime;
#endif

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#include <shlobj.h>
#include <io.h>
#include "inet.h"
#endif

#if defined(PLATFORM_LINUX)
#include "dirent.h"
#endif

#if defined(PLATFORM_MACOS)
#include "MacOSSandboxHelper.h"
#endif

#ifdef PLATFORM_IOS
#import <QuartzCore/QuartzCore.h>
#endif

#ifdef BUILD_STEAM
#include "steam_api.h"
#endif

static FILE *g_LogFile, *g_ActivityLogFile, *g_AILogFile;

int _lastRandomNumber = 0;

char sysAppDataPath[1024];
char sysAppDataPathLast[1024];

//================================================================================================================================================
// Global stuff (used in HE and HEU)
int mouseX = 0;
int mouseY = 0;

int _mouseRBDown  = 0;
int _mouseLBDown  = 0;
int _mouseClick   = 0;
//int KeyList[300];
int keyCount = 0;
//int keyShiftDown = 0;

/*
int input_getLastKey()
{
	if(keyCount > 0)
	{
		int key = KeyList[0];

		for(int i = 0; i < keyCount - 1; i++)
			KeyList[i] = KeyList[i + 1];

		keyCount--;

		return key;
	}
	return 0;
}
*/

int mouseLBClick()
{
	if(_mouseClick == 1)
	{
		_mouseClick = 0;
		return 1;
	}
	else
		return _mouseClick;
}

long sysTimeSeconds()
{
	return sysTimeMilliseconds() / 1000;
}
//================================================================================================================================================


//=======================================================================================================================================
// Misc. system initializations
void sysInit()
{
    
    // Per system initializations

    // Application data folder
    strcpy(sysAppDataPath,"sys-hed-appdata/");

    /*
#if defined(PLATFORM_MACOS) && defined(TARGET_MACAPPSTORE)
    printf("\nSandbox path: %s",MacOSGetSandboxPath());
    sprintf(sysAppDataPath,"%s/",MacOSGetSandboxPath());
#endif
     */
#if defined(PLATFORM_MACOS)
    //sprintf(sysAppDataPath,"%s/",MacOSGetCurrentFolder());
#endif

#if defined(PLATFORM_WINDOWS)
    /*
    // Get application data folder
    TCHAR P[MAX_PATH];
    char appDataFolder[MAX_PATH];

    SHGetFolderPath(NULL,CSIDL_APPDATA|CSIDL_FLAG_CREATE,NULL,0,P);
    wcstombs(appDataFolder,P,MAX_PATH);
    strcat(appDataFolder,"\\exosyphenstudios\\");

    strcpy(sysAppDataPath,appDataFolder);
    */
#endif

#if defined(PLATFORM_LINUX)
    // Application data folder
    //strcpy(sysAppDataPath,"~/.hacker-evolution-duality");
#endif

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
	// Initialize .log files
	g_LogFile         = fopen(sysGetAppDataPath("HackerEvolutionDuality.log"),"wt");
	g_ActivityLogFile = fopen(sysGetAppDataPath("HED-Player-Activity.log"),"a");
	g_AILogFile       = fopen(sysGetAppDataPath("HED-AI.log"),"a");

	if(g_LogFile == NULL || g_ActivityLogFile == NULL || g_AILogFile == NULL)
	{
		printf("\nERROR: Unable to create log file. Running game from Read-Only media?");
	}

	sysActivityLogPrint("=== [START NEW SESSION] ===");

	sysLogPrint("(hedSystem.cpp) sysInit() - system init");
#endif
	
#if defined(PLATFORM_WINDOWS)
    // Initialize the random number generator
    srand(time(NULL));
#endif
	
#if defined(PLATFORM_WINDOWS) && defined (GAME_HACKEREVOLUTIONDUALITY)
	// Check if a keyboard is present
	if(GetSystemMetrics(SM_TABLETPC) == TRUE)
	{
		g_IsTabletPC = 1;
		sysLogPrint("(hedSystem.cpp) sysInit() - TabletPC detected");
	}
#endif
    
#if defined(PLATFORM_IOS)
    g_IsTabletPC = 1;
#endif

#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined (PLATFORM_IOS)
	gettimeofday(&startTime,NULL);
	srand(time(NULL));
#endif

//    sysLogPrint("(hedSystem.cpp) sysInit() - win32/application data path: [%s]",appDataFolder);
	sysLogPrint("(hedSystem.cpp) sysInit() - system init finished.");
}

//=======================================================================================================================================
//
void sysShutdown()
{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)	
	if(g_LogFile != NULL) fclose(g_LogFile);
	if(g_ActivityLogFile != NULL) fclose(g_ActivityLogFile);
	if(g_AILogFile != NULL) fclose(g_AILogFile);
#endif
}

//=======================================================================================================================================
// Returns a random number in a given range
int  sysRandomNumber(int range)
{
#if defined(PLATFORM_WINDOWS)
    LARGE_INTEGER frequency;
    LARGE_INTEGER ticksPerSecond;
        
    if(QueryPerformanceFrequency(&frequency))
    {
        QueryPerformanceCounter(&ticksPerSecond);
        //sysLogPrint("rand = %d(%d)[%d]",(int)(ticksPerSecond.QuadPart % 10000),(int)(ticksPerSecond.QuadPart % 300),rand()% 300);
        return (ticksPerSecond.QuadPart % range);
        //return (ticksPerSecond.LowPart % range);
    }
    else
#endif
        return (rand() % range);
}


//=======================================================================================================================================
// Returns the system time in milliseconds
long sysTimeMilliseconds()
{
#ifdef PLATFORM_WINDOWS	
    clock_t time;

    time = clock();
    time = (time);

    return time;
#endif
	
#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
	timeval  _time;
	long int elapsedTime;
	
	gettimeofday(&_time,NULL);	
	elapsedTime = (_time.tv_sec * 1000 + _time.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
	
	return elapsedTime;
#endif

#if defined(PLATFORM_IOS)
	timeval  _time;
	double   elapsedTime;
	
	gettimeofday(&_time,NULL);	
	elapsedTime = (_time.tv_sec * 1000 + _time.tv_usec / 1000) - (startTime.tv_sec * 1000 + startTime.tv_usec / 1000);
	
	return elapsedTime;
#endif    
}

//=======================================================================================================================================
// Print text to a log file
void sysLogPrint(char *format,...)
{
    char     buffer[1024];
    va_list  param;

    // Print the formated text, to a temporary buffer
    va_start(param,format);
    vsprintf(buffer,format,param);
    va_end(param);

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)    
    // Print the text to the log file
    fprintf(g_LogFile,"%s\n",buffer);
    printf("%s\n",buffer);

	// Flush the file
	fflush(g_LogFile);
#endif

#if defined(PLATFORM_IOS) || defined(PLATFORM_MACOS)
    //return;
    printf("[LOG]%s\n",buffer);
#endif
}

//=======================================================================================================================================
// Print text to a log file (extended debug)
void sysLogPrintDebug(char *format,...)
{
    char     buffer[1024];
    va_list  param;
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

#if defined(GAME_HACKEREVOLUTIONDUALITY)
	if(g_DebugLogOn == 0)
		return;
#endif

	// Print the formated text, to a temporary buffer
    va_start(param,format);
    vsprintf(buffer,format,param);
    va_end(param);

    // Print the text to the log file
    fprintf(g_LogFile,"[DEBUG]%s\n",buffer);
    printf("%s\n",buffer);

	// Flush the file
	fflush(g_LogFile);
#endif
}

//=======================================================================================================================================
// Print text to the AI log
void sysLogPrintAI(char *format,...)
{
    return;
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)	
#ifdef GAME_HACKEREVOLUTIONDUALITY
    char     buffer[1024];
    va_list  param;

	// Print the formated text, to a temporary buffer
    va_start(param,format);
    vsprintf(buffer,format,param);
    va_end(param);

    // Print the text to the log file
    fprintf(g_AILogFile,"[%5d]%s\n",(int)g_LevelTime,buffer);

	// Flush the file
	fflush(g_AILogFile);
#endif
#endif
}
//=======================================================================================================================================
// Print text to the activity log file
void sysActivityLogPrint(char *format,...)
{
    return;
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
	
    char     buffer[1024];
    va_list  param;
    
    // Print the formated text, to a temporary buffer
    va_start(param,format);
    vsprintf(buffer,format,param);
    va_end(param);

    // Print the text to the log file
    fprintf(g_ActivityLogFile,"%s\n",buffer);

	// Flush the file
	fflush(g_ActivityLogFile);
#endif
}

//=======================================================================================================================================
// Opens the game community URL in the default browser
void sysOpenCommunityURL()
{
#if defined(PLATFORM_WINDOWS)
	#ifndef BUILD_WOLFPACKRISK
		HINSTANCE result = ShellExecute(NULL, TEXT("open"), TEXT("http://www.hackerevolutionduality.com/index.php?sel=page-leaderboards.php&utm_source=ingame-windows&utm_medium=app&utm_campaign=HED-ingame-windows"), NULL, NULL, SW_SHOWNORMAL);
	#else
		HINSTANCE result = ShellExecute(NULL, TEXT("open"), TEXT("http://www.hackerevolutionduality.com/page-leaderboardswolfpackrisk.php"), NULL, NULL, SW_SHOWNORMAL);
	#endif
#endif
	
#if defined(PLATFORM_MACOS)
	#ifndef BUILD_WOLFPACKRISK
	system("open /Applications/Safari.app http://www.hackerevolutionduality.com/page_leaderboards.html");
	#else
	system("open /Applications/Safari.app http://www.hackerevolutionduality.com/page-leaderboardswolfpackrisk.php");
	#endif
#endif
}

//=======================================================================================================================================
// Submit Open World score to online leaderboards
int sysSubmitScore(char* nickname, char* level, int score, int speed, int attacks, int time)
{
#ifdef PLATFORM_WINDOWS
    hedString hash;
    hedString url;
    int       i;

    sprintf(hash,"%s %s %s %d %d %d %d",g_GameSettings.ValueStr("serial_number"),level,nickname,score,speed,attacks,time);
    //sysLogPrintDebug("sysSubmitScore() - content: %s",hash);

    // Encrypt
    for(i = 0; i < 1024; i++)
    {
        if(hash[i] >= 'a' && hash[i] <= 'z') hash[i] = 'a' + 'z' - hash[i];
        if(hash[i] >= 'A' && hash[i] <= 'Z') hash[i] = 'A' + 'Z' - hash[i];
        if(hash[i] >= '0' && hash[i] <= '9') hash[i] = '0' + '9' - hash[i];
        if(hash[i] == ' ') hash[i] = '@';
        if(hash[i] == '-') hash[i] = '*';
    }
    //sysLogPrintDebug("sysSubmitScore() -    hash: %s",hash);

    // Setting?
    if(g_GameSettings.ValueInt("game_autosubmit") == 1 && strlen(g_GameSettings.ValueStr("serial_number")) > 1)
    {
        // Prevent submission URL from being clearly visible in the executable
        sprintf(url,"http://www.HackerEvolutionDuality.com/");
        strcat(url,"_service");
#ifndef BUILD_WOLFPACKRISK
        strcat(url,"/leaderboard-ow");
#else
        strcat(url,"/leaderboard-wolfpackrisk");
#endif
        strcat(url,".php");
        strcat(url,"?hash=");
        strcat(url,hash);

        //sysLogPrintDebug("sysSubmitScore() - url: %s",url);

        // Submit score
        InternetDownloadFile(url,"HackerEvolutionDuality.ow");
        return 1;
    }
    else
    {
        return 0;
        sysLogPrintDebug("sysSubmitScore() - Not submitted. Option or serial number not defined.");
    }
#endif

    return 1;
}

//=======================================================================================================================================
// Create a list of all available game mods in the game folder
int       hedModCount = 0;
hedString hedModList[1024];
hedString hedModListDesc[1024];
hedString hedModActive;

void sysPopulateModList()
{
    hedModCount = 0;
    
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    char           buff[256];
    struct dirent *dptr;
    DIR           *dirp;

    strcpy(buff,"hed-gamemods");

    if(((dirp=opendir(buff))==NULL))
    {
        printf("error opening folder (%s)",buff);
    	return;
    }
    
    while(dptr = readdir(dirp))
    {
    	if(strstr(dptr->d_name,".mod") != NULL)
    	{
            sysLogPrintDebug("enumerating game mods: (%s)",dptr->d_name);
            printf("\nenumerating game mods: (%s)",dptr->d_name);
            
			if(strcmp(dptr->d_name,"OpenWorld.mod") != 0)
			{
#if defined(PLATFORM_MACOS) && defined(BUILD_STEAM)
                // Under MacOS, we bundle all DLCs in the game but only allow access to them if the DLC has been purchased
                if( (SteamApps()->BIsSubscribedApp(211990) && strcmp(dptr->d_name,"HED-Inception-1.mod") == 0) ||
                    (SteamApps()->BIsSubscribedApp(211994) && strcmp(dptr->d_name,"HED-Inception-2.mod") == 0) ||
                    (SteamApps()->BIsSubscribedApp(211992) && strcmp(dptr->d_name,"HED-Inception-3.mod") == 0) ||
                    (SteamApps()->BIsSubscribedApp(241950) && strcmp(dptr->d_name,"HED-Hardcore-Pack-1.mod") == 0) ||
                    (SteamApps()->BIsSubscribedApp(259220) && strcmp(dptr->d_name,"HED-Hardcore-Pack-2.mod") == 0) ||
                    (SteamApps()->BIsSubscribedApp(261070) && strcmp(dptr->d_name,"HED-Bootcamp.mod") == 0) ||
                    (strcmp(dptr->d_name,"HackerEvolutionDuality.mod") == 0)
                  )
                {
#endif

    			//strcpy(hedModList[hedModCount],"HackerEvolutionDuality");
    			//strcpy(hedModListDesc[hedModCount],"Hacker Evolution Duality");
                    
                    char temp[1024];
                    int  i;
                    
                    strcpy(temp,dptr->d_name);
                    i = strlen(temp) - 1;
                    while(temp[i] != '.') {temp[i] = 0; i--;}temp[i] = 0;
                    strcpy(hedModList[hedModCount],temp);
                    

    			char  fileName[1024];
    			FILE* file;

				sprintf(fileName,"hed-gamemods/%s",dptr->d_name);
				file = fopen(fileName,"rt");
				fgets(hedModListDesc[hedModCount],1024,file);
				utilStripNewLine(hedModListDesc[hedModCount]);
				fclose(file);

    			hedModCount++;
#if defined(PLATFORM_MACOS) && defined(BUILD_STEAM)
                }
#endif
			}
    	}
    }
    closedir(dirp);
 #endif

#if defined(PLATFORM_WINDOWS)
	struct _finddata_t fileinfo;
	long               filehandle = -1L;
	int                i,nRes;
	FILE*              file;
	char               temp[1024],fileName[1024];

	// * Find all existing mods
	filehandle = _findfirst("hed-gamemods/*.mod",&fileinfo);
	
	strcpy(temp,fileinfo.name);
	i = strlen(temp) - 1;
	while(temp[i] != '.') {temp[i] = 0; i--;}temp[i] = 0;
	strcpy(hedModList[hedModCount],temp);			
	

	sprintf(fileName,"hed-gamemods/%s",fileinfo.name);
	file = fopen(fileName,"rt");
	fgets(hedModListDesc[hedModCount],1024,file);
	utilStripNewLine(hedModListDesc[hedModCount]);
	fclose(file);

	sysLogPrintDebug("enumerating game mods: (%s)(%s)",hedModList[hedModCount],hedModListDesc[hedModCount]);
	hedModCount++;

	if(filehandle != -1L)
	{
		while(_findnext(filehandle,&fileinfo) != -1)
		{
			strcpy(temp,fileinfo.name);
			i = strlen(temp) - 1;
			while(temp[i] != '.') {temp[i] = 0; i--;}temp[i] = 0;

			// Skip adding the OpenWorld mod in the single player mods
			if(strcmp(fileinfo.name,"OpenWorld.mod") != 0)
			{
				strcpy(hedModList[hedModCount],temp);			
			
				sprintf(fileName,"hed-gamemods/%s",fileinfo.name);
				file = fopen(fileName,"rt");
				fgets(hedModListDesc[hedModCount],1024,file);
				utilStripNewLine(hedModListDesc[hedModCount]);
				fclose(file);
			
				sysLogPrintDebug("enumerating game mods: (%s)(%s)",hedModList[hedModCount],hedModListDesc[hedModCount]);
				hedModCount++;
			}
		}
	}
#endif
    
#if defined (PLATFORM_IOS)
    strcpy(hedModList[hedModCount],"HackerEvolutionDuality");
    strcpy(hedModListDesc[hedModCount],"Hacker Evolution Duality");
    hedModCount++;
#endif

	// * Last profile is actually a name to return to the main menu
	strcpy(hedModList[hedModCount],"return");
	strcpy(hedModList[hedModCount],"return to main menu...");
}

//=======================================================================================================================================
// System code for creating the path where application data is saved.
// Some OS-es doesn't allow us to save various game files in the game folder (such as Linux, Vista, etc).
char* sysGetAppDataPath(char* fileName)
{
    strcpy(sysAppDataPathLast,sysAppDataPath);
    strcat(sysAppDataPathLast,fileName);

    return sysAppDataPathLast;
}
