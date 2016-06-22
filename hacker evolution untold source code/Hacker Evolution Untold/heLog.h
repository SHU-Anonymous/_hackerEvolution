// heLog.h
// 
// Log management class
// Code : Robert Muresan (mrobert@exosyphen.com, exosyphen studios)
//

#ifndef _heLog_h_
#define _heLog_h_

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Log management class
class heLog
{
public:
	heLog(char* logFileName);
	~heLog();

	void printf(char* pFormat,...);
protected:

	FILE* gFileHandler;
};

extern heLog gApplicationLog;

#endif