#pragma once

#include "Primitives.hpp"

#include <functional>
#include <string>
#include <unordered_map>

/**
 * @brief The HotKeyComponent class - class to handle pre defined bindings.
 *
 * - Class that suppose to have predefined bindings should have member variable with this class@n
 * - Class that has predefined bindings should be mentioned in specific file inside projects with action and key pairs@n
 * - Most, but not all the input is coming from predefined bindings, rawinput is also a thing@n
 * - Has global and local bindings, Global always works; Local works only if owner selected/active somehow@n
 * - Bindings member variables are tables with key=real keyboard key id and value = action name@n
 * - BindingActions is a member variable that connect action name with functions that represent real actions
 */
class HotKeyComponent
{
public:
	HotKeyComponent(const std::string& ownerName);

	void InsertBindingAction(
		const std::pair<std::string, std::function<void()>>& bindingAction);
	void HandleUserEvent(Event event, bool ownerIsActive);

private:
	void		 LoadBindings(const std::string& ownerName);
	virtual void HandleGlobalBindings(Event event);
	virtual void HandleLocalBindings(Event event);

private:
	std::unordered_map<EEventKey, std::string>			   mGlobalBindings;
	std::unordered_map<EEventKey, std::string>			   mLocalBindings;
	std::unordered_map<std::string, std::function<void()>> mBindingActions;
#ifdef DEBUG
	std::string mOwnerName;
#endif
};
