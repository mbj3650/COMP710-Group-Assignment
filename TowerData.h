#ifndef TOWERDATA_H
#define TOWERDATA_H
#include <string>
class TowerData
{
public:
	TowerData();
	TowerData(std::string sprite, int price, float range, float firerate, std::string projID, float speed);
	~TowerData();

	std::string Sprite;
	int Price;
	float Range;
	float Firerate;
	std::string ProjectileID;
	float Speed;

};

#endif // TOWERDATA_H
