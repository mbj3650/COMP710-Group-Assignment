#ifndef PROJECTILEDATA_H
#define PROJECTILEDATA_H
#include <string>
class ProjectileData
{
public:
	ProjectileData();
	ProjectileData(std::string sprite, int damage, int pierce, int homing, int effect, int lifetime, int id);
	~ProjectileData();

	std::string Sprite;
	int Damage;
	int Pierce;
	int Homing;
	int Effect;
	int Lifetime;
	int ID;
};

#endif // PROJECTILEDATA_H
