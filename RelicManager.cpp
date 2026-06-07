#include "RelicManager.h"
#include "RelicData.h"
#include "GameData.h"
#include "Tower.h"
#include "inlinehelpers.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

RelicManager::RelicManager()
{
}

RelicManager::~RelicManager()
{
	TowerRelics.clear();
	ProjectileRelics.clear();
}

void RelicManager::Initialise()
{
	TowerRelics.clear();
	ProjectileRelics.clear();
}

RelicManager* RelicManager::sm_pInstance = 0;
RelicManager& RelicManager::Get()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new RelicManager();
	}
	return (*sm_pInstance);
}

void RelicManager::Destroy()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

void RelicManager::AddRandomRelic(vector<Tower*> towers)
{
	const auto& ids = GameData::Get().RelicIDs;
	string relicToGet = ids[GetRandom(0, ids.size() - 1)];
	
	RelicData data = GameData::Get().Relic[relicToGet];

	if (data.Imbues == "Tower")
	{
		TowerRelics[relicToGet]++;
		for (Tower* tower : towers)
		{
			tower->ApplyRelicEffect(data.Effect);
		}
	}
	else
	{
		ProjectileRelics[relicToGet]++;
	}
}