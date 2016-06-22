// hePlayerInfo.cpp
// 
// Player information management class
// This holds the variables that are tied to the player
//
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "hePlayerInfo.h"
#include "heLog.h"
#include <stdio.h>
#include <string.h>

// =============================================================================================================================
// Class constructor
hePlayerInfo::hePlayerInfo()
{
	strcpy(m_username,"root");
	strcpy(m_hostname,"localhost");

	resetPlayerInfo();	
}

// =============================================================================================================================
// Class destructor
hePlayerInfo::~hePlayerInfo()
{
}

// =============================================================================================================================
// Set host info
void hePlayerInfo::setHostInfo(char* username, char* hostname)
{
	strcpy(m_username,username);
	strcpy(m_hostname,hostname);
}

// =============================================================================================================================
// Retrieve command prompt text
char* hePlayerInfo::getCommandPrompt()
{
	sprintf(m_commandprompt,"/color %d %d %d %s@%s:> ",SKINCOLOR(gSkinColorCommandPrompt),m_username,m_hostname);
	return m_commandprompt;
}
// =============================================================================================================================
// Find the index of a file on the player's host (if it exists)
int hePlayerInfo::getFileIndex(char* name)
{
	int i, index = -1;

	for(i = 0; i < this->fileCount; i++)
		if(strcmp(name,this->fileList[i].fileName) == 0)
			index = i;

	return index;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Reset player info (before a new level start)
void hePlayerInfo::resetPlayerInfo()
{
	fileCount = 0;

	// * Reset hardware
	cpu1      = 0; 
	cpu2      = -1;
	mem1      = 0;
	mem2      = -1;
	modem     = 0;
	firewall  = 0;
	nnadapter = -1;

	// * Reset stats
	mScore       = 0;
	mGlobalTrace = 0;
	mHackCount   = 0;
	mTraceCount  = 0;

	mLevelCount  = 0;
	mMinuteCount = 0;

	mMoney = 0;

	mLostCount = 0;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Save player info (game)
void hePlayerInfo::saveGame(char* filename)
{
	FILE* file;
	int   checksum = 0;

	gApplicationLog.printf("\n hePlayerInfo::saveGame(%s)",filename);
	file = fopen(filename,"wt");
	
		fprintf(file,"%s - (%s-%s)\n",mNickname,gModName,gModLevelList[gModLevelCurrent]);
		fprintf(file,"%s\n",gModPath);          // * mod path
		fprintf(file,"%d\n",gModLevelCurrent);  // * level index

		fprintf(file,"%d\n",cpu1);  // * player hardware
		fprintf(file,"%d\n",cpu2);
		fprintf(file,"%d\n",mem1);
		fprintf(file,"%d\n",mem2);
		fprintf(file,"%d\n",modem);
		fprintf(file,"%d\n",firewall);

		fprintf(file,"%d\n",mScore); // * player info
		fprintf(file,"%d\n",mMoney);
		fprintf(file,"%d\n",mHackCount);
		fprintf(file,"%d\n",mTraceCount);

		fprintf(file,"%d\n",mMinuteCount);
		fprintf(file,"%d\n",mLevelCount);

		fprintf(file,"%d\n",nnadapter);
		fprintf(file,"%d\n",mGlobalTrace);
		fprintf(file,"%d\n",mLostCount);

		// * calculate and save checksum
		checksum += gModLevelCurrent * 10;
		checksum += cpu1 * 5;
		checksum += cpu2 * 6;
		checksum += mem1 * 7;
		checksum += mem2 * 8;
		checksum += modem * 9;
		checksum += firewall * 11;

		checksum += mScore * 13;
		checksum += mMoney * 14;
		checksum += mHackCount  * 15;
		checksum += mTraceCount * 16;

		checksum += mMinuteCount  * 2;
		checksum += mLevelCount * 3;

		checksum += nnadapter * 17;
		checksum += mGlobalTrace * 18;
		checksum += mLostCount * 19;

		fprintf(file,"%d\n",checksum); // * checksum

	fclose(file);
	gApplicationLog.printf(" ...done");
}	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Load player profile (game)
void hePlayerInfo::loadGame(char* filename)
{
	FILE*    file;
	int      checksum = 0, mChecksum;
	hestring temp;

	gApplicationLog.printf("\n hePlayerInfo::loadGame(%s)",filename);
	file = fopen(filename,"rt");
	
		fgets(temp,1024,file);
		sscanf(temp,"%s",mNickname);         gApplicationLog.printf("\n -Nickname : %s",mNickname);
		fscanf(file,"%s",gModPath);          gApplicationLog.printf("\n -gModPath : %s",gModPath);
		fscanf(file,"%d",&gModLevelCurrent); gApplicationLog.printf("\n -gModLevelCurent : %d",gModLevelCurrent);

		fscanf(file,"%d",&cpu1);             gApplicationLog.printf("\n -cpu1 : %d",cpu1);
		fscanf(file,"%d",&cpu2);             gApplicationLog.printf("\n -cpu2 : %d",cpu2);
		fscanf(file,"%d",&mem1);             gApplicationLog.printf("\n -mem1 : %d",mem1);
		fscanf(file,"%d",&mem2);             gApplicationLog.printf("\n -mem2 : %d",mem2);
		fscanf(file,"%d",&modem);            gApplicationLog.printf("\n -modem : %d",modem);
		fscanf(file,"%d",&firewall);         gApplicationLog.printf("\n -firewall : %d",firewall);

		fscanf(file,"%d",&mScore);           gApplicationLog.printf("\n -mScore : %d",mScore);
		fscanf(file,"%d",&mMoney);           gApplicationLog.printf("\n -mMoney : %d",mMoney);
		fscanf(file,"%d",&mHackCount);       gApplicationLog.printf("\n -mHackCount : %d",mHackCount);
		fscanf(file,"%d",&mTraceCount);      gApplicationLog.printf("\n -mTraceCount : %d",mTraceCount);

		fscanf(file,"%d",&mMinuteCount);     gApplicationLog.printf("\n -mMinuteCount : %d",mMinuteCount);
		fscanf(file,"%d",&mLevelCount);      gApplicationLog.printf("\n -mLevelCount : %d",mLevelCount);

		fscanf(file,"%d",&nnadapter);        gApplicationLog.printf("\n -nnadapter : %d",mLevelCount);
		fscanf(file,"%d",&mGlobalTrace);     gApplicationLog.printf("\n -mGlobalTrace : %d",mGlobalTrace);
		fscanf(file,"%d",&mLostCount);       gApplicationLog.printf("\n -mLostCount : %d",mLostCount);

		fscanf(file,"%d",&mChecksum);        gApplicationLog.printf("\n -mChecksum : %d",mChecksum);

		// * etc
		setHostInfo(mNickname,"localhost");

		// * calculate and save checksum
		checksum += gModLevelCurrent * 10;
		checksum += cpu1 * 5;
		checksum += cpu2 * 6;
		checksum += mem1 * 7;
		checksum += mem2 * 8;
		checksum += modem * 9;
		checksum += firewall * 11;

		checksum += mScore * 13;
		checksum += mMoney * 14;
		checksum += mHackCount  * 15;
		checksum += mTraceCount * 16;

		checksum += mMinuteCount  * 2;
		checksum += mLevelCount * 3;

		checksum += nnadapter * 17;
		checksum += mGlobalTrace * 18;
		checksum += mLostCount * 19;

		if(mChecksum != checksum)
		{
			gApplicationLog.printf("\n ERROR : savegame was changed outside the game...");
			mScore = 0;
		}


	fclose(file);
	gApplicationLog.printf(" ...done");
}	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Checks a savegame to ensure it hasn't been hacked
int hePlayerInfo::validateSaveGame(char* filename)
{
	FILE*     file;
	int       mChecksum;
	hestring  temp;
	int       iTemp, iTempCheckSum = 0;
	hestring  sTemp;

	gApplicationLog.printf("\n hePlayerInfo::validateSaveGame(%s)",filename);
	file = fopen(filename,"rt");
	
		fgets(temp,1024,file);
		fscanf(file,"%s",sTemp);
		fscanf(file,"%d",&iTemp); iTempCheckSum += 10 * iTemp;

		fscanf(file,"%d",&iTemp); iTempCheckSum += 5 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 6 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 7 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 8 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 9 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 11 * iTemp;

		fscanf(file,"%d",&iTemp); iTempCheckSum += 13 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 14 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 15 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 16 * iTemp;

		fscanf(file,"%d",&iTemp); iTempCheckSum += 2 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 3 * iTemp;

		fscanf(file,"%d",&iTemp); iTempCheckSum += 17 * iTemp;
		fscanf(file,"%d",&iTemp); iTempCheckSum += 18 * iTemp;

		fscanf(file,"%d",&iTemp); iTempCheckSum += 19 * iTemp;

		fscanf(file,"%d",&mChecksum);

		// * validate checksum
		if(mChecksum != iTempCheckSum)
		{
			// * gApplicationLog.printf(" - failed (%d-%d)",mChecksum, iTempCheckSum);
			return 0;
		}


	fclose(file);
	gApplicationLog.printf(" - ok");
	return 1;
}	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * CPU multiplier
int hePlayerInfo::getCPUMultiplier()
{
	int _multiplier = 1;
    int cpufactor[4] = {1,2,4,8};

    _multiplier = cpufactor[cpu1];
	if(cpu2 >= 0) _multiplier += cpufactor[cpu2];

	if(nnadapter == 0) _multiplier *= 2;

	//gApplicationLog.printf("\nhePlayerInfo::getCPUMultiplier() is %d (%d-%d)",_multiplier,cpu1,cpu2);

	return _multiplier;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * MEM multiplier
int hePlayerInfo::getMemMultiplier()
{
	int multiplier = 1;

	multiplier = power_of(2,mem1) + power_of(2,mem2);

	if(nnadapter == 0) multiplier *= 2;

	return multiplier;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Modem multiplier
int hePlayerInfo::getModemMultiplier()
{
	int multiplier = 1;
    int modemfactor[4] = {1,2,4,8};

	//multiplier = power_of(2,modem);
    multiplier = modemfactor[modem];

	if(nnadapter == 0) multiplier *= 2;

	return multiplier;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Firewall multiplier
int hePlayerInfo::getFirewallMultiplier()
{
	int multiplier = 1;
    int firewallfactor[4] = {1,2,3,4};

	//multiplier = power_of(2,firewall);
    multiplier = firewallfactor[firewall];

	if(nnadapter == 0) multiplier *= 2;

	return multiplier;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Memory size
int hePlayerInfo::getMemSize()
{
	int size = 0;

	if(mem1 == 0) size = 1024;
	if(mem1 == 1) size = 2048;
	if(mem1 == 2) size = 4096;
	if(mem1 == 3) size = 8192;

	if(mem2 == 0) size += 1024;
	if(mem2 == 1) size += 2048;
	if(mem2 == 2) size += 4096;
	if(mem2 == 3) size += 8192;

	return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Get used up memory size
int hePlayerInfo::getMemSizeUsed()
{
	int i,size = 0;

	for(i = 0; i < fileCount; i++)
		size += fileList[i].size;

	return size;
}
