# Balance & Content Notes

Quick notes on the data added in `assets/info/`. Numbers are first-pass and
still need playtesting, same as the economy values in `EconomyConfig.h`.

## Enemies (`enemy.ini`)

All sprites reuse existing art in `assets/projectiles/` for now (placeholder).
HP is `BonusHealth + 1 + wave * 2`, so `BonusHealth` is a flat bump on top of
the wave scaling in `Enemy::Initialise`.

| Enemy    | Speed | Damage | BonusHealth | Idea                          |
|----------|-------|--------|-------------|-------------------------------|
| Basic    | 1.0   | 1      | 0           | default, unchanged            |
| Runner   | 2.0   | 1      | 0           | fast, low hp, rushes the end  |
| Goblin   | 1.0   | 1      | 2           | slightly chunkier basic       |
| Skeleton | 0.8   | 2      | 5           | slower, hits harder           |
| Brute    | 0.6   | 3      | 10          | slow tank, big damage on leak |

Note: only `Basic` is actually spawned right now (hardcoded in
`SceneGame::Process`). The rest are defined and ready for whoever wires up
wave variety / enemy selection later.

## Projectiles (`projectile.ini`)

| Projectile | Damage | Pierce | Effect | Idea                        |
|------------|--------|--------|--------|-----------------------------|
| Spear      | 2      | 5      | none   | high pierce, straight lines |
| Pulse      | 1      | -1     | none   | short AoE-ish burst         |

## Towers (`tower.ini`)

Renamed the tower IDs to match the design doc (Pathseeker mining theme). The
`Sprite=` fields still point at the existing PNGs in `assets/towers/`, so no
new art is needed and the shop keeps working.

| Old ID    | New ID         | Sprite (unchanged) |
|-----------|----------------|--------------------|
| Shooter   | Torchman       | Shooter            |
| Iceman    | Snowman        | Iceman             |
| Detonator | Demoman        | Detonator          |
| Boomerang | PickaxeThrower | Boomerang          |
| Poisoner  | Poisoner       | Poisoner           |

## Economy (`EconomyConfig.h`)

- `BOUNTY_FLOOR` 2 -> 3 (late waves were too dry).
- Added `WAVE_CLEAR_BONUS`, `BOSS_KILL_BONUS`, `SELL_REFUND_PERCENT` as
  proposed tunables. Not wired into gameplay yet, just keeping the numbers in
  one place.
