// COMP710 GP Framework 2025
// SceneGame.h
// Modified by: MartinYan12138y
// Changes: Added game over state, restart system, and game over screen

#ifndef __SCENEMAIN_H__
#define __SCENEMAIN_H__

// Parent include:
#include "scene.h"

// Forward declarations:
class Renderer;
class Sprite;
class Tilelist;
class Pathmaker;
class Enemy;
class DynamicText;

class SceneGame : public Scene
{
    // Member methods:
public:
    SceneGame();
    virtual ~SceneGame();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime, InputSystem& inputSystem);
    virtual void Draw(Renderer& renderer);
    virtual void DebugDraw();

    bool MovePosition(int xoffset, int yoffset);

    // Resets the entire game state for a fresh start
    void RestartGame(Renderer& renderer);

protected:
private:
    SceneGame(const SceneGame& sceneMain);
    SceneGame& operator=(const SceneGame& sceneMain);

    // Member data:
public:
protected:
    Sprite* m_pCentre;
    float m_angle;
    float m_rotationSpeed;

    Renderer*  m_pRenderer;
    Tilelist*  list;
    Pathmaker* pathmaker;

    bool    moving;
    int     columns;
    int     rows;
    int     x;
    int     y;
    float   m_fTileSize;

    // Wave and lives
    int   m_iLives;
    int   m_iWave;
    int   m_iEnemiesToSpawn;
    bool  m_bWaveComplete;
    float m_fSpawnTimer;

    std::vector<Enemy*> m_enemies;

    DynamicText* m_pLivesText;
    DynamicText* m_pWaveText;

    // Box2D world
    b2WorldId    WorldPointer;
    b2WorldDef*  World;
    int          ScenesubStepCount;

    // --- Game Over ---
    bool    m_bGameOver;
    Sprite* m_pGameOverSprite;   // Displays defeated_gameover.png
    DynamicText* m_pRestartText; // "Press R to restart" hint

private:
};

#endif // __SCENEMAIN_H__
