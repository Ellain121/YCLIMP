#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <vector>

/**
 * @brief The DataBaseTrack - track representation in Database
 */
struct DBTrack
{
	DBTrack() = default;
	DBTrack(int id, int playlistId, std::string fromType, std::string locationId,
		std::string fsPath, std::string name, int inode);

	int			id;
	int			playlistId;
	std::string fromType;
	std::string locationId;
	std::string fsPath;
	std::string name;
	int			inode;
};

/**
 * @brief The TrackDao class -
 * Database class to manipulate tracks related data@n
 *
 * Stores tracks data in one big unordered table
 * - All the methods should be const
 */
class TrackDao
{
public:
	TrackDao(SQLite::Database& database);
	void				 Init() const;
	std::vector<DBTrack> FetchAllFromPlaylist(int playlist_id) const;
	std::vector<DBTrack> FetchAll() const;
	DBTrack				 FetchOneByID(int playlist_id) const;
	DBTrack				 FetchOneByYaID(const std::string& location_id) const;
	DBTrack				 FetchOneByPath(const std::string& fs_path) const;
	int					 AddTrack(DBTrack track) const;
	std::vector<int>	 AddTracks(std::vector<DBTrack> tracks) const;

private:
	SQLite::Database& mDatabase;
};
