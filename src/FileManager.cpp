#include "FileManager.hpp"

#include "DatabaseManager.hpp"
#include "SubWindow.hpp"

#include <algorithm>
#include <boost/system/system_error.hpp>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

// debug
#include "Debug.hpp"

namespace
{
// works with python client on 127.0.0.1:5002
const std::string& _EOF_ = "E!O$F";
std::string		   raw_ip_address = "127.0.0.1";
unsigned short	   port_num = 5002;

inline static std::regex musicExtRegex{".*\\.(?:mp3|mp2|mp1|ogg|wav|aiff|flac)"};

}	 // namespace

bool IsFileHasMusicExtension(const std::string& filename)
{
	return std::regex_match(filename, musicExtRegex);
}

File::File(Client& client, const std::string& name, fs::path path,
	const std::string& location_id, File::Type type, int playlistId)
	: client{client}
	, name{name}
	, path{path}
	, location_id{location_id}
	, type{type}
	, playlistId{playlistId}
{
}

/**
 * @brief File::NewFile - substitute current file with a new file.@n
 * Used to navigate between directories/folders.@n
 * Will be processed by it's specific client
 */
void File::NewFile(File_Ptr newFile)
{
	client.NewFile(this, newFile);
}

/**
 * @brief File::GetDirFiles - get files in current directory/folder.@n
 * Will be processed by it's specific client.
 * @param noDirs - exclude/include directories in return vector
 * @param onlyMusicExtensions - files like .mp3 .wav etc
 * @param recursively - search in each folder recursively
 * @return - files and possibly folders in current directory
 */
std::vector<File_Ptr> File::GetDirFiles(
	bool noDirs, bool onlyMusicExtensions, bool recursively)
{
	return client.GetDirFiles(this, noDirs, onlyMusicExtensions, recursively);
}

std::string File::GetSongData()
{
	return client.GetSongData(this);
}

std::string File::GetFromType() const
{
	return TYPE_UNKNOWN;
}

FsFile::FsFile(Client& client, const std::string& name, fs::path path,
	const std::string& location_id, File::Type type)
	: File{client, name, path, location_id, type}
{
}

File_Ptr FsFile::InitFile(Client& client)
{
	return std::shared_ptr<File>(
		new FsFile{client, "Music", "/home/ellain/Music", "NULL", File::Type::Directory});
}

std::string FsFile::GetFromType() const
{
	return TYPE_LOCAL;
}

YaFile::YaFile(Client& client, const std::string& name, fs::path path,
	const std::string& location_id, File::Type type)
	: File{client, name, path, location_id, type}
{
}

File_Ptr YaFile::InitFile(Client& client)
{
	// 0 - Root
	return std::shared_ptr<File>(
		new YaFile{client, "Yandex", "/Yandex", "0", File::Type::Directory});
}

std::string YaFile::GetFromType() const
{
	return TYPE_YANDEX;
}

AppFile::AppFile(Client& client, const std::string& name, fs::path path,
	const std::string& location_id, File::Type type, int playlistID)
	: File{client, name, path, location_id, type, playlistID}
{
}

File_Ptr AppFile::InitFile(Client& client)
{
	return std::shared_ptr<File>(
		new AppFile{client, "App", "/App", "0", File::Type::Directory, 0});
}

std::string AppFile::GetFromType() const
{
	return TYPE_APPLICATION;
}

File_Ptr AppFile::ConvertFromDBTrack(DBTrack track)
{
	std::shared_ptr<File> filePtr = std::make_shared<AppFile>(AppClient::Instance(),
		track.name, track.fsPath, track.locationId, File::Type::Track, track.playlistId);

	return filePtr;
}

File_Ptr AppFile::ConvertFromDBPlaylist(DBPlaylist playlist)
{
	std::shared_ptr<File> filePtr =
		std::make_shared<AppFile>(AppClient::Instance(), playlist.name,
			"/App/" + playlist.name, "NULL", File::Type::Directory, playlist.id);
	return filePtr;
}

FSClient& FSClient::Instance()
{
	static FSClient instance{};
	return instance;
}

void FSClient::NewFile(File* oldFile, File_Ptr newFile)
{
	assert(newFile->path.string().back() != '/');
	oldFile->path = newFile->path;
	oldFile->name = newFile->name;
	oldFile->location_id = "NULL";
}

/***
 * Returns vector of File_Ptr.
 *
 * @param noDirs   Ignore directories (false by default)
 * @param onlyMusicExtensions if true regex_match to std::make_shared<File>(this) (false
 * by default)
 * ".*\\.(?:mp3|mp2|mp1|ogg|wav|aiff|flac)"
 * @param recursively default:false if true traverse through all folders recursively
 */
std::vector<File_Ptr> FSClient::GetDirFiles(
	File* file, bool noDirs, bool onlyMusicExtensions, bool recursively)
{
	assert(fs::is_directory(file->path));
	std::vector<File_Ptr> files;

	if (recursively)
	{
		fs::recursive_directory_iterator begin{file->path};
		fs::recursive_directory_iterator end{};
		for (auto iter{begin}; iter != end; ++iter)
		{
			auto&		entry{*iter};
			std::string name = entry.path().filename();

			if (onlyMusicExtensions == false || IsFileHasMusicExtension(name))
			{
				std::string name = entry.path().filename();
				fs::path	path = entry.path();
				std::string location_id = "NULL";
				auto&		client = file->client;
				File::Type	type =
					 fs::is_directory(entry) ? File::Type::Directory : File::Type::Track;

				files.push_back(std::shared_ptr<FsFile>{
					new FsFile{client, name, path, location_id, type}});
			}
		}
	}
	else
	{
		fs::path filePath{file->path};
		for (const auto& entry : fs::directory_iterator(filePath))
		{
			std::string name = entry.path().filename();
			if (onlyMusicExtensions == false || IsFileHasMusicExtension(name))
			{
				fs::path	path = entry.path();
				std::string location_id = "NULL";
				auto&		client = file->client;
				File::Type	type =
					 fs::is_directory(entry) ? File::Type::Directory : File::Type::Track;
				files.push_back(std::shared_ptr<FsFile>{
					new FsFile{client, name, path, location_id, type}});
			}
		}
	}

	if (noDirs == false)
	{
		// if we are not in the root folder (where there is no actual parent path)
		if (file->path != file->path.parent_path())
		{
			auto&		client = file->client;
			std::string name = PARENT_DIR_NAME;
			fs::path	path = file->path.parent_path();
			std::string location_id = "NULL";
			File::Type	type =
				 fs::is_directory(path) ? File::Type::Directory : File::Type::Track;
			files.push_back(std::shared_ptr<FsFile>{
				new FsFile{client, name, path, location_id, type}});
		}
	}
	return files;
}

std::string FSClient::GetSongData(File* file)
{
	assert(fs::is_regular_file(file->path));
	std::fstream	  fileInput{file->path};
	std::stringstream ss;
	ss << fileInput.rdbuf();

	return std::string{ss.str()};
}

std::unordered_map<std::string, std::string> YandexClient::folderCache{};

YandexClient::YandexClient()
	: mIsConnectionEstablished{false}
	, ep{asio::ip::address::from_string(raw_ip_address), port_num}
	, ios{}
	, sock{ios, ep.protocol()}
{
}

YandexClient& YandexClient::Instance()
{
	static YandexClient instance{};
	return instance;
}

void YandexClient::NewFile(File* oldFile, File_Ptr newFile)
{
	assert(newFile->path.string().back() != '/');

	if (newFile->name == PARENT_DIR_NAME)
	{
		fs::path	new_path = newFile->path;
		std::string new_name = newFile->name;
		std::string new_location_id = newFile->location_id;

		// /path/to/file/.. -> /path/to/file
		new_path = newFile->path.parent_path();
		new_name = new_path.filename();
		auto found = folderCache.find(new_name);
		Debug::Print("Try find:" + new_name + "|newFilepath:" + newFile->path.string());
		assert(found != folderCache.end());
		new_location_id = found->second;
		Debug::Print("found: " + new_location_id);

		oldFile->path = new_path;
		oldFile->name = new_name;
		oldFile->location_id = new_location_id;
	}
	else
	{
		folderCache.insert(std::make_pair(oldFile->name, oldFile->location_id));
		Debug::Print("INSERT:" + oldFile->path.string() + "|" + oldFile->location_id);

		oldFile->path = newFile->path;
		oldFile->name = newFile->name;
		oldFile->location_id = newFile->location_id;
	}
}

/***
 * Uses python local client to get yandex related data.@n
 * Communicate with it client via
 *
 * @param noDirs   Ignore directories (false by default)
 * @param onlyMusicExtensions if true regex_match to std::make_shared<File>(this) (false
 * by default)
 * ".*\\.(?:mp3|mp2|mp1|ogg|wav|aiff|flac)"
 * @param recursively default:false if true traverse through all folders recursively
 * @return vector if filesNfolders in current directory
 */
std::vector<File_Ptr> YandexClient::GetDirFiles(
	File* file, bool noDirs, bool onlyMusicExtensions, bool recursively)
{
	assert(file->type == File::Type::Directory);
	const std::string& dir_id = file->location_id;

	if (!mIsConnectionEstablished)
		Connect();

	std::vector<File_Ptr> files;
	try
	{
		// send request
		std::string request{std::string{"GET_DIR_FILES "} + dir_id + "\r\n"};
		asio::write(sock, asio::buffer(request));

		// get answer
		asio::streambuf buf;
		asio::read_until(sock, buf, _EOF_);
		std::string bytes{
			std::istreambuf_iterator<char>(&buf), std::istreambuf_iterator<char>()};
		size_t found = bytes.find(_EOF_);
		assert(found != std::string::npos);
		std::string filesList{bytes.begin(), bytes.begin() + found};

		found = 0;
		std::size_t cursor = 0;
		int			i = 0;
		File::Type	type;
		while (true)
		{
			found = filesList.find("\r\n", found + 1);
			if (found == std::string::npos)
				break;
			std::string fileEntry{filesList.substr(cursor, found - cursor)};

			// first argument is always a type of a subsequent list files
			if (i == 0)
			{
				// assert folder for now
				// assert(fileEntry == "folder");
				if (fileEntry == "folder")
					type = File::Type::Directory;
				else
					type = File::Type::Track;
			}
			else
			{
				auto found_comma = fileEntry.find(',');
				// only 2 args divided by comma is expected
				assert(found_comma != std::string::npos);
				std::string name = fileEntry.substr(0, found_comma);
				std::string id = fileEntry.substr(found_comma + 1);

				files.push_back(std::shared_ptr<File>(new YaFile{
					file->client, name, file->path / fs::path{name}, id, type}));
				//				Debug::Print("---------->" + (file->path / fs::path{name}).string());
				File test{file->client, name, file->path, "id", type};
				// Debug::Print("->" + test.location_id);
			}
			// what is a file anyway
			++i;
			cursor = found + 2;
		}
	}
	catch (const system::system_error& err)
	{
		// no handle
		throw err;
	}

	// // if we are not in the root folder (where there is no actual parent path)
	//     if (file->path != file->path.parent_path())
	//     {
	//         auto&       client = file->client;
	//         std::string name = PARENT_DIR_NAME;
	//         fs::path    path = file->path.parent_path();
	//         std::string location_id = "NULL";
	//         File::Type  type = fs::is_directory(path) ? File::Type::Directory
	//                                                   : File::Type::Track;
	//         files.push_back(
	//             std::shared_ptr<File>{new FsFile{client, name, path, location_id,
	//             type}});

	// if dirs included, include parent_path
	if (noDirs == false)
	{
		auto&		client = file->client;
		std::string name = PARENT_DIR_NAME;
		fs::path	path = file->path.parent_path() / "..";
		std::string location_id = "TEMPORARILY_UNKNOWN";
		File::Type	type = File::Type::Directory;

		File_Ptr filePtr{new YaFile{client, name, path, location_id, type}};
		files.push_back(std::move(filePtr));
	}

	return files;
}

std::string YandexClient::GetSongData(File* file)
{
	if (!mIsConnectionEstablished)
		Connect();

	std::string songData;
	try
	{
		// send request
		std::string request{"GET_TRACK " + file->location_id +
							"\r\n"};	// todo: is location_id valid id in python client?
		asio::write(sock, asio::buffer(request));

		// get answer
		asio::streambuf buf;
		asio::read_until(sock, buf, _EOF_);
		std::string bytes{
			std::istreambuf_iterator<char>(&buf), std::istreambuf_iterator<char>()};
		size_t found = bytes.find(_EOF_);
		assert(found != std::string::npos);
		songData = std::string{bytes.begin(), bytes.begin() + found};
	}
	catch (const system::system_error& err)
	{
		// no handle for now
		throw err;
	}
	return songData;
}

void YandexClient::Connect()
{
	assert(mIsConnectionEstablished == false);
	try
	{
		sock.connect(ep);
	}
	catch (const system::system_error& err)
	{
		// no handle
		throw err;
	}
	mIsConnectionEstablished = true;
}

void YandexClient::Disconnect()
{
	assert(mIsConnectionEstablished == true);

	try
	{
		sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	}
	catch (const system::system_error& err)
	{
		// no handle;
		throw err;
	}
	mIsConnectionEstablished = false;
}

AppClient::AppClient(FSClient& fsClient, YandexClient& yaClient)
	: fsClient{fsClient}
	, yaClient{yaClient}
{
}

AppClient& AppClient::Instance()
{
	static AppClient instance{FSClient::Instance(), YandexClient::Instance()};
	return instance;
}

void AppClient::NewFile(File* oldFile, File_Ptr newFile)
{
	// I dunno what is it for
	assert(newFile->path.string().back() != '/');

	if (newFile->name == PARENT_DIR_NAME)
	{
		fs::path	newPath = newFile->path;
		std::string newName = newFile->name;
		std::string newLocationId = newFile->location_id;
		int			newPlaylistId = newFile->playlistId;

		// /path/to/file/.. -> /path/to/file
		newPath = newFile->path.parent_path();
		newName = newPath.filename();

		oldFile->path = newPath;
		oldFile->name = newName;
		oldFile->location_id = newLocationId;
		oldFile->playlistId = newPlaylistId;
	}
	else
	{
		oldFile->path = newFile->path;
		oldFile->name = newFile->name;
		oldFile->location_id = newFile->location_id;
		oldFile->playlistId = newFile->playlistId;
	}
}

std::vector<File_Ptr> AppClient::GetDirFiles(
	File* file, bool noDirs, bool onlyMusicExtensions, bool recursively)
{
	// if (mainDirectory)
	// return list of playlists in app database
	// else - specific playlist
	// return list of tracks in that playlist
	assert(("file is not a directory!", file->type == File::Type::Directory));
	auto& db = DatabaseManager::Instance();
	if (file->name == "App")
	{
		std::vector<File_Ptr> appDirFiles;
		auto				  dbPlaylists = db.playlistDao.FetchAll();
		for (auto& dbPlaylist : dbPlaylists)
		{
			appDirFiles.push_back(AppFile::ConvertFromDBPlaylist(dbPlaylist));
		}
		return appDirFiles;
	}
	else
	{
		std::vector<File_Ptr> appTrackFiles;
		Debug::Print("HERE: " + file->name);

		//		AppFile* appFile{dynamic_cast<AppFile*>(file)};
		//		assert(("Error! File is not an appFile!", appFile != nullptr));
		auto dbTracks = db.trackDao.FetchAllFromPlaylist(file->playlistId);
		Debug::Print("dbTracks.size() " + std::to_string(dbTracks.size()));
		for (auto& dbTrack : dbTracks)
		{
			appTrackFiles.push_back(AppFile::ConvertFromDBTrack(dbTrack));
		}
		if (noDirs == false)
		{
			auto&		client = file->client;
			std::string name = PARENT_DIR_NAME;
			fs::path	path = file->path.parent_path() / "..";
			File::Type	type = File::Type::Directory;

			File_Ptr filePtr{std::make_shared<AppFile>(client, name, path, "NULL", type)};
			appTrackFiles.push_back(std::move(filePtr));
		}

		return appTrackFiles;
	}

	// 1) Запрос к БДшке, чтобы взять файлы
	// Запрос может быть как в главной дире /App, так и в альбомах /App/playlist_name
	// Если к главной дире, то просто берем список плейлистов из бдшки и возвращаем
	// Если к плейлисту, то берем список треков в плейлисте
	//		Нюанс в том, что треки в плейлисте будут как из local так и из yandex
}

std::string AppClient::GetSongData(File* file)
{
	AppFile* appFile{dynamic_cast<AppFile*>(file)};
	assert(("Error! File is not an appFile!", appFile != nullptr));

	Debug::Print("RAZVILKA: " + appFile->path.string());
	if (appFile->path.string().starts_with("/Yandex"))
	{
		return yaClient.GetSongData(file);
	}
	else
	{
		return fsClient.GetSongData(file);
	}
	//	assert(("BAD fromType value!", false));
}
