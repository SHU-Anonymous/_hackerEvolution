#ifndef _he_language_h_
#define _he_language_h_

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * English

#define FONT_CHARSET ANSI_CHARSET
//#define FONT_CHARSET THAI_CHARSET

#define LANG_TEXT_RESWARNING "Warning!\nYour display resolution must be at least 1024x768.\nPlease adjust your display settings."
#define LANG_TEXT_MODWARNING "Main game mod has been changed. Please reinstall Hacker Evolution.\nOnline hiscore submission has been disabled."

#define MENU_PLAY         "New Game"
#define MENU_LOADGAME     "Load or Delete profile"
#define MENU_LOADMOD      "Select mod (DLC)"
#define MENU_LOADMOD_DEMO "Select mod (unavailable in demo)"
#define MENU_OPTIONS      "Options"
#define MENU_CREDITS      "Credits"
#define MENU_EXIT         "Exit"

#define INTRO_TEXT       "When you have only one option ... what will you choose?"

#define INTRO_TOOLTIP_1  "New York, USA\n12:01 December 20, 2015\n\nA sudden 2500% increase\nin exchange volumes\ncollapsed the stock market\ninfrastructure."
#define INTRO_TOOLTIP_2  "Brazil\n12:02 December 20, 2015\n\nAll connections with\nglobal space station Xenti\nwere lost.\n\nReason: unknown"
#define INTRO_TOOLTIP_3  "London, United Kingdom\n12:03 December 20, 2015\n\nTransoceanic fiber optic\nlinks were lost.\n\nReason: unknown"
#define INTRO_TOOLTIP_4  "Tokyo, Japan\n12:04 December 20, 2015\n\nComplete network crash\nat the Asian Central Bank.\n\nReason: unknown"

#define INTRO_TOOLTIP_5 "In 2010, a technology developed by the Xenti Corporation\
\nallowed the transmission of data packets at speeds\
\nexceeding the speed of light. This opened a\
\nnew era of communications and data transmission.\n\
\nAs with any technology, they kept pushing it's speed ahead.\
\n\
\nAs speed increased, the time-space continuum started\
\nto become unstable.\
\n\
\nWhen this happened, data packets started arriving\
\nat their destination sooner than they were actually sent.\
\nIt was like a dream come true: time travel. Not physical,\
\nbut having the ability to send data packets back in time.\
\nIt was kept secret and an A.I was developed to control it.\
\n\
\nNot only could data packets travel in time, but also\
\nthe same software could exist and run in an infinite\
\nnumber of parallel time slices. It resulted in huge\
\nprocessing power, which allowed the controlling A.I\
\nto evolve and learn, at an exponential rate...\
\n\
\n... until nobody was able to control it.\
\nIt doesn't want to kill.\
\nIt only follows the same basic rule of every\
\nlife form. It will try to survive and evolve.\
\nHow will you fight it?"

#define LANG_TEXT_YES "yes"
#define LANG_TEXT_NO  "no"

#define LANG_TEXT_CURRENTMOD    "Current game mod:"
#define LANG_TEXT_LOADPROFILE   "Click to load profile, or press DEL to delete it."
#define LANG_TEXT_RETURNTOMENU  "... return to Main Menu"
#define LANG_TEXT_INTERFACESKIN "Interface skin (click to change):"
#define LANG_TEXT_SERIALNUMBER  "Serial number:"
#define LANG_TEXT_AUTOSUBMIT    "Automaticly submit hiscore after each level"
#define LANG_TEXT_DDIFFICULTY   "Enable dynamic game difficulty"
#define LANG_TEXT_SFXVOL        "Sound effects volume:"
#define LANG_TEXT_MVOL          "Music volume:"
#define LANG_TEXT_NICKNAME      "Choose your nickname:"
#define LANG_TEXT_NEWPROFILE    "Press enter to create new profile..."
#define LANG_TEXT_NEWPROFILE2   "Press enter to create new profile..."
#define LANG_TEXT_NEWPROFILE3   "Error! Profile already exists: [%s]."
#define LANG_TEXT_TIP           "Tip: "
#define LANG_TEXT_PRESSF1       "\nPress F1 for quick help..."
#define LANG_TEXT_UPGRADE       "Type UPGRADE, to upgrade your hardware"

#define LANG_TEXT_FROM          "FROM:"
#define LANG_TEXT_OF            "of"
#define LANG_TEXT_NOMSG         "No messages"
#define LANG_TEXT_MSG_FROM      "SUBJECT:\n%s\n\n\n"

#define LANG_TEXT_OBJECTIVE     "Level objective %d completed!\nPress F2 to view level objectives."
#define LANG_TEXT_LEVELCOMP     "Level completed. %2ds left."

#define LANG_TEXT_TIPBOUNCE     " To learn about bounced links \ntype BOUNCEHELP in the console."
#define LANG_TEXT_TIPHELP       "\nType HELP,\nto see available commands."

#define LANG_TEXT_CONGRATS      "\nCongratulations!"
#define LANG_TEXT_COMPLETED     "\n\nLevel completed successfully!"
#define LANG_TEXT_SCORE         "\nScore            :  %d"
#define LANG_TEXT_MONEY         "\nMoney            : $%d"
#define LANG_TEXT_HACKS         "\nSuccesfull hacks :  %d"
#define LANG_TEXT_TRACE         "\nTrace count      :  %d"
#define LANG_TEXT_LOSTC         "\nLost count       :  %d"
#define LANG_TEXT_TIME1         "\nLevel time       :  %d:%0d"
#define LANG_TEXT_TIME2         "\nLevel time       :  %d:%d"
#define LANG_TEXT_TTIME1        "\nTotal time       :  %d:%0d"
#define LANG_TEXT_TTIME2        "\nTotal time       :  %d:%d"
#define LANG_TEXT_SPECIAL       "\n\n-Special achievements: "
#define LANG_TEXT_SPECIAL1      "\nLevel completed with 0 %% global trace : yes (+500 score points)"
#define LANG_TEXT_SPECIAL2      "\nLevel completed with 0 %% global trace : no"
#define LANG_TEXT_ALLSERV1      "\nFound all servers in the level       : yes (+500 score points)"
#define LANG_TEXT_ALLSERV2      "\nFound all servers in the level       : no"
#define LANG_TEXT_DDIFF1        "\nDynamic game difficulty              : yes (+%d score points)"
#define LANG_TEXT_DDIFF2        "\nDynamic game difficulty              : no"
#define LANG_TEXT_U301          "\nLevel completed under 30 minutes     : yes (+%d score points)"
#define LANG_TEXT_U302          "\nLevel completed under 30 minutes     : no"
#define LANG_TEXT_PRESSENTER    "\n\nPress [ENTER] to advance to the next level..."

#define LANG_TEXT_LOST1  "\n\nLevel lost!"
#define LANG_TEXT_LOST2  "\nYour trace level has reached 100%%."
#define LANG_TEXT_LOST3  "\nPlease restart the level."

// * System panel texts
#define LANG_TEXT_SPSCORE   "Score: %d"
#define LANG_TEXT_SPTRACE1  "\nTrace: %d %%"
#define LANG_TEXT_SPTRACE2  "\nTrace: "
#define LANG_TEXT_SPDDIFF   "\nDynamic difficulty factor: %d"
#define LANG_TEXT_SPMONEY   "\nMoney       : $%d"
#define LANG_TEXT_SPHACK    "\nHack count  :  %d"
#define LANG_TEXT_SPTRACE   "\nTrace count :  %d"
#define LANG_TEXT_SPBOUNCE1 "\n\nBounced link : (not in use)"
#define LANG_TEXT_SPBOUNCE2 "\n\nBounced link : through %d host"
#define LANG_TEXT_SPBOUNCE3 "\n\nBounced link : through %d hosts"

// * Scan command
#define LANG_TEXT_CMDSCAN_1 "  scanning: %s, open ports:"
#define LANG_TEXT_CMDSCAN_2 "\n  port %3d (unprotected), running: %s"
#define LANG_TEXT_CMDSCAN_3 "\n  port %3d (password protected: %2d characters), running: %s"
#define LANG_TEXT_CMDSCAN_4 "\n  encryption key: %d bits"
#define LANG_TEXT_CMDSCAN_5 "\n  encryption key: none"
#define LANG_TEXT_CMDSCAN_6 "\n  number of times you can bounce through this server: %d"
#define LANG_TEXT_CMDSCAN_7 "\n  scan complete."
#define LANG_TEXT_CMDSCAN_8 " Syntax error. You haven't specified the host you wish to scan.\n Usage: scan server \n Example: scan server.com"
#define LANG_TEXT_CMDSCAN_9 " Error: You can't use this command on the localhost\n"
#define LANG_TEXT_CMDSCAN_10 " Server unreachable: %s"

// * Downgrade command
#define LANG_TEXT_CMDDOWNGRADE_1 "You can use this commmand to sell all your hardware, and receive back 90%% of it's value."
#define LANG_TEXT_CMDDOWNGRADE_2 "\nType:"
#define LANG_TEXT_CMDDOWNGRADE_3 "\nDOWNGRADE ALL \nto sell all your hardware."
#define LANG_TEXT_CMDDOWNGRADE_4 "Selling all hardware..."
#define LANG_TEXT_CMDDOWNGRADE_5 "\nYour hardware values $%d"
#define LANG_TEXT_CMDDOWNGRADE_6 "\nYou have received back 90%% of it's value: %d"
#define LANG_TEXT_CMDDOWNGRADE_7 "\nYou only have the basic hardware configuration installed."
#define LANG_TEXT_CMDDOWNGRADE_8 "\nYou don't have any hardware to sell."

// * Upgrade command
#define LANG_TEXT_CMDUPGRADE_1  "\n The following upgrade codes are available: "
#define LANG_TEXT_CMDUPGRADE_2  "\n CPU: "
#define LANG_TEXT_CMDUPGRADE_3  "\n - %s, price: $%5d, Upgrade CODE: CPU%d"
#define LANG_TEXT_CMDUPGRADE_4  "\n MEMORY: "
#define LANG_TEXT_CMDUPGRADE_5  "\n - %s, price: $%5d, Upgrade CODE: MEM%d"
#define LANG_TEXT_CMDUPGRADE_6  "\n FIREWALL: "
#define LANG_TEXT_CMDUPGRADE_7  "\n - %s, price: $%5d, Upgrade CODE: FWL%d"
#define LANG_TEXT_CMDUPGRADE_8  "\n MODEM: "
#define LANG_TEXT_CMDUPGRADE_9  "\n - %s, price: $%5d, Upgrade CODE: MDM%d"
#define LANG_TEXT_CMDUPGRADE_10 "\n Neural Network Adapter: "
#define LANG_TEXT_CMDUPGRADE_11 "\n - Neural Network Adapter, price: $50000, Upgrade CODE: NNA"
#define LANG_TEXT_CMDUPGRADE_12 "\n Type: UPGRADE CODE, to perform an upgrade. "
#define LANG_TEXT_CMDUPGRADE_13 "\n Note: upgrade codes are ok in lowercase. "

#define LANG_TEXT_CMDUPGRADE_14 "\n Upgrading configuration..."
#define LANG_TEXT_CMDUPGRADE_15 "\n Upgrade parameter: %d/%d"
#define LANG_TEXT_CMDUPGRADE_16 "\n Upgrading CPU ... "
#define LANG_TEXT_CMDUPGRADE_17 "\n slot 1. Done!"
#define LANG_TEXT_CMDUPGRADE_18 "\n slot 2. Done!"
#define LANG_TEXT_CMDUPGRADE_19 "\n Upgrading MEMORY ... "
#define LANG_TEXT_CMDUPGRADE_20 "\n Upgrading FIREWALL ... "
#define LANG_TEXT_CMDUPGRADE_21 "\n done!"
#define LANG_TEXT_CMDUPGRADE_22 "\n Upgrading MODEM ... "
#define LANG_TEXT_CMDUPGRADE_23 "\n Installing Neural Network Adapter ... "
#define LANG_TEXT_CMDUPGRADE_24 "\n ERROR: Unable to perform the selected upgrade."
#define LANG_TEXT_CMDUPGRADE_25 "\n You don't have enough money for the selected upgrade."
#define LANG_TEXT_CMDUPGRADE_26 "\n You need at least $%d.\n"
#define LANG_TEXT_CMDUPGRADE_27 "\n ERROR: Invalid upgrade code entered: [%s]"
#define LANG_TEXT_CMDUPGRADE_28 "\n Type: UPGRADE, to see the list of available upgrade codes.\n"
#define LANG_TEXT_CMDUPGRADE_29 "\n ERROR: You are trying to update to a smaller version firewall."
#define LANG_TEXT_CMDUPGRADE_30 "\n ERROR: You are trying to update to a slower modem."
#define LANG_TEXT_CMDUPGRADE_31 "\n ERROR: You are trying to update to a smaller size memory."
#define LANG_TEXT_CMDUPGRADE_32 "\n ERROR: You are trying to update to a lower or same performance CPU."

#define LANG_TEXT_CMDUPGRADE_NONE "none"

// * Hardware names for UPGRADE COMMAND
static char* HE_NAME_CPU[]      = {"1 Ghz CPU","2 Ghz CPU","4 Ghz CPU","8 Ghz CPU"};
static char* HE_NAME_MODEM[]    = {"1 Mbps Modem","2 Mbps Modem","4 Mbps Modem","8 Mbps Modem"};
static char* HE_NAME_FIREWALL[] = {"(none)","class 1 firewall","class 2 firewall","class 3 firewall"};
static char* HE_NAME_MEM[]      = {"1 Gb QRam","2 Gb QRam","4 Gb QRam","8 Gb QRam"};

// * Transfer command
#define LANG_TEXT_CMDXFER_1 " Trying to transfer $%d"
#define LANG_TEXT_CMDXFER_2 "\n Error: There is no money on the server."
#define LANG_TEXT_CMDXFER_3 "\n You can type ABORT anytime, to abort the transfer."
#define LANG_TEXT_CMDXFER_4 "\n Error: Transfer already in progress. Please wait until it's completed."
#define LANG_TEXT_CMDXFER_5 "\n Error: You must be connected to a server, to transfer money."
#define LANG_TEXT_CMDXFER_6 "\n Error: Please enter a positive amount of money to transfer."
#define LANG_TEXT_CMDXFER_7 "\n Error: You can only transfer a maximum of $%d."
#define LANG_TEXT_CMDXFER_8 "\n Error : All services on the server must be hacked, before being able to transfer money."

// * Download command
#define LANG_TEXT_CMDDL_1 "\nYou can type ABORT anytime, to abort the download."
#define LANG_TEXT_CMDDL_2 "\nDownloading %s (%d Mbytes)... please wait."
#define LANG_TEXT_CMDDL_3 "WARNING: download aborted (%s)...\n"
#define LANG_TEXT_CMDDL_4 "Error: You are not connected to a server."
#define LANG_TEXT_CMDDL_5 "\nYou must be connected to a server to be able to download files."
#define LANG_TEXT_CMDDL_6 "Error: There is not enough space to store the file."
#define LANG_TEXT_CMDDL_7 "\nPlease upgrade your system memory or delete unnecesary files from your localhost.\n"
#define LANG_TEXT_CMDDL_8 "Error: file already exists on your localhost\n"


// * Upload command
#define LANG_TEXT_CMDUPLOAD_1 "\n You can type ABORT anytime, to abort the upload."
#define LANG_TEXT_CMDUPLOAD_2 " Error: You are not connected to a server.\n You must be connected to a server to be able to upload files."
#define LANG_TEXT_CMDUPLOAD_3 "Error: file already exists on remote host. Unable to upload."
#define LANG_TEXT_CMDUPLOAD_4 "\nUploading %s (%d Mbytes)... please wait."
#define LANG_TEXT_CMDUPLOAD_5 "WARNING: upload aborted (%s)...\n"

// * Crack command
#define LANG_TEXT_CMDCRACK_1  "\n You can type ABORT anytime, to abort the crack."
#define LANG_TEXT_CMDCRACK_2  " Error: Unable to start cracker. Another command is in progress...(%d)"
#define LANG_TEXT_CMDCRACK_3  " Error: Unable to start cracker. Please LOGOUT first... "
#define LANG_TEXT_CMDCRACK_4  " Error: Server unreachable: [%s:%d]"
#define LANG_TEXT_CMDCRACK_5  " Bouncing link through %d host(s)...\n"
#define LANG_TEXT_CMDCRACK_6  " cracking: %s[port : %d]"
#define LANG_TEXT_CMDCRACK_7  "\n Unable to crack service on port %d. There is no service running on that port."
#define LANG_TEXT_CMDCRACK_8  "\n Type: scan %s, to see what services are running on this server."
#define LANG_TEXT_CMDCRACK_9  "\n Service is unprotected. You don't need to crack it."
#define LANG_TEXT_CMDCRACK_10 " Syntax error. You haven't specified a host and portname to crack.\n Usage: crack server port\n Example: crack server.com 110\n"

// * Decrypt command
#define LANG_TEXT_CMDDECRYPT_1 "\n  You can type ABORT anytime, to abort the decrypt."
#define LANG_TEXT_CMDDECRYPT_2 " Error: Unable to start decrypt. Another command is in progress...(%d)"
#define LANG_TEXT_CMDDECRYPT_3 " Error: Unable to start decrypt. Please LOGOUT first... "
#define LANG_TEXT_CMDDECRYPT_4 " Syntax error. You haven't specified a host to decrypt it's key.\n Usage: decrypt server\n Example: decrypt server.com"
#define LANG_TEXT_CMDDECRYPT_5 " Bouncing link through %d host(s)...\n"
#define LANG_TEXT_CMDDECRYPT_6 " Error: Server unreachable: [%s]"
#define LANG_TEXT_CMDDECRYPT_7 " decrypting: %s"
#define LANG_TEXT_CMDDECRYPT_8 "\n Server has no encryption key. You don't need to decrypt it."
#define LANG_TEXT_CMDDECRYPT_9 " Unable to connect. Server is added to the bounced link.\n Please remove the server from the bounced link, by clicking on it.\n"

// * Exec command
#define LANG_TEXT_CMDEXEC_1 " executing [%s][%s]"
#define LANG_TEXT_CMDEXEC_2 "\n Exploit loaded (%s/%d)"
#define LANG_TEXT_CMDEXEC_3 "\n Launching exploit on [%s], port: %d"
#define LANG_TEXT_CMDEXEC_4 "\n Success! Service password has been disabled."
#define LANG_TEXT_CMDEXEC_5 "\n You may now connect to %s on port %d"
#define LANG_TEXT_CMDEXEC_6 " Syntax error\n Usage: exploit filename.exploit server\n"
#define LANG_TEXT_CMDEXEC_7 "\n Error. Port %d is closed on [%s]\n" 
#define LANG_TEXT_CMDEXEC_8 "\n Error. Exploit file doesn't exist on your localhost: [%s]\n"
#define LANG_TEXT_CMDEXEC_9 " Error. Please disconnect before executing an exploit.\n"
#define LANG_TEXT_CMDEXEC_10 "\n\n Failed to execute exploit.\n The exploit doesn't work on this host/service\n"
#define LANG_TEXT_CMDEXEC_11 "\n Error. Server unreachable: [%s]\n"
#define LANG_TEXT_CMDEXEC_12 "\n Error. Can't execute exploits on localhost\n"
#define LANG_TEXT_CMDEXEC_13 "\n Error: [%s] is not a valid exploit file.\n"

// * Login command
#define LANG_TEXT_CMDLOGIN_1 " Trying password ... [%s]"
#define LANG_TEXT_CMDLOGIN_2 "\n Try: port %3d, service: %25s - "
#define LANG_TEXT_CMDLOGIN_3 "success. Password disabled."
#define LANG_TEXT_CMDLOGIN_4 " Syntax error\n Usage: login server password\n"
#define LANG_TEXT_CMDLOGIN_5 " Syntax error: you must specify a password.\n Usage: login server password\n"
#define LANG_TEXT_CMDLOGIN_6 "skipped. Port is not password protected."
#define LANG_TEXT_CMDLOGIN_7 "failed."
#define LANG_TEXT_CMDLOGIN_8 " Error: server is unreachable: [%s]\n"

// * exosyphen command (easter-egg)
#define LANG_TEXT_CMD_EXOSYPHEN "\nexosyphen studios made this game :-) (www.exosyphen.com, office@exosyphen.com)\n"

// * killtrace command
#define LANG_TEXT_CMDKILL_1  "\n You don't have enough money to reduce your trace level."
#define LANG_TEXT_CMDKILL_2  "\n You need at least $%d to reduce your tracetime by 10%%. \n"
#define LANG_TEXT_CMDKILL_3  "\n - releasing network interface..."
#define LANG_TEXT_CMDKILL_4  "\n - requesting new IP address..."
#define LANG_TEXT_CMDKILL_5  "\n - new IP address received..."
#define LANG_TEXT_CMDKILL_6  "\n - restarting network interface...\n"
#define LANG_TEXT_CMDKILL_7  "\n Your trace level has been reduced from %d%% to %d%%."
#define LANG_TEXT_CMDKILL_8  "\n $%d has been deducted from your account. "
#define LANG_TEXT_CMDKILL_9  "\n Trace level is 0%."
#define LANG_TEXT_CMDKILL_10 "\n There is no need to use killtrace at this time. \n"

// * Connect command
#define LANG_TEXT_CMD_CON_1 " Unable to connect. Server is added to the bounced link."
#define LANG_TEXT_CMD_CON_2 "\n Please remove the server from the bounced link, by clicking on it."
#define LANG_TEXT_CMD_CON_3 " Bouncing link through %d host(s)...\n"
#define LANG_TEXT_CMD_CON_4 " connecting to: %s[%d]"
#define LANG_TEXT_CMD_CON_5 "\n Unable to connect on port %d. There is no service running on that port."
#define LANG_TEXT_CMD_CON_6 "\n Type: scan %s, to see what services are running on this server."
#define LANG_TEXT_CMD_CON_7 " Unable to connect. Server has an encryption key"
#define LANG_TEXT_CMD_CON_8 "\n You must decrypt the key, using the DECRYPT command\n"
#define LANG_TEXT_CMD_CON_9 "\n Unable to connect. Access to this service is password protected."
#define LANG_TEXT_CMD_CON_10 " Error: You are already connected to your localhost.\n"
#define LANG_TEXT_CMD_CON_11 " Unable to connect while the password cracker is running.\n"
#define LANG_TEXT_CMD_CON_12 "\n connected."
#define LANG_TEXT_CMD_CON_13 " Syntax error. You haven't specified the host you are connecting to.\n Usage: connect server port\n Example: connect server.com 110\n"
#define LANG_TEXT_CMD_CON_14 " Unable to connect while the key decrypter is running.\n"
#define LANG_TEXT_CMD_CON_15 " Server unreachable: [%s:%d]"

// * Bounceinfo command
#define LANG_TEXT_CMD_BINFO_1 "\n === Bounced link status:"
#define LANG_TEXT_CMD_BINFO_2 "\n     - no bounced link setup"
#define LANG_TEXT_CMD_BINFO_3 "\n === The following servers can be used to bounce through:"
#define LANG_TEXT_CMD_BINFO_4 "\n     - [%25s] - %d bounces left"                              
#define LANG_TEXT_CMD_BINFO_5 "\n     - [%25s] - %d bounces left (server needs to be hacked first)"
#define LANG_TEXT_CMD_BINFO_6 "\n    %d host(s) in the bounced link"

// * Logout command
#define LANG_TEXT_CMD_LOGOUT_1 "\n Disconnecting from %s..."
#define LANG_TEXT_CMD_LOGOUT_2 " You are not connected to any server."
#define LANG_TEXT_CMD_LOGOUT_3 "\n Money transfer cancelled"
#define LANG_TEXT_CMD_LOGOUT_4 "\n Upload (%s) canceled."
#define LANG_TEXT_CMD_LOGOUT_5 "\n Download (%s) canceled."

// * LS command
#define LANG_TEXT_CMD_LS_1 "\n\n Total: %d file(s)"
#define LANG_TEXT_CMD_LS_2 "\n Storage space: %d Mb (Used: %d Mb | Free: %d Mb)"

// * Abort command
#define LANG_TEXT_CMD_ABORT_1 " Nothing to abort...\n"
#define LANG_TEXT_CMD_ABORT_2 " Money transfer aborted...\n"
#define LANG_TEXT_CMD_ABORT_3 " File download aborted...\n"
#define LANG_TEXT_CMD_ABORT_4 " File upload aborted...\n"
#define LANG_TEXT_CMD_ABORT_5 " Password crack aborted...\n"
#define LANG_TEXT_CMD_ABORT_6 " Key decryption aborted...\n"

// * Cat command
#define LANG_TEXT_CMD_CAT_1 " Error: No file name specified"
#define LANG_TEXT_CMD_CAT_2 "\n Usage: cat filename (example: cat connection.log)"
#define LANG_TEXT_CMD_CAT_3 " Error. No such file: [%s]"
#define LANG_TEXT_CMD_CAT_4 " Access to [%s] is restricted on port %d"
#define LANG_TEXT_CMD_CAT_5 "\n Please connect on port %d, to access the file."


// * Bounce command
#define LANG_TEXT_CMD_BOUNCE_1   " Error. You must specify a hostname to add/remove from/to the bounced link."
#define LANG_TEXT_CMD_BOUNCE_2   "\n Usage: bounce server"
#define LANG_TEXT_CMD_BOUNCE_3   " Host added to bounced link: [%s]"
#define LANG_TEXT_CMD_BOUNCE_3_1 "\n Host added to bounced link: [%s]\n"
#define LANG_TEXT_CMD_BOUNCE_4   " Host removed from bounced link: [%s]"
#define LANG_TEXT_CMD_BOUNCE_4_1 "\n Host removed from bounced link: [%s]\n"

#define LANG_TEXT_BOUNCE_1 "\n Error: All services on [%s] must be hacked,\n in order to be able to bounce through it."
#define LANG_TEXT_BOUNCE_2 "\n Type: BOUNCEHELP in the console to learn more about bounced links.\n"
#define LANG_TEXT_BOUNCE_3 " Error. [%s] doesn't exist."
#define LANG_TEXT_BOUNCE_4 "\n Error: You can't bounce anymore through [%s]"
#define LANG_TEXT_BOUNCE_5 "\n Error: You can't bounce anymore through [%s]\n"

// * Delete command
#define LANG_TEXT_CMDDEL_1 " Error. File not found: %s"
#define LANG_TEXT_CMDDEL_2 " File deleted: %s"
#define LANG_TEXT_CMDDEL_3 " This file can't be accessed on the current port: %d"

// * Config command
#define LANG_TEXT_CMD_CONFIG_1 "\n Your current hardware configuration: "
#define LANG_TEXT_CMD_CONFIG_2 "\n Storage space: %d Mb (Used: %d Mb | Free: %d Mb)"
#define LANG_TEXT_CMD_CONFIG_3 "\n Tracetime decrease factor : %dx (firewall)"
#define LANG_TEXT_CMD_CONFIG_4 "\n Transfer increase factor  : %dx (modem)"
#define LANG_TEXT_CMD_CONFIG_5 "\n Speed increase factor     : %dx (CPU)"
#define LANG_TEXT_CMD_CONFIG_6  "\n CPU Slot 1    : %s"
#define LANG_TEXT_CMD_CONFIG_7  "\n CPU Slot 2    : %s" 
#define LANG_TEXT_CMD_CONFIG_8  "\n CPU Slot 2    : (none)"
#define LANG_TEXT_CMD_CONFIG_9  "\n Memory Slot 1 : %s"
#define LANG_TEXT_CMD_CONFIG_10 "\n Memory Slot 2 : %s"
#define LANG_TEXT_CMD_CONFIG_11 "\n Memory Slot 2 : (none)"
#define LANG_TEXT_CMD_CONFIG_12 "\n Modem         : %s"
#define LANG_TEXT_CMD_CONFIG_13 "\n Firewall      : %s"

// * Unknown command error
#define LANG_TEXT_CMD_ERR_1 "\nUnknown command: [%s](param: %s)"
#define LANG_TEXT_CMD_ERR_2 "\nType HELP, to see a list of available commands\n"

// * Trace status
#define LANG_TEXT_TRACE_1 "Trace ETA: %2d:%1d s"
#define LANG_TEXT_TRACE_2 "You have been traced!"
#define LANG_TEXT_TRACE_3 "Trace lost"

// * Host tooltip texts (when mouse-over)
#define LANG_TEXT_HTIP_1  "ports:  %d (open/hacked:  %d)"
#define LANG_TEXT_HTIP_2  "encryption key:  %d bits"
#define LANG_TEXT_HTIP_3  "encryption key:  none"
#define LANG_TEXT_HTIP_4  "money:  $%d | files: %d"
#define LANG_TEXT_HTIP_5  "bounces left: %d"

// * Hardware tooltip texts (when mouse over)
#define LANG_TEXT_HW_TIP_1 "CPU1: %s [%dx speed increase]"
#define LANG_TEXT_HW_TIP_2 "CPU2: %s [%dx speed increase]"
#define LANG_TEXT_HW_TIP_3 "CPU2: none"
#define LANG_TEXT_HW_TIP_4 "MEM2: none"
#define LANG_TEXT_HW_TIP_5 "%s [%dx faster transfers]"
#define LANG_TEXT_HW_TIP_6 "No firewall installed."
#define LANG_TEXT_HW_TIP_7 "%s [%dx slower trace]"
#define LANG_TEXT_HW_TIP_8 "Neural Network Adapter: 2x more speed."
#define LANG_TEXT_HW_TIP_9 "Neural Network Adapter: Not installed."

// * Misc
#define LANG_TEXT_MISC_NOSUCHFILE " Error: No such file: %s"
#define LANG_TEXT_MISC_GAMEDIFF   "\nGame difficulty increase: %d"

#define LANG_TEXT_MISC_DOWNLOADING "Downloading: %d%%"
#define LANG_TEXT_MISC_TRANSFERING "Transferring: %d%%"
#define LANG_TEXT_MISC_UPLOADING   "Uploading: %d%%"
#define LANG_TEXT_MISC_DECRYPTING  "decrypting (%4d-.-%d)"
#define LANG_TEXT_MISC_CRACKING    "%s-[%3d chars left]"
#define LANG_TEXT_MISC_LAUNCHING   "\n launching."

#define LANG_TEXT_MISC_CHEAT "\nWarning : cheat code has been used. Your score has been reset to 0.\n"

#define LANG_TEXT_CREDITS_1 "Hacker Evolution"
#define LANG_TEXT_CREDITS_2 "Developed by exosyphen studios"
#define LANG_TEXT_CREDITS_3 "Soundtrack by DJ Velocity (www.dj-velocity.com)"
#define LANG_TEXT_CREDITS_4 "website: http://www.exosyphen.com/"
#define LANG_TEXT_CREDITS_5 "e-mail:  office@exosyphen.com"
#define LANG_TEXT_CREDITS_6 "Copyright © 2007, exosyphen studios"
#define LANG_TEXT_CREDITS_7 "All rights reserved."

#endif