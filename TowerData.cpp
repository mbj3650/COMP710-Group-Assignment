#include "TowerData.h"
#include <string>

TowerData::TowerData()
{
}
TowerData::TowerData(
	std::string sprite, int price, float range, float firerate, std::string projID, float speed, int id,
	std::string u1_name, std::string u1_desc, int u1_price,
	std::string u2_name, std::string u2_desc, int u2_price,
	std::string u3_name, std::string u3_desc, int u3_price
)
{
	Sprite = sprite;
	Price = price;
	Range = range;
	Firerate = firerate;
	ProjectileID = projID;
	Speed = speed;
	ID = id;

	Upgrade1Name = u1_name;
	Upgrade1Desc = u1_desc;
	Upgrade1Price = u1_price;

	Upgrade2Name = u2_name;
	Upgrade2Desc = u2_desc;
	Upgrade2Price = u2_price;

	Upgrade3Name = u3_name;
	Upgrade3Desc = u3_desc;
	Upgrade3Price = u3_price;
}
TowerData::~TowerData()
{

}