#ifndef TOWERDATA_H
#define TOWERDATA_H
#include <string>
class TowerData
{
public:
	TowerData();
	TowerData(
		std::string sprite, int price, float range, float firerate, std::string projID, float speed, int id, 
		std::string u1_name, std::string u1_desc, int u1_price,
		std::string u2_name, std::string u2_desc, int u2_price,
		std::string u3_name, std::string u3_desc, int u3_price
	);
	~TowerData();

	std::string Sprite;
	int Price;
	float Range;
	float Firerate;
	std::string ProjectileID;
	float Speed;
	int ID;

	std::string Upgrade1Name;
	std::string Upgrade1Desc;
	int Upgrade1Price;
	std::string Upgrade2Name;
	std::string Upgrade2Desc;
	int Upgrade2Price;
	std::string Upgrade3Name;
	std::string Upgrade3Desc;
	int Upgrade3Price;
};

#endif // TOWERDATA_H
