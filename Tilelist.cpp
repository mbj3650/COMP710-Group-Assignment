// COMP710 JESSE
// This include:
#include "Tilelist.h"

// Local includes:
#include "renderer.h"
#include "Tile.h"
#include "vector2.h"
// Library includes:
#include <cassert>
#include "inlinehelpers.h"
#include "inputsystem.h"
#include "lib/imgui/imgui.h"
Tilelist::Tilelist()
{

};
Tilelist::~Tilelist()
{
	for (int k = 0; k < tiles.size(); ++k)
	{
		delete tiles.at(k);
		tiles.at(k) = 0;
	}
};

bool
Tilelist::Initialise(Renderer& renderer, int rows, int columns)
{
	this->rows = rows;
	this->columns = columns;
	bool setStart = false;
	bool setEnd = false;

	for (int c = 0; c < columns; c++) {//make 40x40 grid
		for (int r = 0; r < rows; r++) {
			Tile* NewTile = new Tile();
			NewTile->Initialise(renderer, { c * 1.0f,r * 1.0f },rows,columns);
			tiles.push_back(NewTile);
		}
	}
	Startpos = { (GetRandomf(4, rows-2)), (GetRandomf(2, 4)) };//set starting tile via random
	Endpos = { (GetRandomf(4, rows-2)), (GetRandomf(2, 4)) };//set end tile via random
	if (Endpos.y >= columns) {//make sure its in bounds
		Endpos.y = columns-1;

	}
	else if (Endpos.y < 0) {//read above
		Endpos.y = 0;
	}
	GetTile(Startpos)->setStart();//set start
	GetTile(Endpos)->setEnd();//set end

	path.push_back(GetStart());//add as start node to list
	return true;
};

void
Tilelist::Process(float deltaTime, InputSystem& input)
{
	Vector2 mousepos = input.GetMousePosition();
	tilehover = {  mousepos.y / tiles.at(0)->GetWidth(),mousepos.x / tiles.at(0)->GetWidth() };
	if(Hovered == NULL){//if not hovering on anything
		Hovered = GetTile(tilehover);//set first hovered tile to be hovered
	}
	else if (GetTile(tilehover) != NULL && GetTile(tilehover) != Hovered) {//if found new tile thats being hovered over
		Hovered->hovered = false;//set previous path to no longer be hovered
		Hovered = GetTile(tilehover);//get the new tile
		Hovered->hovered = true;//set new tile to be hovered
	}
	for (int k = 0; k < tiles.size(); ++k)
	{
		tiles.at(k)->Process(deltaTime);
	}
};
void
Tilelist::Draw(Renderer& renderer)
{
	for (int k = 0; k < tiles.size(); ++k)
	{
		tiles.at(k)->Draw(renderer);
	}
};

Tile* Tilelist::GetTile(Vector2 Pos) {
	int x = Pos.x;
	int y = Pos.y;
	try {
		return tiles.at((x)+(y * rows));
	}
	catch (...) {
		return NULL;
	}
}


Tile* Tilelist::GetTile( int x,int y ) {
	try{
		return tiles.at((x)+(y * rows));
	}
	catch (...) {
		return NULL;
	}
	
}

Vector2 Tilelist::Undo() {
	if (path.size() > 1) {//ONLY undo if path has more than 1 tile
		path.pop_back();//remove last path added
		path.at(path.size() - 1)->Undo();//remove end knowledge from new end path
	}

	return path.at(path.size() - 1)->Position;
}

bool Tilelist::isEnd(Vector2 Position) {//check if player has reached the end
	if (Endpos.x == Position.x) {
		if (Endpos.y == Position.y) {
			return true;
		}
	}
	return false;
}


Tile* Tilelist::GetStart() {//get start tile
	return GetTile(Startpos);
}

void Tilelist::DebugDraw() {
	ImGui::Text("Tile Hovered pos: %d,%d", (int)tilehover.x, (int)tilehover.y);
}