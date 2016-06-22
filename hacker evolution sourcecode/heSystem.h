// heSystem.h
// 
// System specific code
// 
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _heSystem_h_
#define _heSystem_h_

long system_TimeMilliSeconds();   
long system_TimeSeconds();   
void system_Delay(long pSeconds);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * This is to "freeze" the time during a game pause
void system_AddPauseTime(long time);

#endif