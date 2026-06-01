// Projectile.h

#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <box2d.h>

class Renderer;
class Sprite;
class Tile;
struct b2WorldId;
struct b2ShapeId;
struct b2BodyId;
class Tower;
class Enemy;
class Projectile
{
public:
    Projectile();
    ~Projectile();

  

    // owner of tower for boomerang projectlies, target to aim towards, if not homing, travel in straight line else travel towards the target
    bool Initialise(Renderer& renderer, Tower* owner, float tileSize, b2WorldId WorldID, b2ShapeId Target, bool Homing, int pierce, int damage, float speed);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);

    bool GetAlive();

    // Position accessors -- used by SceneGame to spawn particle bursts
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

private:
    Projectile(const Projectile&);
    Projectile& operator=(const Projectile&);

    Sprite* m_pSprite;
    Tile* m_pCurrentTile;

    b2BodyId  ID;
    b2ShapeId shapeId;

    float m_x;
    float m_y;
    float m_speed;
    float m_tileSize;

    float max_x;//values for out of bounds checks
    float max_y;

    bool ishoming;//if it should constantly travel towards a target or not
    int maxenemies = 1;//can only hit one enemy by default, we can set this higher or lower to hit more or less enemies
    int damage;
    b2ShapeId Target;//NOTE: Please do not make a projectile both pierce and homing at this time
};

#endif // PROJECTILE_H
