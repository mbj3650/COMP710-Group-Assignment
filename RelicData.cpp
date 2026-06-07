#include "RelicData.h"
#include <string>

RelicData::RelicData()
{
}
RelicData::RelicData(std::string sprite, std::string name, std::string desc, std::string imbue, int effectID)
{
	Sprite = sprite;
	Name = name;
	Description = desc;
	Imbues = imbue;
	Effect = effectID;
}
RelicData::~RelicData()
{

}