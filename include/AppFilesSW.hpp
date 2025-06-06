#include "ElementSelector.hpp"
#include "SubWindow.hpp"

#include <stack>

/**
 * @brief
 * AppFilesSW = Application Files Subwindow@n
 *
 * This subwindow is resonsible for navigating in application's filesystem, open folders, selecting tracks, etc.@n
 * It uses AppFile and AppClient from FileManager.hpp|.cpp@n
 * AppFiles is mostly metadata stored in application's database, but tracks can be downloaded@n
 * All the downloaded tracks located in unordered manner in one folder in your application's folder
 *
 * - This class is responsible for populating the PlayerSW
 * - This class is able to select tracks and add them to Application's playlists
 */
class AppFilesSW : public SubWindow
{
public:
	AppFilesSW(
		Recti bounds, Context context, SWHolderType inHolderType = SWHolderType::None);

	virtual void HandleUserEvent(Event event, bool rawInputMode) override;
	virtual void HandleSWEvents(std::vector<SWEvent>& swEvents) override;
	virtual void Update() override;
	virtual void Reload() override;
	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;

private:
	virtual void InitializeHotKeysActions() override;
	void		 InitializeInitialState();
	void		 OpenSelectedDirectory();

private:
	File_Ptr						 mCurrentDir;
	FileSelector					 mFileSelector;
	std::stack<ElementSelectorState> mFileselectorPrevStates;
};
