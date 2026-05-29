// COMP710 JESSE
#include <vector>
#ifndef __TILELIST_H__
#define __TILELIST_H__
#include "vector2.h"
// Forward declarations:
class Renderer;
class Tile;
class InputSystem;
// Class declaration:
class Tilelist
{
	// Member methods:
public:
	Tilelist();
	~Tilelist();

	bool Initialise(Renderer& renderer, int rows, int columns);
	void Process(float deltaTime, InputSystem& input);
	void Draw(Renderer& renderer);
	void setStart();//sets tile to be "start"
	void setEnd();//sets tlie to be "end"
	Tile* GetTile(Vector2 Pos);
	Tile* GetTile(int x, int y);
	Vector2 Undo();
	bool isEnd(Vector2 Position);
	Tile* GetStart();
	Tile* GetHovered();
	void DebugDraw();
protected:

private:
	Tilelist(const Tilelist& Tilelist);
	Tilelist& operator=(const Tilelist& Tilelist);

	// Member data:
public:
	std::vector<Tile*> tiles;
	std::vector<Tile*> path;
	Tile* Hovered;
	Vector2 tilehover;
	Vector2 Startpos;
	Vector2 Endpos;
	int rows;
	int columns;
protected:

private:

};

#endif // __TILELIST_H__
