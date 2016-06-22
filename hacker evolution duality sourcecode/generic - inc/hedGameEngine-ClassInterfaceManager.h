/*
Name: hedGameEngine-ClassInterfaceManager.h
Desc: Hacker Evolution Duality - Interface (skin) elements manager

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/

#ifndef _hedGameEngine_ClassInterfaceManager_h_
#define _hedGameEngine_ClassInterfaceManager_h_

#include "hedGraphicsImage.h"
#include "hedGameEngine-ClassTextWindow.h"

#define LOAD_SKIN_IMAGE(image,skinPath,imagePath) 	HED_REALLOC(image,hedImage); (image)->LoadFromFile(CreateSkinPath(skinPath,imagePath));     

class  hedClassInterfaceManager;
extern hedClassInterfaceManager g_InterfaceManager;                 // Interface(skin) manager   


//=======================================================================================================================================
// Class definition
class hedClassInterfaceManager
{
public:
	hedClassInterfaceManager();
	~hedClassInterfaceManager();

	void  LoadSkin(char* skinName);                                     // Load a given skin
	char* CreateSkinPath(char* skinName, char* skinFile);               // Create path for a given skin element file

	// ==================================================================================================================================
	// Skin elements (images)
	hedImage* skinWindowLevelIntro;       // Level intro window

	hedImage* skinBGPattern;              // Main background pattern

	hedImage* skinWindowConsole;
	hedImage* skinWindowObjectives;
	hedImage* skinWindowObjectivesStatus[2];
	hedImage* skinWindowHelp;
	hedImage* skinWindowAchievements;
	hedImage* skinWindowAchievementsStatus[2];
	hedImage* skinWindowMessages;                 // Messages window

	// Server related elements
	hedImage*  skinServerLock[3];                 // Server lock circles
	hedImage*  skinServerEMPBlast;                // EMP Blast image
	hedImage*  skinSatellite;                     // Sattelite image
	hedImage*  skinSatelliteAligned;              // Sattelite image

	hedImage*  skinClickCircle;                   // Circle that is displayed when clicking on the map
	hedImage*  skinFadeBar;                       // Fade bar that moves in the background

	// ==================================================================================================================================
	// ** Hacking tools
	hedImage* skinToolDOS;                             // DOS attack tool
	hedImage* skinToolVoicePrintPassword;              // Voiceprint password tool
	hedImage* skinToolInterface;                       // Interface tool
	hedImage* skinToolFileView;                        // File view window
    hedImage* skinToolFirewallAttack;                  // Firewall attack tool

	hedImage* skinElementSlider;                       // Progress bar cursor
	hedImage* skinButtonLogin[2];                      // Login button
	hedImage* skinButtonCancel[2];                     // Cancel button
	hedImage* skinButtonLogout[2];
	hedImage* skinButtonDownload[2];
	hedImage* skinButtonUpload[2];
	hedImage* skinButtonTransfer[2];
	hedImage* skinToggle[2];
	hedImage* skinFileMarker[2];
	hedImage* skinButtonView[2];
	hedImage* skinButtonClose[2];
	hedImage* skinButtonDelete[2];

    // ** Hardware manager
    hedImage* skinWindowHardwareManager;
    hedImage* skinButtonFirewall_[2];
    hedImage* skinButtonCPU[2];
    hedImage* skinButtonIntegrity[2];
    hedImage* skinHardwareIcon[3];

    // ** Key crack tool
    hedImage* skinWindowKeyCrackTool;

    // ** Retinal scan tool
    hedImage* skinWindowRetinalScanTool;

	// ** Toolbar
	hedImage* skinToolbar;                                  // Bottom toolbar
	hedImage *skinButtonFirewall[2]; 
	hedImage *skinButtonDOS[2];
	hedImage *skinButtonEMP[2];
	hedImage *skinButtonVoiceprint[2];
	hedImage *skinButtonRetina[2];
	hedImage *skinButtonKeycrack[2];
	hedImage *skinButtonEMPDOSFill[2];

	hedImage *skinButtonInterface[2];
	hedImage *skinButtonConsole[2];
	hedImage *skinButtonHardware[2];
	hedImage *skinButtonObjectives[2];

	hedImage *skinButtonMessages[2];
	hedImage *skinButtonAchievements[2];
	hedImage* skinButtonHelp[2];

	// ** Toolbar (top)
	hedImage *skinToolbarTop;

	// ** Targetmap elements
	hedImage* skinServerInfoWindow[2];
	hedImage* skinTargetMapScrollBar;
	hedImage* skinTargetMapScrollBarCursor;


	// ** WINDOW: System Message
	hedImage *skinWindowSystemMessage;
	hedImage *skinWindowSystemConfirm;
	hedImage *skinButtonYes[2];
	hedImage *skinButtonNo[2];
	hedImage *skinButtonOk[2];

    // ** Paused window
    hedImage *skinWindowPaused;

    // ** New message notifier
    hedImage *skinMessageNotifier;
    hedImage *skinButtonRead[2];

	// ** Tablet PC interface elements
	hedImage *skinTabletArrow[2];                         // Left and right arrows
	hedImage *skinTabletESC[2];                           // Escape button

protected:
	// ==================================================================================================================================
	// Misc. variables
	char*              _staticSkinPath;             // This saves the last path created by "CreateSkinPath()"
};

#endif

