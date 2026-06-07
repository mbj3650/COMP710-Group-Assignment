#ifndef RelicManager_H
#define RelicManager_H
#include <string>
#include <map>
#include <vector>
using namespace std;
class RelicData;
class Tower;
enum RelicTypes
{
    RedPotion,
	CrystalShard,
	Campfire,
	Mushroom,
	Coffee,
	Sash,
	Shrimp,
	TheTome
};
class RelicManager
{
public:
	RelicManager();
	~RelicManager();

	static RelicManager& Get();
	static void Destroy();

	void Initialise();
	void AddRandomRelic(vector<Tower*> towers);

	map<string, int> TowerRelics;
	map<string, int> ProjectileRelics;
protected:
	static RelicManager* sm_pInstance;
private:
	RelicManager(const RelicManager&);
	RelicManager& operator=(const RelicManager&);
};

#endif // RelicManager_H
