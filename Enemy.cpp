// COMP710 GP Framework 2025
#include "Enemy.h"

Enemy::Enemy()
    : m_position{}
    , m_health(100.0f)
    , m_maxHealth(100.0f)
    , m_radius(16.0f)
    , m_alive(true)
{
}

Enemy::~Enemy() {}

void Enemy::Update(float deltaTime) { (void)deltaTime; }
void Enemy::Draw() {}

bool Enemy::IsAlive() const
{
    return m_alive && (m_health > 0.0f);
}

float Enemy::GetRadius() const
{
    return m_radius;
}

const Vector2& Enemy::GetPosition() const
{
    return m_position;
}

void Enemy::TakeDamage(float damage)
{
    if (!m_alive) { return; }
    m_health -= damage;
    if (m_health <= 0.0f)
    {
        m_health = 0.0f;
        m_alive  = false;
    }
}
