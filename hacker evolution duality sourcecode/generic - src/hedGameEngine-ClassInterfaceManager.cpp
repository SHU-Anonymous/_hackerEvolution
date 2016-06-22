/*
Name: hedGameEngine-ClassInterfaceManager.h
Desc: Hacker Evolution Duality - Interface (skin) elements manager

Author: Robert Muresan (mrobert@exosyphen.com) - 2011
*/
#include "../generic - inc/hedGameEngine-ClassInterfaceManager.h"
#include "../generic - inc/HackerEvolutionDualityDefines.h"
#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedSystem.h"
#include "../generic - inc/hedSound.h"
#include "../generic - inc/hedRendererOpenGL.h"
#include <stdio.h>

hedClassInterfaceManager g_InterfaceManager;                 // Interface(skin) manager   

//=======================================================================================================================================
// Constructor
hedClassInterfaceManager::hedClassInterfaceManager()
{
	// Initializations
	_staticSkinPath = new char[2048];
	skinWindowLevelIntro = NULL;
}

//=======================================================================================================================================
// Destructor
hedClassInterfaceManager::~hedClassInterfaceManager()
{
	HED_SAFE_DELETE(_staticSkinPath);
}

//=======================================================================================================================================
// Load a given skin
void hedClassInterfaceManager::LoadSkin(char* skinPath)
{
	int i,x,y;
	int _barBottomX, _barBottomY;

	// Load skin elements
	LOAD_SKIN_IMAGE(skinWindowLevelIntro,"hed-black","hedgui-window-levelintro.png");
	LOAD_SKIN_IMAGE(skinBGPattern,"hed-black","hedgui-bg-pattern.png");

	LOAD_SKIN_IMAGE(skinWindowConsole,"hed-black","hedgui-window-console.png");
	LOAD_SKIN_IMAGE(skinWindowObjectives,"hed-black","hedgui-window-objectives.png");
	LOAD_SKIN_IMAGE(skinWindowObjectivesStatus[0],"hed-black","hedgui-window-objectives-status-0.png");
	LOAD_SKIN_IMAGE(skinWindowObjectivesStatus[1],"hed-black","hedgui-window-objectives-status-1.png");
	LOAD_SKIN_IMAGE(skinWindowHelp,"hed-black","hedgui-window-help.png");

	// ** Targetmap/server elements
	LOAD_SKIN_IMAGE(skinServerLock[0],"hed-black","hedgui-serverlock-1.png");
	LOAD_SKIN_IMAGE(skinServerLock[1],"hed-black","hedgui-serverlock-2.png");
	LOAD_SKIN_IMAGE(skinServerLock[2],"hed-black","hedgui-serverlock-3.png");
	LOAD_SKIN_IMAGE(skinClickCircle,"hed-black","hedgui-clickcircle.png");
	LOAD_SKIN_IMAGE(skinFadeBar,"hed-black","hedgui-fadebar.png");

	// ** Hacking tools

    // ** Firewall attack tool
	LOAD_SKIN_IMAGE(skinToolFirewallAttack,"hed-black","hedgui-tool-firewall-attack.png");

	// ** DOS attack tool
	LOAD_SKIN_IMAGE(skinToolDOS,"hed-black","hedgui-tool-dos-window.png");

	// ** Interface tool
	LOAD_SKIN_IMAGE(skinToolInterface,"hed-black","tool-interface/hedgui-window-interface.png");
	LOAD_SKIN_IMAGE(skinButtonLogout[0],"hed-black","tool-interface/hedgui-button-logout-0.png");
	LOAD_SKIN_IMAGE(skinButtonLogout[1],"hed-black","tool-interface/hedgui-button-logout-1.png");
	LOAD_SKIN_IMAGE(skinButtonDownload[0],"hed-black","tool-interface/hedgui-button-download-0.png");
	LOAD_SKIN_IMAGE(skinButtonDownload[1],"hed-black","tool-interface/hedgui-button-download-1.png");
	LOAD_SKIN_IMAGE(skinButtonUpload[0],"hed-black","tool-interface/hedgui-button-upload-0.png");
	LOAD_SKIN_IMAGE(skinButtonUpload[1],"hed-black","tool-interface/hedgui-button-upload-1.png");
	LOAD_SKIN_IMAGE(skinButtonTransfer[0],"hed-black","tool-interface/hedgui-button-transfer-0.png");
	LOAD_SKIN_IMAGE(skinButtonTransfer[1],"hed-black","tool-interface/hedgui-button-transfer-1.png");
	LOAD_SKIN_IMAGE(skinToggle[0],"hed-black","tool-interface/hedgui-element-toggle-0.png");
	LOAD_SKIN_IMAGE(skinToggle[1],"hed-black","tool-interface/hedgui-element-toggle-1.png");
	LOAD_SKIN_IMAGE(skinFileMarker[0],"hed-black","tool-interface/hedgui-element-filemarker-0.png");
	LOAD_SKIN_IMAGE(skinFileMarker[1],"hed-black","tool-interface/hedgui-element-filemarker-1.png");

	LOAD_SKIN_IMAGE(skinToolFileView,"hed-black","tool-interface/hedgui-window-fileview.png");
	LOAD_SKIN_IMAGE(skinButtonView[0],"hed-black","tool-interface/hedgui-button-view-0.png");
	LOAD_SKIN_IMAGE(skinButtonView[1],"hed-black","tool-interface/hedgui-button-view-1.png");
	LOAD_SKIN_IMAGE(skinButtonClose[0],"hed-black","tool-interface/hedgui-button-close-0.png");
	LOAD_SKIN_IMAGE(skinButtonClose[1],"hed-black","tool-interface/hedgui-button-close-1.png");

	// UPDATE 1
	LOAD_SKIN_IMAGE(skinButtonDelete[0],"hed-black","tool-interface/hedgui-button-delete-0.png");
	LOAD_SKIN_IMAGE(skinButtonDelete[1],"hed-black","tool-interface/hedgui-button-delete-1.png");

	// ** Voiceprint password tool
	HED_REALLOC(skinToolVoicePrintPassword,hedImage);     skinToolVoicePrintPassword->LoadFromFile(CreateSkinPath("hed-black","hedgui-tool-voiceprintpassword-window.png"));             
	HED_REALLOC(skinElementSlider,hedImage);              skinElementSlider->LoadFromFile(CreateSkinPath("hed-black","hedgui-element-slider.png"));    
	LOAD_SKIN_IMAGE(skinButtonLogin[0],"hed-black","hedgui-button-login.png");     
	LOAD_SKIN_IMAGE(skinButtonLogin[1],"hed-black","hedgui-button-login-over.png");     
	LOAD_SKIN_IMAGE(skinButtonCancel[0],"hed-black","hedgui-button-cancel.png");     
	LOAD_SKIN_IMAGE(skinButtonCancel[1],"hed-black","hedgui-button-cancel-over.png");     

	// ** EMP attack
	LOAD_SKIN_IMAGE(skinServerEMPBlast,"hed-black","hedgui-tool-emp-blast.png");
	LOAD_SKIN_IMAGE(skinSatellite,"hed-black","hedgui-tool-emp-satellite.png");
	LOAD_SKIN_IMAGE(skinSatelliteAligned,"hed-black","hedgui-tool-emp-satellite-aligned.png");

	// ** Toolbar
	LOAD_SKIN_IMAGE(skinToolbar,"hed-black","toolbar/hedgui-toolbar.png");
	LOAD_SKIN_IMAGE(skinButtonFirewall[0],"hed-black","toolbar/hedgui-button-firewall.png");
	LOAD_SKIN_IMAGE(skinButtonFirewall[1],"hed-black","toolbar/hedgui-button-firewall-over.png");
	LOAD_SKIN_IMAGE(skinButtonDOS[0],"hed-black","toolbar/hedgui-button-dos.png");
	LOAD_SKIN_IMAGE(skinButtonDOS[1],"hed-black","toolbar/hedgui-button-dos-over.png");
	LOAD_SKIN_IMAGE(skinButtonEMP[0],"hed-black","toolbar/hedgui-button-emp.png");
	LOAD_SKIN_IMAGE(skinButtonEMP[1],"hed-black","toolbar/hedgui-button-emp-over.png");
	LOAD_SKIN_IMAGE(skinButtonVoiceprint[0],"hed-black","toolbar/hedgui-button-voiceprint.png");
	LOAD_SKIN_IMAGE(skinButtonVoiceprint[1],"hed-black","toolbar/hedgui-button-voiceprint-over.png");
	LOAD_SKIN_IMAGE(skinButtonRetina[0],"hed-black","toolbar/hedgui-button-retina.png");
	LOAD_SKIN_IMAGE(skinButtonRetina[1],"hed-black","toolbar/hedgui-button-retina-over.png");
	LOAD_SKIN_IMAGE(skinButtonKeycrack[0],"hed-black","toolbar/hedgui-button-keycrack.png");
	LOAD_SKIN_IMAGE(skinButtonKeycrack[1],"hed-black","toolbar/hedgui-button-keycrack-over.png");

	LOAD_SKIN_IMAGE(skinButtonEMPDOSFill[0],"hed-black","toolbar/hedgui-button-empdos-fill-0.png");
	LOAD_SKIN_IMAGE(skinButtonEMPDOSFill[1],"hed-black","toolbar/hedgui-button-empdos-fill-1.png");

	LOAD_SKIN_IMAGE(skinButtonInterface[0],"hed-black","toolbar/hedgui-button-interface.png");
	LOAD_SKIN_IMAGE(skinButtonInterface[1],"hed-black","toolbar/hedgui-button-interface-over.png");
	LOAD_SKIN_IMAGE(skinButtonConsole[0],"hed-black","toolbar/hedgui-button-console.png");
	LOAD_SKIN_IMAGE(skinButtonConsole[1],"hed-black","toolbar/hedgui-button-console-over.png");
	LOAD_SKIN_IMAGE(skinButtonHardware[0],"hed-black","toolbar/hedgui-button-hardware.png");
	LOAD_SKIN_IMAGE(skinButtonHardware[1],"hed-black","toolbar/hedgui-button-hardware-over.png");
	LOAD_SKIN_IMAGE(skinButtonObjectives[0],"hed-black","toolbar/hedgui-button-objectives.png");
	LOAD_SKIN_IMAGE(skinButtonObjectives[1],"hed-black","toolbar/hedgui-button-objectives-over.png");

	LOAD_SKIN_IMAGE(skinButtonMessages[0],"hed-black","toolbar/hedgui-button-messages.png");
	LOAD_SKIN_IMAGE(skinButtonMessages[1],"hed-black","toolbar/hedgui-button-messages-over.png");
	LOAD_SKIN_IMAGE(skinButtonAchievements[0],"hed-black","toolbar/hedgui-button-achievements.png");
	LOAD_SKIN_IMAGE(skinButtonAchievements[1],"hed-black","toolbar/hedgui-button-achievements-over.png");
	LOAD_SKIN_IMAGE(skinButtonHelp[0],"hed-black","toolbar/hedgui-button-help.png");
	LOAD_SKIN_IMAGE(skinButtonHelp[1],"hed-black","toolbar/hedgui-button-help-over.png");

	LOAD_SKIN_IMAGE(skinWindowAchievements,"hed-black","hedgui-window-achievements.png");
	LOAD_SKIN_IMAGE(skinWindowAchievementsStatus[0],"hed-black","hedgui-window-achievements-status-0.png");
	LOAD_SKIN_IMAGE(skinWindowAchievementsStatus[1],"hed-black","hedgui-window-achievements-status-1.png");

	LOAD_SKIN_IMAGE(skinWindowMessages,"hed-black","hedgui-window-messages.png");

    // ** Hardware manager
    LOAD_SKIN_IMAGE(skinWindowHardwareManager,"hed-black","tool-hardware/hedgui-tool-hardware.png");
    LOAD_SKIN_IMAGE(skinButtonFirewall_[0],"hed-black","tool-hardware/hedgui-button-firewall-0.png");
    LOAD_SKIN_IMAGE(skinButtonFirewall_[1],"hed-black","tool-hardware/hedgui-button-firewall-1.png");
    LOAD_SKIN_IMAGE(skinButtonCPU[0],"hed-black","tool-hardware/hedgui-button-cpu-0.png");
    LOAD_SKIN_IMAGE(skinButtonCPU[1],"hed-black","tool-hardware/hedgui-button-cpu-1.png");
    LOAD_SKIN_IMAGE(skinButtonIntegrity[0],"hed-black","tool-hardware/hedgui-button-integrity-0.png");
    LOAD_SKIN_IMAGE(skinButtonIntegrity[1],"hed-black","tool-hardware/hedgui-button-integrity-1.png");

    LOAD_SKIN_IMAGE(skinHardwareIcon[0],"hed-black","tool-hardware/hedgui-tool-hardware-icon-firewall.png");
    LOAD_SKIN_IMAGE(skinHardwareIcon[1],"hed-black","tool-hardware/hedgui-tool-hardware-icon-cpu.png");
    LOAD_SKIN_IMAGE(skinHardwareIcon[2],"hed-black","tool-hardware/hedgui-tool-hardware-icon-integrity.png");

    // ** Key crack tool
    LOAD_SKIN_IMAGE(skinWindowKeyCrackTool,"hed-black","hedgui-window-keycrack.png");

    // ** Retinal scan tool
    LOAD_SKIN_IMAGE(skinWindowRetinalScanTool,"hed-black","hedgui-window-retinalscan.png");


	// ** Toolbar (top)
	LOAD_SKIN_IMAGE(skinToolbarTop,"hed-black","hedgui-toolbar-top.png");

	// ** Targetmap elements
	LOAD_SKIN_IMAGE(skinServerInfoWindow[0],"hed-black","hedgui-server-infowindow-left.png");
	LOAD_SKIN_IMAGE(skinServerInfoWindow[1],"hed-black","hedgui-server-infowindow-right.png");

	LOAD_SKIN_IMAGE(skinTargetMapScrollBar,"hed-black","targetmap/horiz-scroll-bar.png");
	LOAD_SKIN_IMAGE(skinTargetMapScrollBarCursor,"hed-black","targetmap/horiz-scroll-bar-cursor.png");

	// ** WINDOW: System Message
	LOAD_SKIN_IMAGE(skinWindowSystemMessage,"hed-black","hedgui-window-systemmessage.png");
	LOAD_SKIN_IMAGE(skinWindowSystemConfirm,"hed-black","hedgui-window-systemmessage.png");
	LOAD_SKIN_IMAGE(skinButtonYes[0],"hed-black","hedgui-button-yes.png");
	LOAD_SKIN_IMAGE(skinButtonYes[1],"hed-black","hedgui-button-yes-over.png");
	LOAD_SKIN_IMAGE(skinButtonNo[0],"hed-black","hedgui-button-no.png");
	LOAD_SKIN_IMAGE(skinButtonNo[1],"hed-black","hedgui-button-no-over.png");
	LOAD_SKIN_IMAGE(skinButtonOk[0],"hed-black","hedgui-button-ok-0.png");
	LOAD_SKIN_IMAGE(skinButtonOk[1],"hed-black","hedgui-button-ok-1.png");

    // ** Paused window
	LOAD_SKIN_IMAGE(skinWindowPaused,"hed-black","hedgui-window-paused.png");

    // ** New message notifier
    LOAD_SKIN_IMAGE(skinMessageNotifier,"hed-black","new-message-notifier/hedgui-newmessagenotifier.png");
    LOAD_SKIN_IMAGE(skinButtonRead[0],"hed-black","new-message-notifier/hedgui-button-read-0.png");
    LOAD_SKIN_IMAGE(skinButtonRead[1],"hed-black","new-message-notifier/hedgui-button-read-1.png");

	// ** Tablet PC interface elements
	LOAD_SKIN_IMAGE(skinTabletArrow[0],"hed-black","tablet/hedgui-tablet-arrow-left.png");
	LOAD_SKIN_IMAGE(skinTabletArrow[1],"hed-black","tablet/hedgui-tablet-arrow-right.png");
	skinTabletArrow[0]->m_AnimAlphaSpeed /= 3.0f;
	skinTabletArrow[1]->m_AnimAlphaSpeed /= 3.0f;

	LOAD_SKIN_IMAGE(skinTabletESC[0],"hed-black","tablet/hedgui-button-esc.png");
	LOAD_SKIN_IMAGE(skinTabletESC[1],"hed-black","tablet/hedgui-button-esc-over.png");
}

//=======================================================================================================================================
// Create path for a given skin element file
char* hedClassInterfaceManager::CreateSkinPath(char* skinName, char* skinFile)
{
	sprintf(_staticSkinPath,"hed-skins/%s/%s",skinName,skinFile);
	return _staticSkinPath;
}
