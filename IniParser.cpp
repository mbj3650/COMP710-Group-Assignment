// COMP710 GP Framework 2022
// This include:
#include "IniParser.h"
// Local includes:
#include "renderer.h"
#include <map>
#include <string>
#include <fstream>
using namespace std;
// Library includes:
#include <cassert>
IniParser::IniParser()
{
}
IniParser::~IniParser()
{
}

bool IniParser::LoadIniFile(const std::string& filename)
{
	string Text;
	ifstream Reader(filename);
	string sectionname = "";
	while (getline(Reader, Text)) {
		if (!Text.empty()) {//if line isnt empty
			// Output the text from the file
			if (Text.at(0) == '[')//a [ indicates a section name like [name]
			{//if new section then get the new name
				Text.erase(Text.begin() + 0);
				Text.erase(Text.end() - 1);
				sectionname = Text;
				sections.push_back(sectionname);//add section to list of sections
			}
			else {//IF ITS NOT A SECTION, ASSUME ITS A VALUE SO WE CAN ADD TO I
				string valuename = Text.substr(0, Text.find('=')); //split at "|"
				string valuevalue = Text.substr(Text.find('=') + 1); //split at "="
				//std::cout << valuevalue << "\n";
				string namevalue = "";
				namevalue.append(sectionname + "|" + valuename);

				layout.insert({ namevalue, valuevalue });//add valuename and then actual value  
				//std::cout << "name:" << namevalue << "\nvalue:" << valuevalue << "\n";
			}
		}
		
	
	}
	Reader.close();
	return true;
}

std::string IniParser::GetValueAsString(const std::string& key)
{
	auto str = layout.find(key);
	if (str != layout.end()) {
		return (str->second);
	}
	else {
		return "";
	}
}

int IniParser::GetValueAsInt(const std::string& key)
{
	auto str = layout.find(key);
	if (str != layout.end()) {
		return atoi(str->second.c_str());
	}
	else {
		return -1;
	}
}

float IniParser::GetValueAsFloat(const std::string& key)
{
	auto str = layout.find(key);
	if (str != layout.end()) {
		return atof(str->second.c_str());
	}
	else {
		return -1;
	}
}

bool IniParser::GetValueAsBoolean(const std::string& key)
{
	auto str = layout.find(key);
	if (str != layout.end()) {	
		if (str->second == "true" || "0") {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}
