// heLog.h
// 
// Log management class
// Code : Robert Muresan (robert@exosyphen.com, exosyphen studios)
//

#ifndef _heLog_h_
#define _heLog_h_

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Log management class
class heLog
{
public:
	heLog();
	~heLog();

	void printf(char* pFormat,...);
protected:
};

static heLog gApplicationLog;

#endif