// COMP710 JESSE
#ifndef __TILE_H__
#define __TILE_H__

// Forward declarations:
class Renderer;
class Sprite;
#include "vector2.h"
// Class declaration:
class Tile
{
	// Member methods:
public:
	Tile();
	~Tile();

	bool Initialise(Renderer& renderer, Vector2 Pos, int rows, int columns);
	void Process(float deltaTime);
	void UpdateDraw();//manually called to update a tile to change its appearance 
	void Draw(Renderer& renderer);
	void setStart();//set as start
	void setEnd();
	void setNext(Tile* NextPosition);
	void setPath();
	void setPrevious(Tile* PrevPosition);
	void Undo();
	//set as end
protected:

private:
	Tile(const Tile& Tile);
	Tile& operator=(const Tile& Tile);

	// Member data:
public:
	Vector2 Position;
	Sprite* m_pSprite;
	bool isPath;
	bool isStart;
	bool isEnd;
	int Connections;//if has more than 2 connections, something has gone wrong and we must not allow the player to continue as long as theres 3+ connections 
	Tile* PrevPosition;
	Tile* NextPosition;
protected:

private:

};

#endif // __TILE_H__
