#include "EnemyData.h"
#include <string>

EnemyData::EnemyData()
{
}
EnemyData::EnemyData(std::string sprite, int price, int damage, float speed, int bonushealth)
{
	Sprite = sprite;
	Price = price;
	Damage = damage;
	Speed = speed;
	BonusHealth = bonushealth;
}
EnemyData::~EnemyData()
{

}