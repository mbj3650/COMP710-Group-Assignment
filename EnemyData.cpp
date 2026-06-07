#include "EnemyData.h"
#include <string>
EnemyData::EnemyData()
{
}
EnemyData::EnemyData(std::string sprite, int price, int damage, float speed, int bonushealth, int id, int spritesizeX, int spritesizeY)
{
	Sprite = sprite;
	Price = price;
	Damage = damage;
	Speed = speed;
	BonusHealth = bonushealth;
	ID = id;
	SpriteSizeX = spritesizeX;
	SpriteSizeY = spritesizeY;
}
EnemyData::~EnemyData()
{

}