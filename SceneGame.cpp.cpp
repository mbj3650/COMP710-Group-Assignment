// COMP710 GP Framework 2025
// This include:
#include "SceneGame.h"
// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "AnimatedSprite.h"
#include "Pathmaker.h"
#include "Tile.h"
// Library includes:
#include <cassert>
#include "lib/imgui/imgui.h"
#include <time.h>
#include "inlinehelpers.h"
#include "Tilelist.h"
#include <iostream>

SceneGame::SceneGame()
	: m_pCentre(0)
	, m_angle(0.0f)
	, m_rotationSpeed(1.0f)
	, m_fSpawnTimer(0.0f)
	, m_fSpawnInterval(3.0f)   // one enemy every 3 seconds
	, m_fPlayerHP(100.0f)
	, m_bPathLocked(false)
	, m_iWave(1)
	, m_iEnemiesSpawnedThisWave(0)
	, m_iEnemiesPerWave(5)     // wave 1 has 5 enemies
	, m_pRenderer(0)
{
}

SceneGame::~SceneGame()
{
	delete list;
	delete m_pCentre;
	delete pathmaker;
	m_pCentre = 0;

	// clean up all enemies
	for (int i = 0; i < (int)m_enemies.size(); i++)
	{
		delete m_enemies[i];
		m_enemies[i] = 0;
	}
	m_enemies.clear();
}

bool SceneGame::Initialise(Renderer& renderer)
{
	srand(time(NULL));
	const int SCREEN_WIDTH = renderer.GetWidth();
	const int SCREEN_HEIGHT = renderer.GetHeight();

	list = new Tilelist();
	pathmaker = new Pathmaker();

	columns = SCREEN_WIDTH / 40;
	rows = SCREEN_HEIGHT / 40;

	list->Initialise(renderer, rows, columns);
	pathmaker->Initialise(renderer, list->Startpos);

	// add the start tile as the first point in the path
	// so enemies always begin at the correct position
	Path.push_back(list->GetTile(list->Startpos));
	list->GetTile(list->Startpos)->isPath = true;

	
	m_pRenderer = &renderer;

	return true;
}

void
SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{
	list->Process(deltaTime);

	// -------------------------------------------------------
	// PHASE 1: drawing the path (only works before path locked)
	// -------------------------------------------------------
	if (!m_bPathLocked)
	{
		// MOVEMENT FOR MAKING PATHS
		if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED)
		{
			std::cout << "w\n";
			if (list->GetTile({ pathmaker->pos.y, pathmaker->pos.x - 1 }) != NULL)
			{
				pathmaker->pos.x -= 1;
				Tile* newTile = list->GetTile(pathmaker->pos);
				newTile->isPath = true;

				// record this tile in the path as long as its not already in there
				// (stops player from revisiting the same tile and creating a loop)
				bool alreadyInPath = false;
				for (int i = 0; i < (int)Path.size(); i++)
				{
					if (Path[i] == newTile)
					{
						alreadyInPath = true;
						break;
					}
				}
				if (!alreadyInPath)
				{
					Path.push_back(newTile);
				}
			}
		}
		else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED)
		{
			std::cout << "s\n";
			if (list->GetTile({ pathmaker->pos.y, pathmaker->pos.x + 1 }) != NULL)
			{
				pathmaker->pos.x += 1;
				Tile* newTile = list->GetTile(pathmaker->pos);
				newTile->isPath = true;

				bool alreadyInPath = false;
				for (int i = 0; i < (int)Path.size(); i++)
				{
					if (Path[i] == newTile)
					{
						alreadyInPath = true;
						break;
					}
				}
				if (!alreadyInPath)
				{
					Path.push_back(newTile);
				}
			}
		}
		else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED)
		{
			std::cout << "a\n";
			if (list->GetTile({ pathmaker->pos.y - 1, pathmaker->pos.x }) != NULL)
			{
				pathmaker->pos.y -= 1;
				Tile* newTile = list->GetTile(pathmaker->pos);
				newTile->isPath = true;

				bool alreadyInPath = false;
				for (int i = 0; i < (int)Path.size(); i++)
				{
					if (Path[i] == newTile)
					{
						alreadyInPath = true;
						break;
					}
				}
				if (!alreadyInPath)
				{
					Path.push_back(newTile);
				}
			}
		}
		else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED)
		{
			std::cout << "d\n";
			if (list->GetTile({ pathmaker->pos.y + 1, pathmaker->pos.x }) != NULL)
			{
				pathmaker->pos.y += 1;
				Tile* newTile = list->GetTile(pathmaker->pos);
				newTile->isPath = true;

				bool alreadyInPath = false;
				for (int i = 0; i < (int)Path.size(); i++)
				{
					if (Path[i] == newTile)
					{
						alreadyInPath = true;
						break;
					}
				}
				if (!alreadyInPath)
				{
					Path.push_back(newTile);
				}
			}
		}

		// press ENTER to lock the path and start the waves
		// need at least 2 tiles in the path otherwise enemies have nowhere to go
		if (inputSystem.GetKeyState(SDL_SCANCODE_RETURN) == BS_PRESSED)
		{
			if ((int)Path.size() >= 2)
			{
				m_bPathLocked = true;
				// also copy the path into pathmaker so other systems can use it
				pathmaker->Path = Path;
				std::cout << "Path locked! Total tiles: " << Path.size() << "\n";
				std::cout << "Wave " << m_iWave << " starting! Enemies this wave: " << m_iEnemiesPerWave << "\n";
			}
			else
			{
				std::cout << "Path too short! Draw more tiles first.\n";
			}
		}
	}

	// -------------------------------------------------------
	// PHASE 2: wave/enemy logic (only runs after path is locked)
	// -------------------------------------------------------
	if (m_bPathLocked)
	{
		// count up the spawn timer
		m_fSpawnTimer += deltaTime;

		// spawn a new enemy if enough time has passed and we havent hit the wave limit yet
		if (m_fSpawnTimer >= m_fSpawnInterval && m_iEnemiesSpawnedThisWave < m_iEnemiesPerWave)
		{
			m_fSpawnTimer = 0.0f;

			// boss every 10th wave - bigger hp, slower speed
			float hp = 50.0f + (m_iWave * 10.0f); // enemies get tougher each wave
			float speed = 80.0f;
			bool isBoss = (m_iWave % 10 == 0);
			if (isBoss)
			{
				hp = hp * 3.0f;
				speed = 50.0f;
				std::cout << "BOSS spawned! Wave " << m_iWave << "\n";
			}

			
			Enemy* newEnemy = new Enemy();
			newEnemy->Initialise(*m_pRenderer, Path, hp, speed, 40.0f);
			m_enemies.push_back(newEnemy);

			m_iEnemiesSpawnedThisWave++;
			std::cout << "Spawned enemy " << m_iEnemiesSpawnedThisWave
				<< "/" << m_iEnemiesPerWave << " (Wave " << m_iWave << ")\n";
		}

		// update all enemies
		for (int i = 0; i < (int)m_enemies.size(); i++)
		{
			m_enemies[i]->Process(deltaTime);

			// enemy reached the end - deal remaining hp as damage to player
			if (m_enemies[i]->HasReachedEnd())
			{
				float damage = m_enemies[i]->GetCurrentHP();
				m_fPlayerHP -= damage;
				std::cout << "Player took " << damage << " damage! HP now: " << m_fPlayerHP << "\n";

				// remove this enemy from the list
				delete m_enemies[i];
				m_enemies[i] = 0;
				m_enemies.erase(m_enemies.begin() + i);
				i--; // step back so we dont skip the next enemy
				continue;
			}

			// enemy died (killed by a tower)
			if (!m_enemies[i]->IsAlive())
			{
				// TODO: give player gold here
				delete m_enemies[i];
				m_enemies[i] = 0;
				m_enemies.erase(m_enemies.begin() + i);
				i--;
				continue;
			}
		}

		// check if the wave is done
		// wave is done when all enemies are spawned and none are left on the map
		if (m_iEnemiesSpawnedThisWave >= m_iEnemiesPerWave && m_enemies.empty())
		{
			// start the next wave
			m_iWave++;
			m_iEnemiesSpawnedThisWave = 0;
			m_iEnemiesPerWave = m_iEnemiesPerWave + 2; // each wave adds 2 more enemies
			m_fSpawnTimer = 0.0f;
			std::cout << "Wave " << m_iWave << " starting! Enemies: " << m_iEnemiesPerWave << "\n";
		}

		// check game over
		if (m_fPlayerHP <= 0.0f)
		{
			m_fPlayerHP = 0.0f;
			std::cout << "GAME OVER! Player HP hit 0.\n";
			// TODO: switch to a game over scene
		}
	}
}

void
SceneGame::Draw(Renderer& renderer)
{
	list->Draw(renderer);

	// draw all enemies
	for (int i = 0; i < (int)m_enemies.size(); i++)
	{
		m_enemies[i]->Draw(renderer);
	}
}

void SceneGame::DebugDraw()
{
	ImGui::Text("Scene: Grid");
	ImGui::InputFloat("Rotation speed", &m_rotationSpeed);
	ImGui::SliderInt("Start row", &x, 0, rows - 1, "%d");
	ImGui::SliderInt("Start column", &y, 0, columns - 1, "%d");
	list->GetTile(x, y)->isPath = true;

	// show game state in debug window
	ImGui::Separator();
	ImGui::Text("Player HP: %.1f", m_fPlayerHP);
	ImGui::Text("Wave: %d", m_iWave);
	ImGui::Text("Path locked: %s", m_bPathLocked ? "YES" : "NO - press ENTER");
	ImGui::Text("Path tiles: %d", (int)Path.size());
	ImGui::Text("Enemies alive: %d", (int)m_enemies.size());
	ImGui::Text("Enemies spawned this wave: %d / %d", m_iEnemiesSpawnedThisWave, m_iEnemiesPerWave);
}