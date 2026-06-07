#ifndef ENEMYDATA_H
#define ENEMYDATA_H
#include <string>
class EnemyData
{
public:
	EnemyData();
	EnemyData(std::string sprite, int price, int damage, float speed, int bonushealth, int id);
	~EnemyData();

	std::string Sprite;
	int Price;
	int Damage;
	float Speed;
	int BonusHealth;
	int ID;
};

#endif // ENEMYDATA_H
