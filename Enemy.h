// Enemy.h
// Handles an enemy that walks along the path the player drew
// Uses the tile linked list (NextPosition) to figure out where to go

#ifndef ENEMY_H
#define ENEMY_H
#include <box2d.h>
class Renderer;
class Sprite;
class Tile;
struct b2WorldId;
struct b2ShapeId;
struct b2BodyId;
class Enemy
{
public:
	Enemy();
	~Enemy();


	// pass in the start tile and how big each tile is in pixels, as well as the WorldID for box2d implementation
	bool Initialise(Renderer& renderer, Tile* startTile, float tileSize, b2WorldId WorldID);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	bool HasReachedEnd() { return m_bReachedEnd; }

private:
	Enemy(const Enemy&);
	Enemy& operator=(const Enemy&);

	Sprite* m_pSprite;
	Tile* m_pCurrentTile;  // which tile the enemy is currently moving toward

	b2BodyId ID;//its hitbox
	b2ShapeId shapeId;//its shapeid (this is for if we want to change friction, density or what it can collide with on the fly)
	// screen position (not grid position)
	float m_x;
	float m_y;

	float m_speed;     // pixels per second
	float m_tileSize;  // needed to convert grid coords to screen coords

	bool m_bReachedEnd;
};

#endif // ENEMY_H
