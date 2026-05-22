// COMP710 GP Framework 2025
// Enemy.h  -  Base class for all enemy units.
// ProjectilePool::ResolveCollisions() depends on IsAlive(), GetPosition(),
// GetRadius(), and TakeDamage() - all declared here.
#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "vector2.h"

class Enemy
{
public:
    Enemy();
    virtual ~Enemy();

    virtual void Update(float deltaTime);
    virtual void Draw();

    bool           IsAlive()     const;
    float          GetRadius()   const;
    const Vector2& GetPosition() const;
    void           TakeDamage(float damage);

    float GetHealth()    const { return m_health;    }
    float GetMaxHealth() const { return m_maxHealth; }

protected:
    Vector2 m_position;
    float   m_health;
    float   m_maxHealth;
    float   m_radius;
    bool    m_alive;

private:
    Enemy(const Enemy&)            = delete;
    Enemy& operator=(const Enemy&) = delete;
};

#endif // __ENEMY_H__
