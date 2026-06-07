// COMP710 GP Framework 2025
// SceneGame.h
// Modified by: MartinYan12138y
// Changes: Game over state + restart, FMOD audio, instructions overlay,
//          particle burst effects, enemy HP integration.
//          Gold economy system (kill bounty + decay, pay window,
//          short-path bonus, spend interface for towers).

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
class Projectile;
class Enemy;
class Tower;
class DynamicText;
class UIShopSlot;
class UIShopManager;
class SoundSystem;
class AnimatedSprite;
// Number of instruction text lines shown at game start
const int NUM_INSTRUCTION_LINES = 9;

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

    // --- Economy (Gold) ---
    // Other systems (towers, upgrades, relics) use these to read/spend gold.
    int  GetGold() const { return m_iGold; }
    bool TrySpend(int cost);       // pay for a tower/upgrade; false if too poor
    void AddGold(int amount);      // generic reward (relics, events, etc.)
    void AddGoldStrikeBonus();     // Pickaxe "Gold Striker" extra gold on a kill

private:
    SceneGame(const SceneGame&);
    SceneGame& operator=(const SceneGame&);

    // Economy helpers (kept private -- internal book-keeping)
    int  KillBounty(int wave) const;  // gold for one normal kill (with wave decay)
    bool IsPayWindowOpen() const;     // are kills still rewarded this wave?
    void RefreshGoldText();           // update the HUD gold label

protected:
    Sprite*    m_pCentre;
    Sprite*    m_pCursor;
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
    std::vector<Projectile*> m_projectiles;//tower list
    DynamicText* m_pLivesText;
    DynamicText* m_pWaveText;

    // --- Economy (Gold) ---
    int          m_iGold;
    int          m_iGoldPrev;
    DynamicText* m_pGoldText;
    float        m_fWaveTimer;         // seconds since the first enemy of this wave spawned
    bool         m_bWaveTimerStarted;  // pay window only starts ticking once enemies appear

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

    // --- Particles ---
    Sprite*   m_pParticleSprite;              // shared sprite (explosion.png)
    Particle  m_particlePool[PARTICLE_POOL_SIZE];
    void      SpawnBurst(float x, float y);   // activates a burst of particles

    AnimatedSprite* testAni;
    // UI
    bool m_bHideUI;
    UIShopManager* m_pShopManager;
};

#endif // __SCENEMAIN_H__
