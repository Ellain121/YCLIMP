#include "MusicPlayer.hpp"

#include "SWManager.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

/***
 *  :
 *  USE Bass function instead of BASS if some of them missing
 *  just add one and check for error code and add a proper
 *  exception for error
 *
 *  Bass same as BASS but with error checking and exception
 *  throwing
 *
 * ***/
namespace
{
std::string extractFilenameFromPath(const std::string& path)
{
	std::string name;
	for (auto itr = path.rbegin(); itr != path.rend(); ++itr)
	{
		if (*itr == '/')
			break;

		name += *itr;
	}

	std::reverse(name.begin(), name.end());
	return name;
}

/***
 * Bass_StreamCreateFile is a wrapper with error checking
 * around BASS_StreamCreateFile
 *
 *  accepts std::string instead of const void* file, uses file.c_str() later
 *
 * @throw if BASS_StreamCreateFile fails
 */
HSTREAM Bass_StreamCreateFile(
	BOOL mem, const void* file, QWORD offset, QWORD length, DWORD flags)
{
	HSTREAM stream = BASS_StreamCreateFile(mem, file, offset, length, flags);
	if (stream == 0)
	{
		throw std::runtime_error{"BASS_StreamCreateFile failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
	return stream;
}

/***
 * Bass_ChannelBytes2Seconds execute BASS_ChannelBytes2Seconds and
 * checks for errors
 *
 * @throw if BASS_ChannelBytes2Seconds fails
 */
double Bass_ChannelBytes2Seconds(DWORD handle, DWORD pos)
{
	double value = BASS_ChannelBytes2Seconds(handle, pos);
	if (value < 0)
	{
		throw std::runtime_error{"BASS_ChannelBytes2Seconds failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
	return value;
}

/***
 * Bass_ChannelSeconds2Bytes execute BASS_ChannelSeconds2Bytes and
 * checks for errors
 *
 * @throw if BASS_ChannelSeconds2Bytes fails
 */
QWORD Bass_ChannelSeconds2Bytes(DWORD handle, double pos)
{
	QWORD bytesValue = BASS_ChannelSeconds2Bytes(handle, pos);
	if (bytesValue == -1)
	{
		throw std::runtime_error{"BASS_ChannelSeconds2Bytes failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
	return bytesValue;
}

/***
 * Bass_ChannelGetLength execute BASS_ChannelGetLength and
 * checks for errors
 *
 * @throw if BASS_ChannelGetLength fails
 */
QWORD Bass_ChannelGetLength(DWORD handle, DWORD mode)
{
	QWORD channelLength = BASS_ChannelGetLength(handle, mode);
	if (channelLength == -1)
	{
		throw std::runtime_error{"BASS_ChannelGetLength failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
	return channelLength;
}

/***
 * Bass_ChannelGetPosition execute BASS_ChannelGetPosition and
 * checks for errors
 *
 * @throw if BASS_ChannelGetPosition fails
 */
QWORD Bass_ChannelGetPosition(DWORD handle, DWORD mode)
{
	QWORD channelPosition = BASS_ChannelGetPosition(handle, mode);
	if (channelPosition == -1)
	{
		throw std::runtime_error{"Bass_ChannelGetPosition failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
	return channelPosition;
}

/***
 * Bass_ChannelSetPosition execute BASS_ChannelSetPosition and
 * checks for errors
 *
 * @throw if BASS_ChannelSetPosition fails
 */
void Bass_ChannelSetPosition(DWORD handle, QWORD pos, DWORD mode)
{
	if (!BASS_ChannelSetPosition(handle, pos, mode))
	{
		throw std::runtime_error{"BASS_ChannelSetPosition failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
}

/***
 * Bass_PluginLoad execute BASS_PluginLoad and
 * checks for errors
 *
 * @throw if BASS_PluginLoad fails
 */
HPLUGIN Bass_PluginLoad(const char* file, DWORD flags)
{
	int pluginFlac = BASS_PluginLoad(file, flags);
	if (pluginFlac == 0)
	{
		throw std::runtime_error{"BASS_PluginLoad failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
	return pluginFlac;
}

/***
 * Bass_Init execute BASS_Init and
 * checks for errors
 *
 * @throw if BASS_Init fails
 */
void Bass_Init(int device, DWORD freq, DWORD flags, void* win, void* dsguid)
{
	if (!BASS_Init(device, freq, flags, win, dsguid))
	{
		throw std::runtime_error{
			"BASS_Init failed! (code: " + std::to_string(BASS_ErrorGetCode()) + ")"};
	}
}

/***
 * Bass_ChannelPause execute BASS_ChannelPause and
 * checks for errors
 *
 * @throw if BASS_ChannelPause fails
 */
void Bass_ChannelPause(DWORD handle)
{
	if (!BASS_ChannelPause(handle))
	{
		throw std::runtime_error{"BASS_ChannelPause failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
}

/***
 * Bass_ChannelStop execute BASS_ChannelStop and
 * checks for errors
 *
 * @throw if BASS_ChannelStop fails
 */
void Bass_ChannelStop(DWORD handle)
{
	if (!BASS_ChannelStop(handle))
	{
		throw std::runtime_error{"BASS_ChannelStop failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
}

/***
 * Bass_ChannelPlay execute BASS_ChannelPlay and
 * checks for errors
 *
 * @throw if BASS_ChannelPlay fails
 */
void Bass_ChannelPlay(DWORD handle, BOOL restart)
{
	if (!BASS_ChannelPlay(handle, restart))
	{
		throw std::runtime_error{"BASS_ChannelPlay failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
}

/***
 * Bass_StreamFree execute BASS_StreamFree and
 * checks for errors
 *
 * @throw if BASS_StreamFree fails
 */
void Bass_StreamFree(HSTREAM handle)
{
	if (!BASS_StreamFree(handle))
	{
		throw std::runtime_error{"BASS_StreamFree failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
}

/***
 * Bass_ChannelSetAttribute execute BASS_ChannelSetAttribute and
 * checks for errors
 *
 * @throw if BASS_ChannelSetAttribute fails
 */
void Bass_ChannelSetAttribute(DWORD handle, DWORD attrib, float value)
{
	if (!BASS_ChannelSetAttribute(handle, attrib, value))
	{
		throw std::runtime_error{"BASS_ChannelSetAttribute failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
}

/***
 * Bass_ChannelIsActive execute BASS_ChannelIsActive and
 * checks for errors
 *
 * @throw if BASS_ChannelIsActive fails
 */
DWORD Bass_ChannelIsActive(DWORD handle)
{
	DWORD res = BASS_ChannelIsActive(handle);
	if (BASS_ErrorGetCode() != BASS_OK)
	{
		throw std::runtime_error{"Bass_ChannelIsActive failed! (code: " +
								 std::to_string(BASS_ErrorGetCode()) + ")"};
	}
	return res;
}

}	 // namespace

// SWManager pointer required to add SWEvent when music is done playing
MusicPlayer::MusicPlayer(SWManager* swManager)
	: mSWManager{swManager}
	, mCurMusicName{""}
	, mIsStreamActive{false}
	, mIsMusicPlaying{false}
	, mVolume{1.f}
	, mRepeatMode{ERepeatMode::None}
{
	// todo: разобраться с путем этой либы, думаю лучше ее включить в директорию проекта
	// разобраться с лицензией
	int pluginFlac =
		Bass_PluginLoad("/home/ellain/Program_Files/bass_flac/libbassflac.so", 0);
	Bass_Init(-1, 44100, BASS_DEVICE_3D, 0, 0);
}

void MusicPlayer::Update()
{
	if (IsMusicPlayedDone())
	{
		mIsMusicPlaying = false;
		NextSongRequest();
	}
}

void MusicPlayer::PlayMusicFromFile(const std::string& path)
{
	ActivateStreamFromFile(path);
	Continue();
}

void MusicPlayer::PlayMusicFromBytes(std::string bytes, std::string name)
{
	ActivateStreamFromBytes(std::move(bytes), name);
	Continue();
}

void MusicPlayer::FastForward5Sec()
{
	if (!mIsStreamActive || IsMusicPlayedDone())
		return;

	double newProgress = GetCurSongProgress() + 5.0;
	if (newProgress >= GetCurSongDuration())
	{
		FinishPlayedMusic();
		return;
	}

	Bass_ChannelSetPosition(
		mStream, Bass_ChannelSeconds2Bytes(mStream, newProgress), BASS_POS_BYTE);
}

void MusicPlayer::Rewind5Sec()
{
	if (!mIsStreamActive || IsMusicPlayedDone())
		return;

	double newProgress = std::max(GetCurSongProgress() - 5.0, 0.0);

	Bass_ChannelSetPosition(
		mStream, Bass_ChannelSeconds2Bytes(mStream, newProgress), BASS_POS_BYTE);
}

double MusicPlayer::GetCurSongDuration() const
{
	if (!mIsStreamActive)
	{
		throw std::logic_error{"MusicPlayer::GetCurSongDuration: stream is not active!"};
	}

	QWORD channelLength = Bass_ChannelGetLength(mStream, BASS_POS_BYTE);
	return Bass_ChannelBytes2Seconds(mStream, channelLength);
}

double MusicPlayer::GetCurSongProgress() const
{
	if (!mIsStreamActive)
	{
		throw std::logic_error{"MusicPlayer::GetCurSongProgress: stream is not active!"};
	}

	QWORD channelPosition = Bass_ChannelGetPosition(mStream, BASS_POS_BYTE);
	return Bass_ChannelBytes2Seconds(mStream, channelPosition);
}

double MusicPlayer::GetCurSongProgressPercent() const
{
	return GetCurSongProgress() / GetCurSongDuration() * 100.0;
}

void MusicPlayer::ToggleContinuePause()
{
	if (!mIsStreamActive)
		return;

	if (mIsMusicPlaying)
	{
		Pause();
	}
	else
	{
		Continue();
	}
}

void MusicPlayer::Continue()
{
	Bass_ChannelPlay(mStream, false);
	mIsMusicPlaying = true;
}

void MusicPlayer::Pause()
{
	Bass_ChannelPause(mStream);
	mIsMusicPlaying = false;
}

// Same behaviour when music ends naturally
void MusicPlayer::FinishPlayedMusic()
{
	Bass_ChannelStop(mStream);
}

void MusicPlayer::NextSongRequest()
{
	mSWManager->AddSWEvent(SWEvent{
		ESWTypes::Player, ESWEventAction::PlayedMusicDone, GetStringName(mRepeatMode)});
}

void MusicPlayer::ActivateStreamFromFile(const std::string& path)
{
	EndActiveStream();

	mStream = Bass_StreamCreateFile(0, path.c_str(), 0, 0, 0);
	mIsStreamActive = true;
	mCurMusicName = extractFilenameFromPath(path);

	SetVolume(mVolume);
}

void MusicPlayer::ActivateStreamFromBytes(std::string bytes, std::string name)
{
	mCurrentSongBytes = std::move(bytes);
	EndActiveStream();

	mStream = Bass_StreamCreateFile(
		true, mCurrentSongBytes.c_str(), 0, mCurrentSongBytes.size(), 0);
	mIsStreamActive = true;
	mCurMusicName = name;

	SetVolume(mVolume);
}

void MusicPlayer::EndActiveStream()
{
	if (mIsStreamActive)
	{
		Bass_StreamFree(mStream);
		mIsStreamActive = false;
	}
}

bool MusicPlayer::IsMusicPlayedDone() const
{
	return (mIsMusicPlaying && Bass_ChannelIsActive(mStream) == BASS_ACTIVE_STOPPED);
}

bool MusicPlayer::IsStreamActive() const
{
	return mIsStreamActive;
}

bool MusicPlayer::IsChannelStopped() const
{
	return Bass_ChannelIsActive(mStream) == BASS_ACTIVE_STOPPED;
}

void MusicPlayer::SetVolume(const float& volume)
{
	if (!IsStreamActive())
		return;
	if (volume > 1.f || volume < 0.f)
		return;

	Bass_ChannelSetAttribute(mStream, BASS_ATTRIB_VOL, volume);
	mVolume = volume;
}

void MusicPlayer::VolumeUp5Percent()
{
	float newVolume = mVolume + 0.05f;
	SetVolume(newVolume);
}

void MusicPlayer::VolumeDown5Percent()
{
	float newVolume = mVolume - 0.05f;
	SetVolume(newVolume);
}

void MusicPlayer::ToggleRepeatMode()
{
	mRepeatMode = static_cast<ERepeatMode>(static_cast<int>(mRepeatMode) + 1);
	if (mRepeatMode == ERepeatMode::Size)
		mRepeatMode = static_cast<ERepeatMode>(0);
}

float MusicPlayer::GetVolume() const
{
	return mVolume;
}

std::string MusicPlayer::GetRepeatMode() const
{
	return GetStringName(mRepeatMode);
}

const std::string& MusicPlayer::GetCurMusicName() const
{
	return mCurMusicName;
}

std::string MusicPlayer::GetStringName(ERepeatMode rMode)
{
	if (rMode == ERepeatMode::None)
	{
		return "None";
	}
	if (rMode == ERepeatMode::All)
	{
		return "All ";	  // extra space
	}
	if (rMode == ERepeatMode::One)
	{
		return "One ";
	}
	assert(("Repeat Mode not found!", false));
}
