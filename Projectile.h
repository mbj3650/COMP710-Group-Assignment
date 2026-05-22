// COMP710 GP Framework 2025
// Projectile.h  -  Flat, cache-line-friendly projectile record for the Object Pool.
// No heap pointers; all fields are primitive or trivially-copyable types so the
// compiler can pack the pool as a single contiguous array in L1-cache-friendly order.
#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "vector2.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// ProjectileType
// Drives per-projectile behaviour inside ProjectilePool::ResolveCollisions().
// Stored as a single byte to minimise struct padding.
// ---------------------------------------------------------------------------
enum class ProjectileType : uint8_t
{
    FireBolt,   // Torchman  - single-target, immediate damage on first hit
    Dynamite,   // Demoman   - AoE detonation when lifetime expires or on first hit
    Spear,      // Spear Thrower - high-pierce; travels until m_remainingPierce reaches 0
};

// ---------------------------------------------------------------------------
// Projectile
//
// Memory layout (MSVC x64, no extra padding):
//   m_position       8 B  (2 x float)
//   m_velocity       8 B  (2 x float)
//   m_damage         4 B
//   m_radius         4 B
//   m_lifetime       4 B
//   m_remainingPierce 4 B
//   m_type           1 B  (uint8_t enum)
//   m_isActive       1 B
//   implicit pad     2 B
//   ──────────────────────
//   Total           36 B
//
// 512 slots x 36 B = 18 KB  ->  fits entirely within a typical 32 KB L1 data cache.
// ---------------------------------------------------------------------------
struct Projectile
{
    Vector2         m_position;
    Vector2         m_velocity;
    float           m_damage;
    float           m_radius;           // collision radius (world units)
    float           m_lifetime;         // seconds remaining before auto-expiry
    int             m_remainingPierce;  // hits remaining before deactivation
    ProjectileType  m_type;
    bool            m_isActive;         // pool management flag; false = slot is free
};

#endif // __PROJECTILE_H__
