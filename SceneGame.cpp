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
	moving = true;
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

bool SceneGame::MovePosition(int xoffset, int yoffset) {//CHECKS IF GIVEN POSITION (WHEN OFFSET) IS VALID
	Vector2 position = pathmaker->pos;
	if ((position.x+ xoffset >= rows) //theres probably a more efficient way to do this 
	|| (position.x + xoffset < 0)//but this is pretty succinct so i think its ok
	|| (position.y + yoffset < 0)
	|| (position.y + yoffset >= columns))
	{
		return false;
	}
	if (list->GetTile({ pathmaker->pos.x + xoffset,pathmaker->pos.y + yoffset })->isPath) {//check if not already a path
		return false;
	}
	else {
		Tile* CurrentTile = list->GetTile(pathmaker->pos);//get current tile
		pathmaker->pos.x += xoffset;//MAKE MOVEMENT
		pathmaker->pos.y += yoffset;
		Tile* NextTile = list->GetTile(pathmaker->pos);//get tile player will go to 
		CurrentTile->setNext(NextTile);//set current tile's next tile to be the new position's tile
		NextTile->setPrevious(CurrentTile); //set new tile's previous tile to be this one
		NextTile->setPath();
		list->path.push_back(NextTile);
		if (list->isEnd(pathmaker->pos)) {
			moving = false;
		}
		return true;
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