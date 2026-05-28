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
    if (b2Body_IsValid(ID))
    {
        b2DestroyBody(ID);
    }
    delete m_pSprite;
    m_pSprite = 0;
}

bool Enemy::Initialise(Renderer& renderer, Tile* startTile, float tileSize,
                       b2WorldId WorldID, int waveNumber)
{
    assert(startTile);

    m_tileSize     = tileSize;
    m_pCurrentTile = startTile;

    // HP scales with wave: wave 1 = 3 HP, wave 2 = 5 HP, wave 3 = 7 HP, etc.
    m_iMaxHP = 1 + waveNumber * 2;
    m_iHP    = m_iMaxHP;

    m_x = startTile->Position.x * tileSize + tileSize * 0.5f;
    m_y = startTile->Position.y * tileSize + tileSize * 0.5f;

    m_pSprite = renderer.CreateSprite("..\\assets\\ball.png");

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
    shapeDef.filter.maskBits     = 0x0002;
    shapeId = b2CreatePolygonShape(ID, &shapeDef, &box);

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
        b2Vec2 vel = { (dx / dist) * m_speed, (dy / dist) * m_speed };
        b2Body_SetLinearVelocity(ID, vel);
    }

    m_pSprite->SetX(b2Body_GetPosition(ID).x);
    m_pSprite->SetY(b2Body_GetPosition(ID).y);
}

void Enemy::Draw(Renderer& renderer)
{
    if (!m_bReachedEnd && !IsDead())
    {
        m_pSprite->Draw(renderer);
    }
}
