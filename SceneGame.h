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
class Enemy;
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

private:
};
#endif // SCENEGAME_H
