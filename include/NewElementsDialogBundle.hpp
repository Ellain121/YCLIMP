#pragma once

#include "DialogBundle.hpp"
#include "SubWindow.hpp"

#include <memory>
#include <vector>

class DialogElement;

/**
 * @brief The NewElementsDialogBundle class - sequence of DialogElements that aims to add new tracks(metadata) to application's data.@n
 *
 * Rough schema:@n
 * 1) Dialog with elements(tracks or folders) list to confirm@n
 * 2) Select where to which playlist(possible to create a new playlist) add those elements@n
 * 3) Select source folders if any (Source folder - if source folder updates, app database also uptades with newest tracks from this source)@n
 * 4) Add them to the app's database@n
 */
class NewElementsDialogBundle : public DialogBundle
{
public:
	enum class Steps
	{
		Confirm_Selected_Elements = -1,
		Select_Playlist_Or_Create_New = 0,
		Set_Playlist_Name_Or_Use_Selected = 1,
		Conform_Playlist_name_Or_Skip = 2,
		Choose_Source_Directories_If_Any = 3,
		Add_Data_To_Database = 4,
	};

public:
	NewElementsDialogBundle(
		std::vector<File_Ptr> elements, Context context, const Recti& bounds);

	virtual bool EventLoopIterationStart() override;
	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;
	virtual void HandleRawInput(int rawInputChar) override;

	virtual void InputUp() override;
	virtual void InputDown() override;
	virtual void InputLeft() override;
	virtual void InputRight() override;
	virtual void InputEnter() override;
	virtual void InputTab() override;

private:
	bool						   mIsActive;
	std::vector<File_Ptr>		   mElements;
	std::unique_ptr<DialogElement> mActiveDialogElement;
	Steps						   mNextDialogStep;
	std::string					   mTargetPlaylistName;
};
