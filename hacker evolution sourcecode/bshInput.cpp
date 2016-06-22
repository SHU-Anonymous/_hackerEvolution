// bshInput.cpp
// 
// Input device interface
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "bshInput.h"
#include "heLog.h"
#include <stdio.h>

// * AZERTY keyboard support
int isKbAzerty = 0;

int mouseX = 0,mouseY = 0;
int _mouseLBDown = 0;
int _mouseRBDown = 0;

int keyShiftDown = 0;

int keyCount = 0;
int KeyList[300];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * key mappings
int gScanToKey[256];

// * input functions
//==============================================================================================================================
// Initialize input device
void input_Init()
{
	int i;

	// * Initialize key mappings
	for(i = 0; i < 256; i++) gScanToKey[i] = 0;

	gScanToKey[27]  = HEKEY_ESC;
	gScanToKey[112] = HEKEY_F1;
	gScanToKey[113] = HEKEY_F2;
	gScanToKey[114] = HEKEY_F3;
	gScanToKey[115] = HEKEY_F4;
	gScanToKey[116] = HEKEY_F5;
	gScanToKey[117] = HEKEY_F6;
	gScanToKey[118] = HEKEY_F7;

	gScanToKey[121] = HEKEY_F10;
	gScanToKey[122] = HEKEY_F11;
	gScanToKey[123] = HEKEY_F12;

	gScanToKey[192] = '`';
	gScanToKey[49]  = '1';
	gScanToKey[50]  = '2';
	gScanToKey[51]  = '3';
	gScanToKey[52]  = '4';
	gScanToKey[53]  = '5';
	gScanToKey[54]  = '6';
	gScanToKey[55]  = '7';
	gScanToKey[56]  = '8';
	gScanToKey[57]  = '9';
	gScanToKey[48]  = '0';
	gScanToKey[189] = '-';
	gScanToKey[187] = '=';
	gScanToKey[8]   = HEKEY_BACKSPACE;
	gScanToKey[36]  = HEKEY_HOME;

	gScanToKey[9]   = HEKEY_TAB;
	gScanToKey[81]  = 'q';
	gScanToKey[87]  = 'w';
	gScanToKey[69]  = 'e';
	gScanToKey[82]  = 'r';
	gScanToKey[84]  = 't';
	gScanToKey[89]  = 'y';
	gScanToKey[85]  = 'u';
	gScanToKey[73]  = 'i';
	gScanToKey[79]  = 'o';
	gScanToKey[80]  = 'p';
	gScanToKey[219] = '[';
	gScanToKey[221] = ']';
	gScanToKey[220] = '\\';
	gScanToKey[33]  = HEKEY_PGUP;

	gScanToKey[20]  = HEKEY_CAPSLOCK;
	gScanToKey[65]  = 'a';
	gScanToKey[83]  = 's';
	gScanToKey[68]  = 'd';
	gScanToKey[70]  = 'f';
	gScanToKey[71]  = 'g';
	gScanToKey[72]  = 'h';
	gScanToKey[74]  = 'j';
	gScanToKey[75]  = 'k';
	gScanToKey[76]  = 'l';
	gScanToKey[186] = ';';
	gScanToKey[222] = '\'';
	gScanToKey[13]  = HEKEY_ENTER;
	gScanToKey[34]  = HEKEY_PGDN;

	gScanToKey[16]  = HEKEY_SHIFT;
	gScanToKey[90]  = 'z';
	gScanToKey[88]  = 'x';
	gScanToKey[67]  = 'c';
	gScanToKey[86]  = 'v';
	gScanToKey[66]  = 'b';
	gScanToKey[78]  = 'n';
	gScanToKey[77]  = 'm';
	gScanToKey[188] = ',';
	gScanToKey[190] = '.';
	gScanToKey[191] = '/';
	gScanToKey[16]  = HEKEY_SHIFT;
	gScanToKey[38]  = HEKEY_UP;
	gScanToKey[35]  = HEKEY_END;

	gScanToKey[17]  = HEKEY_CTRL;
	gScanToKey[32]  = ' ';
	gScanToKey[37]  = HEKEY_LEFT;
	gScanToKey[40]  = HEKEY_DOWN;
	gScanToKey[39]  = HEKEY_RIGHT;

	gScanToKey[19]  = HEKEY_PAUSE;
	gScanToKey[45]  = HEKEY_INS;
	gScanToKey[46]  = HEKEY_DEL;

	// * Numpad mappings
	gScanToKey[97]  = '1';
	gScanToKey[98]  = '2';
	gScanToKey[99]  = '3';
	gScanToKey[100]  = '4';
	gScanToKey[101]  = '5';
	gScanToKey[102]  = '6';
	gScanToKey[103]  = '7';
	gScanToKey[104]  = '8';
	gScanToKey[105]  = '9';
	gScanToKey[109]  = '-';
	gScanToKey[96]  = '0';

}
//==============================================================================================================================
// Update input device
void input_Update()
{
}
//==============================================================================================================================
// Get last key
int input_getLastKey()
{
	if(keyCount > 0)
	{
		keyCount--;
		return(KeyList[keyCount]);
	}
	else
		return 0;
}
//==============================================================================================================================
// Check for mouseclick
int mouseLBClick()
{
	if(_mouseLBDown == 1)
	{
		_mouseLBDown = 0;
		return 1;
	}
	else
		return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Check for right mouse click
int mouseRBClick()
{
	if(_mouseRBDown == 1)
	{
		_mouseRBDown = 0;
		return 1;
	}
	else
		return 0;
}
//==============================================================================================================================
// Close input device
void input_Close()
{
}
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check if a character is alphanumeric
int isAscii(int key)
{
	if(key >= 'a' && key <= 'z') return 1;
	if(key >= 'A' && key <= 'Z') return 1;
	if(key >= '0' && key <= '9') return 1;
    if(key == '-') return 1;

	return 0;
}

