// COMP710 GP Framework 2025
#include "ProjectilePool.h"
#include "Enemy.h"
#include "renderer.h"
#include "sprite.h"
#include "logmanager.h"

#include <algorithm>    // std::clamp
#include <cassert>

// ---------------------------------------------------------------------------
// Tuning constants
// ---------------------------------------------------------------------------
static constexpr float DYNAMITE_BLAST_RADIUS      = 80.0f;  // world units
static constexpr float DYNAMITE_AOE_DAMAGE_SCALE  = 0.5f;   // fraction of m_damage applied to splash targets

// ---------------------------------------------------------------------------
// Texture paths - swap in real asset paths once art is finalised.
// ---------------------------------------------------------------------------
static const char* s_textureByType[ProjectilePool::TYPE_COUNT] =
{
    "assets\\firebolt.png",   // ProjectileType::FireBolt
    "assets\\dynamite.png",   // ProjectileType::Dynamite
    "assets\\spear.png",      // ProjectileType::Spear
};

// ===========================================================================
// Construction / Destruction
// ===========================================================================

ProjectilePool::ProjectilePool()
    : m_sprites{}
    , m_activeCount(0)
    , m_freeSlotHint(0)
{
    // Zero-initialise every slot so m_isActive starts false without a manual loop.
    m_pool.fill(Projectile{});
}

ProjectilePool::~ProjectilePool()
{
    // Sprites are owned by the Renderer's TextureManager; do not delete them here.
    // If CreateSprite() allocates uniquely, swap this for explicit deletion.
}

bool ProjectilePool::Initialise(Renderer& renderer)
{
    for (int i = 0; i < TYPE_COUNT; ++i)
    {
        m_sprites[i] = renderer.CreateSprite(s_textureByType[i]);
        if (!m_sprites[i])
        {
            LogManager::GetInstance().Log(
                ("ProjectilePool::Initialise - failed to load sprite: " +
                 std::string(s_textureByType[i])).c_str());
            return false;
        }
    }
    return true;
}

// ===========================================================================
// Spawn  (O(1) amortised)
// ===========================================================================

bool ProjectilePool::Spawn(const Vector2& position, const Vector2& velocity,
                           float damage, float radius, int pierce,
                           ProjectileType type, float lifetime)
{
    // Rotate through the array starting at the last-freed slot.
    // In steady-state the hint lands on a free slot immediately.
    for (int i = 0; i < MAX_PROJECTILES; ++i)
    {
        const int idx = (m_freeSlotHint + i) % MAX_PROJECTILES;

        if (!m_pool[idx].m_isActive)
        {
            Projectile& p       = m_pool[idx];
            p.m_position        = position;
            p.m_velocity        = velocity;
            p.m_damage          = damage;
            p.m_radius          = radius;
            p.m_lifetime        = lifetime;
            p.m_remainingPierce = pierce;
            p.m_type            = type;
            p.m_isActive        = true;

            // Advance hint past this slot so the next Spawn() call starts ahead.
            m_freeSlotHint = (idx + 1) % MAX_PROJECTILES;
            ++m_activeCount;
            return true;
        }
    }

    // Pool fully saturated - soft failure; tune MAX_PROJECTILES if this fires often.
    LogManager::GetInstance().Log("ProjectilePool::Spawn - pool exhausted, projectile dropped.");
    return false;
}

// ===========================================================================
// Update  -  the hot path; called once per frame for every active projectile
// ===========================================================================

void ProjectilePool::Update(float deltaTime,
                            std::vector<std::unique_ptr<Enemy>>& enemies)
{
    m_activeCount = 0;

    for (Projectile& proj : m_pool)
    {
        if (!proj.m_isActive) { continue; }

        // --- 1. Integrate position -------------------------------------------
        proj.m_position += proj.m_velocity * deltaTime;

        // --- 2. Age out -------------------------------------------------------
        proj.m_lifetime -= deltaTime;
        if (proj.m_lifetime <= 0.0f)
        {
            // Dynamite explodes on timeout (reached destination / max range).
            if (proj.m_type == ProjectileType::Dynamite)
            {
                TriggerAoEExplosion(proj.m_position, DYNAMITE_BLAST_RADIUS,
                                    proj.m_damage * DYNAMITE_AOE_DAMAGE_SCALE,
                                    enemies);
            }
            proj.m_isActive = false;
            continue;
        }

        // --- 3. Collision detection against all live enemies ------------------
        ResolveCollisions(proj, enemies);

        if (proj.m_isActive) { ++m_activeCount; }
    }
}

// ===========================================================================
// ResolveCollisions  -  linear sweep; cache-friendly because both arrays are
// contiguous and the enemy pointer vector is short-circuited on pierce == 0.
// ===========================================================================

void ProjectilePool::ResolveCollisions(Projectile& proj,
                                       std::vector<std::unique_ptr<Enemy>>& enemies)
{
    for (const auto& enemyPtr : enemies)
    {
        if (!enemyPtr || !enemyPtr->IsAlive()) { continue; }

        if (!CircleVsCircle(proj.m_position, proj.m_radius,
                            enemyPtr->GetPosition(), enemyPtr->GetRadius()))
        {
            continue;   // no overlap - skip cheaply
        }

        // --- Hit confirmed ---------------------------------------------------
        switch (proj.m_type)
        {
        case ProjectileType::FireBolt:
            enemyPtr->TakeDamage(proj.m_damage);
            proj.m_remainingPierce = 0;     // FireBolt always stops on first hit
            break;

        case ProjectileType::Dynamite:
            // Direct contact: explode immediately with full AoE.
            TriggerAoEExplosion(proj.m_position, DYNAMITE_BLAST_RADIUS,
                                proj.m_damage, enemies);
            proj.m_remainingPierce = 0;
            break;

        case ProjectileType::Spear:
            enemyPtr->TakeDamage(proj.m_damage);
            --proj.m_remainingPierce;       // Spear pierces; counts down per hit
            break;
        }

        if (proj.m_remainingPierce <= 0)
        {
            proj.m_isActive = false;
            return;     // early-out: no further enemies need testing
        }
    }
}

// ===========================================================================
// TriggerAoEExplosion  -  damages every enemy within blastRadius of epicenter
// ===========================================================================

void ProjectilePool::TriggerAoEExplosion(const Vector2& epicenter,
                                         float blastRadius, float damage,
                                         std::vector<std::unique_ptr<Enemy>>& enemies)
{
    for (const auto& enemyPtr : enemies)
    {
        if (!enemyPtr || !enemyPtr->IsAlive()) { continue; }

        if (CircleVsCircle(epicenter, blastRadius,
                           enemyPtr->GetPosition(), enemyPtr->GetRadius()))
        {
            enemyPtr->TakeDamage(damage);
        }
    }
}

// ===========================================================================
// Draw  -  shared sprite moved to each projectile's position; zero allocations
// ===========================================================================

void ProjectilePool::Draw(Renderer& renderer)
{
    for (const Projectile& proj : m_pool)
    {
        if (!proj.m_isActive) { continue; }

        const int typeIdx = static_cast<int>(proj.m_type);
        assert(typeIdx >= 0 && typeIdx < TYPE_COUNT);

        Sprite* sprite = m_sprites[typeIdx];
        if (!sprite) { continue; }

        sprite->SetX(static_cast<int>(proj.m_position.x));
        sprite->SetY(static_cast<int>(proj.m_position.y));
        renderer.DrawSprite(*sprite);
    }
}

// ===========================================================================
// Clear  -  resets all slots; called on wave end / scene reload
// ===========================================================================

void ProjectilePool::Clear()
{
    for (Projectile& proj : m_pool) { proj.m_isActive = false; }
    m_activeCount  = 0;
    m_freeSlotHint = 0;
}

// ===========================================================================
// Collision helpers
// ===========================================================================

// Squared-distance test: avoids sqrt on every projectile-enemy pair.
// The hot inner loop calls this hundreds of times per frame.
bool ProjectilePool::CircleVsCircle(const Vector2& posA, float radiusA,
                                    const Vector2& posB, float radiusB)
{
    const Vector2 delta     = posA - posB;
    const float   combinedR = radiusA + radiusB;
    return delta.LengthSquared() < (combinedR * combinedR);
}

// Projects the circle's centre onto the AABB to find the nearest surface point,
// then tests that squared distance against the circle radius.
// Available for Boss-type enemies that carry a wider rectangular hitbox.
bool ProjectilePool::AABBVsCircle(const Vector2& boxMin, const Vector2& boxMax,
                                  const Vector2& circlePos, float circleRadius)
{
    const float nearestX = std::clamp(circlePos.x, boxMin.x, boxMax.x);
    const float nearestY = std::clamp(circlePos.y, boxMin.y, boxMax.y);
    const float dx       = circlePos.x - nearestX;
    const float dy       = circlePos.y - nearestY;
    return (dx * dx + dy * dy) < (circleRadius * circleRadius);
}
