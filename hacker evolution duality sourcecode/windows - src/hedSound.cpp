/*
Name: hedSound.cpp
Desc: Sound and music code (win32)

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/

#include "../generic - inc/HackerEvolutionDualityUtil.h"
#include "../generic - inc/hedSound.h"
#include "../generic - inc/hedSystem.h"
#include "../generic - inc/hedGameSettings.h"

#ifdef PLATFORM_WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef PLATFORM_LINUX
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#endif

#ifdef PLATFORM_IOS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef PLATFORM_MACOS
//#include <MacOSSandBoxHelper.h>
#endif

hedSoundManager g_SoundManager;

//=======================================================================================================================================
// Class constructor
hedSoundManager::hedSoundManager()
{
#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
	m_PlayListCount      = 0;
	m_PlayListIndex      = 0;
	m_SDLWAVChannelIndex = 0;
    m_Paused             = 0;
#endif    

#if defined(PLATFORM_IOS)
	m_PlayListCount      = 0;
	m_PlayListIndex      = 0;
    m_Paused             = 0;
#endif  
}

//=======================================================================================================================================
// Class destructor
hedSoundManager::~hedSoundManager()
{
}

//=======================================================================================================================================
// Initialize sound manager
void hedSoundManager::Init()
{
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS)
    sysLogPrint("(hedSound.cpp)hedSoundManager::Init() - Sound initialized succesfully.");
	
	// Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);  

#ifndef PLATFORM_LINUX
	Mix_Init(MIX_INIT_MP3);
#endif

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096); 
	m_SDLMusicTrack = NULL;

    UpdateVolumeFromSettings();
#endif    
}

//=======================================================================================================================================
// Shutdown sound manager
void hedSoundManager::Shutdown()
{
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
    sysLogPrint("(hedSound.cpp)hedSoundManager::Shutdown() - Shutting down...");
	Mix_FreeMusic(m_SDLMusicTrack); 
	Mix_CloseAudio();     
	SDL_Quit();
#endif

    sysLogPrint("(hedSound.cpp)hedSoundManager::Shutdown() - done.");
}

//=======================================================================================================================================
// Update playlist (this is called every frame, to advance to the next track)
void hedSoundManager::Update(int forceNext)
{
    /*
#if defined(PLATFORM_MACOS)
    
	// Update playlist
	if( _macos_MP3_isPlaying() == false || forceNext == 1)
	{
        
		m_PlayListIndex++;
		if(m_PlayListIndex >= m_PlayListCount)
			m_PlayListIndex = 0;
        
		this->PlayList_Play();
	}
#endif
     */

#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS)
	// Update playlist
	if( Mix_PlayingMusic() == 0 || forceNext == 1)
	{
        Mix_HaltMusic();
		Mix_FreeMusic(m_SDLMusicTrack); 
		m_PlayListIndex++;
		if(m_PlayListIndex >= m_PlayListCount)
			m_PlayListIndex = 0;

		this->PlayList_Play();
	}
#endif    

#if defined(PLATFORM_IOS)
	// Update playlist
	if( exoIOS_IsMP3Playing() == 0|| forceNext == 1)
	{
		m_PlayListIndex++;
		if(m_PlayListIndex >= m_PlayListCount)
			m_PlayListIndex = 0;
        
		this->PlayList_Play();
	}
#endif 

}

//=======================================================================================================================================
// Play soundFX - WAV
void hedSoundManager::PlaySoundFX(char* soundFilePath)
{
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS)
	// Clean up the current channel if all channels are full and we are stopping the current one
	Mix_FreeChunk(m_SDLWAVData[m_SDLWAVChannelIndex]);

	m_SDLWAVData[m_SDLWAVChannelIndex]    = Mix_LoadWAV(soundFilePath);
	m_SDLWAVChannel[m_SDLWAVChannelIndex] = Mix_PlayChannel(-1, m_SDLWAVData[m_SDLWAVChannelIndex],0);	
    
	m_SDLWAVChannelIndex++;
	if(m_SDLWAVChannelIndex >= SND_MAX_CHANNELS)
	{
		m_SDLWAVChannelIndex = 0;
	}
#endif  
    
 /*
#if defined(PLATFORM_MACOS)
    _macos_WAV_Play(soundFilePath);
#endif
*/

#if defined(PLATFORM_IOS)
    exoIOS_PlaySound(soundFilePath);
#endif
}

//=======================================================================================================================================
// Load playlist
void hedSoundManager::PlayList_Load(char* playListPath)         
{
#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_IOS)
	FILE* playListFile;
	char  fileLine[1024];
	int   i;

    this->PlayList_Clear();
    sysLogPrint("(hedSound.cpp)hedSoundManager::PlayList_Load() - Loading playlist");

	playListFile = fopen(playListPath,"rt");
    if(playListFile == NULL)
    {
        sysLogPrint("[ERROR] Failed to load playlist(%s)",playListPath);
        return;
    }
    
	fgets(fileLine,1024,playListFile);
	m_PlayListCount = atoi(fileLine);
	m_PlayListIndex = 0;

	sysLogPrint("(hedSound.cpp)hedSoundManager::PlayList_Load() - %d song(s)",m_PlayListCount);

	for(i = 0; i < m_PlayListCount; i++)
	{
		fgets(fileLine,1024,playListFile);
		utilStripNewLine(fileLine);

        strcpy(m_PlayListTrackNames[i],fileLine);
		sysLogPrint("(hedSound.cpp)hedSoundManager::PlayList_Load() - (%s)",m_PlayListTrackNames[i]);
	}

    // Shuffle the playlist
    int index;
    
    for(i = 0; i < m_PlayListCount; i++)
    {
        index = rand() % m_PlayListCount;
        hedString tmp;

        strcpy(tmp,m_PlayListTrackNames[index]);
        strcpy(m_PlayListTrackNames[index],m_PlayListTrackNames[m_PlayListCount - 1 - index]);
        strcpy(m_PlayListTrackNames[m_PlayListCount - 1 - index],tmp);
    }
    
    for(i = 0; i < m_PlayListCount; i++)
		sysLogPrint("(hedSound.cpp)hedSoundManager::SHUFFLE - (%s)",m_PlayListTrackNames[i]);

	// Set volume
    UpdateVolumeFromSettings();
    this->PlayList_Play();
    sysLogPrint("(hedSound.cpp)hedSoundManager::PlayList_Load() - Done");
#endif    
}

//=======================================================================================================================================
// Play music
void hedSoundManager::PlayList_Play()
{
    /*
#if defined(PLATFORM_MACOS)

    _macos_MP3_Play(m_PlayListTrackNames[m_PlayListIndex]);

#endif
     */
    
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS)

    sysLogPrint("(hedSound.cpp)hedSoundManager::PLAY - (%d)(%s)",m_PlayListIndex,m_PlayListTrackNames[m_PlayListIndex]);
	m_SDLMusicTrack = Mix_LoadMUS(m_PlayListTrackNames[m_PlayListIndex]);    

	sysLogPrint("(hedSound.cpp)hedSoundManager::PlayList_Play() - playing track (%s)",m_PlayListTrackNames[m_PlayListIndex]);

	if(!m_SDLMusicTrack)
		sysLogPrint("(hedSound.cpp)hedSoundManager::PlayList_Play() - error loading track (%s)(%d)",Mix_GetError(),m_PlayListIndex);
	
	if(Mix_PlayMusic(m_SDLMusicTrack, 0) == -1)
		sysLogPrint("(hedSound.cpp)hedSoundManager::PlayList_Play() - error playing track (%s)",Mix_GetError());

#endif
    
#if defined(PLATFORM_IOS)
    
    exoIOS_PlayMP3(m_PlayListTrackNames[m_PlayListIndex]);

#endif
    
    m_Paused = 0;
}

//=======================================================================================================================================
// Clear playlist (called when closing the music or switching playlists)
void hedSoundManager::PlayList_Clear()
{
}

//=======================================================================================================================================
// Set volume from global settings
void hedSoundManager::UpdateVolumeFromSettings()
{
    /*
#if defined(PLATFORM_MACOS)
    
    _macos_SetVolume(g_GameSettings.ValueInt("sound_volume_fx"), g_GameSettings.ValueInt("sound_volume_music"));
    
#endif
     */
    
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS)
#ifdef GAME_HACKEREVOLUTIONDUALITY
    Mix_Volume(-1,g_GameSettings.ValueInt("sound_volume_fx"));
    Mix_VolumeMusic(g_GameSettings.ValueInt("sound_volume_music"));
#endif
#endif    
}

//=======================================================================================================================================
// Start/Stop playlist
void hedSoundManager::PlayList_Pause()
{
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS)
    if(m_Paused == 0)
    {
        m_Paused = 1;
        Mix_PauseMusic();
    }
    else
    {
        m_Paused = 0;
        Mix_ResumeMusic();
    }
    sysLogPrintDebug("(hedSound.cpp)hedSoundManager::PlayList_Pasue() - Music paused(%d)",m_Paused);
#endif 
    
    /*
#if defined(PLATFORM_MACOS) 
    if(m_Paused == 0)
    {
        m_Paused = 1;
        _macos_MP3_Pause(1);
    }
    else
    {
        m_Paused = 0;
        _macos_MP3_Pause(0);
    }
#endif
     */
}
