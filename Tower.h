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

enum UpgradeID
{
    Shooter_RapidFire = 1,
    Shooter_LongRange = 2,
    Shooter_LethalShot = 3,
    Iceman_SwiftThrow = 4,
    Iceman_Coldness = 5,
    Iceman_ThickSnow = 6,
    Poisoner_LongReach = 7,
    Poisoner_ExtraToxic = 8,
    Poisoner_QuickFog = 9,
    Detonator_u1 = 10,
    Detonator_u2 = 11,
    Detonator_u3 = 12,
    Boomerang_u1 = 13,
    Boomerang_u2 = 14,
    Boomerang_u3 = 15,
};
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
    Sprite* GetUpgradeSprite(int index);
    bool Upgrade(int index, int* gold);
    bool CanUpgrade(int index);
    void ApplyUpgrade(int upgrade);

    bool IsTargetingLast();
    void SwapTargeting();
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
    int Price;
    float range;
    float speed;    
    float firedelay;
    float firetimer;

    bool AimForLast;

    // sidepanel
    std::string towerID;
    int m_iTowerIDUpgrade;
    bool m_bSelling;
    Sprite* m_pUpgrade1Sprite;
    Sprite* m_pUpgrade2Sprite;
    Sprite* m_pUpgrade3Sprite;
    bool m_bUpgrade1;
    bool m_bUpgrade2;
    bool m_bUpgrade3;
    int m_iUpgrade1Price;
    int m_iUpgrade2Price;
    int m_iUpgrade3Price;

    // upgrades
    int m_iExtraDamage;
    bool m_bExtraCold;
    bool m_bExtraToxic;
    bool m_bExtraHoming;
    int m_iExtraPierce;
};

#endif // __TOWER_H__
