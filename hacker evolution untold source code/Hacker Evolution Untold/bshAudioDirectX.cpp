//================================================================================================
//
// Brian Spencer's Hacker (Game Engine 2)
//
// Desc : Audio Code (DirectX 8)
//
// Copyright (c) Robert Muresan, exoSyphen Studios (http://www.exosyphen.com)
//================================================================================================
#include "bshAudioDirectX.h"
#include "heLog.h"
#include "HackerEvolutionDefines.h"
#include "heSystem.h"
#include <stdio.h>

#include "SDL_mixer.h"

//===========================================================================================================
// 2014 update
Mix_Chunk* wavAudio = nullptr;
Mix_Music* mp3Audio = nullptr;

/***********************************************************************************************************/
/* Class constructor */
CSound::CSound()
{	
}
/***********************************************************************************************************/
/* Class destructor */
CSound::~CSound()
{
	Stop();
}
/***********************************************************************************************************/
/* Check if sound is playing */
int CSound::isPlaying()
{
	return Mix_PlayingMusic();
}
/***********************************************************************************************************/
/* Play sound */
void CSound::Play(int dwNumOfRepeats)
{
	if(Mix_FadeInMusic(mp3Audio,0,3000) ==  -1)
	{
		gApplicationLog.printf("\nCSound::Play() - MixPlayMusic() FAILED.");
	}

}
/***********************************************************************************************************/
/* Stop sound */
void CSound::Stop()
{
	Mix_HaltMusic();
}
/***********************************************************************************************************/
/* Load sound */
void CSound::LoadSound(const char* szSoundFileName)
{
	char filePath[1024];
	sprintf(filePath,"he-music/%s",szSoundFileName);

	if(mp3Audio != nullptr)
	{
		Mix_HaltMusic();
		//Mix_FreeMusic(mp3Audio);
	}

	mp3Audio = Mix_LoadMUS(filePath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Sets the volume 
void CSound::SetVolume(long volume)
{
    Mix_Volume(-1,volume);
    Mix_VolumeMusic(volume);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Return volume level
long CSound::GetVolume()
{
	// SDLTODO(2)
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * Adjust volume up or down
void CSound::AdjustVolume(int pDirection)
{
	long mVolumeLevel;

	mVolumeLevel = gSoundVolume;
	mVolumeLevel += pDirection * 5;

	if(mVolumeLevel < 0)   mVolumeLevel = 0;
	if(mVolumeLevel > 100) mVolumeLevel = 100;

	gSoundVolume = mVolumeLevel;
	SetVolume(gSoundVolume);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
void CSound::PlayIntro()
{
}

void audio_Init()
{
}

/***********************************************************************************************************/
/* Play soundFX - WAV */
void audio_PlaySoundFX(char* szSoundFilePath)
{
	// Stop currently playing sound
	if(wavAudio != nullptr)
		Mix_FreeChunk(wavAudio);
    
	wavAudio = Mix_LoadWAV(szSoundFilePath);
	wavAudio->volume = gSoundFXVolume;
	Mix_PlayChannel(-1,wavAudio,0);
}
