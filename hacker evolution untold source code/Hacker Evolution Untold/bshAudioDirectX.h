//================================================================================================
//
// Brian Spencer's Hacker (Game Engine 2)
//
// Desc : Audio Code (DirectX 8)
//
// Copyright (c) Robert Muresan, exoSyphen Studios (http://www.exosyphen.com)
//================================================================================================

#ifndef _bshAudioDirectX_h_
#define _bshAudioDirectX_h_

/***********************************************************************************************************/
/* Sounds management class */
#define MP3 1
#define WAV 2

class CSound
{
    /* Class constructor / destructor */
	CSound();
	~CSound();

	int  isPlaying();
	void Play(int dwNumOfRepeats = 0); //DMUS_SEG_REPEAT_INFINITE
	void Stop();
	void Pause()
	{
		// SDLTODO pause code
	}
	void LoadSound(const char* szSoundFilePath);
    void SetVolume(long volume);          // * Sets the volume
	long GetVolume();                     // * Return volume level
	void AdjustVolume(int pDirection);    // * Adjust volume up or down

	void PlayIntro();

protected:
	int                       m_sndType;
	int                       m_startTime;
};

void audio_Init();
void audio_PlaySoundFX(char* szSoundFilePath);

#endif
