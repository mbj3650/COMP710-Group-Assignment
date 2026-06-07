// Projectile.h

#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <box2d.h>
#include <string>
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

  

    // give owner of tower for boomeranging projectlies, shapeid is the enemy target to aim towards, if not homing, travel in straight line else travel towards the target
    bool Initialise(Renderer& renderer, Tower* owner, float tileSize, b2WorldId WorldID, b2ShapeId Target, std::string ProjectileID, float speed);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);

    bool GetAlive();

    // Position accessors -- used by SceneGame to spawn particle bursts
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

    // apply extra (for upgrades)
    void ApplyExtraDamage(int dmg);
    void ApplyExtraPierce(int dmg);
    void ApplyExtraHoming();
    void ApplyExtraCold();
    void ApplyExtraPoison();

private:
    Projectile(const Projectile&);
    Projectile& operator=(const Projectile&);

    //internal information
    Sprite* m_pSprite;
    Tile* m_pCurrentTile;
    Tower* owner;

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
    int effect;
    float lifetime;
    b2ShapeId Target;//NOTE: Please do not make a projectile both pierce and homing at this time
};

#endif // PROJECTILE_H
