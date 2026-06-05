// COMP710 JESSE
// This include:
#include "Tilelist.h"

// Local includes:
#include "renderer.h"
#include "Tile.h"
#include "vector2.h"
#include "sprite.h"
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
	delete Endflag;
	delete Startflag;
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
	// Coordinate convention (UNIFIED): Vector2.x = COLUMN, Vector2.y = ROW.
	// Start sits near the left edge (small column), end near the right edge.
	Startpos = { (GetRandomf(2, 4)),            (GetRandomf(4, rows - 2)) };//set starting tile via random
	Endpos   = { columns - (GetRandomf(2, 4)), (GetRandomf(4, rows - 2)) };//set end tile via random
	if (Endpos.x >= columns) {//make sure its in bounds
		Endpos.x = columns - 1;
	}
	else if (Endpos.x < 0) {//read above
		Endpos.x = 0;
	}
	GetTile(Startpos)->setStart();//set start
	GetTile(Startpos)->isPath = true;
	GetTile(Endpos)->setEnd();//set end

	path.push_back(GetStart());//add as start node to list

	Endflag = renderer.CreateSprite("..\\assets\\flag.png"); // set sprite
	Startflag = renderer.CreateSprite("..\\assets\\flag.png"); // set sprite
	Startflag->SetScale( // set scale to make sure it fits in screen
		renderer.GetWidth() / (Startflag->GetWidth()/2 * (1.0 * columns))
	);

	Startflag->SetX(GetTile(Startpos)->Position.x * Startflag->GetWidth() / 2 + (Startflag->GetWidth() / 4));
	Startflag->SetY(GetTile(Startpos)->Position.y * Startflag->GetWidth() / 2 + (Startflag->GetWidth() /  8));

	Endflag = renderer.CreateSprite("..\\assets\\flag.png"); // set sprite
	Endflag->SetScale( // set scale to make sure it fits in screen
		renderer.GetWidth() / (Endflag->GetWidth() / 2 * (1.0 * columns))
	);

	Endflag->SetX(GetTile(Endpos)->Position.x * Endflag->GetWidth() / 2 + (Endflag->GetWidth() / 4));
	Endflag->SetY(GetTile(Endpos)->Position.y * Endflag->GetWidth() / 2 + (Endflag->GetWidth() / 8));
	return true;
};

void
Tilelist::Process(float deltaTime, InputSystem& input)
{
	Vector2 mousepos = input.GetMousePosition();
	// x = COLUMN (from mouse X), y = ROW (from mouse Y)
	tilehover = { mousepos.x / tiles.at(0)->GetWidth(), mousepos.y / tiles.at(0)->GetWidth() };
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
	for (int k = 0; k < path.size(); ++k)
	{
		path.at(k)->Draw(renderer);
	}
	Startflag->Draw(renderer);
	Endflag->Draw(renderer);

};

Tile* Tilelist::GetTile(Vector2 Pos) {
	int x = Pos.x; // column
	int y = Pos.y; // row
	try {
		// tiles are stored column-major: index = column * rows + row
		return tiles.at((x * rows) + (y));
	}
	catch (...) {
		return NULL;
	}
}


Tile* Tilelist::GetTile( int x,int y ) { // x = column, y = row
	try{
		return tiles.at((x * rows) + (y));
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