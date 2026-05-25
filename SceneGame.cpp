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
{
	m_pRenderer = 0;
	m_fSpawnTimer = 0.0f;
	m_fTileSize = 40.0f;
	m_iLives = 20;
}

SceneGame::~SceneGame()
{
	delete list;
	delete m_pCentre;
	delete pathmaker;
	m_pCentre = 0;

	// clean up any enemies still on screen
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
	moving = true;
	columns = SCREEN_WIDTH / 40;
	rows = SCREEN_HEIGHT / 40;
	list->Initialise(renderer, rows, columns); // this holds all the tiles that the map needs, including start and end
	pathmaker->Initialise(renderer, list->Startpos); // this guy tracks the position of where the "player" is in terms of grid

	// store renderer so we can spawn enemies later in Process
	m_pRenderer = &renderer;
	// work out the actual pixel width of a tile
	m_fTileSize = (float)SCREEN_WIDTH / (float)columns;

	return true;
}

void
SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{
	list->Process(deltaTime); // process all tiles in lists in case they need updates or whatnot

	if (moving)
	{
		// MOVEMENT FOR MAKING PATHS
		if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) // CHECK IF KEY PRESSED
		{
			std::cout << "w\n"; // DEBUG OUTPUT
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

		if (inputSystem.GetKeyState(SDL_SCANCODE_B) == BS_PRESSED)
		{
			std::cout << "undo\n";
			Vector2 pos = list->Undo(); // set pathmaker to previous position
			// fixed: these were swapped before which caused the undo to jump to the wrong tile
			pathmaker->pos.x = pos.x;
			pathmaker->pos.y = pos.y;
		}
	}
	else
	{
		// path is done, start spawning enemies
		// each enemy follows the path using the tile linked list

		m_fSpawnTimer += deltaTime;

		// spawn a new enemy every 1.5 seconds
		if (m_fSpawnTimer >= 1.5f && m_pRenderer != 0)
		{
			m_fSpawnTimer = 0.0f;

			Enemy* newEnemy = new Enemy();
			newEnemy->Initialise(*m_pRenderer, list->GetStart(), m_fTileSize);
			m_enemies.push_back(newEnemy);
		}

		// update all enemies, delete any that have reached the end
		for (int i = (int)m_enemies.size() - 1; i >= 0; i--)
		{
			m_enemies[i]->Process(deltaTime);

			if (m_enemies[i]->HasReachedEnd())
			{
				delete m_enemies[i];
				m_enemies.erase(m_enemies.begin() + i);
				m_iLives--;
				std::cout << "enemy got through! lives left: " << m_iLives << "\n";
				// TODO: game over screen when lives hit 0
			}
	list->Process(deltaTime);//process all tiles in lists in case they need updates or whatnot
	if (moving) {
		//MOVEMENT FOR MAKING PATHS
		if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) {//CHECK IF KEY PRESSED
			std::cout << "w\n";//DEBUG OUTPUT
			(MovePosition(-1, 0));

		}

		//SAME FOR THESE KEYS
		else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED) {
			std::cout << "s\n";
			(MovePosition(1, 0));
		}


		else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED) {
			std::cout << "a\n";
			(MovePosition(0, -1));
		}

		else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED) {
			std::cout << "d\n";
			(MovePosition(0, 1));
		}

		if (inputSystem.GetKeyState(SDL_SCANCODE_B) == BS_PRESSED) {//CHECK IF KEY PRESSED
			std::cout << "w\n";//DEBUG OUTPUT
			Vector2 pos = (list->Undo());//set pathmaker to previous position
			pathmaker->pos.x = pos.y;
			pathmaker->pos.y = pos.x;
		}

	}
}

bool SceneGame::MovePosition(int xoffset, int yoffset) // CHECKS IF GIVEN POSITION (WHEN OFFSET) IS VALID
{
	Vector2 position = pathmaker->pos;

	// fixed: rows and columns were the wrong way around here
	// x is the column direction so it should be bounded by columns, not rows
	if ((position.x + xoffset >= columns)
	||  (position.x + xoffset < 0)
	||  (position.y + yoffset < 0)
	||  (position.y + yoffset >= rows))
	{
		return false;
	}

	if (list->GetTile({ pathmaker->pos.x + xoffset, pathmaker->pos.y + yoffset })->isPath) // check if not already a path
	{
		return false;
	}
	else
	{
		Tile* CurrentTile = list->GetTile(pathmaker->pos); // get current tile
		pathmaker->pos.x += xoffset; // MAKE MOVEMENT
		pathmaker->pos.y += yoffset;
		Tile* NextTile = list->GetTile(pathmaker->pos); // get tile player will go to
		CurrentTile->setNext(NextTile); // set current tile's next tile to be the new position's tile
		NextTile->setPrevious(CurrentTile); // set new tile's previous tile to be this one
		NextTile->setPath();
		list->path.push_back(NextTile);
		if (list->isEnd(pathmaker->pos))
		{
	
}

void
SceneGame::Draw(Renderer& renderer)
{
	list->Draw(renderer);

	// draw enemies on top of the tiles
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

	// show enemy/lives info in the debug window
	ImGui::Text("Lives: %d", m_iLives);
	ImGui::Text("Enemies active: %d", (int)m_enemies.size());
}
