#pragma once

#include "Primitives.hpp"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace rapidxml;

using BindingMap = std::unordered_map<EEventKey, std::string>;

/**
 * @brief A convenient class to parse xml keybindings.@n
 * Uses opensource rapidxml libarary (xml lib is probably from there https://github.com/Fe-Bell/RapidXML)
 *
 * - Main method is LoadBindings that used once for each HotkeyComponent@n
 * - 1) Has a table to bind key name to it's real key id in ncurses(xml table at xmlKeysIDFile)@n
 * - 2) Has a table to bind action names with real key id@n
 * - 3) XML file with actions binds action name with key name. (1)- helps to connect action names with real key ids@n
 */
class XmlParser
{
public:
	XmlParser();

	void LoadBindings(const std::string& SW_Name, BindingMap& globalBindings,
		BindingMap& localBindingds);

private:
	void	  LoadKeyToEnumTable();
	EEventKey GetEventKey(const std::string& keyName);

private:
	/// key name to key real id convertion table
	std::unordered_map<std::string, EEventKey> mKeyNameToEnum;
	std::unordered_set<EEventKey>			   mGlobalHotkeyExist;
};
