#ifndef ENEMYDATA_H
#define ENEMYDATA_H
#include <string>
class EnemyData
{
public:
	EnemyData();
	EnemyData(std::string sprite, int price, int damage, float speed, int bonushealth);
	~EnemyData();

	std::string Sprite;
	int Price;
	int Damage;
	float Speed;
	int BonusHealth;

};

#endif // ENEMYDATA_H
