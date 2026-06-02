#include "GameData.h"
#include "ProjectileData.h"
#include "TowerData.h"
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
		Parser.GetValueAsFloat(section + "|Speed")
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
		Parser.GetValueAsInt(section + "|Lifetime")
		)});
	}
}