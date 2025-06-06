#pragma once

#include "Context.hpp"
#include "HotKeyComponent.hpp"
#include "Primitives.hpp"
#include "Utility.hpp"

#include <memory>
#include <variant>

/// = Enum SubWindow Types
enum class ESWTypes
{
	None = -1,
	Player,
	PlayerStatus,
	FileExplorer,
	LocalFiles,
	YandexFiles,
	AppFiles,
	Dialog,
	Settings,
	StartWindow
};

/// = Enum SubWindow Event Action
/// SubWindow's event list
enum class ESWEventAction
{
	None,
	PlayMusicFile,
	PlayMusicBytes,
	PlayedMusicDone,
	OpenNewDirInPlayerSW,
	OpenOneFileInPlayerSW,
	AppendNewDirInPlayerSW,
	AppendOneFileToPlayerSW,
	OpenPlaylistOfTheDayInPlayerSW,
	OpenMyVibeInPlayerSW,
	EnableCentralHolder,
	DisableCentralHolder,
	ChooseActionWithSelectedElements,
};

class File;
using File_Ptr = std::shared_ptr<File>;

/**
 * @brief SubWindow Event
 *
 * SWManger manages all the SubWindows@n
 * All the SubWindows able to send SWEvents between each other@n
 * If target is ESWTypes::None, it's target is SWManager
 */
struct SWEvent
{
	SWEvent(ESWTypes evTarget, ESWEventAction evAction,
		const std::variant<std::vector<File_Ptr>, File_Ptr, std::string>& data,
		const std::string&												  data_2 = "");

	ESWTypes												   target;
	ESWEventAction											   action;
	std::variant<std::vector<File_Ptr>, File_Ptr, std::string> data;
	std::string												   data_2;
};

/**
 * @brief The SubWindow class
 *
 * SWHolder holds SubWindows and responsible for size and pos, unable to draw@n
 * SWManager manages all the SubWindows@n
 *
 *  - Has bindings/hotkeys@n
 *  - Selectable, if selected, local bindings will be active@n
 *  - Can be visible/invisible, simply not drawn if invisible@n
 *  - Usually created right from the start of the programm@n
 *  - Has boolean DrawPriority, if true, wll be drawn last@n
 *  - Has Context
 */
class SubWindow
{
public:
	using Ptr = std::shared_ptr<SubWindow>;

public:
	SubWindow(ESWTypes swType, const Recti& rect, Context text,
		SWHolderType inHolderType = SWHolderType::None);
	virtual ~SubWindow() = default;

public:
	virtual void EventLoopIterationStart();
	virtual void HandleUserEvent(Event event, bool rawInputMode);
	virtual void HandleSWEvents(std::vector<SWEvent>& swEvents) = 0;
	virtual void Update() = 0;
	virtual void Draw() const = 0;
	virtual void HandleResizeEvent(const Recti& newRect);

	// manually reload the data displayed on a SubWindow
	virtual void Reload();

	virtual bool HasSelection() const;
	virtual void Select();
	virtual void Deselect();

	virtual bool IsVisible() const;
	virtual void SetVisible(bool visibleValue);

	/// if true, drawn last
	virtual void SetHighDrawPriority(bool priority);
	/// only boolean, if true, drawn last
	virtual bool GetDrawPriority() const;

	SWHolderType	   GetHolderType() const;
	const std::string& GetName() const;
	ESWTypes		   GetSWType() const;
	const Recti&	   GetBounds() const;

public:
	/// Convert ESWTypes to it's std::string representation
	static const std::string& ConvertToSWName(ESWTypes swType);

private:
	virtual void InitializeHotKeysActions() = 0;

protected:
	Context GetContext();
	void	DrawBoundsRectangle(const std::string& rectName = "") const;

protected:
	HotKeyComponent mHotKeyComponent;

private:
	Recti		   mBounds;
	const ESWTypes mType;
	Context		   mContext;
	SWHolderType   mInHolderType;
	bool		   mSelected;
	bool		   mIsVisible;
	bool		   mHighDrawPriority;

	static std::unordered_map<ESWTypes, std::string> mTypeToNameTable;
};
