#include "ElementSelector.hpp"
#include "SubWindow.hpp"

#include <stack>

/**
 * @brief The YandexFilesSW class;
 * Yandex Files SubWindow is responsible for navigating in yandex music "filesystem"
 *
 * - This class is responsible for populating the PlayerSW
 * - This class is able to select tracks and add them to Application's albums
 */
class YandexFilesSW : public SubWindow
{
public:
	YandexFilesSW(
		Recti bounds, Context context, SWHolderType inHolderType = SWHolderType::None);

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
