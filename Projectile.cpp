// Projectile.cpp
// Modified by: MartinYan12138y
// Changes: Added HP system -- TakeDamage() and IsDead() so towers
//          can damage enemies. HP scales with wave number.

#include "Projectile.h"
#include "Tile.h"
#include "sprite.h"
#include "Enemy.h"
#include "renderer.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <box2d.h>
#include "Tower.h"
#include "GameData.h"
#include "ProjectileData.h"

Projectile::Projectile()
{
    m_pSprite = 0;
    m_pCurrentTile = 0;
    m_tileSize = 40.0f;
}

Projectile::~Projectile()
{
    //std::cout << "Erased shape!\n";
    if (b2Body_IsValid(ID))
    {
        b2DestroyBody(ID);
    }
    delete m_pSprite;
    m_pCurrentTile = 0;
    m_pSprite = 0;
}


bool Projectile::Initialise(Renderer& renderer, Tower* owner, float tileSize, b2WorldId WorldID, b2ShapeId Target, std::string ProjectileID, float speed)
{
    m_tileSize = tileSize;
	ProjectileData data = GameData::Get().Projectile[ProjectileID];
	maxenemies = data.Pierce;
	damage = data.Damage;
	ishoming = data.Homing;
	effect = data.Effect;
	lifetime = data.Lifetime;
    alphaoffset = lifetime;
    this->owner = owner;
    m_x = owner->GetX();
    m_y = owner->GetY();
    m_bAlive = true;
    max_x = renderer.GetWidth();
    max_y = renderer.GetHeight();
    
    string SpritePath = "..\\assets\\projectiles\\" + data.Sprite + ".png";
    m_pSprite = renderer.CreateSprite(SpritePath.c_str());

    scale = data.Size*(tileSize * 0.65f) / m_pSprite->GetWidth() ;
    m_pSprite->SetScale(scale);
    //std::cout << scale << " is size of projectile\n";
    if (ProjectileID == "Boomerang") {//if the id is a boomerang
        isboomerang = true;
        this->Target = owner->shapeId;//what it should hit 
    }
    else {
        this->Target = Target;//what it should hit 
    }
    
    m_speed = (500.0f - (ishoming * 380)) * speed * (32 / m_tileSize);//speed of it

    // Box2D body setup
    b2BodyDef WorldObj = b2DefaultBodyDef();
    WorldObj.position = { m_x, m_y };
    ID = b2CreateBody(WorldID, &WorldObj);
    b2Body_SetType(ID, b2_dynamicBody);
    b2Body_SetUserData(ID, this);

    b2Polygon box = b2MakeRoundedBox(9 * scale * scale, 9 * scale * scale, 9.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.1f;
    shapeDef.filter.categoryBits = 0x0003;
    if (isboomerang) {
        specialtimer = 2.5;//set timer that will let it hit towers after a certain period
    }
    shapeDef.filter.maskBits = 0x0012;//detect enemies
    
    shapeDef.isSensor = true;//set it to be a sensor
    shapeId = b2CreatePolygonShape(ID, &shapeDef, &box);
    b2Shape_SetUserData(shapeId, this);
    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);
    m_pSprite->SetAngle(atan2(b2Body_GetLinearVelocity(ID).x, b2Body_GetLinearVelocity(ID).y) * (180 / M_PI));

    if (!ishoming || isboomerang) {//if not homing, then just set the velocity to go towards at the start 
        float targetX = static_cast<Enemy*>(b2Shape_GetUserData(Target))->GetX();//get x position of enemy 
        float targetY = static_cast<Enemy*>(b2Shape_GetUserData(Target))->GetY();
        float dx = targetX - m_x;
        float dy = targetY - m_y;
        float dist = sqrtf(dx * dx + dy * dy);
        b2Vec2 vel = { (dx / dist) * m_speed, (dy / dist) * m_speed };
        b2Body_SetLinearVelocity(ID, vel);
    }

    return true;
}


void Projectile::Process(float deltaTime)
{
    
    m_x = b2Body_GetPosition(ID).x;
    m_y = b2Body_GetPosition(ID).y;
    if (specialtimer > 0) {
        specialtimer -= deltaTime;
    }
    //if cant hit anymore enemies -- do nothing
    if (maxenemies == 0)
    {
        return;
    }

    if (ishoming){//check if its homing AND 
        if (b2Shape_IsValid(Target)) {//if its target is still alive, look to turn towards it

            float targetX = static_cast<Enemy*>(b2Shape_GetUserData(Target))->GetX();//get position of enemy 
            float targetY = static_cast<Enemy*>(b2Shape_GetUserData(Target))->GetY();
            float speedboost = 12;
            if (isboomerang) {
                targetX = owner->GetX();
                targetY = owner->GetY();
                speedboost = 12 * (2 * (2.5f - specialtimer))*0.01;//reduce homing offset by how much time has passed to add a feeling of momentum
                
            }

            if (!(isboomerang && specialtimer > 2.4)) {
                float dx = targetX - m_x;//update distances
                float dy = targetY - m_y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist < 1.5f && isboomerang && specialtimer <= 2)//if distance to target is small and is boomerang
                {
                    std::cout << "kill said valid shape\n";
                        m_bAlive = false;
                }

                bool atspeed = true;
                b2Vec2 vel = { (dx / dist) * m_speed*5 * speedboost, (dy / dist) * m_speed * 5 * speedboost };//change velocity and direction
           
                if (b2Body_GetLinearVelocity(ID).x > vel.x + 2 || b2Body_GetLinearVelocity(ID).x < vel.x - 2) {
                    b2Body_ApplyLinearImpulseToCenter(ID, {vel.x, 0}, true);
                    atspeed = false;
                }
                if ( b2Body_GetLinearVelocity(ID).y > vel.y+2 || b2Body_GetLinearVelocity(ID).y < vel.y - 2) {
                    b2Body_ApplyLinearImpulseToCenter(ID, { 0 , vel.y }, true);
                    atspeed = false;
                }
                if (atspeed) {
                    b2Body_SetLinearVelocity(ID, vel);
                }
            }
           
        }
        else {
            if (owner != NULL) {
                if (!owner->EnemyInRadius.empty()) {//if there was another enemy
                    Target = owner->EnemyInRadius.at(0);//aim for that instead
                }
                else {
                    ishoming = false;//else make it no longer home
                }
            }
           
            else {
                ishoming = false;//else make it no longer home
            }
           
        }
    }

    if (lifetime != -1) {
        if (lifetime > 0) {
            lifetime -= deltaTime;
        }
        if (effect == 2)//if a fireball
        {
            m_pSprite->SetScale(scale * lifetime / alphaoffset);
        }
        m_pSprite->SetAlpha(lifetime / alphaoffset);
    }

   //THIS CHECKS IF PROJCETILE HAS BEGUN INTERSECTING WITH ENEMY!
    b2SensorEvents sensorEvents = b2World_GetSensorEvents(b2Shape_GetWorld(shapeId));//get all the sensor events
    for (int i = 0; i < sensorEvents.beginCount; ++i)//go through all events where the shape is beginning to collide
    {
        b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;//get the beginevent 
        if (B2_ID_EQUALS(shapeId, beginTouch->sensorShapeId)) {//CHECK IF THE SENSOR EVENT IS RELEVANT 
                    void* myUserData = b2Shape_GetUserData(beginTouch->visitorShapeId);//get the object its colliding with
                    Enemy* NewEnemy = reinterpret_cast<Enemy*>(myUserData);
                    //NewEnemy->TurnBlue();
                    NewEnemy->TakeDamage(damage);
                    if (effect != -1) {//if projectile has an effect
                        NewEnemy->TakeEffect(effect);
                    }
                    maxenemies--;//decrement max amount of enemies it can hit by 1
        } 


    }

    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);
    if (!isboomerang) {//if not a boomerang, use normal turning animation
        m_pSprite->SetAngle(atan2(b2Body_GetLinearVelocity(ID).x, b2Body_GetLinearVelocity(ID).y) * (180 / M_PI));
    }
    else {
        m_pSprite->SetAngle(m_pSprite->GetAngle() + (360 * deltaTime));
    }
}

void Projectile::Draw(Renderer& renderer)
{
    if (maxenemies != 0)
    {
        m_pSprite->Draw(renderer);
    }
}

bool Projectile::GetAlive() {
    //check if projectile is in bounds first because we want to kill it if out of bounds
    if ((m_x > max_x + m_pSprite->GetWidth())
        || (m_y > max_y + m_pSprite->GetHeight())
        || (m_y < 0- m_pSprite->GetWidth())
        || (m_y < 0- m_pSprite->GetHeight())
        && !isboomerang) //if out of bounds
    {
       // std::cout << "out of bounds!\n";
        return(false);//return false (dead)
    }
    else if (lifetime <= 0 && lifetime > -1) {//if lifetime doesnt equal -1 and lifetime is dead
        return false;
    }

    //if in bounds, just return 
    else if (maxenemies == 0) {
       // std::cout << "out of pierce!\n";
        return false;
    }
    else {
        return m_bAlive;
    }
    
}

void Projectile::ApplyExtraDamage(int i)
{
    damage += i;
}

void Projectile::ApplyExtraPierce(int i)
{
    maxenemies += i;
}

void Projectile::ApplyExtraHoming()
{
    ishoming = true;
}
void Projectile::ApplyExtraCold()
{
    effect = 3;
}
void Projectile::ApplyExtraPoison()
{
    effect = 4;
}
void Projectile::ApplyExtraSize(float f)
{
    scale += f;
    m_pSprite->SetScale(scale);
}