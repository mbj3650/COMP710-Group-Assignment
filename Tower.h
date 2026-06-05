// COMP710 JESSE
#ifndef __TOWER_H__
#define __TOWER_H__

#include <box2d.h>
#include <vector>
#include <string>

// Forward declarations:
class Renderer;
class Sprite;
class Tile;
class Projectile;
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

    bool Initialise(Renderer& renderer, Tile* startTile, float tileSize, b2WorldId WorldID, std::vector<Projectile*>& projectileaddress, std::string TowerID);

    void Process(float deltaTime);
    void Draw(Renderer& renderer);


    // HP system -- used by towers to damage enemies
    void TakeDamage(int amount);

    // Position accessors -- used by SceneGame to spawn particle bursts
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

    // Used by sidepanel
	Tile* GetCurrentTile() const { return m_pCurrentTile; }
	std::string GetTowerID() const { return towerID; }
    void Sell();
	bool IsSold() const { return m_bSelling; }
    int GetSellValue() const;
protected:

private:
	Tower(const Tower& Tower);
	Tower& operator=(const Tower& Tower);

	// Member data:
public:
    std::vector<b2ShapeId> EnemyInRadius;
    std::vector<Projectile*>* m_projectiles;
    b2ShapeId shapeId;//setting this to public for projectile use
protected:

private:
    Renderer* m_renderer;//save renderer for projectile making
    Sprite* m_pSprite;
    Tile* m_pCurrentTile;

    b2BodyId  ID;


    b2BodyId  RadiusID;
    b2ShapeId RadiusshapeId;

    float m_x;
    float m_y;
    float m_tileSize;

    //information for the projectile
    std::string projectileID;
    bool canhome;
    int Price;
    float range;
    float speed;    
    float firedelay;
    float firetimer;

    // sidepanel
    std::string towerID;
    bool m_bSelling;
};

#endif // __TOWER_H__
