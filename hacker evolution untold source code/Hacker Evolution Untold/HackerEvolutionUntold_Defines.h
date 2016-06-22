// HackerEvolutionUntoldDefines.cpp
// 
// Global game defines
// Code : Robert Muresan (mrobert@exosyphen.com, exoSyphen Studios)
//

#ifndef _HackerEvolutionUntoldDefines_h_
#define _HackerEvolutionUntoldDefines_h_

//#define DEMO
#define NO_DEMO_END_SPLASH
#define ENABLE_CHEATS
#define DEVELOPER
#define DEVELOPER_NOCHECK // don't check demo/game file integrity

#define PLATFORM_WINDOWS
//#define PLATFORM_MACOS
//#define PLATFORM_LINUX

///////////////////////////////////////////////////////////////////////////////////////////////////
// * engine version
#if defined(PLATFORM_WINDOWS)
#define HEUT_ENGINE_VERSION "heut.engine-4.08.win32 ["## __DATE__ ## "." ## __TIME__ ##"]"
#endif

#if defined(PLATFORM_MACOS)
#define HEUT_ENGINE_VERSION "heut.engine-4.08.macos"
#define nullptr NULL
#endif

#if defined(PLATFORM_LINUX)
#define HEUT_ENGINE_VERSION "heut.engine-4.08.linux"

#define nullptr NULL
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// * development defines
//#define HE_MOUSE_AREA_HIGHLIGHT // highlight areas where mouseclicks are checked

///////////////////////////////////////////////////////////////////////////////////////////////////
// * level intro

// * game settings 
#define HE_FACTOR_DELETELOGS  2     // The amount by which the trace amount is divided before reduction
#define HE_LEVELDONE_WAITTIME 600   // Time left to explore a level when it's completed
#define HE_TEXTANIM_WAITTIME  400   // Number of milliseconds to wait between drawing text animation
#define HE_CHARANIM_WAITTIME  25    // Number of milliseconds to wait between 2 characters in the intro

#define HE_GLOBALTRACE_BLINK     70  // Above this value, the global trace message will blink
#define HE_GLOBALTRACE_BLINK_RED 85  // Above this value, the global trace message will blink in red

extern char gGameLanguage[256];
extern int  gNextLevel;             // Set to 1, when player advances to the next level

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Data types
typedef char heString[2048];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Graphics constants
//#define HEUT_FONT_PATH           "heuntold-graphics/heut-font.bmp"
#define FONT_SIZE                14
#define FONT_CHARSET             RUSSIAN_CHARSET

#define ADJUST_TO_WIDTH(x)  ((x) - ((1280.0f - g_screen_width) / 2))
#define ADJUST_TO_HEIGHT(y) ((y) + ((g_screen_height - 800) / 2))

#define HEUT_MAINMENUWINDOW_X  320
#define HEUT_MAINMENUWINDOW_Y  245
#define HEUT_MAINMENU_ITEM_X   370
#define HEUT_MAINMENU_ITEM_Y     295
#define HEUT_MANIMENU_ANIMBAR_X  250
#define HEUT_MANIMENU_ANIMBAR_Y  160

#define HEUT_MAINMENU_OPTIONS_SUBMENU_OFFSET 20

#define HEUT_MENU_COLOR_1        171,223,1
#define HEUT_MENU_COLOR_2        31,127,127

#define HC_WINDOW_OFFS_Y          25            // vertical offset for in-game windows
#define HC_TARGETMAPIMAGE_WIDTH   680           // targetmap image size
#define HC_TARGETMAPIMAGE_HEIGHT  320

///////////////////////////////////////////////////////////////////////////////////////////////////
// * Misc text positions
#define HE_TEXT_MESSAGEINFO_X     205           // Message count text (messages window)
#define HE_TEXT_MESSAGEINFO_Y     344

// system panel window
#define HE_TEXT_STATUS_X          22
#define HE_TEXT_STATUS_Y          335

#define HE_TEXT_PROGRESS_X        100
#define HE_TEXT_PROGRESS_Y        335

#define HE_TEXT_HARDWAREINFO_X    22
#define HE_TEXT_HARDWAREINFO_Y    306

#define HE_TEXT_CLOCK_X           235
#define HE_TEXT_CLOCK_Y           38

// * Console window scroll bar position
#define HE_SCROLL_X          695
#define HE_SCROLL_Y           35
#define HE_SCROLL_WIDTH       10
#define HE_SCROLL_HEIGHT      60
#define HE_SCROLL_AREAHEIGHT 325

// messages window
#define HE_HIGHLIGHT_DISTANCE     290 // The distance travelled by the highlight bar, when a new message is received 

// level intro window element positions
#define HE_LEVELINTRO_IMAGE_X  275
#define HE_LEVELINTRO_IMAGE_Y  180

// * Trace animation
#define TRACE_RECT_SIZE     10
#define TRACE_WINDOW_OFFS_Y 30

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Game graphic file paths
#define GFILE_INTRO_BACKGROUND                 "heuntold-graphics/intro-background-1440-1024.png"

#define GFILE_MAIN_BACKGROUND                  "heuntold-graphics/mainmenu-background-1440-1024.png"
#define GFILE_MAINMENU_BACKGROUND              "heuntold-graphics/mainmenu-background-1440-1024.png"
#define GFILE_MAINMENU_BACKGROUND_DEMO         "heuntold-graphics/mainmenu-background-1440-1024-demo.png"
#define GFILE_MAINMENU_ANIM_WORLDMAP           "heuntold-graphics/mainmenu-anim-worldmap.png"
#define GFILE_MAINMENU_ANIMBAR                 "heuntold-graphics/mainmenu-animbar.png"
#define GFILE_MAINMENU_WINDOW                  "heuntold-graphics/mainmenu-window.png"
#define GFILE_MAINMENU_WINDOW_CURSORBACKGROUND "heuntold-graphics/mainmenu-window-cursorbackground.png"
#define GFILE_MAINMENU_VOLUMEBAR               "heuntold-graphics/mainmenu-volumebar.png"
#define GFILE_MAINMENU_VOLUMEBAR_SLIDER        "heuntold-graphics/mainmenu-volumebar-slider.png"
#define GFILE_MAINMENU_CHECK_FALSE             "heuntold-graphics/mainmenu-check-false.png"
#define GFILE_MAINMENU_CHECK_TRUE              "heuntold-graphics/mainmenu-check-true.png"
#define GFILE_MAINMENU_RETURN                  "heuntold-graphics/mainmenu-return.png"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Audio and music
#define HC_PLAYLIST_MENU  "heuntold-music/hc-playlist-menu.txt"
#define HC_PLAYLIST_INTRO "heuntold-music/hc-playlist-intro.txt"

#define HE_SOUNDFX_MENU_TICK "heuntold-soundfx/hesfx_untold_ui2.wav"

#endif
