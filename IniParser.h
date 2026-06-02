// COMP710 GP Framework 2022
#ifndef __INIPARSER_H__
#define __INIPARSER_H__
// Local includes:
#include <string>
#include <map>
#include <vector>
#include <iostream>
using namespace std;

// Forward declarations:
// Class declaration:
class IniParser
{
	// Member methods:
public:
	IniParser();
	~IniParser();
	bool LoadIniFile(const string& filename);
	string GetValueAsString(const string& key);
	int GetValueAsInt(const string& key);
	float GetValueAsFloat(const string& key);
	bool GetValueAsBoolean(const string& key);
protected:
private:
	IniParser(const IniParser& IniParser);
	IniParser& operator=(const IniParser& IniParser);
	// Member data:
public:
	map<string, string> layout;
	vector<string> sections;
protected:
private:
};
#endif // __IniParser_H__