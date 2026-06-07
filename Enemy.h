// Enemy.h
// Handles an enemy that walks along the path the player drew
// Uses the tile linked list (NextPosition) to figure out where to go

#ifndef ENEMY_H
#define ENEMY_H

class Renderer;
class Sprite;
class Tile;

class Enemy
{
public:
	Enemy();
	~Enemy();

	// pass in the start tile and how big each tile is in pixels
	bool Initialise(Renderer& renderer, Tile* startTile, float tileSize);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	bool HasReachedEnd() { return m_bReachedEnd; }

private:
	Enemy(const Enemy&);
	Enemy& operator=(const Enemy&);

	Sprite* m_pSprite;
	Tile* m_pCurrentTile;  // which tile the enemy is currently moving toward

	// screen position (not grid position)
	float m_x;
	float m_y;

	float m_speed;     // pixels per second
	float m_tileSize;  // needed to convert grid coords to screen coords

	bool m_bReachedEnd;
};

#endif // ENEMY_H
