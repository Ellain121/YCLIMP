#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <vector>

/**
 * @brief DataBasePlaylist - playlist representation in database
 */
struct DBPlaylist
{
public:
	struct BasedOn
	{
		std::string fromType;
		std::string locationId;
		std::string fsPath;
		std::string name;
	};

public:
	DBPlaylist(int id, const std::string& name);
	DBPlaylist(int id, const std::string& name, std::vector<int> trackOrder,
		std::vector<BasedOn> basedOnList);

public:
	int					 id;
	std::string			 name;
	std::vector<int>	 trackOrder;
	std::vector<BasedOn> basedOnList;
};

/**
 * @brief The PlaylistDao class -
 * Database class to manipulate playlist related data@n
 *
 * Has 3 types of tables:@n
 * 1) One table with all playlists@n
 * 2) Table for each playlist defining tracks order in that playlist@n
 * 3) Table for each playlist defining based on sources@n
 * (based on sources - if one of the sources have new tracks, the playlist will be also updated)@n
 *
 * - All the methods should be const
 */
class PlaylistDao
{
public:
	PlaylistDao(SQLite::Database& database);

	void					Init() const;
	int						AddPlaylist(const std::string& name) const;
	int						AddNewOrGetPlaylist(const std::string& name) const;
	void					RenamePlaylist(int id, const std::string& newName) const;
	std::vector<DBPlaylist> FetchAll() const;

	void AddTracksToPlaylist(int playlistId, std::vector<int> trackIds) const;
	void AddTrackToPlaylist(int playlistId, int trackId) const;

private:
	SQLite::Database& mDatabase;
};
