#include "GameData.h"
#include "ProjectileData.h"
#include "TowerData.h"
#include "EnemyData.h"
#include "RelicData.h"
#include "IniParser.h"
#include <map>
#include <string>
#include <iostream>
GameData::GameData()
{
}

GameData::~GameData()
{
	Tower.clear();
	Projectile.clear();
	Enemy.clear();
	Relic.clear();
	RelicIDs.clear();
}

GameData* GameData::sm_pInstance = 0;
GameData& GameData::Get()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new GameData();
	}
	return (*sm_pInstance);
}

void GameData::Destroy()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

void GameData::Initialise()
{
	LoadTowerData();
	LoadProjectileData();
	LoadEnemyData();
	LoadRelicData();
}
void GameData::LoadTowerData()
{
	IniParser Parser;
	Parser.LoadIniFile("..\\assets\\info\\tower.ini");
	for (const string section : Parser.sections)
	{
		Tower.insert({ section, TowerData(
		Parser.GetValueAsString(section + "|Sprite"),
		Parser.GetValueAsInt(section + "|Price"),
		Parser.GetValueAsFloat(section + "|Range"),
		Parser.GetValueAsFloat(section + "|Firerate"),
		Parser.GetValueAsString(section + "|ProjectileID"),
		Parser.GetValueAsFloat(section + "|Speed"),
		Parser.GetValueAsInt(section + "|ID"),
		Parser.GetValueAsString(section + "|Upgrade1"),
		Parser.GetValueAsString(section + "|Upgrade1Desc"),
		Parser.GetValueAsInt(section + "|Upgrade1Price"),
		Parser.GetValueAsString(section + "|Upgrade2"),
		Parser.GetValueAsString(section + "|Upgrade2Desc"),
		Parser.GetValueAsInt(section + "|Upgrade2Price"),
		Parser.GetValueAsString(section + "|Upgrade3"),
		Parser.GetValueAsString(section + "|Upgrade3Desc"),
		Parser.GetValueAsInt(section + "|Upgrade3Price")
		)});
		
	}
}
void GameData::LoadProjectileData()
{
	IniParser Parser;
	Parser.LoadIniFile("..\\assets\\info\\projectile.ini");
	for (const string section : Parser.sections)
	{
		Projectile.insert({ section, ProjectileData(
		Parser.GetValueAsString(section + "|Sprite"),
		Parser.GetValueAsInt(section + "|Damage"),
		Parser.GetValueAsInt(section + "|Pierce"),
		Parser.GetValueAsInt(section + "|Homing"),
		Parser.GetValueAsInt(section + "|Effect"),
		Parser.GetValueAsInt(section + "|Lifetime"),
		Parser.GetValueAsInt(section + "|Size")
		)});
	}
}
void GameData::LoadEnemyData()
{
	IniParser Parser;
	Parser.LoadIniFile("..\\assets\\info\\enemy.ini");
	for (const string section : Parser.sections)
	{
		Enemy.insert({ section, EnemyData(
		Parser.GetValueAsString(section + "|Sprite"),
		Parser.GetValueAsInt(section + "|Price"),
		Parser.GetValueAsInt(section + "|Damage"),
		Parser.GetValueAsFloat(section + "|Speed"),
		Parser.GetValueAsInt(section + "|BonusHealth"),
		Parser.GetValueAsInt(section + "|ID"),
		Parser.GetValueAsInt(section + "|SpriteSizeX"),
		Parser.GetValueAsInt(section + "|SpriteSizeY")
		)});
	}
}
void GameData::LoadRelicData()
{
	IniParser Parser;
	Parser.LoadIniFile("..\\assets\\info\\relic.ini");
	for (const string section : Parser.sections)
	{
		Relic.insert({ section, RelicData(
		Parser.GetValueAsString(section + "|Sprite"),
		Parser.GetValueAsString(section + "|Name"),
		Parser.GetValueAsString(section + "|Description"),
		Parser.GetValueAsString(section + "|Imbues"),
		Parser.GetValueAsInt(section + "|Effect")
		)});
		RelicIDs.push_back(section);
	}
}