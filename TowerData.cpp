#include "TowerData.h"
#include <string>

TowerData::TowerData()
{
}
TowerData::TowerData(std::string sprite, int price, float range, float firerate, std::string projID, float speed)
{
	Sprite = sprite;
	Price = price;
	Range = range;
	Firerate = firerate;
	ProjectileID = projID;
	Speed = speed;
}
TowerData::~TowerData()
{

}