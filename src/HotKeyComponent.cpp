#include "HotKeyComponent.hpp"

#include "XmlParser.hpp"

/**
* @brief HotKeyComponent::HotKeyComponent
* @param ownerName - should match bindings file(for now it's KeyBindings.xml but could be changed)
*/
HotKeyComponent::HotKeyComponent(const std::string& ownerName)
{
	LoadBindings(ownerName);
#ifdef DEBUG
	mOwnerName = ownerName;
#endif
}

void HotKeyComponent::LoadBindings(const std::string& ownerName)
{
	XmlParser xmlParser;
	xmlParser.LoadBindings(ownerName, mGlobalBindings, mLocalBindings);
}

/**
 * @brief Process global bindings. If ownerIsActive is true process local bindings as well.
 */
void HotKeyComponent::HandleUserEvent(Event event, bool ownerIsActive)
{
	HandleGlobalBindings(event);
	if (ownerIsActive)
	{
		HandleLocalBindings(event);
	}
}

void HotKeyComponent::HandleGlobalBindings(Event event)
{
	if (mGlobalBindings.find(event.key) != mGlobalBindings.end())
	{
		const auto& actionName = mGlobalBindings[event.key];
		if (mBindingActions.find(actionName) == mBindingActions.end())
			throw std::runtime_error(
				"HotKeyComponent::HandleGlobalBindings: \
                                            Cannot find appropriate action!");

		mBindingActions[actionName]();
	}
}

void HotKeyComponent::HandleLocalBindings(Event event)
{
	if (mLocalBindings.find(event.key) != mLocalBindings.end())
	{
		const auto& actionName = mLocalBindings[event.key];
		if (mBindingActions.find(actionName) == mBindingActions.end())
			throw std::runtime_error(
				"HotKeyComponent::HandleLocalBindings: \
                                            Cannot find appropriate action!");

		mBindingActions[actionName]();
	}
}

void HotKeyComponent::InsertBindingAction(
	const std::pair<std::string, std::function<void()>>& bindingAction)
{
#ifdef DEBUG
	bool requiredActionNameExist = false;
	for (auto& lBinding : mLocalBindings)
	{
		if (lBinding.second == bindingAction.first)
		{
			requiredActionNameExist = true;
			break;
		}
	}
	if (!requiredActionNameExist)
	{
		for (auto& gBinding : mGlobalBindings)
		{
			if (gBinding.second == bindingAction.first)
			{
				requiredActionNameExist = true;
				break;
			}
		}
	}
	if (!requiredActionNameExist)
	{
		throw std::runtime_error{
			"HotKeyComponent::InsertBindingAction: \
            required action name does not exist! (action_name: " +
			bindingAction.first + " owner name: " + mOwnerName + ")"};
	}
#endif

	if (mBindingActions.find(bindingAction.first) != mBindingActions.end())
	{
		throw std::runtime_error{
			"HotKeyComponent::InsertBindingAction: \
            Binding already exist! (action_name: " +
			bindingAction.first + ")"};
	}

	mBindingActions.insert(bindingAction);
}
