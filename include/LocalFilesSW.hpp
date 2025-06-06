#include "ElementSelector.hpp"
#include "SubWindow.hpp"

#include <stack>

/**
 * @brief The LocalFilesSW class;
 * Local Files SubWindow is responsible for navigating in local filesystem, open folders, selecting tracks, etc.
 *
 * - This class is responsible for populating the PlayerSW
 * - This class is able to select tracks and add them to Application's albums
 */
class LocalFilesSW : public SubWindow
{
public:
	LocalFilesSW(const Recti& bounds, Context context,
		SWHolderType inHolderType = SWHolderType::None);

	virtual void HandleUserEvent(Event event, bool rawInputMode) override;
	virtual void HandleSWEvents(std::vector<SWEvent>& swEvents) override;
	virtual void Update() override;
	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;

	//	void DrawElemSelectorInfo() const;

private:
	virtual void InitializeHotKeysActions() override;
	void		 InitializeInitialState();
	void		 OpenSelectedDirectory();

private:
	File_Ptr						 mCurrentDir;
	FileSelector					 mFileSelector;
	std::stack<ElementSelectorState> mFileselectorPrevStates;
};
