// COMP710 GP Framework 2025
#ifndef SCENEGAME_H
#define SCENEGAME_H

// Local includes:
#include "scene.h"
#include <vector>
#include <memory>

// Forward declarations:
class Renderer;
class Sprite;
class AnimatedSprite;
class Tile;
class Pathmaker;
class Tilelist;
class Enemy;
class ProjectilePool;
class InputSystem;

// Class declaration:
class SceneGame : public Scene
{
	// Member methods:
public:
	SceneGame();
	virtual ~SceneGame();

	virtual bool Initialise(Renderer& renderer) override;
	void Move(SDL_Scancode key);
	virtual void Process(float deltaTime, InputSystem& inputSystem) override;
	virtual void Draw(Renderer& renderer) override;
	virtual void DebugDraw() override;

	// Martin's High-Performance Reset Link
	void Reset(Renderer& renderer);

protected:
private:
	SceneGame(const SceneGame& SceneGame) = delete;
	SceneGame& operator=(const SceneGame& SceneGame) = delete;

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

private:
	// Martin's Core Backend Modules
	std::vector<std::unique_ptr<Enemy>> m_enemyPool;
	ProjectilePool* m_pProjectilePool = nullptr;

	// Player State System
	int m_iPlayerHealth = 100;
	int m_iPlayerGold = 0;

	// Cached Renderer reference for re-initialization
	Renderer* m_pCachedRenderer = nullptr;
};

#endif // SCENEGAME_H