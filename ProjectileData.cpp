#include "ProjectileData.h"
#include <string>

ProjectileData::ProjectileData()
{
}
ProjectileData::ProjectileData(std::string sprite, int damage, int pierce, int homing, int effect, int lifetime, int size)
{
	Sprite = sprite;
	Damage = damage;
	Pierce = pierce;
	Homing = homing;
	Effect = effect;
	Lifetime = lifetime;
	Size = size;
}
ProjectileData::~ProjectileData()
{

}