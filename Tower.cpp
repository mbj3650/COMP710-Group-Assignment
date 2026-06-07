// Tower.cpp

#include "Tower.h"
#include "Tile.h"
#include "sprite.h"
#include "renderer.h"
#include "projectile.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <box2d.h>
#include "Enemy.h"
#include <algorithm>
#include "string.h"
#include "GameData.h"
#include "TowerData.h"
#include "RelicData.h"
#include "RelicManager.h"
#include "EconomyConfig.h" // tower attack radius config
Tower::Tower()
{
    m_pSprite = 0;
    m_pCurrentTile = 0;
    m_x = 0;
    m_y = 0;
    m_tileSize = 40.0f;
    m_pUpgrade1Sprite = 0;
    m_pUpgrade2Sprite = 0;
    m_pUpgrade3Sprite = 0;
}

Tower::~Tower()
{
    if (b2Body_IsValid(ID))
    {
        b2DestroyBody(ID);
    }
    delete m_pSprite;
    m_pSprite = 0;

    delete m_pUpgrade1Sprite;
    m_pUpgrade1Sprite = 0;
    delete m_pUpgrade2Sprite;
    m_pUpgrade2Sprite = 0;
    delete m_pUpgrade3Sprite;
    m_pUpgrade3Sprite = 0;
}

bool Tower::Initialise(Renderer& renderer, Tile* startTile, float tileSize, b2WorldId WorldID, std::vector<Projectile*>& projectileaddress, std::string TowerID)
{
    m_renderer = &renderer;
    assert(startTile);
    startTile->hastower = true;//set it to true so we cant place another tower on it
    m_tileSize = tileSize;
    m_pCurrentTile = startTile;

	TowerData data = GameData::Get().Tower[TowerID];
    projectileID = data.ProjectileID;
	speed = data.Speed;
	range = data.Range;
	firedelay = data.Firerate;
    firetimer = 0;
	towerID = TowerID;
    Price = data.Price;
    AimForLast = false;

    string SpritePath = "..\\assets\\towers\\" + data.Sprite + ".png";
    m_pSprite = renderer.CreateSprite(SpritePath.c_str());

    // upgrade sprites
    SpritePath = "..\\assets\\upgrades\\" + data.Upgrade1Name + ".png";
    m_pUpgrade1Sprite = renderer.CreateSprite(SpritePath.c_str());
    SpritePath = "..\\assets\\upgrades\\" + data.Upgrade2Name + ".png";
    m_pUpgrade2Sprite = renderer.CreateSprite(SpritePath.c_str());
    SpritePath = "..\\assets\\upgrades\\" + data.Upgrade3Name + ".png";
    m_pUpgrade3Sprite = renderer.CreateSprite(SpritePath.c_str());

    m_iUpgrade1Price = data.Upgrade1Price;
    m_iUpgrade2Price = data.Upgrade2Price;
    m_iUpgrade3Price = data.Upgrade3Price;
    m_iTowerIDUpgrade = data.ID;
    //PROJECTILE STATS; AT SOME POINT WE WANT TO USE INI IMPORTATION TO GET THIS INSTEAD
    m_bExtraHoming = false;
    m_projectiles = &projectileaddress;

    m_x = startTile->Position.x * tileSize + tileSize * 0.5f;
    m_y = startTile->Position.y * tileSize + tileSize * 0.5f;

    float scale = (tileSize * 0.95f) / m_pSprite->GetWidth();
    m_pSprite->SetScale(scale);

    // Box2D sensor setup
    b2BodyDef WorldObj = b2DefaultBodyDef();
    WorldObj.position = { m_x, m_y };
    ID = b2CreateBody(WorldID, &WorldObj);
    b2Body_SetType(ID, b2_dynamicBody);
    b2Body_SetUserData(ID, this);

    //b2Polygon box = b2MakeRoundedBox(6 * scale, 6 * scale, 5.0f);
    b2Circle circleShape;//make circle radius
    circleShape.center = { 0,0 };
    // Radius now scales with the tile size instead of a hardcoded 200px.
    // Tune the default in EconomyConfig.h (TOWER_RADIUS_TILES_DEFAULT).
    circleShape.radius = TOWER_RADIUS_TILES_DEFAULT * m_tileSize*range;
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.1f;
    shapeDef.filter.categoryBits = 0x0001;
    shapeDef.filter.maskBits = 0x0012;//detect enemies
    shapeDef.isSensor = true;//set it to be a sensor
   
    shapeId = b2CreateCircleShape(ID, &shapeDef, &circleShape);
    b2Shape_SetUserData(shapeId, this);
    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);
   


    m_x = b2Body_GetPosition(ID).x;
    m_y = b2Body_GetPosition(ID).y;
    std::cout << "made tower!\n" << b2Shape_GetUserData(shapeId) << "\n";
    // apply relics to new tower
    ApplyAllRelics();
    return true;
}

void Tower::Process(float deltaTime)
{
    
    b2SensorEvents sensorEvents = b2World_GetSensorEvents(b2Shape_GetWorld(shapeId));//get all the sensor events
    for (int z = 0; z < EnemyInRadius.size(); z++) {
        if (!b2Shape_IsValid(EnemyInRadius.at(z))) {//if enemy shape is null
            EnemyInRadius.erase(EnemyInRadius.begin() + z);//erase it from memory
            z--;//reduce i by 1 so it doesnt risk skipping over certain enemies
           // std::cout << "Removed Enemy reached end!\n";
        }
    }
    for (int i = 0; i < sensorEvents.beginCount; ++i)//go through all events where the shape is beginning to collide
    {
        b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;//get the beginevent 
        if (B2_ID_EQUALS(shapeId, beginTouch->sensorShapeId)) {//CHECK IF THE SENSOR EVENT IS RELEVANT 
            void* myUserData = b2Shape_GetUserData(beginTouch->visitorShapeId);//get the object its colliding with
            Enemy* NewEnemy = reinterpret_cast<Enemy*>(myUserData);
            EnemyInRadius.push_back(beginTouch->visitorShapeId);//add enemy to list of enemies in radius
            //NewEnemy->TurnRed();
        }
       
    }

    for (int i = 0; i < sensorEvents.endCount; ++i)//go through all events where the shape is beginning to collide
    {
        b2SensorEndTouchEvent* endTouch = sensorEvents.endEvents + i;//get the endevent 
        if (B2_ID_EQUALS(shapeId, endTouch->sensorShapeId)) {//CHECK IF THE SENSOR EVENT IS RELEVANT 
            if (b2Shape_IsValid(endTouch->visitorShapeId)) {
               // std::cout << b2Shape_GetUserData(shapeId) << "Removed Enemy left radius!\n";
                void* myUserData = b2Shape_GetUserData(endTouch->visitorShapeId);//get the object its colliding with

                Enemy* NewEnemy = reinterpret_cast<Enemy*>(myUserData);//turn to enemy
                //NewEnemy->TurnBlue();
                for (int i = 0; i < EnemyInRadius.size(); i++) {
                    if (B2_ID_EQUALS(EnemyInRadius.at(i), endTouch->visitorShapeId)) {
                        EnemyInRadius.erase(EnemyInRadius.begin() + i);//remove from list of enemies in range
                        break;
                    }
                }
            }
        }
        
        
       
    }
    b2ShapeId Target;
    if (!EnemyInRadius.empty()) {//if can fire and enemy is in radius
        if (AimForLast) {//check if we're aiming for the last enemy 
            //std::cout << "AIMING FOR LAST\n";
            Target = EnemyInRadius.at(EnemyInRadius.size() - 1);//if so aim at last enemy
        }
        else {//else aim at first enemy
            Target = EnemyInRadius.at(0);
        }
    }



    if (firetimer > 0) {//decrease timer
        firetimer -= deltaTime;
    }
    else if (!EnemyInRadius.empty()) {//if can fire and enemy is in radius 
        std::cout << "firing!\n";
        Projectile* newprojectile = new Projectile();//make new projectile
        newprojectile->Initialise(*m_renderer, this, m_tileSize, b2Shape_GetWorld(shapeId), Target, projectileID, speed);//add to it
        if (m_iExtraDamage > 0) newprojectile->ApplyExtraDamage(m_iExtraDamage);
        if (m_iExtraPierce > 0) newprojectile->ApplyExtraPierce(m_iExtraPierce);
        if (m_fExtraSize > 0.0f) newprojectile->ApplyExtraSize(1.0f+m_fExtraSize);
        if (m_bExtraHoming) newprojectile->ApplyExtraHoming();
        if (m_bExtraToxic) newprojectile->ApplyExtraPoison();
        if (m_bExtraCold) newprojectile->ApplyExtraCold();
        m_projectiles->push_back(newprojectile);//
        firetimer = firedelay;
    }




    if (towerID == "Detonator") {
        m_pSprite->SetAngle(m_pSprite->GetAngle() - ((30 + (firetimer / firedelay * 360)) * deltaTime));
    }
    else if (!EnemyInRadius.empty()) {
        m_pSprite->SetAngle(
                atan2(
                    (b2Body_GetPosition(b2Shape_GetBody(Target)).x - m_x),
                    (b2Body_GetPosition(b2Shape_GetBody(Target)).y - m_y)
                    ) * (180 / M_PI));
    }
    
}

void Tower::Draw(Renderer& renderer)
{
        m_pSprite->Draw(renderer);
}

void Tower::Sell()
{
	m_bSelling = true;
}

int Tower::GetSellValue() const
{
    return Price / 2;
}

Sprite* Tower::GetUpgradeSprite(int index)
{
    switch (index)
    {
        case 1:
            return m_pUpgrade1Sprite;
        case 2:
            return m_pUpgrade2Sprite;
        case 3:
            return m_pUpgrade3Sprite;
    }
    return m_pUpgrade1Sprite;
}

bool Tower::Upgrade(int index, int* gold)
{
    switch (index)
    {
    case 1:
        if (*gold >= m_iUpgrade1Price)
        {
            m_bUpgrade1 = true;
            *gold -= m_iUpgrade1Price;
            Price += m_iUpgrade1Price;
            ApplyUpgrade(3 * m_iTowerIDUpgrade + 1);
            return true;
        }
        break;
    case 2:
        if (*gold >= m_iUpgrade2Price)
        {
            m_bUpgrade2 = true;
            *gold -= m_iUpgrade2Price;
            Price += m_iUpgrade2Price;
            ApplyUpgrade(3 * m_iTowerIDUpgrade + 2);
            return true;
        }
        break;
    case 3:
        if (*gold >= m_iUpgrade3Price)
        {
            m_bUpgrade3 = true;
            *gold -= m_iUpgrade3Price;
            Price += m_iUpgrade3Price;
            ApplyUpgrade(3 * m_iTowerIDUpgrade + 3);
            return true;
        }
        break;
    }
    return false;
}

bool Tower::CanUpgrade(int index)
{
    switch (index)
    {
    case 1:
        return !m_bUpgrade1;
    case 2:
        return !m_bUpgrade2;
    case 3:
        return !m_bUpgrade3;
    }
    return false;
}

bool Tower::IsTargetingLast()
{
    return AimForLast;
}
void Tower::SwapTargeting()
{
    AimForLast = !AimForLast;
}

// Upgrades and Relic effects
void Tower::ApplyUpgrade(int upgrade)
{
    std::cout << "Apply Upgrade being ran\n";
    switch (upgrade)
    {
    case Shooter_RapidFire:
        firedelay -= 1.25f;
        speed += 4.0f;
        break;
    case Shooter_LongRange:
        range += 3.5f;
        break;
    case Shooter_LethalShot:
        m_iExtraDamage += 1;
        break;
    case Iceman_SwiftThrow:
        firedelay -= 1.0f;
        break;
    case Iceman_Coldness:
        m_bExtraCold = true;
        break;
    case Iceman_ThickSnow:
        m_iExtraPierce += 3;
        m_iExtraDamage += 1;
        break;
    case Poisoner_LongReach:
        range += 3.0f;
        break;
    case Poisoner_ExtraToxic:
        m_bExtraToxic = true;
        break;
    case Poisoner_QuickFog:
        firedelay -= 1.0f;
        speed += 1.5f;
        break;
    case Detonator_RapidBlast:
        firedelay -= 1.0f;
        break;
    case Detonator_BigBlast:
        m_fExtraSize += 0.5f;
        range += 1.0f;
        break;
    case Detonator_Firepower:
        m_iExtraDamage += 2;
        break;
    case Boomerang_SpeedyRang:
        firedelay -= 1.25f;
        break;
    case Boomerang_Sharpness:
        m_iExtraDamage += 2;
        break;
    case Boomerang_HeavyRangs:
        speed -= 5.0f;
        m_fExtraSize += 0.25f;
        m_iExtraPierce += 3;
        break;
    }
}

void Tower::ApplyRelicEffect(int upgrade)
{
    switch (upgrade)
    {
        case RedPotion:
            m_iExtraDamage += 2;
            break;
        case Campfire:
            if (towerID == "Detonator") firedelay *= 0.95f;
            break;
        case Coffee:
            firedelay *= 0.975f;
            break;
        case Sash:
            if (towerID == "Boomerang") range += 0.1f;
            break;
        case Shrimp:
            if (towerID == "Poisoner") firedelay *= 0.925f;
            break;

    }
}

void Tower::ApplyAllRelics()
{
    const auto& relics = RelicManager::Get().TowerRelics;
    for (const auto& pair : relics)
    {
        for (int i = 0; i < pair.second; i++) // when you have multiple relics do the effect multiple times
        {
            ApplyRelicEffect(GameData::Get().Relic[pair.first].Effect);
        }
    }
}