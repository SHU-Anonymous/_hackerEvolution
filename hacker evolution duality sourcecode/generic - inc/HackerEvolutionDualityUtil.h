/*
Name: HackerEvolutionDualityUtil.h
Desc: Misc utility functions (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _HackerEvolutionDualityUtil_h_
#define _HackerEvolutionDualityUtil_h_

#include "HackerEvolutionDualityDefines.h"
#include "hedSystem.h"
#include <math.h>

#define CAPMAX(x,y) if((x) > (y)) x = (y);                                                         // Cap the maximum value of a variable
#define CAPMIN(x,y) if((x) < (y)) x = (y);                                                         // Cap the maximum value of a variable
#define CLAMP(x,left,right) if (x < (left)) x = (left); if (x > (right)) x = (right);              // Clamp a value to an interval
#define WRAP(x,left,right) if (x > (right)) x = (left);                                            // Wrap a value around an interwal

#define INCREMENT_WITH_CAP(value,delta,max) {value += (delta); if(value > (max)) value = (max);}   // Increment <value> by <delta>, but make sure <value> doesn't go above <max>
#define DECREMENT_WITH_CAP(value,delta,min) {value -= (delta); if(value < (min)) value = (min);}   // Decrement <value> by <delta>, but make sure <value> doesn't go below <min>
#define TOGGLE(x) {if(x == 0) x = 1; else x = 0;}                                                  // Toggle a variable between 0 and 1

#define HED_SAFE_DELETE(x)  {if(x != 0) delete (x); (x) = 0;}                                // Safe class deletion
#define HED_REALLOC(x,type) {if(x) delete (x); (x) = 0; (x) = new type;}                           // Safe class deletion + realloc

// 
#define VERIFY_AND_LOG(result,value,logline) if((result) == (value)) {sysLogPrint("["##__FILE__##"]\n[VERIFY_AND_LOG FAILED] %s", logline); return;}  
#define LOGVAR_S(x,y) sysLogPrint("var(%s)=(%s)",x,y);

int   utilPointInRect(int x, int y, int x1, int y1, int x2, int y2);                                 // point-in-rectangle test function
float utilDistance(float x1, float y1, float x2, float y2);                                          // distance between 2 points 
int   utilNearestPowerOf2(int value);                                                                // calculate the nearest power of 2
int   utilArraysEqual(int* a, int* b, int count);                                                    // check if 2 arrays are equal
char* utilFormatTime(float  seconds);                                                                // Convert time (seconds, float) to a char (mm:ss::ms)
void  utilStripComment(char* line);                                                                  // Strips the comments from the end of a line
void  utilStripNewLine(char* line);                                                                  // Strips NEW LINE from the end of a string    
int   utilGetDigit(int number, int digit);                                                           // Returns a given digit from a number

//=======================================================================================================================================
// 2D Vector class definiton
class hedVector
{
public:
	hedVector() { x = 0; y = 0;};
	hedVector(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	// + operator
	hedVector operator+(const hedVector& v) const
	{
		return hedVector(x + v.x, y + v.y);
	}
	// - operator
	hedVector operator-(const hedVector& v) const
	{
		return hedVector(x - v.x, y - v.y);
	}
	hedVector operator-() const
	{
		return hedVector(-x,-y);
	}
	// * operator
	hedVector operator*(float value) const
	{
		return hedVector(x * value, y * value);
	}
	// Vector length
	float Length() const
	{
		return sqrt(x * x + y * y);
	}
	// Normalized value
	hedVector Normalized() const
	{
		hedVector normalizedVector(x / this->Length(), y / this->Length());
		return normalizedVector;
	}

    float Dot(const hedVector& v) const
    {
        return x * v.x + y * v.y;
    } 

    float Cross(const hedVector& v) const
    {
        return (x * v.y - y * v.x);
    } 

	void Rotate(float angle)
	{
		float _x, _y;

        _x = x * cos(angle * HED_PI / 180.0f) - y * sin(angle * HED_PI / 180.0f);         
		_y = y * cos(angle * HED_PI / 180.0f) + x * sin(angle * HED_PI / 180.0f); 

		x = _x;
		y = _y;
	}

	float x,y;
};

//=======================================================================================================================================
// Triggers. This class is for the management of various timed events in the game.
#define HED_MAX_TIMED_EVENTS 100

#define HED_EVENT_BACKGROUND_BAR 0

class hedTimedEvents
{
public:
	// Class constructor
	hedTimedEvents()
	{
		int i;

		for(i = 0; i < HED_MAX_TIMED_EVENTS; i++)
		{
			eventTimes[i]   = 0;
			eventLength[i]  = 0;
			eventRunning[i] = 0;
		}
	}

	// Start an event. After <length> seconds, it will be triggered
	void Start(int eventID, float _eventLength)
	{
		eventTimes[eventID]   = 0;
		eventLength[eventID]  = _eventLength;
		eventRunning[eventID] = 1;
	}

	// Restart the event with same parameters
	void ReStart(int eventID)
	{
		eventTimes[eventID]   = 0;
		eventRunning[eventID] = 1;
	}

	// Stop (or pause) an event.
	void Stop(int eventID)
	{
		eventRunning[eventID] = 0;
	}

	// Resume paused event
	void Resume(int eventID)
	{
		eventRunning[eventID] = 1;
	}

	// Update all events (called once every frame)
	void UpdateEvents(float frameTime)
	{
		int i;

		for(i = 0; i < HED_MAX_TIMED_EVENTS; i++)
			if(eventRunning[i] == 1)
				eventTimes[i] += frameTime;
	}

	// Check if an event should be triggered
	int EventHappened(int eventID)
	{
		if(eventTimes[eventID] >= eventLength[eventID])
		{
			Stop(eventID);
			return 1;
		}

		return 0;
	}

protected:
	float eventTimes[HED_MAX_TIMED_EVENTS];
	float eventLength[HED_MAX_TIMED_EVENTS];
	int   eventRunning[HED_MAX_TIMED_EVENTS];
};

#endif
