#include "NewElementsDialogBundle.hpp"

#include "ConfirmDialogElement.hpp"
#include "DatabaseManager.hpp"
#include "LineEditDialogElement.hpp"
#include "ListDialogElement.hpp"
#include "SWManager.hpp"

#include <algorithm>
#include <cassert>

NewElementsDialogBundle::NewElementsDialogBundle(
	std::vector<File_Ptr> elements, Context context, const Recti& bounds)
	: DialogBundle{context, bounds}
	, mIsActive{true}
	, mElements{elements}
	, mActiveDialogElement{nullptr}
	, mNextDialogStep{Steps::Confirm_Selected_Elements}
	, mTargetPlaylistName{""}
{
	// ReadOnly list to confirm selected elements
	// Steps::Confirm_Selected_Elements
	{
		mActiveDialogElement = std::make_unique<ListDialogElement<File_Ptr>>(bounds,
			"Confirm selected elements", std::move(elements),
			ListDialogElement<File_Ptr>::Type::ReadOnly);
		mNextDialogStep = Steps::Select_Playlist_Or_Create_New;
	}
}

bool NewElementsDialogBundle::EventLoopIterationStart()
{
	assert(("mActiveDialogElement is nullptr!", mActiveDialogElement != nullptr));

	if (mActiveDialogElement->IsFinished())
	{
		if (mActiveDialogElement->GetFinishedStatus() == CANCEL_OPTION)
		{
			mActiveDialogElement = nullptr;
			mIsActive = false;
			return mIsActive;
		}
		// Next DialogElement is created in this switch case
		// mActiveDialogIndx only increase or stay the same, cannot go down/backwards
		switch (mNextDialogStep)
		{
			case Steps::Select_Playlist_Or_Create_New:
			{
				using DBPlaylistPtr = std::shared_ptr<DBPlaylist>;
				std::vector<DBPlaylist> playlists =
					mContext.dbManager.playlistDao.FetchAll();
				auto playlistPtrs =
					ElementSelector<DBPlaylistPtr>::CrutchConverter<DBPlaylist>(
						playlists);

				mActiveDialogElement = std::make_unique<ListDialogElement<DBPlaylistPtr>>(
					mBounds, "playlists", std::move(playlistPtrs),
					ListDialogElement<DBPlaylistPtr>::Type::OneChoice,
					std::vector<std::string>{
						CANCEL_OPTION, OK_OPTION, ADD_TO_NEW_PLAYLIST_OPTION});

				mNextDialogStep = Steps::Set_Playlist_Name_Or_Use_Selected;
			}
			break;

			case Steps::Set_Playlist_Name_Or_Use_Selected:
			{
				// if previos dialogElement opton is to add new playlist
				if (mActiveDialogElement->GetFinishedStatus() ==
					ADD_TO_NEW_PLAYLIST_OPTION)
				{
					mContext.swManager.setRawInputMode(true);
					mActiveDialogElement = std::make_unique<LineEditDialogElement>(
						mBounds, "Set new playlist name");

					mNextDialogStep = Steps::Conform_Playlist_name_Or_Skip;
				}
				// if we chose existing playlist, skip to Choose_Source_Directories_If_Any
				else if (mActiveDialogElement->GetFinishedStatus() == OK_OPTION)
				{
					auto data = mActiveDialogElement->GetData();
					mTargetPlaylistName = std::get<std::string>(data);

					mNextDialogStep = Steps::Choose_Source_Directories_If_Any;
					assert(mActiveDialogElement->GetFinishedStatus() == OK_OPTION);
				}
				else
				{
					assert(false);
				}
			}
			break;

			case Steps::Conform_Playlist_name_Or_Skip:
			{
				mContext.swManager.setRawInputMode(false);
				mTargetPlaylistName =
					std::get<std::string>(mActiveDialogElement->GetData());

				mActiveDialogElement = std::make_unique<ConfirmDialogElement>(
					"Confirm playlist name", mTargetPlaylistName, mBounds);

				mNextDialogStep = Steps::Choose_Source_Directories_If_Any;
			}
			break;

			case Steps::Choose_Source_Directories_If_Any:
			{
				std::vector<File_Ptr> dirElements;
				for (auto& element : mElements)
				{
					if (element->type == File::Type::Directory)
					{
						dirElements.push_back(element);
					}
				}
				if (!dirElements.empty())
				{
					mActiveDialogElement =
						std::make_unique<ListDialogElement<File_Ptr>>(mBounds,
							"Choose directories as source directories for auto-update "
							"tracks from them",
							std::move(dirElements),
							ListDialogElement<File_Ptr>::Type::MultipleChoice);
				}

				mNextDialogStep = Steps::Add_Data_To_Database;
			}
			break;

			case Steps::Add_Data_To_Database:
			{
				std::vector<File_Ptr> dirElements;
				for (auto& element : mElements)
				{
					if (element->type == File::Type::Directory)
					{
						dirElements.push_back(element);
					}
				}

				auto& dbManager = mContext.dbManager;
				int	  playlistID =
					dbManager.playlistDao.AddNewOrGetPlaylist(mTargetPlaylistName);

				std::vector<DBTrack> DBTracks;
				DBTracks.reserve(mElements.size());
				for (int i = 0; i < mElements.size(); ++i)
				{
					auto el = mElements[i];
					if (el->type == File::Type::Directory)
					{
						auto tracksInDir = el->GetDirFiles(true, true, true);
						mElements.reserve(mElements.size() + tracksInDir.size());
						std::move(tracksInDir.begin(), tracksInDir.end(),
							std::back_inserter(mElements));
						continue;
					}
					// convert File_Ptr to DBTrack
					assert(
						("Element type is not a track!", el->type == File::Type::Track));
					DBTrack dbTrack{-1, playlistID, el->GetFromType(), el->location_id,
						el->path, el->name, -1};
					DBTracks.push_back(std::move(dbTrack));
				}
				///
				std::vector<int> idsOrder =
					dbManager.trackDao.AddTracks(std::move(DBTracks));
				dbManager.playlistDao.AddTracksToPlaylist(
					playlistID, std::move(idsOrder));
				// 1) плейлист, куда добавлять треки, нвоый или существующий, неважно
				// 2) Элементы, которые нужно добавить, они есть при создании класса, в конструкторе
				// 3) SourceList - набор папок-источников
				// some database operations
				mActiveDialogElement = nullptr;
				mIsActive = false;
			}
			break;

			default:
			{
				throw 42;
			}
		}
	}
	return mIsActive;
}

void NewElementsDialogBundle::Draw() const
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->Draw();
}

void NewElementsDialogBundle::HandleResizeEvent(const Recti& newRect)
{
	mBounds = newRect;
	mActiveDialogElement->HandleResizeEvent(newRect);
}

void NewElementsDialogBundle::HandleRawInput(int rawChar)
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->HandleRawInput(rawChar);
}

void NewElementsDialogBundle::InputUp()
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->InputUp();
}

void NewElementsDialogBundle::InputDown()
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->InputDown();
}

void NewElementsDialogBundle::InputLeft()
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->InputLeft();
}

void NewElementsDialogBundle::InputRight()
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->InputRight();
}

void NewElementsDialogBundle::InputEnter()
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->InputEnter();
}

void NewElementsDialogBundle::InputTab()
{
	if (mActiveDialogElement == nullptr)
		return;

	mActiveDialogElement->InputTab();
}
