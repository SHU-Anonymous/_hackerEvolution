// heSystem.cpp
// 
// System specific code
// 
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "heSystem.h"
#include "SDL.h"

int pauseTime = 0;

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
// * This is to "freeze" the time during a game pause
void system_AddPauseTime(long time)
{
	pauseTime += time;
}