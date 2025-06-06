#pragma once

#include "Context.hpp"
#include "Primitives.hpp"

/**
 * @brief The DialogBundle class - DialogElement Bundle; Abstract class.@n
 * Create and switches a sequence of DialogElements@n
 */
class DialogBundle
{
public:
	DialogBundle(Context context, const Recti& bounds);

	virtual bool EventLoopIterationStart() = 0;
	virtual void Draw() const = 0;
	virtual void HandleResizeEvent(const Recti& newRect) = 0;
	virtual void HandleRawInput(int inputChar) = 0;

	virtual void InputUp() = 0;
	virtual void InputDown() = 0;
	virtual void InputLeft() = 0;
	virtual void InputRight() = 0;
	virtual void InputEnter() = 0;
	virtual void InputTab() = 0;

protected:
	Recti	mBounds;
	Context mContext;
};
