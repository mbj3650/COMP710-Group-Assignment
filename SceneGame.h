// COMP710 GP Framework 2025
// SceneGame.h
// Modified by: MartinYan12138y
// Changes: Game over state + restart, FMOD audio, instructions overlay,
//          particle burst effects, enemy HP integration.

#ifndef __SCENEMAIN_H__
#define __SCENEMAIN_H__

#include "scene.h"
#include "fmod.hpp"
#include "Particle.h"
#include <vector>
#include "box2d.h"
class Renderer;
class Sprite;
class Tilelist;
class Pathmaker;
class Enemy;
class Tower;
class DynamicText;

// Number of instruction text lines shown at game start
const int NUM_INSTRUCTION_LINES = 8;

// Particle pool size
const int PARTICLE_POOL_SIZE = 40;

class SceneGame : public Scene
{
public:
    SceneGame();
    virtual ~SceneGame();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime, InputSystem& inputSystem);
    virtual void Draw(Renderer& renderer);
    virtual void DebugDraw();

    bool MovePosition(int xoffset, int yoffset);
    void RestartGame(Renderer& renderer);

private:
    SceneGame(const SceneGame&);
    SceneGame& operator=(const SceneGame&);

protected:
    Sprite*    m_pCentre;
    float      m_angle;
    float      m_rotationSpeed;

    Renderer*  m_pRenderer;
    Tilelist*  list;
    Pathmaker* pathmaker;

    bool  moving;
    int   columns;
    int   rows;
    int   x;
    int   y;
    float m_fTileSize;

    // Wave and lives
    int   m_iLives;
    int   m_iWave;
    int   m_iEnemiesToSpawn;
    bool  m_bWaveComplete;
    float m_fSpawnTimer;

    std::vector<Enemy*> m_enemies;
    std::vector<Tower*> m_towers;//tower list
    DynamicText* m_pLivesText;
    DynamicText* m_pWaveText;

    // Box2D
    b2WorldId   WorldPointer;
    b2WorldDef* World;
    int         ScenesubStepCount;

    // --- Game Over ---
    bool         m_bGameOver;
    Sprite*      m_pGameOverSprite;
    DynamicText* m_pRestartText;

    // --- Instructions overlay ---
    bool         m_bShowInstructions;
    DynamicText* m_pInstructions[NUM_INSTRUCTION_LINES];

    // --- FMOD ---
    FMOD::Sound*   m_pMusicBG;
    FMOD::Sound*   m_pSoundGameOver;
    FMOD::Sound*   m_pSoundWaveStart;
    FMOD::Channel* m_pMusicChannel;

    // --- Particles ---
    Sprite*   m_pParticleSprite;              // shared sprite (explosion.png)
    Particle  m_particlePool[PARTICLE_POOL_SIZE];
    void      SpawnBurst(float x, float y);   // activates a burst of particles
};

#endif // __SCENEMAIN_H__
