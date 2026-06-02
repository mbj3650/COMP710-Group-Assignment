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
#include "IniParser.h"
#include "EconomyConfig.h" // tower attack radius config
Tower::Tower()
{
    m_pSprite = 0;
    m_pCurrentTile = 0;
    m_x = 0;
    m_y = 0;
    m_tileSize = 40.0f;
}

Tower::~Tower()
{
    if (b2Body_IsValid(ID))
    {
        b2DestroyBody(ID);
    }
    delete m_pSprite;
    m_pSprite = 0;
}

bool Tower::Initialise(Renderer& renderer, Tile* startTile, float tileSize, b2WorldId WorldID, std::vector<Projectile*>& projectileaddress, std::string TowerID)
{
    m_renderer = &renderer;
    assert(startTile);
    startTile->hastower = true;//set it to true so we cant place another tower on it
    m_tileSize = tileSize;
    m_pCurrentTile = startTile;


    IniParser Parser;
    Parser.LoadIniFile("..\\assets\\info\\tower.ini");

    

    //PROJECTILE STATS; AT SOME POINT WE WANT TO USE INI IMPORTATION TO GET THIS INSTEAD
    projectileID = Parser.GetValueAsString(TowerID + "|ProjectileID");
    speed = Parser.GetValueAsFloat(TowerID + "|Speed");
    range = Parser.GetValueAsFloat(TowerID + "|Range");
    firedelay = Parser.GetValueAsFloat(TowerID + "|Firerate");
    firetimer = 0.1;
    canhome = false;
    m_projectiles = &projectileaddress;

    m_x = startTile->Position.x * tileSize + tileSize * 0.5f;
    m_y = startTile->Position.y * tileSize + tileSize * 0.5f;

    string SpritePath = "..\\assets\\towers\\" + Parser.GetValueAsString(TowerID + "|Sprite") + ".png";
    std::cout << "SPRITE:" << Parser.GetValueAsString(TowerID + "|Sprite");
    m_pSprite = renderer.CreateSprite(SpritePath.c_str());

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
    shapeDef.filter.maskBits = 0x0002;//detect enemies
    shapeDef.isSensor = true;//set it to be a sensor
   
    shapeId = b2CreateCircleShape(ID, &shapeDef, &circleShape);
    b2Shape_SetUserData(shapeId, this);
    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);
   
    std::cout << "made tower!\n" << b2Shape_GetUserData(shapeId) << "\n";

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

    if (firetimer > 0) {//decrease timer
        firetimer -= deltaTime;
    }
    else if (!EnemyInRadius.empty()) {//if can fire and enemy is in radius
        std::cout << "firing!\n";
        Projectile* newprojectile = new Projectile();//make new projectile
        newprojectile->Initialise(*m_renderer, this, m_tileSize, b2Shape_GetWorld(shapeId), EnemyInRadius.at(0), projectileID, speed);//add to it
        m_projectiles->push_back(newprojectile);//
        firetimer = firedelay;
    }




    m_x = b2Body_GetPosition(ID).x;
    m_y = b2Body_GetPosition(ID).y;

    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);
    if (!EnemyInRadius.empty()) {
        m_pSprite->SetAngle(
                atan2(
                    (b2Body_GetPosition(b2Shape_GetBody(EnemyInRadius.at(0))).x - m_x),
                    (b2Body_GetPosition(b2Shape_GetBody(EnemyInRadius.at(0))).y - m_y)
                    ) * (180 / M_PI));
    }
    
}

void Tower::Draw(Renderer& renderer)
{
        m_pSprite->Draw(renderer);
}
