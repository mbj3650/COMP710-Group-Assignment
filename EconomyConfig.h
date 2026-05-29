// EconomyConfig.h
// All the gold / economy numbers live in this one file.
// If you want to rebalance the economy, change it HERE -- don't scatter
// these values around the code, otherwise tuning becomes a nightmare.
//
// Values are the proposed defaults from the economy design doc (v0.1).
// They still need to be checked in playtesting.

#ifndef ECONOMYCONFIG_H
#define ECONOMYCONFIG_H

// -------------------------------------------------------
// Global constants
// -------------------------------------------------------
const int   START_GOLD       = 300;    // enough for 2 basic towers, or 1 mid + change
const int   KILL_BOUNTY_BASE = 6;      // gold for one normal kill on wave 1
const float BOUNTY_DECAY     = 0.90f;  // bounty is multiplied by this each wave
const int   BOUNTY_FLOOR     = 2;      // bounty never drops below this
const float PAY_WINDOW       = 40.0f;  // seconds a wave keeps paying for kills
const int   PATH_PAR         = 25;     // paths with this many tiles or fewer get a bonus

// Short-path reward (paid once when the player finishes their path)
const int   SHORT_PATH_BONUS_PER_TILE = 5;    // gold per tile shorter than PATH_PAR
const int   SHORT_PATH_BONUS_CAP      = 150;  // most you can get from it

// Pickaxe "Gold Striker" upgrade -- flat bonus on a gold-pickaxe kill.
// Flat (no wave decay) so it stays useful in late waves.
const int   GOLD_STRIKER_BONUS         = 5;
const int   GOLD_STRIKER_CHANCE_PERCENT = 15;  // used by the tower system, not here

// -------------------------------------------------------
// Tower prices + upgrade costs (reference for the tower system)
// Tower placement / upgrade code should read these via TrySpend(cost),
// so all the gold values stay in this single file.
//
// Rule: each tower has two upgrade slots costing 75% and 125% of the
// placement price (rounded to the nearest 10), so a fully upgraded tower
// ends up costing 3x its placement price.
// -------------------------------------------------------

// Placement prices
const int TORCHMAN_PRICE       = 100;  // basic, cost-efficient single target
const int SNOWMAN_PRICE        = 130;  // slow support, little/no damage
const int SPEARTHROWER_PRICE   = 170;  // high pierce, good in straight lines
const int PICKAXETHROWER_PRICE = 200;  // arc/boomerang, can turn a profit (Gold Striker)
const int PULSEBOT_PRICE       = 220;  // short range AoE, good on corners
const int DEMOMAN_PRICE        = 320;  // AoE specialist, dynamite

// Upgrade costs -- slot 1 is always the cheaper one (75%), slot 2 is 125%.
const int TORCHMAN_UPGRADE_1       = 80;   const int TORCHMAN_UPGRADE_2       = 130;
const int SNOWMAN_UPGRADE_1        = 100;  const int SNOWMAN_UPGRADE_2        = 160;
const int SPEARTHROWER_UPGRADE_1   = 130;  const int SPEARTHROWER_UPGRADE_2   = 210;
const int PICKAXETHROWER_UPGRADE_1 = 150;  const int PICKAXETHROWER_UPGRADE_2 = 250;
const int PULSEBOT_UPGRADE_1       = 170;  const int PULSEBOT_UPGRADE_2       = 280;
const int DEMOMAN_UPGRADE_1        = 240;  const int DEMOMAN_UPGRADE_2        = 400;

#endif // ECONOMYCONFIG_H
