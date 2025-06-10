#pragma once

#include "DatabaseIdentifiers.hpp"

// I did't investigate it, but boost didn't compile without it
#include <utility>

//
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/system/system_error.hpp>
#include <filesystem>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace boost;

namespace fs = std::filesystem;

#define PARENT_DIR_NAME ".."

const std::string TYPE_LOCAL = "LOCAL";
const std::string TYPE_YANDEX = "YANDEX";
const std::string TYPE_APPLICATION = "APPLICATION";
const std::string TYPE_UNKNOWN = "UNKNOWN";

class File;
using File_Ptr = std::shared_ptr<File>;

class Client;
using Client_Ptr = std::shared_ptr<Client>;

bool IsFileHasMusicExtension(const std::string& filename);

/**
 * @brief The File class - Abstract class for FsFile, YaFile and AppFile
 *
 * Designed to work with it like it's simple file/folder@n
 *
 * - Each File has Client, FsFile - FSClient; YaFile - YandexClient, etc.
 * - path - mostly for FsFile, but I update it for all
 * - location_id - for Yandex; yandex id or album id
 * - playlistId - for AppFile;
 */
class File
{
public:
	enum class Type
	{
		Directory = 0,
		Track = 1,
		Radio = 2
	};

public:
	File(Client& client, const std::string& name, fs::path path,
		const std::string& location_id, File::Type type, int playlistId = -1);

	virtual void NewFile(File_Ptr newFile);

	virtual std::vector<File_Ptr> GetDirFiles(
		bool noDirs = false, bool onlyMusicExtensions = false, bool recursively = false);
	virtual std::pair<std::string, std::string> GetSongData();
	virtual std::string							GetFromType() const;

public:
	Client&		client;
	std::string name;
	fs::path	path;
	std::string			// FsFile:      | YandexFile:             | AppFile:
		location_id;	// NULL         | yandex_song or album id | FsFile or YandexFile data
	// -----------------------------------------------------------------------------------------------
	////////////////////// FsFile:      | YandexFile:             | AppFile:
	int playlistId;	   // NULL(-1)      | NULL(-1)                | playlistID for tracks; self id for playlsits and NULL(-1) for root dir
	File::Type type;
};

class FsFile : public File
{
public:
	FsFile(Client& client, const std::string& name, fs::path path,
		const std::string& location_id, File::Type type);

	static File_Ptr		InitFile(Client& client);
	virtual std::string GetFromType() const override;
};

class YaFile : public File
{
public:
	YaFile(Client& client, const std::string& name, fs::path path,
		const std::string& location_id, File::Type type);

	static File_Ptr		InitFile(Client& client);
	virtual std::string GetFromType() const override;
};

class AppFile : public File
{
public:
	enum class FromType
	{
		Local = 0,
		Yandex
	};

public:
	AppFile(Client& client, const std::string& name, fs::path path,
		const std::string& location_id, File::Type type, int playlistID = -1);

	static File_Ptr		InitFile(Client& client);
	virtual std::string GetFromType() const override;

	static File_Ptr ConvertFromDBTrack(DBTrack track);
	static File_Ptr ConvertFromDBPlaylist(DBPlaylist playlist);

public:
	//	FromType fromType;
};

/**
 * @brief The Client class - abstract class for FSClient, YandexClient, AppClient.@n
 * Designed to work with File dervied clases.@n
 * Do all the real job getting data.@n
 *
 * - All it's derived clases are singletons@n
 */
class Client
{
public:
	// virtual void OpenParentDir(File* file) = 0;
	virtual void NewFile(File* oldFile, File_Ptr newFile) = 0;

	/***
	 * Returns vector of File_Ptr.
	 *
	 * @param noDirs   Ignore directories (false by default)
	 * @param onlyMusicExtensions if true regex_match to std::make_shared<File>(this)
	 * (false by default)
	 * ".*\\.(?:mp3|mp2|mp1|ogg|wav|aiff|flac)"
	 * @param recursively if true traverse through all folders recursively (false by
	 * default)
	 */
	virtual std::vector<File_Ptr> GetDirFiles(File* file, bool noDirs = false,
		bool onlyMusicExtensions = false,
		bool recursively = false) = 0;	  // if directory
	virtual std::pair<std::string, std::string> GetSongData(File* file) = 0;	// if file
	// virtual std::string       getFileNewName() = 0;

public:
	// fs::path currentPath;    // test
};

class FSClient : public Client
{
private:
	FSClient() = default;

public:
	FSClient(const FSClient&) = delete;
	void operator=(const FSClient&) = delete;

	static FSClient& Instance();

public:
	virtual void NewFile(File* oldFile, File_Ptr newFile) override;

	virtual std::vector<File_Ptr> GetDirFiles(File* file, bool noDirs = false,
		bool onlyMusicExtensions = false,
		bool recursively = false) override;	   // if directory
	virtual std::pair<std::string, std::string> GetSongData(
		File* file) override;	 // if file
};

class YandexClient : public Client
{
public:
	static std::unordered_map<std::string, std::string> folderCache;

private:
	YandexClient();

public:
	YandexClient(const YandexClient&) = delete;
	void operator=(const YandexClient&) = delete;

	static YandexClient& Instance();

public:
	virtual void NewFile(File* oldFile, File_Ptr newFile) override;

	virtual std::vector<File_Ptr> GetDirFiles(File* file, bool noDirs = false,
		bool onlyMusicExtensions = false,
		bool recursively = false) override;	   // if directory
	virtual std::pair<std::string, std::string> GetSongData(
		File* file) override;	 // if file
private:
	void Connect();
	void Disconnect();

private:
	bool mIsConnectionEstablished;

	asio::ip::tcp::endpoint ep;
	asio::io_service		ios;
	asio::ip::tcp::socket	sock;
};

class AppClient : public Client
{
private:
	AppClient(FSClient& fsClient, YandexClient& yaClient);

public:
	AppClient(const FSClient&) = delete;
	void operator=(const AppClient&) = delete;

	static AppClient& Instance();

public:
	virtual void NewFile(File* oldFile, File_Ptr newFile) override;

	virtual std::vector<File_Ptr> GetDirFiles(File* file, bool noDirs = false,
		bool onlyMusicExtensions = false,
		bool recursively = false) override;	   // if directory
	virtual std::pair<std::string, std::string> GetSongData(
		File* file) override;	 // if file

private:
	FSClient&	  fsClient;
	YandexClient& yaClient;
};
