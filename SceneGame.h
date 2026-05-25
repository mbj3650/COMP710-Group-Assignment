// COMP710 GP Framework 2022
#ifndef SCENEGAME_H
#define SCENEGAME_H
// Local includes:
#include "scene.h"
#include <vector>

// Forward declarations:
class Renderer;
class Sprite;
class AnimatedSprite;
class Tile;
class Pathmaker;
class Tilelist;
class Enemy;  // added for enemy wave stuff

// Class declaration:
class SceneGame : public Scene
{
	// Member methods:
public:
	SceneGame();
	virtual ~SceneGame();
	virtual bool Initialise(Renderer& renderer);
	void Move(SDL_Scancode key);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	bool MovePosition(int xoffset, int yoffset);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();
protected:
private:
	SceneGame(const SceneGame& SceneGame);
	SceneGame& operator=(const SceneGame& SceneGame);
	// Member data:
public:
protected:
	int x = 0;
	int y = 0;
	int rows;
	int columns;
	Tilelist* list;
	Pathmaker* pathmaker;
	std::vector<Tile*> Path;
	AnimatedSprite* m_pCentre;
	float m_angle;
	float m_rotationSpeed;
	bool moving;

	// enemy / wave stuff
	Renderer* m_pRenderer;          // need this to spawn enemies inside Process
	std::vector<Enemy*> m_enemies;
	float m_fSpawnTimer;            // counts up, spawn when it hits the interval
	float m_fTileSize;              // pixel size of one tile, set in Initialise
	int m_iLives;                   // player loses a life each time an enemy gets through

private:
};
#endif // SCENEGAME_H
