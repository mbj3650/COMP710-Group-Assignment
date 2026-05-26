// COMP710 GP Framework 2025
// This include:
#include "SceneGame.h"
// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "AnimatedSprite.h"
#include "Pathmaker.h"
#include "Tile.h"
#include "Enemy.h"
#include "DynamicText.h"
// Library includes:
#include <cassert>
#include "lib/imgui/imgui.h"
#include <time.h>
#include "inlinehelpers.h"
#include "Tilelist.h"
#include <iostream>
#include <string>

// each wave will spawn this many enemies per wave level
// so wave 1 = 3 enemy, wave 2 = 6 enemy, and so on
const int ENEMIES_PER_WAVE = 3;

// how many second between each enemy spawn
const float SPAWN_INTERVAL = 1.5f;

// use windows system font because we dont have font file in assets folder
// if not work, change to any ttf path on your computer
const char* FONT_PATH = "C:\\Windows\\Fonts\\arial.ttf";

SceneGame::SceneGame()
	: m_pCentre(0)
	, m_angle(0.0f)
	, m_rotationSpeed(1.0f)
{
	m_pRenderer = 0;
	m_fSpawnTimer = 0.0f;
	m_fTileSize = 40.0f;

	// player start with 20 lives
	m_iLives = 20;

	// wave start from 1
	m_iWave = 1;
	m_iEnemiesToSpawn = ENEMIES_PER_WAVE;
	m_bWaveComplete = false;

	// set to null first, will create in Initialise
	m_pLivesText = 0;
	m_pWaveText = 0;
}

SceneGame::~SceneGame()
{
	delete list;
	delete m_pCentre;
	delete pathmaker;
	m_pCentre = 0;

	// delete all enemy that still alive on screen
	for (int i = 0; i < (int)m_enemies.size(); i++)
	{
		delete m_enemies[i];
		m_enemies[i] = 0;
	}
	m_enemies.clear();

	// also delete the HUD text object
	delete m_pLivesText;
	m_pLivesText = 0;

	delete m_pWaveText;
	m_pWaveText = 0;
}

bool SceneGame::Initialise(Renderer& renderer)
{
	srand(time(NULL));
	const int SCREEN_WIDTH = renderer.GetWidth();
	const int SCREEN_HEIGHT = renderer.GetHeight();
	list = new Tilelist();
	pathmaker = new Pathmaker();
	moving = true;
	columns = SCREEN_WIDTH / 40;
	rows = SCREEN_HEIGHT / 40;
	list->Initialise(renderer, rows, columns);
	pathmaker->Initialise(renderer, list->Startpos);

	// save renderer pointer so we can use it later when spawn enemy inside Process
	m_pRenderer = &renderer;
	m_fTileSize = (float)SCREEN_WIDTH / (float)columns;

	// create the lives text, put it at top left corner of screen
	// centered = false mean position is anchor to top left of the text
	m_pLivesText = new DynamicText();
	m_pLivesText->Initialise(renderer, FONT_PATH, 24, false);
	m_pLivesText->SetPosition(16, 16);
	m_pLivesText->SetText(renderer, "Lives: 20");

	// wave text go just below the lives text
	m_pWaveText = new DynamicText();
	m_pWaveText->Initialise(renderer, FONT_PATH, 24, false);
	m_pWaveText->SetPosition(16, 48);
	m_pWaveText->SetText(renderer, "Wave: 1");

	return true;
}

void
SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{
	list->Process(deltaTime);

	if (moving)
	{
		// player is still drawing the path, handle movement input
		if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED)
		{
			std::cout << "w\n";
			MovePosition(-1, 0);
		}
		else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED)
		{
			std::cout << "s\n";
			MovePosition(1, 0);
		}
		else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED)
		{
			std::cout << "a\n";
			MovePosition(0, -1);
		}
		else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED)
		{
			std::cout << "d\n";
			MovePosition(0, 1);
		}

		// B key for undo last step
		if (inputSystem.GetKeyState(SDL_SCANCODE_B) == BS_PRESSED)
		{
			std::cout << "undo\n";
			Vector2 pos = list->Undo();
			// bug fix: before this was x = pos.y and y = pos.x, it was wrong
			pathmaker->pos.x = pos.x;
			pathmaker->pos.y = pos.y;
		}
	}
	else
	{
		// path drawing is finish, now run the enemy wave logic
		m_fSpawnTimer += deltaTime;

		// if still have enemy to spawn this wave and timer is ready, spawn one
		if (m_iEnemiesToSpawn > 0 && m_fSpawnTimer >= SPAWN_INTERVAL && m_pRenderer != 0)
		{
			m_fSpawnTimer = 0.0f;

			Enemy* newEnemy = new Enemy();
			newEnemy->Initialise(*m_pRenderer, list->GetStart(), m_fTileSize);
			m_enemies.push_back(newEnemy);

			m_iEnemiesToSpawn--;
			std::cout << "spawned enemy, " << m_iEnemiesToSpawn << " left this wave\n";
		}

		// update all enemy, if enemy reach end then remove it and minus lives
		for (int i = (int)m_enemies.size() - 1; i >= 0; i--)
		{
			m_enemies[i]->Process(deltaTime);

			if (m_enemies[i]->HasReachedEnd())
			{
				delete m_enemies[i];
				m_enemies.erase(m_enemies.begin() + i);
				m_iLives--;
				std::cout << "enemy got through! lives left: " << m_iLives << "\n";

				// refresh the lives text on screen with new number
				m_pLivesText->SetText(*m_pRenderer, "Lives: " + std::to_string(m_iLives));
			}
		}

		// check if current wave is finish
		// wave is finish when no more enemy to spawn AND no enemy left on screen
		if (m_iEnemiesToSpawn == 0 && m_enemies.empty())
		{
			m_iWave++;
			m_iEnemiesToSpawn = m_iWave * ENEMIES_PER_WAVE;
			m_fSpawnTimer = 0.0f;

			std::cout << "wave " << m_iWave << " starting! enemies this wave: " << m_iEnemiesToSpawn << "\n";

			// update wave number on screen
			m_pWaveText->SetText(*m_pRenderer, "Wave: " + std::to_string(m_iWave));
		}
	}
}

bool SceneGame::MovePosition(int xoffset, int yoffset)
{
	Vector2 position = pathmaker->pos;

	// check boundary, make sure player not go outside the grid
	// bug fix: rows and columns was swap here before, cause player can walk off screen
	if ((position.x + xoffset >= columns)
	||  (position.x + xoffset < 0)
	||  (position.y + yoffset < 0)
	||  (position.y + yoffset >= rows))
	{
		return false;
	}

	// also check if that tile is already part of path, cannot go back there
	if (list->GetTile({ pathmaker->pos.x + xoffset, pathmaker->pos.y + yoffset })->isPath)
	{
		return false;
	}
	else
	{
		// move is valid, update the tile link and mark as path
		Tile* CurrentTile = list->GetTile(pathmaker->pos);
		pathmaker->pos.x += xoffset;
		pathmaker->pos.y += yoffset;
		Tile* NextTile = list->GetTile(pathmaker->pos);
		CurrentTile->setNext(NextTile);
		NextTile->setPrevious(CurrentTile);
		NextTile->setPath();
		list->path.push_back(NextTile);

		// if player reach the end tile, stop movement
		if (list->isEnd(pathmaker->pos))
		{
			moving = false;
		}
		return true;
	}
}

void
SceneGame::Draw(Renderer& renderer)
{
	// draw tile grid first
	list->Draw(renderer);

	// draw enemy on top of tile
	for (int i = 0; i < (int)m_enemies.size(); i++)
	{
		m_enemies[i]->Draw(renderer);
	}

	// draw HUD text last so it appear on top of everything
	// only show after path is done and wave start
	if (!moving)
	{
		m_pLivesText->Draw(renderer);
		m_pWaveText->Draw(renderer);
	}
}

void SceneGame::DebugDraw()
{
	ImGui::Text("Scene: Grid");
	ImGui::InputFloat("Rotation speed", &m_rotationSpeed);
	ImGui::SliderInt("Start row", &x, 0, rows - 1, "%d");
	ImGui::SliderInt("Start column", &y, 0, columns - 1, "%d");
	list->GetTile(x, y)->isPath = true;

	// show game state info in debug window
	ImGui::Text("Lives: %d", m_iLives);
	ImGui::Text("Wave: %d", m_iWave);
	ImGui::Text("Enemies to spawn: %d", m_iEnemiesToSpawn);
	ImGui::Text("Enemies on screen: %d", (int)m_enemies.size());
}
