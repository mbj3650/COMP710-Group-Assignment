// COMP710 GP Framework 2025
// ProjectilePool.h  -  Centralised, cache-friendly Object Pool for all in-flight projectiles.
//
// Design rationale (see docs/TDD_ProjectileSystem.md for full analysis):
//   - One contiguous std::array<Projectile, MAX_PROJECTILES> avoids heap fragmentation.
//   - No individual new/delete calls during gameplay; the pool is pre-allocated at startup.
//   - A free-slot hint (m_freeSlotHint) amortises Spawn() to O(1) in the common case.
//   - Owned by SceneGame; the same reference is passed to each Tower::FireAt() call.
#ifndef __PROJECTILEPOOL_H__
#define __PROJECTILEPOOL_H__

#include "Projectile.h"

#include <array>
#include <memory>
#include <vector>

// Forward declarations - no full headers pulled into translation units that only
// include ProjectilePool.h.
class Renderer;
class Enemy;
class Sprite;

// ---------------------------------------------------------------------------
// ProjectilePool
// ---------------------------------------------------------------------------
class ProjectilePool
{
    // Member methods:
public:
    static constexpr int MAX_PROJECTILES = 512;

    ProjectilePool();
    ~ProjectilePool();

    // Initialise per-type sprites.  Must be called once before Spawn().
    bool Initialise(Renderer& renderer);

    // Activates one dormant slot.  Zero heap allocation; O(1) amortised.
    // Returns false (and logs) only if every slot is occupied - treat as a
    // gameplay tuning signal, not a crash condition.
    bool Spawn(const Vector2& position,
               const Vector2& velocity,
               float          damage,
               float          radius,
               int            pierce,
               ProjectileType type,
               float          lifetime = 5.0f);

    // Integrates motion and resolves hits for every active projectile.
    // Pass the scene's enemy list directly; no copies made.
    void Update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies);

    // Renders each active projectile using its type-indexed shared sprite.
    void Draw(Renderer& renderer);

    // Deactivates all slots (used on scene reset / wave clear).
    void Clear();

    int  GetActiveCount()  const { return m_activeCount;  }
    int  GetCapacity()     const { return MAX_PROJECTILES; }

protected:
private:
    ProjectilePool(const ProjectilePool&)            = delete;
    ProjectilePool& operator=(const ProjectilePool&) = delete;

    // Collision helpers -------------------------------------------------------

    // Circle vs Circle: squared-distance test avoids sqrt on every call.
    static bool CircleVsCircle(const Vector2& posA, float radiusA,
                               const Vector2& posB, float radiusB);

    // AABB vs Circle: projects the circle centre onto the box, then distance check.
    // Available for enemies that carry a rectangular hitbox (e.g., Boss variants).
    static bool AABBVsCircle(const Vector2& boxMin, const Vector2& boxMax,
                             const Vector2& circlePos, float circleRadius);

    // Per-projectile hit resolution; handles pierce countdown and type effects.
    void ResolveCollisions(Projectile& projectile,
                           std::vector<std::unique_ptr<Enemy>>& enemies);

    // Dynamite detonation: damages all enemies within blast radius.
    void TriggerAoEExplosion(const Vector2& epicenter, float blastRadius,
                             float damage,
                             std::vector<std::unique_ptr<Enemy>>& enemies);

    // Member data: ------------------------------------------------------------
    std::array<Projectile, MAX_PROJECTILES> m_pool;     // flat, pre-allocated slab

    // One shared sprite per ProjectileType (indexed by static_cast<int>(type)).
    // Ownership lives here; no per-projectile allocation.
    static constexpr int TYPE_COUNT = 3; // FireBolt, Dynamite, Spear
    Sprite* m_sprites[TYPE_COUNT];

    int m_activeCount;   // live counter for stats / debug overlay
    int m_freeSlotHint;  // rotating hint so Spawn() rarely scans the full array
};

#endif // __PROJECTILEPOOL_H__
