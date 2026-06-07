#ifndef RELICDATA_H
#define RELICDATA_H
#include <string>
class RelicData
{
public:
	RelicData();
	RelicData(std::string sprite, std::string name, std::string desc, std::string imbue, int effect);
	~RelicData();

	std::string Sprite;
	std::string Name;
	std::string Description;
	std::string Imbues;
	int Effect;
};

#endif // RELICDATA_H
