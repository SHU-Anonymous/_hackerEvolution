/*
Name: hedSound.h
Desc: Sound and music code

Author: Robert Muresan (mrobert@exosyphen.com) - 2010
*/
#ifndef _hedSound_h_
#define _hedSound_h_

#include "HackerEvolutionDualityDefines.h"

#ifdef PLATFORM_WINDOWS
#include "SDL.h"
#include "SDL_mixer.h"
#endif

#ifdef PLATFORM_MACOS
#include "SDL.h"
#include "SDL_mixer.h"
#endif

#ifdef PLATFORM_LINUX
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#endif

#ifdef PLATFORM_IOS
// These are implemented in hedViewController.mm
void exoIOS_PlayMP3(char* path);
void exoIOS_PlaySound(char* path);
int  exoIOS_IsMP3Playing();
#endif

#define SOUNDTYPE_MP3 1
#define SOUNDTYPE_WAV 2

//=======================================================================================================================================
// Sound file management class
typedef char trackName[1024];

class hedSoundManager
{
public:
	// Constructor and destructor
	hedSoundManager();
	~hedSoundManager();

	// Functions
	void Init();
	void Shutdown();
	void Update(int forceNext = 0);                 // Update sound engine (this is called every frame)
	void PlaySoundFX(char* soundFilePath);

	// Music playlist management
	void PlayList_Load(char* playListPath);         // Load playlist
	void PlayList_Play();                           // Play music
	void PlayList_Clear();                          // Clear playlist (called when closing the music or switching playlists)


    // Volume
    void UpdateVolumeFromSettings();

    // Playlist control
    void PlayList_Pause();

	// Music playlist variables
	int       m_PlayListCount;
	int       m_PlayListIndex;
	trackName m_PlayListTrackNames[SND_MAX_PLAYLIST_SIZE];
    int       m_Paused;

protected:
#ifndef PLATFORM_IOS    
	// SDL Variables
	Mix_Music *m_SDLMusicTrack;

	Mix_Chunk *m_SDLWAVData[SND_MAX_CHANNELS];
	int        m_SDLWAVChannel[SND_MAX_CHANNELS];
	int        m_SDLWAVChannelIndex;
#endif
};

// Global access to the sound engine
extern hedSoundManager g_SoundManager;

#endif
