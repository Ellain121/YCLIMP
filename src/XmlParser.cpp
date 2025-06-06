#include "XmlParser.hpp"

#include "Primitives.hpp"
#include "Utility.hpp"

#include <cassert>

namespace
{
// hardcoded keybindig xml files
const std::string xmlBindingsFile = getProjectDir() + "/data/KeyBindings.xml";
const std::string xmlKeysIDFile = getProjectDir() + "/data/KeysID.xml";
}	 // namespace

XmlParser::XmlParser()
{
	LoadKeyToEnumTable();
}

/// Fill in keys id to keys text representation table
void XmlParser::LoadKeyToEnumTable()
{
	rapidxml::file<>		 xFile{xmlKeysIDFile.c_str()};
	rapidxml::xml_document<> doc;
	doc.parse<0>(xFile.data());
	xml_node<>* rootNode = doc.first_node("keys");

	for (xml_node<>* keyNode = rootNode->first_node("key"); keyNode;
		 keyNode = keyNode->next_sibling("key"))
	{
		std::string keyName = keyNode->first_attribute("name")->value();
		int			id = std::stoi(keyNode->first_attribute("id")->value());

		if (!isEventKeyValid(id))
		{
			throw std::runtime_error(
				"XmlParser::LoadKeyToEnumTable: Key is not valid! (key_name: " + keyName +
				")");
		}
		mKeyNameToEnum.insert(std::make_pair(keyName, EEventKey(id)));
	}
}

/// Get real key id from key name
EEventKey XmlParser::GetEventKey(const std::string& keyName)
{
	if (mKeyNameToEnum.find(keyName) == mKeyNameToEnum.end())
	{
		throw std::logic_error(
			"XmlParser::GetEventKey: Cannot find appropriate enum\
                                            element for the key (key_name: " +
			keyName + ")");
	}

	return mKeyNameToEnum[keyName];
}

/**
 * @brief Load bindings(local & global). bindings - table with real key id <-> action name.@n
 * Uses mKeyNameToEnum to get real key id from key name.
 */
void XmlParser::LoadBindings(
	const std::string& name, BindingMap& globalBindings, BindingMap& localBindingds)
{
	rapidxml::file<>		 xFile{xmlBindingsFile.c_str()};
	rapidxml::xml_document<> doc;
	doc.parse<0>(xFile.data());
	xml_node<>* rootNode = doc.first_node("bindings");
	assert(rootNode != nullptr);
	xml_node<>* bNode = rootNode->first_node(name.c_str());
	assert(bNode != nullptr);
	xml_node<>* globalNode = bNode->first_node("global");
	assert(globalNode != nullptr);
	xml_node<>* localNode = bNode->first_node("local");
	assert(localNode != nullptr);

	for (xml_node<>* gActionNode = globalNode->first_node("action"); gActionNode;
		 gActionNode = gActionNode->next_sibling("action"))
	{
		std::string actionName = gActionNode->first_attribute("name")->value();
		std::string keyName = gActionNode->first_attribute("key")->value();
		EEventKey	eventKey = GetEventKey(keyName);

		if (mGlobalHotkeyExist.find(eventKey) != mGlobalHotkeyExist.end())
		{
			throw std::runtime_error(
				"XmlParser::LoadBindings: Global hotkeys collision! (key_name: " +
				keyName + ")");
		}
		mGlobalHotkeyExist.insert(eventKey);

		globalBindings.insert(std::make_pair(std::move(eventKey), std::move(actionName)));
	}

	for (xml_node<>* lActionNode = localNode->first_node("action"); lActionNode;
		 lActionNode = lActionNode->next_sibling("action"))
	{
		std::string actionName = lActionNode->first_attribute("name")->value();
		std::string keyName = lActionNode->first_attribute("key")->value();
		EEventKey	eventKey = GetEventKey(keyName);

		localBindingds.insert(std::make_pair(std::move(eventKey), std::move(actionName)));
	}
}
