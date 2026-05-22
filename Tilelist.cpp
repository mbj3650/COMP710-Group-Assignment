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
	Startpos = { (GetRandomf(0, rows)),(GetRandomf(2, 4)) };//set starting tile via random
	Endpos = {  (GetRandomf(0, rows)),columns -4 +  (GetRandomf(0,3)) };//set end tile via random
	if (Endpos.y >= columns) {//make sure its in bounds
		Endpos.y = columns-1;

	}
	else if (Endpos.y < 0) {//read above
		Endpos.y = 0;
	}
	GetTile(Startpos)->setStart();//set start
	GetTile(Endpos)->setEnd();//set end
	return true;
};

void
Tilelist::Process(float deltaTime)
{
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

Tile* Tilelist::GetStart() {
	return GetTile(Startpos);
}