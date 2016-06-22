// heLog.h
// 
// Log management class implementation
// Code : Robert Muresan (mrobert@exosyphen.com, exosyphen studios)
//
#include "heLog.h"
#include "HackerEvolutionDefines.h"
#include <stdio.h>
#include <stdarg.h>

heLog gApplicationLog("application.log");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * class constructor
heLog::heLog(char* logFileName)
{
#ifdef PLATFORM_MACOS
    return;
#endif
	// * initialize log file
	gFileHandler = fopen(logFileName,"wt");
	printf("heLog::heLog() - log file opened.");
	this->printf("Staring Hacker Evolution (%s)...\n",HEUT_ENGINE_VERSION);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * class destructor
heLog::~heLog()
{
#ifdef PLATFORM_MACOS
    return;
#endif
	// * close log file
	printf("\nheLog::~heLog() - log file closed..");
    fclose(gFileHandler);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * print a line of formated text, to the log file
void heLog::printf(char *pFormat,...)
{
#ifdef PLATFORM_MACOS
    return;
#endif
    char     mBuffer[1024];
    va_list  mParam;
    
    // * print the formated text, to a temporary buffer
    va_start(mParam,pFormat);
    vsprintf(mBuffer,pFormat,mParam);
    va_end(mParam);

    // * print the text to the log file
    fprintf(gFileHandler,"%s",mBuffer);

	// * flush the file
	fflush(gFileHandler);
}
