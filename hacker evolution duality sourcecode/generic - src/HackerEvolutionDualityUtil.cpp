/*
Name: HackerEvolutionDualityUtil.h
Desc: Misc utility functions (platform independent)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

char* _utilFormatTime[1024];

//=======================================================================================================================================
// point-in-rectangle test function
int utilPointInRect(int x, int y, int x1, int y1, int x2, int y2)
{
	if(x > x1 && x < x2 && y > y1 && y < y2)
		return 1;
	return 0;
}

//=======================================================================================================================================
// distance between 2 points
float utilDistance(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)); 
}

//=======================================================================================================================================
// calculate the nearest power of 2
int utilNearestPowerOf2(int value)
{
	int retVal = 1;
	
	while(retVal < value) retVal *= 2;
	
	return retVal;
}

//=======================================================================================================================================
// check if 2 arrays are equal
int utilArraysEqual(int* a, int* b, int count)
{
	for(int i = 0; i < count; i++)
		if(a[i] != b[i]) return 0;

	return 1;
}

//=======================================================================================================================================
// Convert time (seconds, float) to a char (mm:ss::ms)
char* utilFormatTime(float  seconds)
{
	int mm,ss,ms;

	if(seconds < 0.0f)
	{
		sprintf((char*)&_utilFormatTime[0],"unlimited");
		return (char*)&_utilFormatTime[0];
	}

	ms = (int)(seconds * 10.0f) % 10;
	ss = (int)(seconds) % 60;
	mm = (int)(seconds) / 60;

	if(ss > 9)
		sprintf((char*)&_utilFormatTime[0],"%2d:%2d.%1d",mm,ss,ms);
	else
		sprintf((char*)&_utilFormatTime[0],"%2d:0%1d.%1d",mm,ss,ms);

	return (char*)&_utilFormatTime[0];
}

//=======================================================================================================================================
// Strips the comments from the end of a line
void  utilStripComment(char* line)
{
	int index = 0;
	int length = strlen(line);

	while(index <  length - 2)
	{
		if(line[index] == '/' && line[index + 1] == '/') line[index] = 0;
		index++;
	}
}

//=======================================================================================================================================
void  utilStripNewLine(char* line)
{
#if defined(PLATFORM_MACOS) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined (PLATFORM_IOS)
	if(strlen(line) == 0)
	{
		printf("\nutilStripNewLine 0\n");
		return;
	}
	
	for(int i = 0; i < strlen(line); i++)
		if(line[i] == '\n' || line[i] == '\r') line[i] = 0;
	
	//if(strlen(line) > 0) line[strlen(line) - 1] = 0;
	//printf("\nutilStripNewLine [%s]\n", line);
	//if(strlen(line) > 0) line[strlen(line) - 1] = 0;
	
#endif

}
//=======================================================================================================================================
// Returns a given digit from a number
int utilGetDigit(int number, int digit)
{
    int i,div = 1;

    for(i = 0; i < digit; i++) div *= 10;
    return (number / div) % 10;
}

