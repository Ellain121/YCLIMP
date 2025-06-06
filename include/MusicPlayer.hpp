#pragma once

#include "bass.h"

#include <string>
#include <vector>

enum class ERepeatMode
{
	None = 0,
	All,
	One,
	Size
};

class SWManager;

/**
 * @brief The MusicPlayer class - class to play music, change music volume, fastforward/rewind, etc.
 *
 * - Uses bass library(https://www.un4seen.com/)@n
 * - All the bass functions was wrapped in my local functions. The only difference between them  is additional logs + exception throwing@n
 */
class MusicPlayer
{
public:
	MusicPlayer(SWManager* swManager);

	void Update();

	void PlayMusicFromFile(const std::string& path);
	void PlayMusicFromBytes(std::string bytes, std::string name);
	void ToggleContinuePause();

	void FastForward5Sec();
	void Rewind5Sec();

	void VolumeUp5Percent();
	void VolumeDown5Percent();

	void ToggleRepeatMode();

	// bool    IsStreamDone() const;
	double			   GetCurSongDuration() const;
	double			   GetCurSongProgress() const;
	double			   GetCurSongProgressPercent() const;
	float			   GetVolume() const;
	std::string		   GetRepeatMode() const;
	const std::string& GetCurMusicName() const;

public:
	static std::string GetStringName(ERepeatMode rMode);
	// static bool IsFilePlayable(const std::string& path);

private:
	void ActivateStreamFromFile(const std::string& path);
	// Step 1: try
	void ActivateStreamFromBytes(std::string bytes, std::string name);
	void EndActiveStream();
	bool IsStreamActive() const;
	bool IsMusicPlayedDone() const;
	bool IsChannelStopped() const;

	void Continue();
	void Pause();
	void SetVolume(const float& volume);
	void FinishPlayedMusic();
	void NextSongRequest();

private:
	SWManager*	mSWManager;
	HSTREAM		mStream;
	std::string mCurMusicName;
	bool		mIsStreamActive;
	bool		mIsMusicPlaying;
	float		mVolume;

	ERepeatMode mRepeatMode;

	// Step 1: try
	std::string mCurrentSongBytes;
};
