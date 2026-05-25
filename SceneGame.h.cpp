// COMP710 GP Framework 2022
#ifndef SCENEGAME_H
#define SCENEGAME_H

// Local includes:
#include "scene.h"
#include "Enemy.h"
#include <vector>

// Forward declarations:
class Renderer;
class Sprite;
class AnimatedSprite;
class Tile;
class Pathmaker;
class Tilelist;

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
	std::vector<Tile*> Path;        // stores tiles in the order the player drew them
	AnimatedSprite* m_pCentre;
	float m_angle;
	float m_rotationSpeed;

	
	Renderer* m_pRenderer;

	// --- new stuff added for enemy/wave system ---
	std::vector<Enemy*> m_enemies;  // all enemies currently on the map
	float m_fSpawnTimer;            // counts up, spawns enemy when it hits interval
	float m_fSpawnInterval;         // seconds between enemy spawns
	float m_fPlayerHP;              // player starts at 100 hp
	bool m_bPathLocked;             // true once the player presses ENTER to confirm the path
	int m_iWave;                    // which wave we are on
	int m_iEnemiesSpawnedThisWave;  // how many enemies have spawned this wave
	int m_iEnemiesPerWave;          // how many enemies per wave (increases each wave)

private:
};
#endif // SCENEGAME_H