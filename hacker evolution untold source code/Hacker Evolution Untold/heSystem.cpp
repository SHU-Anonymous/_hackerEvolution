// heSystem.cpp
// 
// System specific code
// 
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "heSystem.h"
#include "SDL.h"

long pauseTime = 0;

// =============================================================================================================================
// Returns the system time in milliseconds
long system_TimeMilliSeconds()
{
    int time;

    time = SDL_GetTicks() - pauseTime;

    return time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the system time in seconds
long system_TimeSeconds()
{
    int time;

    time = SDL_GetTicks() - pauseTime;

    return (time / 1000);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Simple delay function
void system_Delay(long pSeconds)
{
    long start = system_TimeSeconds();

    while( (system_TimeSeconds() - start) < pSeconds )
    {
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Simple delay function (milliseconds);
void system_DelayMS(long pMilliSeconds)
{
    long start = system_TimeMilliSeconds();

    while( (system_TimeMilliSeconds() - start) < pMilliSeconds )
    {
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * This is to "freeze" the time during a game pause
void system_AddPauseTime(long time)
{
	pauseTime += time;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Returns the system time in milliseconds (without considering game pauses)
long system_TimeMilliSecondsAbsolute()
{
    return SDL_GetTicks();
}
