// heLog.h
// 
// Log management class implementation
// Code : Robert Muresan (robert@exosyphen.com, exosyphen studios)
//
#include "heLog.h"
#include "HackerEvolutionDefines.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// heLog gApplicationLog;
FILE* gFileHandler = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * class constructor
heLog::heLog()
{
	// * initialize log file
    char logFilePath[1024];

    strcpy(logFilePath,"");
#ifdef PLATFORM_MACOS
	return;
#endif
    
    strcat(logFilePath,"application.log");
	gFileHandler = fopen(logFilePath,"wt");
	this->printf("Staring Hacker Evolution (%s)...\n",ENGINE_VERSION);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * class destructor
heLog::~heLog()
{
	// * close log file
	//printf("\nheLog::~heLog() - log file closed..");
    if(gFileHandler != 0) fclose(gFileHandler);
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
