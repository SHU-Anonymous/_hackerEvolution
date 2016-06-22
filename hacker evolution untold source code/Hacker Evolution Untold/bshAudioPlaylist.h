//================================================================================================
//
// Brian Spencer's Hacker (Game Engine 2)
//
// Desc : Music playlist management
//
// Copyright (c) Robert Muresan, exoSyphen Studios (http://www.exosyphen.com)
//================================================================================================
#ifndef _bshAudioPlaylist_h_
#define _bshAudioPlaylist_h_

#include "SDL_mixer.h"

typedef char szString[1024];
/***********************************************************************************************************/
/* Playlist management class */
class bshAudioPlaylist
{
public:
public:
    /* Constructor / Destructor */
    bshAudioPlaylist();
    ~bshAudioPlaylist();

    /* Playlist management      */
    void loadPlayList(char* szPlaylistFileName);
    void Play();
	void Pause();
    void Stop();
	void nextTrack();
	void Update();
    void SetVolume(int volume);
 	long GetVolume();
    void AdjustVolume(int pDirection);
	void PlayWav(char* fileName);

	// * Retrieve track name
	char* getTrackName();
	char  mTrackName[1024];

protected:
    Mix_Music*  m_pTrack[100];
	Mix_Chunk*  m_pWAVAudio;
    szString    m_trackList[100];
    int         m_trackCount;
    int         m_currentTrack;
};

extern long             gVolume;
extern bshAudioPlaylist gameMusic;

#endif
