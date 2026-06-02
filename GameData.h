#ifndef GAMEDATA_H
#define GAMEDATA_H
#include <map>
#include <string>
using namespace std;
class ProjectileData;
class TowerData;
class EnemyData;
class GameData
{
public:
	GameData();
	~GameData();

	static GameData& Get();
	static void Destroy();

	void Initialise();
	void LoadTowerData();
	void LoadProjectileData();
	void LoadEnemyData();
	map<string, TowerData> Tower;
	map<string, ProjectileData> Projectile;
	map<string, EnemyData> Enemy;
protected:
	static GameData* sm_pInstance;
private:
	GameData(const GameData&);
	GameData& operator=(const GameData&);
};

#endif // GAMEDATA_H
