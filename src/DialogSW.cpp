#include "DialogSW.hpp"

#include "DialogElement.hpp"
#include "NewElementsDialogBundle.hpp"
#include "SWManager.hpp"

DialogSW::DialogSW(const Recti& bounds, Context context, SWHolderType inHolderType)
	: SubWindow{ESWTypes::Dialog, bounds, context, inHolderType}
{
	DialogSW::InitializeHotKeysActions();
}

void DialogSW::EventLoopIterationStart()
{
	if (mDialogBundle != nullptr && !mDialogBundle->EventLoopIterationStart())
	{
		// set await state
		mDialogBundle = nullptr;
		GetContext().swManager.AddSWEvent(
			SWEvent{ESWTypes::None, ESWEventAction::DisableCentralHolder, "pass"});
	}
}

void DialogSW::InitializeHotKeysActions()
{
	mHotKeyComponent.InsertBindingAction(std::make_pair("up",
		[this]()
		{
			if (mDialogBundle)
				mDialogBundle->InputUp();
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("down",
		[this]()
		{
			if (mDialogBundle)
				mDialogBundle->InputDown();
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("left",
		[this]()
		{
			if (mDialogBundle)
				mDialogBundle->InputLeft();
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("right",
		[this]()
		{
			if (mDialogBundle)
				mDialogBundle->InputRight();
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("enter",
		[this]()
		{
			if (mDialogBundle)
				mDialogBundle->InputEnter();
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("tab",
		[this]()
		{
			if (mDialogBundle)
				mDialogBundle->InputTab();
		}));
}

void DialogSW::HandleUserEvent(Event event, bool rawInputMode)
{
	SubWindow::HandleUserEvent(event, rawInputMode);
	if (rawInputMode && mDialogBundle != nullptr)
	{
		mDialogBundle->HandleRawInput(event.GetRawKey());
	}
}

void DialogSW::HandleSWEvents(std::vector<SWEvent>& swEvents)
{
	for (int i = swEvents.size() - 1; i >= 0; --i)
	{
		auto& swEvent = swEvents[i];
		if (swEvent.target != GetSWType())
			continue;

		if (swEvent.action == ESWEventAction::ChooseActionWithSelectedElements)
		{
			std::vector<File_Ptr> data = std::get<std::vector<File_Ptr>>(swEvent.data);
			mDialogBundle = std::make_unique<NewElementsDialogBundle>(
				std::move(data), GetContext(), GetBounds());
			swEvents.erase(swEvents.begin() + i);
		}
	}
}

void DialogSW::Update()
{
}

void DialogSW::Draw() const
{
	if (!IsVisible())
		return;

	SubWindow::DrawBoundsRectangle();
	if (mDialogBundle)
		mDialogBundle->Draw();
}

void DialogSW::HandleResizeEvent(const Recti& newRect)
{
	SubWindow::HandleResizeEvent(newRect);
	if (mDialogBundle)
		mDialogBundle->HandleResizeEvent(newRect);
}
