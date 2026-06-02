// Enemy.h
// Handles an enemy that walks along the path the player drew.
// Uses the tile linked list (NextPosition) to figure out where to go.
// Modified by: MartinYan12138y
// Changes: Added HP system (TakeDamage, IsDead) for tower damage integration,
//          GetX/GetY accessors for particle effects.

#ifndef ENEMY_H
#define ENEMY_H
#include <box2d.h>
#include <string>
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

    // waveNumber scales the enemy's starting HP (wave 1 = 3HP, wave 2 = 5HP, etc.)
    bool Initialise(Renderer& renderer, Tile* startTile, float tileSize, b2WorldId WorldID, int waveNumber = 1, std::string EnemyID = "Basic");
    void Process(float deltaTime);
    void TurnRed();
    void TurnBlue();
    void Draw(Renderer& renderer);

    bool HasReachedEnd() const { return m_bReachedEnd; }

    // HP system -- used by towers to damage enemies
    void TakeDamage(int amount);
    bool IsDead()     const { return m_iHP <= 0; }
    int  GetHP()      const { return m_iHP; }
    int  GetMaxHP()   const { return m_iMaxHP; }

    //effect system
    void TakeEffect(int amount);

    // Position accessors -- used by SceneGame to spawn particle bursts
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

    int  GetDamage()   const { return m_damage; }
private:
    Enemy(const Enemy&);
    Enemy& operator=(const Enemy&);

    Sprite* m_pSprite;
    Tile*   m_pCurrentTile;

    b2BodyId  ID;
    b2ShapeId shapeId;

    float m_x;
    float m_y;
    float m_speed;
    float m_tileSize;

    bool m_bReachedEnd;

    // HP
    int m_iHP;
    int m_iMaxHP;

    int m_damage;//damage it deals upon reaching end

    //effects
    float poisontimer;
    int poisoncount;
    float slowtimer;
};

#endif // ENEMY_H
