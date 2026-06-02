// Enemy.cpp
// Modified by: MartinYan12138y
// Changes: Added HP system -- TakeDamage() and IsDead() so towers
//          can damage enemies. HP scales with wave number.

#include "Enemy.h"
#include "Tile.h"
#include "sprite.h"
#include "renderer.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <box2d.h>
#include "IniParser.h"

Enemy::Enemy()
{
    m_pSprite      = 0;
    m_pCurrentTile = 0;
    m_x            = 0;
    m_y            = 0;
    m_speed        = 100.0f;
    m_tileSize     = 40.0f;
    m_bReachedEnd  = false;
    m_iHP          = 3;
    m_iMaxHP       = 3;
}

Enemy::~Enemy()
{
    std::cout << "Erased shape!\n";
    if (b2Body_IsValid(ID))
    {
        b2DestroyBody(ID);
    }
    delete m_pSprite;
    m_pSprite = 0;
}

bool Enemy::Initialise(Renderer& renderer, Tile* startTile, float tileSize,
                       b2WorldId WorldID, int waveNumber, std::string EnemyID)
{
    assert(startTile);

    IniParser Parser;
    Parser.LoadIniFile("..\\assets\\info\\enemy.ini");

    m_tileSize     = tileSize;
    m_pCurrentTile = startTile;

    // HP scales with wave: wave 1 = 3 HP, wave 2 = 5 HP, wave 3 = 7 HP, etc.
    int health_bonus = Parser.GetValueAsInt(EnemyID + "|BonusHealth");
    m_iMaxHP = health_bonus + 1 + waveNumber * 2 ;
    m_iHP    = m_iMaxHP;

    m_x = startTile->Position.x * tileSize + tileSize * 0.5f;
    m_y = startTile->Position.y * tileSize + tileSize * 0.5f;

    m_pSprite = renderer.CreateSprite("..\\assets\\ball.png");

   
    m_damage = Parser.GetValueAsInt(EnemyID + "|Damage");
    m_speed *= Parser.GetValueAsFloat(EnemyID + "|Speed");//multiply the speed so they can move faster or slower

    std::string SpritePath = "..\\assets\\projectiles\\" + Parser.GetValueAsString(EnemyID + "|Sprite") + ".png";
    m_pSprite = renderer.CreateSprite(SpritePath.c_str());

    float scale = (tileSize * 0.65f) / m_pSprite->GetWidth();
    m_pSprite->SetScale(scale);

    // Box2D body setup
    b2BodyDef WorldObj = b2DefaultBodyDef();
    WorldObj.position  = { m_x, m_y };
    ID = b2CreateBody(WorldID, &WorldObj);
    b2Body_SetType(ID, b2_dynamicBody);
    b2Body_SetUserData(ID, this);

    b2Polygon box = b2MakeRoundedBox(6 * scale, 6 * scale, 5.0f);
    b2ShapeDef shapeDef   = b2DefaultShapeDef();
    shapeDef.density      = 1.0f;
    shapeDef.friction     = 0.1f;
    shapeDef.filter.categoryBits = 0x0002;
    shapeDef.filter.maskBits  = 0x0002 | 0x0001 | 0x0003;
    shapeId = b2CreatePolygonShape(ID, &shapeDef, &box);
    b2Shape_SetUserData(shapeId, this);
    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);

    return true;
}

// Reduces HP by amount; if HP drops to 0 the enemy is considered dead
void Enemy::TakeDamage(int amount)
{
    m_iHP -= amount;
    if (m_iHP < 0) m_iHP = 0;
}


void Enemy::TakeEffect(int effect)
{
    enum Effects {
        FROST,
        POISON,
    };
    switch (effect) {
        case FROST:
            slowtimer = 2;
            break;
        case POISON:
            poisoncount += 3;//make it stackable instead of capping it
            poisontimer = 0.5;
            break;
    }
}

void Enemy::Process(float deltaTime)
{
    m_x = b2Body_GetPosition(ID).x;
    m_y = b2Body_GetPosition(ID).y;

    // Dead or already done -- do nothing
    if (m_bReachedEnd || IsDead() || m_pCurrentTile == 0)
    {
        return;
    }

    Tile* nextTile = m_pCurrentTile->NextPosition;
    if (nextTile == 0)
    {
        m_bReachedEnd = true;
        return;
    }

    //EFFECT TIMERS
    if (slowtimer > 0) {
        slowtimer -= deltaTime;
    }
    if (poisoncount > 0) {//if needs to be poisoned
        if (poisontimer > 0) {//if poisontimer is greater than 0
            poisontimer -= deltaTime;//decrease until 0 or less
        }
        else {//if poison timer is 0 then take damage
            TakeDamage(1);
            poisontimer = 0.5;//reset timer
            poisoncount--;//reduce poisoncount(ticks) by 1
        }
    }





    float targetX = nextTile->Position.x * m_tileSize + m_tileSize * 0.5f;
    float targetY = nextTile->Position.y * m_tileSize + m_tileSize * 0.5f;
    float dx = targetX - m_x;
    float dy = targetY - m_y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < 1.5f)
    {
        m_x            = targetX;
        m_y            = targetY;
        m_pCurrentTile = nextTile;
        if (nextTile->isEnd)
        {
            m_bReachedEnd = true;
        }
    }
    else
    {
        b2Vec2 vel = { (dx / dist) * m_speed, (dy / dist) * m_speed };//set speed
        if (slowtimer > 0) {//if slow
            vel = { vel.x * 0.5f, vel.y * 0.5f };//halve speed instead
        }    
        b2Body_SetLinearVelocity(ID, vel);
    }

    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);
}

void Enemy::TurnRed() {
    m_pSprite->SetBlueTint(0.0f);
    m_pSprite->SetGreenTint(0.0f);
    m_pSprite->SetRedTint(1.0f);
}

void Enemy::TurnBlue() {
    m_pSprite->SetBlueTint(1.0f);
    m_pSprite->SetGreenTint(0.0f);
    m_pSprite->SetRedTint(0.0f);
}

void Enemy::Draw(Renderer& renderer)
{
    if (!m_bReachedEnd && !IsDead())
    {
        m_pSprite->Draw(renderer);
    }
}
