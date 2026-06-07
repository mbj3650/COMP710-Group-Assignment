// Enemy.cpp
// Modified by: MartinYan12138y
// Changes: Added HP system -- TakeDamage() and IsDead() so towers
//          can damage enemies. HP scales with wave number.
//          Added boss enemy on every 10th wave (the blob): bigger hp,
//          slower move speed, and heals itself over time.

#include "Enemy.h"
#include "Tile.h"
#include "animatedsprite.h"
#include "renderer.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <box2d.h>
#include "EnemyData.h"
#include "GameData.h"
#include "game.h"

Enemy::Enemy()
{
    m_pAniSprite      = 0;
    m_pCurrentTile = 0;
    m_x            = 0;
    m_y            = 0;
    m_speed        = 100.0f;
    m_tileSize     = 40.0f;
    m_bReachedEnd  = false;
    m_iHP          = 3;
    m_iMaxHP       = 3;
    m_bIsBoss      = false;
    regentimer     = 0.0f;
}

Enemy::~Enemy()
{
    std::cout << "Erased Enemy!\n";
    if (b2Body_IsValid(ID))
    {
        b2DestroyBody(ID);
    }
    delete m_pAniSprite;
    m_pAniSprite = 0;
}

bool Enemy::Initialise(Renderer& renderer, Tile* startTile, float tileSize,
                       b2WorldId WorldID, int waveNumber, std::string EnemyID)
{
    assert(startTile);

    m_tileSize     = tileSize;
    m_pCurrentTile = startTile;

    EnemyData data = GameData::Get().Enemy[EnemyID];

    // every 10th wave (10, 20, 30...) we make a boss enemy.
    // for now any enemy spawned on those waves becomes the boss (the blob).
    if (waveNumber % 10 == 0)
    {
        m_bIsBoss = true;
    }

    // HP scales with wave: wave 1 = 3 HP, wave 2 = 5 HP, wave 3 = 7 HP, etc.
    m_iMaxHP = data.BonusHealth + 1 + waveNumber * 2 ;
    if (m_bIsBoss)
    {
        m_iMaxHP = m_iMaxHP * 5;//boss has way more hp so it takes a while to kill
    }
    m_iHP    = m_iMaxHP;

    m_x = startTile->Position.x * tileSize + tileSize * 0.5f;
    m_y = startTile->Position.y * tileSize + tileSize * 0.5f;
   
    m_damage = data.Damage;
    m_speed *= data.Speed;//multiply the speed so they can move faster or slower
    if (m_bIsBoss)
    {
        m_speed = m_speed * 0.5f;//boss moves slower than normal enemies
    }

    std::string SpritePath = "..\\assets\\enemies\\" + data.Sprite + ".png";
    m_pAniSprite = renderer.CreateAnimatedSprite(SpritePath.c_str());
    m_pAniSprite->SetupFrames(data.SpriteSizeX, data.SpriteSizeY);
    m_pAniSprite->SetLooping(true);
    m_pAniSprite->SetFrameDuration(0.5f);
    m_pAniSprite->Animate();
    // bosses are drawn bigger so the player can tell them apart
    float scalesize = 0.9f;
    if (m_bIsBoss)
    {
        scalesize = 1.2f;
    }
    float scale = (tileSize * scalesize) / m_pAniSprite->GetWidth();
    m_pAniSprite->SetScale(scale);

    // give the boss a green tint so it looks like a blob and stands out
    if (m_bIsBoss)
    {
        m_pAniSprite->SetRedTint(0.3f);
        m_pAniSprite->SetGreenTint(1.0f);
        m_pAniSprite->SetBlueTint(0.3f);
    }

    // Box2D body setup
    b2BodyDef WorldObj = b2DefaultBodyDef();
    WorldObj.position  = { m_x, m_y };
    ID = b2CreateBody(WorldID, &WorldObj);
    b2Body_SetType(ID, b2_kinematicBody);
    b2Body_SetUserData(ID, this);
    //b2Body_EnableContactEvents(ID, true);
    //b2Body_EnableHitEvents(ID, true);
    b2Polygon box = b2MakeRoundedBox(6 * scale, 6 * scale, 5.0f);
    b2ShapeDef shapeDef   = b2DefaultShapeDef();
    shapeDef.density      = 1.0f;
    shapeDef.friction     = 0.1f;
    shapeDef.filter.categoryBits = 0x0002;
    shapeDef.filter.maskBits  = 0x0001 | 0x0003;
    shapeId = b2CreatePolygonShape(ID, &shapeDef, &box);
    b2Shape_SetUserData(shapeId, this);
    m_pAniSprite->SetX(b2Body_GetPosition(ID).x);
    m_pAniSprite->SetY(b2Body_GetPosition(ID).y);

    return true;
}

// Reduces HP by amount; if HP drops to 0 the enemy is considered dead
void Enemy::TakeDamage(int amount)
{
    m_iHP -= amount;
    if (m_iHP < 0) m_iHP = 0;
    Game::GetInstance().GetSoundSystem()->PlaySound("..\\assets\\sounds\\hit.wav");
}


void Enemy::TakeEffect(int effect)
{
    enum Effects {
        FROST,
        POISON,
        HEATED,
        EXTRAFROST,
        EXTRAPOISON,
    };
    switch (effect) {
        case FROST:
            slowtimer = 2;
            break;
        case POISON:
            poisoncount += 3;//make it stackable instead of capping it
            poisontimer = 0.5;
            break;
        case HEATED://if slowed, melt undo that effcet
            slowtimer = 0;
            break;
        case EXTRAFROST:
            slowtimer = 4;
            break;
        case EXTRAPOISON:
            poisoncount += 5;
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

    //BOSS HEALING
    //the boss heals a bit of hp every half second, so the player has to
    //out-damage the healing to actually kill it. normal enemies dont do this.
    if (m_bIsBoss) {
        if (regentimer > 0) {
            regentimer -= deltaTime;
        }
        else {
            m_iHP += 2;//heal 2 hp
            if (m_iHP > m_iMaxHP) {
                m_iHP = m_iMaxHP;//dont go over the max hp
            }
            regentimer = 0.5;//reset the timer
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
        if (slowtimer > 2) {//if super slow
            vel = { vel.x * 0.33f, vel.y * 0.33f };// 1/3 speed
        }    
        else if (slowtimer > 0) {//if slow
            vel = { vel.x * 0.5f, vel.y * 0.5f };//halve speed instead
        }    
        b2Body_SetLinearVelocity(ID, vel);
    }
    m_pAniSprite->Process(deltaTime);
    m_pAniSprite->SetX(b2Body_GetPosition(ID).x);
    m_pAniSprite->SetY(b2Body_GetPosition(ID).y);
}

void Enemy::TurnRed() {
    m_pAniSprite->SetBlueTint(0.0f);
    m_pAniSprite->SetGreenTint(0.0f);
    m_pAniSprite->SetRedTint(1.0f);
}

void Enemy::TurnBlue() {
    m_pAniSprite->SetBlueTint(1.0f);
    m_pAniSprite->SetGreenTint(0.0f);
    m_pAniSprite->SetRedTint(0.0f);
}

void Enemy::Draw(Renderer& renderer)
{
    if (!m_bReachedEnd && !IsDead())
    {
        m_pAniSprite->Draw(renderer);
    }
}
