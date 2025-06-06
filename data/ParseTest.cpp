#include "../xml_lib/rapidxml.hpp"
#include "../xml_lib/rapidxml_utils.hpp"
#include "Utility.hpp"

#include <iostream>
#include <unordered_map>

const std::string xmlBindingsFile = getProjectDir() + "/data/KeyBindings.xml";
const std::string xmlKeysIDFile = getProjectDir() + "/data/KeysID.xml";

using namespace rapidxml;

int main()
{
	{
		const std::string&							 SW_Name = "MainWindow";
		std::unordered_map<std::string, std::string> globalBindings;
		std::unordered_map<std::string, std::string> localBindingds;

		rapidxml::file<>		 xFile(xmlBindingsFile.c_str());
		rapidxml::xml_document<> doc;
		doc.parse<0>(xFile.data());
		xml_node<>* rootNode = doc.first_node("bindings");
		xml_node<>* swNode = rootNode->first_node(SW_Name.c_str());
		xml_node<>* globalNode = swNode->first_node("global");
		xml_node<>* localNode = swNode->first_node("local");

		for (xml_node<>* gActionNode = globalNode->first_node("action"); gActionNode;
			 gActionNode = gActionNode->next_sibling("action"))
		{
			std::string actionName = gActionNode->first_attribute("name")->value();
			std::string keyName = gActionNode->first_attribute("key")->value();

			// globalBindings.insert(std::make_pair(std::move(keyName),
			//     std::move(actionName)));
			std::cout << "Global: " << "Key: " << keyName << " "
					  << " action: " << actionName << "\n";
		}

		for (xml_node<>* lActionNode = localNode->first_node("action"); lActionNode;
			 lActionNode = lActionNode->next_sibling("action"))
		{
			std::string actionName = lActionNode->first_attribute("name")->value();
			std::string keyName = lActionNode->first_attribute("key")->value();

			// localBindingds.insert(std::make_pair(std::move(keyName),
			//     std::move(actionName)));

			std::cout << "Local: " << "Key: " << keyName << " "
					  << " action: " << actionName << "\n";
		}
	}

	{
		std::unordered_map<std::string, int> mKeyNameToEnum;
		rapidxml::file<>					 xFile(xmlKeysIDFile.c_str());
		rapidxml::xml_document<>			 doc;
		doc.parse<0>(xFile.data());
		xml_node<>* rootNode = doc.first_node("keys");

		for (xml_node<>* keyNode = rootNode->first_node("key"); keyNode;
			 keyNode = keyNode->next_sibling("key"))
		{
			std::string keyName = keyNode->first_attribute("name")->value();
			int			id = std::stoi(keyNode->first_attribute("id")->value());

			// if (!isEventKeyValid(id))
			// {
			//     throw;
			// }
			mKeyNameToEnum.insert(std::make_pair(keyName, (id)));
		}
		for (auto& pair : mKeyNameToEnum)
		{
			std::cout << pair.first << " " << pair.second << "\n";
		}
	}
}