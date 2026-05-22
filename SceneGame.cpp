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
{
}
SceneGame::~SceneGame()
{
	delete list;
	delete m_pCentre;
	delete pathmaker;
	m_pCentre = 0;
}
bool SceneGame::Initialise(Renderer& renderer)
{
	srand(time(NULL));
	const int SCREEN_WIDTH = renderer.GetWidth();
	const int SCREEN_HEIGHT = renderer.GetHeight();
	list = new Tilelist();
	pathmaker = new Pathmaker();

	columns = SCREEN_WIDTH /40;
	rows = SCREEN_HEIGHT /40;
	list->Initialise(renderer,rows,columns);//this holds all the tiles that the map needs, including start and end
	pathmaker->Initialise(renderer, list->Startpos);//this guy tracks the position of where the "player" is in terms of grid
	return true;
}

void
SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{

	list->Process(deltaTime);//process all tiles in lists in case they need updates or whatnot

	//MOVEMENT FOR MAKING PATHS
	if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) {//CHECK IF KEY PRESSED
		std::cout << "w\n";//DEBUG OUTPUT
		if (list->GetTile({ pathmaker->pos.y,pathmaker->pos.x-1 }) != NULL) {//IF NOT NULL
			pathmaker->pos.x -= 1;//MAKE MOVEMENT
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}
	
	//SAME FOR THESE KEYS
	else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED) {
		std::cout << "s\n";
		if (list->GetTile({ pathmaker->pos.y,pathmaker->pos.x + 1 }) != NULL) {
			pathmaker->pos.x += 1;
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}
	
	
	else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED) {
		std::cout << "a\n";
		if (list->GetTile({ pathmaker->pos.y - 1 ,pathmaker->pos.x}) != NULL) {
			pathmaker->pos.y -= 1;
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}
	
	else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED) {
		std::cout << "d\n";
		if (list->GetTile({ pathmaker->pos.y + 1 ,pathmaker->pos.x }) != NULL) {
			pathmaker->pos.y += 1;
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}
}
void
SceneGame::Draw(Renderer& renderer)
{
	list->Draw(renderer);
}
void SceneGame::DebugDraw()
{
	ImGui::Text("Scene: Grid");
	ImGui::InputFloat("Rotation speed", &m_rotationSpeed);
	ImGui::SliderInt("Start row", &x, 0, rows - 1, "%d");
	ImGui::SliderInt("Start column", &y, 0, columns-1, "%d");
	list->GetTile( x, y )->isPath = true;
}