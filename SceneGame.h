// COMP710 GP Framework 2022
#ifndef SCENEGAME_H
#define SCENEGAME_H
// Local includes:
#include "scene.h"
#include <vector>
#include "box2d.h"
// Forward declarations:
class Renderer;
class Sprite;
class AnimatedSprite;
class Tile;
class Pathmaker;
class Tilelist;
class Enemy;
struct b2WorldDef;
struct b2WorldId;
class DynamicText;  // for the HUD labels

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
	Renderer* m_pRenderer;
	std::vector<Enemy*> m_enemies;
	float m_fSpawnTimer;
	float m_fTileSize;
	int m_iLives;

	// wave tracking
	int m_iWave;              // which wave we're on
	int m_iEnemiesToSpawn;    // how many left to spawn this wave
	bool m_bWaveComplete;     // waiting between waves

	// HUD text labels
	DynamicText* m_pLivesText;
	DynamicText* m_pWaveText;


	//box2d world variables
	int ScenesubStepCount;//how many times we check for collisions basically
	b2WorldDef* World;//the worlddef that holds its attributes
	b2WorldId WorldPointer;//the world pointer for us to access the world

private:
};
#endif // SCENEGAME_H
