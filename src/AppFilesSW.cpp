#include "AppFilesSW.hpp"

#include "Debug.hpp"
#include "FileManager.hpp"
#include "SWManager.hpp"

AppFilesSW::AppFilesSW(Recti bounds, Context context, SWHolderType inHolderType)
	: SubWindow{ESWTypes::AppFiles, bounds, context, inHolderType}
	, mCurrentDir{AppFile::InitFile(GetContext().appClient)}
	, mFileSelector{Recti{
		  bounds.left + 1, bounds.top + 1, bounds.width - 2, bounds.height - 2}}
	, mFileselectorPrevStates{}
{
	Debug::Print("I hate the beach!");
	InitializeHotKeysActions();
	InitializeInitialState();
}

void AppFilesSW::InitializeHotKeysActions()
{
	mHotKeyComponent.InsertBindingAction(
		std::make_pair("up", [this]() { mFileSelector.SelectUp(); }));

	mHotKeyComponent.InsertBindingAction(
		std::make_pair("down", [this]() { mFileSelector.SelectDown(); }));

	//	mHotKeyComponent.InsertBindingAction(std::make_pair(
	//		"lasting_select", [this]() { mFileSelector.LastingSelectOnCurrent(); }));

	//	mHotKeyComponent.InsertBindingAction(std::make_pair("action_on_last_selected",
	//		[this]()
	//		{
	//			std::vector<File_Ptr> selected = mFileSelector.GetLastingSelectedElements();
	//			GetContext().swManager.AddSWEvent(SWEvent{ESWTypes::None,
	//				ESWEventAction::EnableCentralHolder, std::move(selected)});
	//		}));

	mHotKeyComponent.InsertBindingAction(
		std::make_pair("open_dir", [this]() { OpenSelectedDirectory(); }));

	mHotKeyComponent.InsertBindingAction(std::make_pair("open_entry_in_playersw",
		[this]()
		{
			const File_Ptr file = mFileSelector.GetSelected().value();

			// if file is a parent dir, we do nothing, parent dirs purpose is to
			// only move inside FileExplorer
			if (file->type == File::Type::Directory && file->name == PARENT_DIR_NAME)
				return;

			if (file->type == File::Type::Directory)
			{
				GetContext().swManager.AddSWEvent(SWEvent{ESWTypes::Player,
					ESWEventAction::OpenNewDirInPlayerSW, std::move(file)});
			}
			else
			{
				GetContext().swManager.AddSWEvent(SWEvent{ESWTypes::Player,
					ESWEventAction::OpenOneFileInPlayerSW, std::move(file)});
			}
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("add_entry_in_playersw",
		[this]()
		{
			const File_Ptr file = mFileSelector.GetSelected().value();

			// if file is a parent dir, we do nothing, parent dirs purpose is to
			// only move inside FileExplorer
			if (file->type == File::Type::Directory && file->name == PARENT_DIR_NAME)
				return;

			// if file is Directory, send signal for directory, else for a single
			// file
			if (file->type == File::Type::Directory)
			{
				GetContext().swManager.AddSWEvent(SWEvent{ESWTypes::Player,
					ESWEventAction::AppendNewDirInPlayerSW, std::move(file)});
			}
			else
			{
				GetContext().swManager.AddSWEvent(SWEvent{ESWTypes::Player,
					ESWEventAction::AppendOneFileToPlayerSW, std::move(file)});
			}
		}));
}

void AppFilesSW::InitializeInitialState()
{
	auto vec = mCurrentDir->GetDirFiles();
	mFileSelector.Load(std::move(vec), true);
	mFileSelector.SortDirsFirst();
}

void AppFilesSW::HandleUserEvent(Event event, bool rawInputMode)
{
	SubWindow::HandleUserEvent(event, rawInputMode);
}

void AppFilesSW::HandleSWEvents(std::vector<SWEvent>& swEvents)
{
}

void AppFilesSW::Update()
{
	mFileSelector.Update();
}

void AppFilesSW::Reload()
{
	// reload current state
	auto vec = mCurrentDir->GetDirFiles();
	mFileSelector.Load(std::move(vec), true);
	mFileSelector.SortDirsFirst();
}

void AppFilesSW::Draw() const
{
	if (!IsVisible())
		return;

	SubWindow::DrawBoundsRectangle("Application");
	mFileSelector.Draw();
}

void AppFilesSW::OpenSelectedDirectory()
{
	const File_Ptr file = mFileSelector.GetSelected().value();

	// we can only navigate through directories here
	if (file->type != File::Type::Directory)
		return;

	mFileselectorPrevStates.push(mFileSelector.GetState());

	mCurrentDir->NewFile(file);
	mFileSelector.Load(mCurrentDir->GetDirFiles(false, false), true);
	mFileSelector.SortDirsFirst();
	mFileSelector.SelectByIndx(0);
}

void AppFilesSW::HandleResizeEvent(const Recti& newRect)
{
	SubWindow::HandleResizeEvent(newRect);
	mFileSelector.HandleResize(
		Recti{newRect.left + 1, newRect.top + 1, newRect.width - 2, newRect.height - 2});
}
