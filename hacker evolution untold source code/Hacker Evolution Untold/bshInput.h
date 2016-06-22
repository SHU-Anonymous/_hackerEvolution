// bshInput.h
// 
// Input device interface
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#ifndef _bshInput_h_
#define _bshInput_h_

// * AZERTY keyboard support
extern int isKbAzerty;

// * Mouse scroll support
extern int mouseScroll;

// current mouse coordinates
extern int mouseX,mouseY;
extern int _mouseLBDown;
extern int _mouseRBDown;
extern int _mouseLBReleased;

extern int keyShiftDown;

// keys
extern int keyCount;
extern int KeyList[300];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * key mappings
extern int gScanToKey[256];

#define HEKEY_ESC -25

#define HEKEY_F1 -1
#define HEKEY_F2 -2
#define HEKEY_F3 -3
#define HEKEY_F4 -4
#define HEKEY_F5 -5
#define HEKEY_F6 -6
#define HEKEY_F7 -7

#define HEKEY_F10 -28
#define HEKEY_F11 -26
#define HEKEY_F12 -27

#define HEKEY_BACKSPACE -8
#define HEKEY_HOME      -9
#define HEKEY_TAB       -10
#define HEKEY_PGUP      -11
#define HEKEY_CAPSLOCK  -12
#define HEKEY_ENTER     -13
#define HEKEY_PGDN      -14
#define HEKEY_SHIFT     -15
#define HEKEY_UP        -16
#define HEKEY_END       -17

#define HEKEY_CTRL      -18
#define HEKEY_LEFT      -19
#define HEKEY_DOWN      -20
#define HEKEY_RIGHT     -21

#define HEKEY_PAUSE     -22
#define HEKEY_INS       -23
#define HEKEY_DEL       -24

// * input functions
void          input_Init();
void          input_Update();
int           input_getLastKey();
int           mouseLBClick();
int           mouseLBReleased();
int           mouseRBClick();
void          input_Close();

//
int isAscii(int key);

#endif