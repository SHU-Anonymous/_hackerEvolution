// heGameLevel.cpp
// 
// Game level management class
// Code : Robert Muresan (robert@exosyphen.com, exoSyphen Studios)
//
#include "heGameLevel.h"
#include "HackerEvolutionDefines.h"
#include "HackerEvolutionUntold_Defines.h"
#include "heutGameLanguage.h"
#include "heLog.h"
#include <string.h>

#ifndef HE_EDITOR_BUILD
#include "heSystem.h"
#include "bshAudioDirectX.h"
#include "heKeyStack.h"
#endif

#include <stdio.h>

#if defined(PLATFORM_WINDOWS)
#include <io.h>
#include <stdlib.h>
#endif

#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

extern hePlayerInfo hePlayer; // reference to hePlayer defined in heEngine.cpp

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Random password generation
void generateRandomPassword(char* password, int length)
{

	// * Initialize random number seed
	srand(system_TimeMilliSeconds());

	for(int i = 0; i < length; i++)
		password[i] = 'a' + rand() % 20;
    password[length] = 0;
}

// =============================================================================================================================
// Load host data from file
void heHost::LoadFromFile(char* pHostName, char* pGameModName, char* pGameLevelName)
{
	char  temp[1024];
	FILE* file;
	int   i;

	gApplicationLog.printf("\nModule::heHost::loadFromFile() - loading : [%s]",pHostName);
	strcpy(hostName,pHostName);

	// load host info
	gApplicationLog.printf("\nModule::heHost::loadFromFile() - loading host info");
	sprintf(temp,"%s%s/info",createModLevelFilePath(""),pHostName);
	gApplicationLog.printf("(%s)",temp);
	file = fopen(temp,"rt");
		fscanf(file,"%d %d",&mapx,&mapy);
		fscanf(file,"%d",&mMoney);
		fscanf(file,"%d",&mEncryptionKey);
	gApplicationLog.printf("\nModule::heHost::loadFromFile() - encryption key : %d bits",mEncryptionKey);
	fclose(file);
	mapx += HE_TARGETMAP_XOFFS; 
	mapy += HE_TARGETMAP_YOFFS;

   // * Dinamic difficulty adjustment
   if(gDinamicDifficulty == 1)
   {
      mEncryptionKey *= hePlayer.getDinamicPerformanceMultiplier();
   }


	// load services info
	gApplicationLog.printf("\nModule::heHost::loadFromFile() - loading services");
	sprintf(temp,"%s%s/services",createModLevelFilePath(""),pHostName);
	file = fopen(temp,"rt");
		if(file == 0)
			gApplicationLog.printf("\n[ERROR]: Module::heHost::loadFromFile() - error opening: [%s]",temp);

		fscanf(file,"%d",&serviceCount);
		if(serviceCount > 0) serviceList = new hostService[serviceCount];
		for(i = 0; i < serviceCount; i++)
		{
			fscanf(file,"%d %s %s",&serviceList[i].port,serviceList[i].name,serviceList[i].password);
			gApplicationLog.printf("\n[%d %s %s]",serviceList[i].port,serviceList[i].name,serviceList[i].password);
			if(strcmp("none",serviceList[i].password) == 0)
				serviceList[i].hasPassword = 0;
			else
				serviceList[i].hasPassword = 1;

			// * random password generation
			if(strcmp("rn04",serviceList[i].password) == 0) generateRandomPassword(serviceList[i].password,4);
			if(strcmp("rn08",serviceList[i].password) == 0) generateRandomPassword(serviceList[i].password,8);
			if(strcmp("rn12",serviceList[i].password) == 0) generateRandomPassword(serviceList[i].password,12);
			if(strcmp("rn16",serviceList[i].password) == 0) generateRandomPassword(serviceList[i].password,16);
			if(strcmp("rn32",serviceList[i].password) == 0) generateRandomPassword(serviceList[i].password,32);
			if(strcmp("rn64",serviceList[i].password) == 0) generateRandomPassword(serviceList[i].password,64);

            decryptPassword(serviceList[i].password);

			// * This is a password set for the final level of Hacker Evolution: Untold
			// to avoid people peeking into files to complete the game
			if(strcmp("hackerevolutionuntold",serviceList[i].password) == 0)
				strcpy(serviceList[i].password,"tleitleitlei");

			gApplicationLog.printf("\nModule::heHost::loadFromFile() - service:[%d][%s][%d]",serviceList[i].port,serviceList[i].name,serviceList[i].hasPassword);
		}	
	fclose(file);

	// * sort services in ascending order (by port number)
	int found = 1;
	while(found == 1)
	{
		found = 0;
		for(i = 0; i < serviceCount - 1; i++)
		{
			if(serviceList[i].port > serviceList[i+1].port)
			{
				hostService temp;
				temp = serviceList[i];
				serviceList[i] = serviceList[i + 1];
				serviceList[i + 1] = temp;
				found = 1;
			}
		}
	}

	// =========================================================================================================================
	// load files
	fileCount = 0;
	gApplicationLog.printf("\nModule::heHost::loadFromFile() - loading file list");

	/* Create a listing of all files on the system */
#if defined(PLATFORM_WINDOWS)
	struct _finddata_t fileinfo;
	long               filehandle = -1L;
	int                nRes;
	char               folder[1024];

	sprintf(folder,"%s%s/files/*.*",createModLevelFilePath(""),pHostName);

	filehandle = _findfirst(folder,&fileinfo);
	if(filehandle != -1L)
	{
		if(strcmp(fileinfo.name,".") != 0 && strcmp(fileinfo.name,".."))
		{
			strcpy(fileList[fileCount].fileName,fileinfo.name);
			sprintf(fileList[fileCount].filePath,"%s%s/files/%s",createModLevelFilePath(""),pHostName,fileinfo.name);
			fileList[fileCount].size    = fileinfo.size;
			if(fileList[fileCount].size < 1) fileList[fileCount].size = 1;
			fileCount++;
		}

		nRes = _findnext(filehandle,&fileinfo);
		

		while((-1L != nRes) && (fileCount < MAX_SERVER_FILES))
		{
			if(strcmp(fileinfo.name,".") != 0 && strcmp(fileinfo.name,".."))
			{
				strcpy(fileList[fileCount].fileName,fileinfo.name);
				sprintf(fileList[fileCount].filePath,"%s%s/files/%s",createModLevelFilePath(""),pHostName,fileinfo.name);
				fileList[fileCount].size    = fileinfo.size;
				if(fileList[fileCount].size < 1)     fileList[fileCount].size = 1;
				if(fileList[fileCount].size > 5000) fileList[fileCount].size = 5000;
				fileCount++;
			}

			nRes = _findnext(filehandle,&fileinfo);
		}
	}
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    char           buff[256];
    struct dirent *dptr;
    DIR           *dirp;

    sprintf(buff,"%s%s/files",createModLevelFilePath(""),pHostName);

    if(((dirp=opendir(buff))==NULL))
    	return;

    while(dptr = readdir(dirp))
    {
        strcpy(fileList[fileCount].fileName,dptr->d_name);
        sprintf(fileList[fileCount].filePath,"%s%s/files/%s",createModLevelFilePath(""),pHostName,dptr->d_name);
        fileList[fileCount].size    = fileSizeBytes(fileList[fileCount].filePath);
        if(fileList[fileCount].size < 1)     fileList[fileCount].size = 1;
        if(fileList[fileCount].size > 5000) fileList[fileCount].size = 5000;
        fileCount++;
    }
    closedir(dirp);
#endif

	// load file rules
	gApplicationLog.printf("\nModule::heHost::loadFromFile() - loading file rules");
	sprintf(temp,"%s%s/filerules",createModLevelFilePath(""),pHostName);
	file = fopen(temp,"rt");
		fscanf(file,"%d",&ruleCount);
	    for(i = 0; i < ruleCount; i++)
		{
			fscanf(file,"%d %s",&ruleList[i].port,ruleList[i].fileName);
			gApplicationLog.printf("\nrule : [%d] [%s]",ruleList[i].port,ruleList[i].fileName);
		}
	fclose(file);

	// * misc other initializations
	mBounceCount = HE_MAX_BOUNCECOUNT;
	mTraceAdded  = 0;
}
// =============================================================================================================================
// returns the index of a service running on the give port, if any
int heHost::isService(int port)
{
	int ret = -1;
	int i;

	for(i = 0; i < serviceCount; i++)
		if(serviceList[i].port == port)
		{
			ret = i;
		}

	return ret;
}
// =============================================================================================================================
// check if there is a rule for a certain file
int heHost::getRulePort(char* file,  int port)
{
	int rulePort = -1,i;
	
	for(i = 0; i < ruleCount; i++)
		if(strcmp(file,ruleList[i].fileName) == 0 && port == ruleList[i].port)
			return 1;

	return 0;
}

// =============================================================================================================================
// return the index of a certain file
int heHost::getFileIndex(char* fileName)
{
	int index = -1, i;
	

    for(i = 0; i < fileCount; i++)
		if(strcmp(fileName,fileList[i].fileName) == 0)
			index = i;

	return index;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * return the number of open ports on a host (wich are not password protected)
int heHost::getOpenPortCount()
{
	int i;
	int count = 0;

	for(i = 0; i < serviceCount; i++)
		if(serviceList[i].hasPassword == 0) count++;

	return count;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * returns 1, if all services and the encryption key, have been hacked
int heHost::hackedCompletely()
{
	int mHackedCompletely = 0;

	if((getOpenPortCount() == serviceCount) && (mEncryptionKey == 0)) 
		mHackedCompletely = 1;

	return mHackedCompletely;
}

// =============================================================================================================================
// Return host index by name
int heGameLevel::getHostIndex(char* hostName)
{
	int i,index = -1;

	for(i = 0; i < hostCount; i++)
	{
		if(strcmp(hostName,hostList[i].hostName) == 0) index = i;
	}

	return index;
}
#ifndef HE_EDITOR_BUILD
/////////////////////////////////////////////////////////////////////////////////////////////////////
// * Start target map animation (fade in)
void heGameLevel::startTargetMapAnimation()
{
	if(targetMapAnimationStarted == 0)
	{
		mTargetmapImage.mAnimationModifier->SetSpeed(30);
		mTargetmapImage.mAnimationModifier->Revert();
		mTargetmapImage.mAnimationModifier->Start();

		targetMapAnimationStarted = 1;
	}
}
// =============================================================================================================================
// Draw targetmap image
void heGameLevel::drawTargetMap(int x, int y)
{
	mTargetMapX = x + WINDOW_HOFFS;
	mTargetMapY = y + WINDOW_VOFFS;

	mTargetmapImage.Draw(x + WINDOW_HOFFS,y + WINDOW_VOFFS);
}
// =============================================================================================================================
// Draw the hosts on the map
void heGameLevel::drawHosts(int x, int y)
{
	int i;
	int xpos,ypos;

	// * Initial fade in in progress?
	if((targetMapAnimationStarted == 1 && mTargetmapImage.mAnimationModifier->mStarted == 1) ||
	   (targetMapAnimationStarted == 0 ))
		return;

	for(i = 0; i < hostCount; i++)
	{
		if(hostList[i].mVisible == 1)
		{
			xpos = x + hostList[i].mapx;
			ypos = y + hostList[i].mapy;
		
			// draw a cross on the map, where the host is located
			render_DrawLine(xpos - 5 ,ypos,xpos - 2,ypos,SKINCOLOR(gSkinColorTargetMap));
			render_DrawLine(xpos + 2 ,ypos,xpos + 5,ypos,SKINCOLOR(gSkinColorTargetMap));

			render_DrawLine(xpos,ypos - 5,xpos,ypos - 2,SKINCOLOR(gSkinColorTargetMap));
			render_DrawLine(xpos,ypos + 5,xpos,ypos + 2,SKINCOLOR(gSkinColorTargetMap));

			// March 4, 2001 - Adjust the hostname coordinates if they the text is going outside the map area
			if(hostList[i].mapx + 8 * strlen(hostList[i].hostName) > HE_TARGETMAP_WIDTH)
			{
				xpos -= (8 * strlen(hostList[i].hostName));
				xpos += 5;
			}
			// draw a rectangle under the hostname, for better visibility
			render_DrawRectangle(xpos + 5,ypos + 3,strlen(hostList[i].hostName) * 7,15,SKINCOLOR(gSkinColorHostNameBase),0);
			
			// Display host name
			if(hostList[i].hackedCompletely() == 1)
				render_DrawText(xpos + 5, ypos + 2, hostList[i].hostName,SKINCOLOR(gSkinColorHostHacked),0);
			else
				render_DrawText(xpos + 5, ypos + 2, hostList[i].hostName,SKINCOLOR(gSkinColorHost),0);
		}
	}

	xpos = x + player_mapx;
	ypos = y + player_mapy;

	// Draw player's server location
	render_DrawRectangle_Empty(xpos - 5, ypos - 5, 10, 10,SKINCOLOR(gSkinColorTargetMap));

	render_DrawLine(xpos - 3 ,ypos,xpos - 2,ypos,SKINCOLOR(gSkinColorTargetMap));
	render_DrawLine(xpos + 2 ,ypos,xpos + 3,ypos,SKINCOLOR(gSkinColorTargetMap));

	render_DrawLine(xpos,ypos - 3,xpos,ypos - 2,SKINCOLOR(gSkinColorTargetMap));
	render_DrawLine(xpos,ypos + 3,xpos,ypos + 2,SKINCOLOR(gSkinColorTargetMap));

	// display host name
	render_DrawText(xpos + 8, ypos - 5, "localhost",SKINCOLOR(gSkinColorTargetMap));

	// Draw a line if connected
	// if(connected == 1)
	//	render_DrawLine(xpos,ypos,x + hostList[con_hostindex].mapx,y + hostList[con_hostindex].mapy,HE_SKINCOLOR_1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * draw a tooltip when the mouse is over the host
void heGameLevel::drawHostToolTip(int pMouseX, int pMouseY)
{
	
	int i,x,y;
	int hostIndex = -1;

	// * Initial fade in in progress?
	if((targetMapAnimationStarted == 1 && mTargetmapImage.mAnimationModifier->mStarted == 1) ||
	   (targetMapAnimationStarted == 0 ))
		return;

	for(i = 0; i < hostCount; i++)
	{
		x = hostList[i].mapx + WINDOW_X_OFFS;
		y = hostList[i].mapy + WINDOW_Y_OFFS;
	
		// * Wait for the animation to finish, before jumping to a new host
		// if(mToolTipImage->mAnimationModifier->mStarted == 0)
			if(abs(pMouseX - x) < 20 && abs(pMouseY - y) < 20 && hostList[i].mVisible == 1) hostIndex = i;
	}

	// * draw tooltip (if case)
	if(hostIndex != - 1)
	{
		if(hostIndex != mLastToolTipHost)
		{
			//mToolTipImage->mAnimationModifier->Revert();
			mToolTipImage->mAnimationModifier->ForceDirection(1,1);
			mToolTipImage->mAnimationModifier->Start();

			mLastToolTipHost = hostIndex;
		}

		mLastToolTipX = hostList[hostIndex].mapx + WINDOW_X_OFFS + woffset_x;
		mLastToolTipY = hostList[hostIndex].mapy + WINDOW_Y_OFFS;

		// * To avoid the tooltip going outside the map area, choose the side where is drawn.
		if(hostList[hostIndex].mapx + HE_HOSTTOOLTIP_WIDTH > HE_TARGETMAP_WIDTH)
		{
			mLastToolTipX -= (HE_HOSTTOOLTIP_WIDTH - 10);
		}

		// * Draw the tooltip above the host, if the host is close to the bottom of the map
		if(hostList[hostIndex].mapy + HE_HOSTTOOLTIP_HEIGHT > HE_TARGETMAP_HEIGHT)
		{
			mLastToolTipY -= (HE_HOSTTOOLTIP_HEIGHT - 10);
		}	
	}

	if((hostIndex != mLastToolTipHost) || (hostIndex == -1))
	{
		// * reset tooltip data
		// mToolTipImage->mAnimationModifier->Revert();
		mToolTipImage->mAnimationModifier->ForceDirection(-1,1);
		mToolTipImage->mAnimationModifier->Start();

		mLastToolTipHost = -1;		
	}

	if(mLastToolTipX != -1)	
		mToolTipImage->Draw(mLastToolTipX + 5,mLastToolTipY - 10);			

	// * display misc. host info, inside the tooltip
	if(hostIndex != -1 && mToolTipImage->mAnimationModifier->mStarted == 0)
	{
		render_DrawText_Formated(mLastToolTipX + 15,mLastToolTipY - 5,SKINCOLOR(gSkinColorHost),"%s",hostList[hostIndex].hostName);
		render_DrawText_Formated(mLastToolTipX + 15,mLastToolTipY + 8,192,192,192,GAMETEXT("ports:  %d (open/hacked:  %d)"),hostList[hostIndex].serviceCount,hostList[hostIndex].getOpenPortCount());
		if(hostList[hostIndex].mEncryptionKey != 0)
            render_DrawText_Formated(mLastToolTipX + 15,mLastToolTipY + 23,192,192,192,GAMETEXT("encryption key:  %d bits"),hostList[hostIndex].mEncryptionKey);
		else
			render_DrawText_Formated(mLastToolTipX + 15,mLastToolTipY + 23,192,192,192,GAMETEXT("encryption key:  none"));
		render_DrawText_Formated(mLastToolTipX + 15,mLastToolTipY + 38,192,192,192,GAMETEXT("money:  $%d | files: %d"),hostList[hostIndex].mMoney,hostList[hostIndex].fileCount);
		render_DrawText_Formated(mLastToolTipX + 15,mLastToolTipY + 53,192,192,192,GAMETEXT("bounces left: %d | trace added: %d%%"),hostList[hostIndex].mBounceCount,hostList[hostIndex].mTraceAdded);
	}
	

}
////////////////////////////////////////////////////////////////////////////////////////////////////
// * Draw trace progress
void heGameLevel::drawTraceProgress(int pTracePercent, int pHostIndex, int x, int y, int pLastBounceHost)
{
	int xpos,ypos;
	float deltax, deltay;

	xpos = x + player_mapx;
	ypos = y + player_mapy;

	deltax = (x + hostList[pHostIndex].mapx - xpos) / 100.0f;
	deltay = (y + hostList[pHostIndex].mapy - ypos) / 100.0f;

	// * bounced link in use?
	if(pLastBounceHost != -1)
		render_DrawLine(x + hostList[pLastBounceHost].mapx,y + hostList[pLastBounceHost].mapy,
		                x + hostList[pHostIndex].mapx,y + hostList[pHostIndex].mapy,SKINCOLOR(gSkinColorBouncedLink));

	//render_DrawRectangle_Empty(xpos,ypos,(100 - pTracePercent) * deltax,(100 - pTracePercent) * deltay,SKINCOLOR(gSkinColorTargetMap));

	// * this is only drawn when we are not connected to a host, to avoid overlapping graphics
	//if(connected == 0 || pLastBounceHost != -1)
	//	render_DrawLine(xpos + (100 - pTracePercent) * deltax,ypos + (100 - pTracePercent) * deltay,x + hostList[pHostIndex].mapx,y + hostList[pHostIndex].mapy,SKINCOLOR(gSkinColorTargetMap));
		render_DrawLine(xpos, ypos,x + hostList[pHostIndex].mapx,y + hostList[pHostIndex].mapy,SKINCOLOR(gSkinColorBouncedLink));

	// * New trace style (Hacker Evolution: Untold)
	float tracePosX,tracePosY;

	tracePosX = xpos + (100 - pTracePercent) * deltax;
	tracePosY = ypos + (100 - pTracePercent) * deltay;

	render_DrawRectangle_Empty(tracePosX - TRACE_RECT_SIZE / 2,tracePosY - TRACE_RECT_SIZE / 2,TRACE_RECT_SIZE,TRACE_RECT_SIZE,SKINCOLOR(gSkinColorTargetMap));
	
	render_DrawLine(tracePosX,tracePosY - 1,tracePosX,tracePosY + 1,SKINCOLOR(gSkinColorTargetMap));
	render_DrawLine(tracePosX - 1,tracePosY,tracePosX + 1,tracePosY,SKINCOLOR(gSkinColorTargetMap));

	render_DrawLine(tracePosX,y + TRACE_WINDOW_OFFS_Y,tracePosX,tracePosY - TRACE_RECT_SIZE,SKINCOLOR(gSkinColorTargetMap));
	render_DrawLine(tracePosX,y + TRACE_WINDOW_OFFS_Y + 320,tracePosX,tracePosY + TRACE_RECT_SIZE,SKINCOLOR(gSkinColorTargetMap));

	render_DrawLine(x + 5,tracePosY, tracePosX - TRACE_RECT_SIZE, tracePosY,SKINCOLOR(gSkinColorTargetMap));
	render_DrawLine(x + 685,tracePosY, tracePosX + TRACE_RECT_SIZE, tracePosY,SKINCOLOR(gSkinColorTargetMap));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// * Check if the mouse is over a host
int heGameLevel::mouseOverHost(int pMouseX, int pMouseY)
{
	int index = -1;
	int i,x,y;
	
	// * Initial fade in in progress?
	if((targetMapAnimationStarted == 1 && mTargetmapImage.mAnimationModifier->mStarted == 1) ||
	   (targetMapAnimationStarted == 0 ))
		return -1;

	for(i = 0; i < hostCount; i++)
	{
		x = hostList[i].mapx + WINDOW_X_OFFS;
		y = hostList[i].mapy + WINDOW_Y_OFFS;
		
		if(abs(pMouseX - x) < 20 && abs(pMouseY - y) < 20 && hostList[i].mVisible == 1) index = i;
	}

	return index;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// * Draw bounced links
void heGameLevel::drawBouncedLink(int x, int y, int pCount, int pList[])
{
	int i;
	int color;
	
	if(pCount > 0)
	{
		if(connected == 1)
			render_DrawLine(x + player_mapx,y + player_mapy,x + hostList[pList[0]].mapx,y + hostList[pList[0]].mapy,SKINCOLOR(gSkinColorBouncedLink));
		else
			render_DrawLine(x + player_mapx,y + player_mapy,x + hostList[pList[0]].mapx,y + hostList[pList[0]].mapy,SKINCOLOR(gSkinColorBouncedLink));
	}

	for(i = 0; i < pCount - 1; i++)
	{
		if(connected == 1)
			render_DrawLine(x + hostList[pList[i]].mapx,y + hostList[pList[i]].mapy,
							x + hostList[pList[i + 1]].mapx,y + hostList[pList[i + 1]].mapy,SKINCOLOR(gSkinColorBouncedLink));
		else
			render_DrawLine(x + hostList[pList[i]].mapx,y + hostList[pList[i]].mapy,
							x + hostList[pList[i + 1]].mapx,y + hostList[pList[i + 1]].mapy,SKINCOLOR(gSkinColorBouncedLink));
	}

	if(connected == 1 && pCount > 0)
		render_DrawLine(x + hostList[con_hostindex].mapx,y + hostList[con_hostindex].mapy,
		                x + hostList[pList[pCount - 1]].mapx,y + hostList[pList[pCount - 1]].mapy,SKINCOLOR(gSkinColorBouncedLink));		
	
	if(connected == 1 && pCount == 0)
		render_DrawLine(x + player_mapx,y + player_mapy,
		                x + hostList[con_hostindex].mapx,y + hostList[con_hostindex].mapy,SKINCOLOR(gSkinColorBouncedLink));		
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
// Set mod and level name
void heGameLevel::SetLevelInfo(char* pModName, char* pLevelName)
{
	strcpy(mGameModName,pModName);
	strcpy(mGameLevelName,pLevelName);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the level time in text format
void  heGameLevel::GetLevelTimeText(char* pString)
{
	long hours,minutes,seconds,milli;
	char chours[8],cminutes[8],cseconds[8],cmilli[8];
	
	seconds = mGameLevelTime;
	seconds /= 1000;
	milli   = mGameLevelTime % 100;
	milli   /= 20;

	hours   = seconds / 3600;
	seconds = seconds - hours * 3600;
	minutes = seconds / 60;
	seconds = seconds - minutes * 60;

	// Format time text
	sprintf(chours,"%d",hours);
	sprintf(cminutes,"%d",minutes);
	sprintf(cseconds,"%d",seconds);
	sprintf(cmilli,"%d",milli);

	// Format hours
	strcpy(pString,"");

	if(hours > 0) 
	{
		strcat(pString,chours);
		strcat(pString,":");
	}

	// Format minutes
	if(minutes > 9 && hours > 0) strcat(pString,cminutes);
	else if (minutes > 0)
	{
		if(minutes < 10) strcat(pString,"0");
		strcat(pString,cminutes);
	}
	if(minutes > 0) strcat(pString,":");

	// Format seconds
	if(seconds > 9) strcat(pString,cseconds);
	else
	{
		strcat(pString,"0");
		strcat(pString,cseconds);
	}
	strcat(pString,".");
	strcat(pString,cmilli);
		
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the elapsed level time, in seconds
long  heGameLevel::GetLevelTimeSeconds()
{
	long seconds;

	seconds = mGameLevelTime / 1000;

	return seconds;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Load level from file
void heGameLevel::LoadFromFile()
{
	char  filePath[1024],fileLine[1024];
	FILE* fileHandler;
	int   i;

	// * Reset game level info
	mGameLevelTime            = 0;
    mLastCompletedObjective   = -1;
	con_hostindex             = -1;
	targetMapAnimationStarted = 0;

	for(i = 0; i < HE_MAX_LEVELOBJECTIVES; i++)
		mLastCompletedObjectiveList[i] = 0;

	gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - start loading level");

	///////////////////////////////////////////////////////////////////////////////////////////
	// Load targetmap image
#ifndef HE_EDITOR_BUILD
	gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - loading targetmap");
	mTargetmapImage.LoadFromFile(createModLevelFilePath("targetmap.bmp"),HC_TARGETMAPIMAGE_WIDTH,HC_TARGETMAPIMAGE_HEIGHT);

	// * Attach fade in animation modifier
	mTargetmapImage.AttachAnimationModifier(new CAnimationModifier_ScaleIn);
	mTargetmapImage.mAnimationModifier->Reset(690,320);
#endif

	///////////////////////////////////////////////////////////////////////////////////////////
	// Load level info	
	int temp_money = -1;

	strcpy(filePath,createModLevelFilePath("levelinfo"));
	gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - loading level info (%s)",filePath);
	fileHandler = fopen(filePath,"rt");
	if(fileHandler == NULL)
        gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - ERROR opening: (%s)",filePath);
        fscanf(fileHandler,"%d %d",&player_mapx, &player_mapy);
		
		/*
		2012.12.08 - For some odd reason this doesn't work well under MSVC 2010
		fscanf(fileHandler,"%d",&temp_money);
		mPlayerInfo->mMoney = 0;
		if(temp_money != -1)
		{
			mPlayerInfo->mMoney = temp_money;
			gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - SET PLAYER MONEY (%d)",temp_money);
		}
		*/
	fclose(fileHandler);
	player_mapx += HE_TARGETMAP_XOFFS;
	player_mapy += HE_TARGETMAP_YOFFS;

	///////////////////////////////////////////////////////////////////////////////////////////
	// Load hosts
	gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - loading hosts");
	strcpy(filePath,createModLevelFilePath("hostlist"));
	fileHandler = fopen(filePath,"rt");
		fscanf(fileHandler,"%d",&hostCount);
		gApplicationLog.printf(": %d host(s)",hostCount);

		// Load each host
		hostList = new heHost[hostCount];
		for(i = 0; i < hostCount; i++)
		{
			fscanf(fileHandler,"%s %d",fileLine,&hostList[i].mVisible);
			hostList[i].LoadFromFile(fileLine,mGameModName,mGameLevelName);
#ifndef HE_MOD_EDITOR
			// * Add (visible) hostnames, to the completion dictionary
			if(hostList[i].mVisible == 1)
				gDictionaryHostNames.AddWord(fileLine);
#endif
		}

	fclose(fileHandler);

	///////////////////////////////////////////////////////////////////////////////////////////
	// Load and initialize messages
	gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - loading messages");
	strcpy(filePath,createModLevelFilePath("data_messages/list_messages"));
	fileHandler = fopen(filePath,"rt");
		fscanf(fileHandler,"%d\n",&mMessageCount);
		gApplicationLog.printf(": %d message(s)",mMessageCount);

		// Load each message
		for(i = 0; i < mMessageCount; i++)
		{
			fscanf(fileHandler,"%d %d %d %s %s %s",&mMessageList[i].mMessageCondition,
 				                                   &mMessageList[i].mMinute,
											 	   &mMessageList[i].mSecond,
												   mMessageList[i].mFilename,
												   mMessageList[i].mHostName,
												   mMessageList[i].mMessageContentPath);
			mMessageList[i].mReceived = 0;

			// Adjust message path
			sprintf(fileLine,"%sdata_messages/%s",createModLevelFilePath(""),mMessageList[i].mMessageContentPath);			
			strcpy(mMessageList[i].mMessageContentPath,fileLine);
			gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - loaded : [%s]",mMessageList[i].mMessageContentPath);
		}

	fclose(fileHandler);

	// * Add a last message which is displayed when the player completed the game level
	// and should type NEXTLEVEL
	mMessageList[mMessageCount].mFilename[0] = 0;
	mMessageList[mMessageCount].mHostName[0] = 0;
	mMessageList[mMessageCount].mMinute = 0;
	mMessageList[mMessageCount].mSecond = 0;

	mMessageList[mMessageCount].mMessageCondition = MESSAGE_CONDITION_LEVELCOMPLETED;
	strcpy(mMessageList[mMessageCount].mMessageContentPath,"heuntold-data/message-levelcompleted.txt");
	mMessageList[mMessageCount].mReceived = 0;
	mMessageCount++;

	mActiveMessageCount = 0;
	mActiveMessageIndex = -1;

	// * load level objectives
	gApplicationLog.printf("\nModule::heHost::loadFromFile() - loading level objectives");
	strcpy(filePath,createModLevelFilePath("level_objectives"));
	fileHandler = fopen(filePath,"rt");
		fscanf(fileHandler,"%d",&mLevelObjectiveCount);
	    for(i = 0; i < mLevelObjectiveCount; i++)
		{
			fscanf(fileHandler,"%s %s %d",mLevelObjectiveList[i].mFileName,mLevelObjectiveList[i].mHostName,&mLevelObjectiveList[i].mType);
			fgets(mLevelObjectiveList[i].mDescription1,1024,fileHandler);
			fgets(mLevelObjectiveList[i].mDescription1,1024,fileHandler);
			fgets(mLevelObjectiveList[i].mDescription2,1024,fileHandler);

			stripNewLine(mLevelObjectiveList[i].mDescription1);
			stripNewLine(mLevelObjectiveList[i].mDescription2);
			
			mLevelObjectiveList[i].mCompleted = 0;
			gApplicationLog.printf("\nlevel objective : [%s] [%s] [%d]",mLevelObjectiveList[i].mFileName,mLevelObjectiveList[i].mHostName,mLevelObjectiveList[i].mType);
			gApplicationLog.printf("\n- [%s]",mLevelObjectiveList[i].mDescription1);
			gApplicationLog.printf("\n- [%s]",mLevelObjectiveList[i].mDescription2);
		}
	fclose(fileHandler);


	// Done
	gApplicationLog.printf("\nModule::heGameLevel::loadFromFile() - finished loading level");

	// Misc. initializations
	connected = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Update game level time
void heGameLevel::AddLevelTime(long pDeltaTime)
{
	mGameLevelTime += pDeltaTime;
}

#ifndef HE_EDITOR_BUILD
////////////////////////////////////////////////////////////////////////////////////////////////////
// Updates in-level messages
void heGameLevel::UpdateMessages(heImage *pImage, heImage *pImage2)
{
	int i, index;
	int newMessage = 0;

	// Iterate through all messages, and if the conditions are met, send the message to the player
	for(i = 0; i < mMessageCount; i++)
	{
		// Check if the message hasn't already been received
		if(mMessageList[i].mReceived == 0)
		{
			// Check all conditions that can show the message to the player

			// * MESSAGE_CONDITION_LEVELCOMPLETED
			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_LEVELCOMPLETED)
			{
				// * check if level is completed
				if(allObjectivesCompleted() == 1)
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}

			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_TIMED)
			{
				// This is a timed message
				// If the time has elapsed, send the message to the player
				long seconds = GetLevelTimeSeconds();
				if(seconds > (mMessageList[i].mMinute * 60 + mMessageList[i].mSecond))
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}

			// * MESSAGE_CONDITION_HOSTHACKED
			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_HOSTHACKED)
			{
				// * check if host has been hacked
				index = getHostIndex(mMessageList[i].mHostName);

				if(hostList[index].hackedCompletely() == 1)
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}

			// * MESSAGE_CONDITION_HOSTVISIBLE
			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_HOSTVISIBLE)
			{
				// * check if host is visible
				index = getHostIndex(mMessageList[i].mHostName);

				if(hostList[index].mVisible == 1)
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}
			// * MESSAGE_CONDITION_MONEY
			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_MONEY)
			{
				if(hePlayer.mMoney >= mMessageList[i].mMinute)
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}
			// * MESSAGE_CONDITION_PLAYERFILE
			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_PLAYERFILE)
			{
				if(hePlayer.getFileIndex(mMessageList[i].mFilename) > -1)
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}
			// * MESSAGE_CONDITION_HOSTFILE
			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_HOSTFILE)
			{
				int hostindex;
				hostindex = getHostIndex(mMessageList[i].mHostName);
				if(hostindex == -1)
				{
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - ERROR (no such host : %s)",mMessageList[i].mHostName);
				}
				else
				if(hostList[hostindex].getFileIndex(mMessageList[i].mFilename) > -1)
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}
			// * MESSAGE_CONDITION_HOSTFILEDELETED
			if(mMessageList[i].mMessageCondition == MESSAGE_CONDITION_HOSTFILEDELETED)
			{
				int hostindex;
				hostindex = getHostIndex(mMessageList[i].mHostName);
				if(hostindex == -1)
				{
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - ERROR (no such host : %s)",mMessageList[i].mHostName);
				}
				else
				if(hostList[hostindex].getFileIndex(mMessageList[i].mFilename) == -1)
				{
					// Send message to player
					gApplicationLog.printf("\nheGameLevel::UpdateMessages() - Sending message [%d]",i);
					mActiveMessageList[mActiveMessageCount] = mMessageList[i];
					mMessageList[i].mReceived = 1;

					mActiveMessageIndex = mActiveMessageCount;
					mActiveMessageCount++;

		            // * Sound fx
                    audio_PlaySoundFX("heuntold-soundfx/voice-incoming-transmission.wav");
					
					pImage->mAnimationModifier->Reset(0,HE_HIGHLIGHT_DISTANCE);
					pImage->mAnimationModifier->Start();
					newMessage = 1;
				}
			}
			
			// * Start new message animations
			if(newMessage == 1)
			{
				//pImage2->mAnimationModifier->Reset();
				pImage2->mAnimationModifier->Start();
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Display a message in a window
void heGameLevel::DisplayMessage(heGameWindow* pWindow, int pMessageIndex)
{
	FILE* file;
	char  line[1024];

	// Clear the window
	pWindow->clear();

	// Read and display message
	file = fopen(mActiveMessageList[pMessageIndex].mMessageContentPath,"rt");

	if(file == 0)
	{
		gApplicationLog.printf("\nheGameLevel::DisplayMessage - failed to open : %s",mActiveMessageList[pMessageIndex].mMessageContentPath);
		return;
	}

	// Read the first 2 lines (message origin and subject);
	fgets(line,1000,file);
	stripNewLine(line);
	//if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = 0;
	pWindow->printf("%s %s\n\n",GAMETEXT("FROM:"),line);

	fgets(line,1000,file);
	stripNewLine(line);
	//if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = 0;
	pWindow->printf("%s\n%s\n\n",GAMETEXT("SUBJECT:"),line);

	while(fgets(line,1000,file))
    {
		stripNewLine(line);
        //if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = 0;
		pWindow->printf("%s\n",line);
    }
	fclose(file);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Jump to the next active message
void heGameLevel::NextMessage()
{
	if(mActiveMessageCount <= 0) return;

	mActiveMessageIndex++;
	if(mActiveMessageIndex > mActiveMessageCount - 1) 
        //mActiveMessageIndex = mActiveMessageCount - 1;
        mActiveMessageIndex = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Jump to the previous active message
void heGameLevel::PrevMessage()
{
	if(mActiveMessageCount <= 0) return;

	mActiveMessageIndex--;
	    if(mActiveMessageIndex < 0) 
        {
            mActiveMessageIndex = mActiveMessageCount - 1;
            //mActiveMessageIndex = 0;
        }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Attach tooltip image
void heGameLevel::AttachToolTipImage(heImage* pImage)
{
	mToolTipImage    = pImage;
	mLastToolTipHost = -1;
	mLastToolTipX    = -1;
	mLastToolTipY    = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Returns 1, if all objectives are completed
int heGameLevel::allObjectivesCompleted()
{
	int i, index;
	int allCompleted = 1;

	for(i = 0; i < mLevelObjectiveCount; i++)
	{
		// * check if objective is completed
		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_GETFILE)
		{
			if(mPlayerInfo->getFileIndex(mLevelObjectiveList[i].mFileName) != -1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_MONEY)
		{
			if(mPlayerInfo->mMoney >= atoi(mLevelObjectiveList[i].mFileName))
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
			else
				mLevelObjectiveList[i].mCompleted = 0;
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_HACKHOST)
		{
			index = getHostIndex(mLevelObjectiveList[i].mHostName);
			if((hostList[index].getOpenPortCount() == hostList[index].serviceCount) && hostList[index].mEncryptionKey == 0)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_PUTFILE)
		{
			if(hostList[getHostIndex(mLevelObjectiveList[i].mHostName)].getFileIndex(mLevelObjectiveList[i].mFileName) != -1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_HOSTVISIBLE)
		{
			if(hostList[getHostIndex(mLevelObjectiveList[i].mHostName)].mVisible == 1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_TRACELEVEL)
		{
			if(mPlayerInfo->mGlobalTrace < atoi(mLevelObjectiveList[i].mFileName))
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
			else
				mLevelObjectiveList[i].mCompleted = 0;
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_DELETEFILE)
		{
			if(hostList[getHostIndex(mLevelObjectiveList[i].mHostName)].getFileIndex(mLevelObjectiveList[i].mFileName) == -1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		// * display objective in the level objectives window
		if(mLevelObjectiveList[i].mCompleted == 0) allCompleted = 0;
	}

	return allCompleted;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Update and display level objectives
// * returns 1, if all objectives are completed
int heGameLevel::UpdateLevelObjectives(heGameWindow* pWindow)
{
	int i, index;
	int allCompleted = 1;

	pWindow->clear();
	for(i = 0; i < mLevelObjectiveCount; i++)
	{
		// * check if objective is completed
		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_GETFILE)
		{
			if(mPlayerInfo->getFileIndex(mLevelObjectiveList[i].mFileName) != -1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_MONEY)
		{
			if(mPlayerInfo->mMoney >= atoi(mLevelObjectiveList[i].mFileName))
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
			else
				mLevelObjectiveList[i].mCompleted = 0;
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_HACKHOST)
		{
			index = getHostIndex(mLevelObjectiveList[i].mHostName);
			if((hostList[index].getOpenPortCount() == hostList[index].serviceCount) && hostList[index].mEncryptionKey == 0)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_PUTFILE)
		{
			if(hostList[getHostIndex(mLevelObjectiveList[i].mHostName)].getFileIndex(mLevelObjectiveList[i].mFileName) != -1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_HOSTVISIBLE)
		{
			if(hostList[getHostIndex(mLevelObjectiveList[i].mHostName)].mVisible == 1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_TRACELEVEL)
		{
			if(mPlayerInfo->mGlobalTrace < atoi(mLevelObjectiveList[i].mFileName))
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
			else
				mLevelObjectiveList[i].mCompleted = 0;
		}

		if(mLevelObjectiveList[i].mType == HE_LEVELOBJECTIVE_DELETEFILE)
		{
			if(hostList[getHostIndex(mLevelObjectiveList[i].mHostName)].getFileIndex(mLevelObjectiveList[i].mFileName) == -1)
			{
				mLevelObjectiveList[i].mCompleted = 1;
				mLastCompletedObjective = i;
			}
		}

		// * display objective in the level objectives window
		if(mLevelObjectiveList[i].mCompleted == 0) allCompleted = 0;

		/*
		if(mLevelObjectiveList[i].mCompleted == 0) 
			pWindow->printf("\n[ ] - ");
		else
			pWindow->printf("\n[X] - ");
		*/

		pWindow->printf("\n      %s",mLevelObjectiveList[i].mDescription1);
		pWindow->printf("\n      %s\n",mLevelObjectiveList[i].mDescription2);
	}

	return allCompleted;
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the last completed objective
int heGameLevel::getLastCompletedObjective()
{
	int retval;

	retval = -1;

	if(mLastCompletedObjective > -1)
	{
		if(mLastCompletedObjectiveList[mLastCompletedObjective] == 0)
			retval = mLastCompletedObjective;

		mLastCompletedObjectiveList[mLastCompletedObjective] = 1;
	}

	mLastCompletedObjective = -1;

	return retval;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Check if the coordinates are inside the targetmap
int heGameLevel::clickInTargetMap(int pX, int pY)
{
	int result = 0;

	if(pX > mTargetMapX && pX < (mTargetMapX + HE_TARGETMAP_WIDTH) && pY > mTargetMapY && pY < (mTargetMapY + HE_TARGETMAP_HEIGHT))
		result = 1;

	return 0;
}

