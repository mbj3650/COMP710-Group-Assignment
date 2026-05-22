// COMP710 JESSE
#include <vector>
#ifndef __TILELIST_H__
#define __TILELIST_H__
#include "vector2.h"
// Forward declarations:
class Renderer;
class Tile;
// Class declaration:
class Tilelist
{
	// Member methods:
public:
	Tilelist();
	~Tilelist();

	bool Initialise(Renderer& renderer, int rows, int columns);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void setStart();//sets tile to be "start"
	void setEnd();//sets tlie to be "end"
	Tile* GetTile(Vector2 Pos);

	Tile* GetTile(int x, int y);

	Tile* GetStart();

protected:

private:
	Tilelist(const Tilelist& Tilelist);
	Tilelist& operator=(const Tilelist& Tilelist);

	// Member data:
public:
	std::vector<Tile*> tiles;
	Vector2 Startpos;
	Vector2 Endpos;
	int rows;
	int columns;
protected:

private:

};

#endif // __TILELIST_H__
