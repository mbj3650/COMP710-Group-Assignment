// COMP710 JESSE
#ifndef __TOWER_H__
#define __TOWER_H__

#include <box2d.h>
#include <vector>

// Forward declarations:
class Renderer;
class Sprite;
class Tile;
struct b2WorldId;
struct b2ShapeId;
struct b2BodyId;
class Enemy;
class Renderer;

// Class declaration:
class Tower
{
	// Member methods:
public:
    Tower();
    ~Tower();


    bool Initialise(Renderer& renderer, Tile* startTile, float tileSize, b2WorldId WorldID);
    void Process(float deltaTime);
    void Draw(Renderer& renderer);


    // HP system -- used by towers to damage enemies
    void TakeDamage(int amount);

    // Position accessors -- used by SceneGame to spawn particle bursts
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

protected:

private:
	Tower(const Tower& Tower);
	Tower& operator=(const Tower& Tower);

	// Member data:
public:
    std::vector<b2ShapeId> EnemyInRadius;
protected:

private:
    Sprite* m_pSprite;
    Tile* m_pCurrentTile;

    b2BodyId  ID;
    b2ShapeId shapeId;

    b2BodyId  RadiusID;
    b2ShapeId RadiusshapeId;

    float m_x;
    float m_y;
    float m_tileSize;

};

#endif // __TOWER_H__
