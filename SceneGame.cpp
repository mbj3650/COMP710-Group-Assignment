// COMP710 GP Framework 2025
// SceneGame.cpp
// Modified by: MartinYan12138y
// Changes: Game over screen, restart system, FMOD audio,
//          instructions overlay, particle burst effects, enemy HP integration.
//          Gold economy system (kill bounty + decay, pay window,
//          short-path bonus, spend interface for towers).

#include "SceneGame.h"
#include "renderer.h"
#include "sprite.h"
#include "AnimatedSprite.h"
#include "Pathmaker.h"
#include "Tile.h"
#include "Enemy.h"
#include "projectile.h"
#include "Tower.h"
#include "DynamicText.h"
#include "game.h"
#include <cassert>
#include "lib/imgui/imgui.h"
#include <time.h>
#include <cstdlib>
#include <cmath>
#include "inlinehelpers.h"
#include "Tilelist.h"
#include "EconomyConfig.h"
#include "UIShopSlot.h"
#include "UIShopManager.h"
#include "GameData.h"
#include "TowerData.h"
#include "ProjectileData.h"
#include <iostream>
#include <string>
#include <box2d.h>

const int   ENEMIES_PER_WAVE    = 3;
const float SPAWN_INTERVAL      = 1.5f;
const char* FONT_PATH           = "C:\\Windows\\Fonts\\arial.ttf";
const char* GAMEOVER_IMAGE_PATH = "..\\assets\\defeated_gameover.png";
const char* MUSIC_BG_PATH       = "..\\assets\\music_bg.mp3";
const char* SOUND_GAMEOVER_PATH = "..\\assets\\sound_gameover.wav";
const char* SOUND_WAVESTART_PATH= "..\\assets\\sound_wavestart.wav";
const char* EXPLOSION_PATH      = "..\\assets\\explosion.png";

// Text shown in the instructions overlay (one string per line)
const char* INSTRUCTION_LINES[NUM_INSTRUCTION_LINES] = {
    "--- PATHSEEKER ---",
    "",
    "Draw a path from the START tile to the END tile",
    "W / S    Move cursor up / down",
    "A / D    Move cursor left / right",
    "B        Undo last step",
    "ESC      Quit",
    "SPACE    Start game",
    "H        Hide UI"
};

SceneGame::SceneGame()
    : m_pCentre(0), m_angle(0.0f), m_rotationSpeed(1.0f)
{
    m_pRenderer        = 0;
    m_fSpawnTimer      = 0.0f;
    m_fTileSize        = 40.0f;
    m_iLives           = 20;
    m_iWave            = 1;
    m_iEnemiesToSpawn  = ENEMIES_PER_WAVE;
    m_bWaveComplete    = false;
    m_pLivesText       = 0;
    m_pWaveText        = 0;
    m_iGold            = START_GOLD;
    m_pGoldText        = 0;
    m_fWaveTimer       = 0.0f;
    m_bWaveTimerStarted= false;
    m_bGameOver        = false;
    m_pGameOverSprite  = 0;
    m_pRestartText     = 0;
    m_bShowInstructions= true;
    m_pMusicBG         = 0;
    m_pSoundGameOver   = 0;
    m_pSoundWaveStart  = 0;
    m_pMusicChannel    = 0;
    m_pParticleSprite  = 0;
    for (int i = 0; i < NUM_INSTRUCTION_LINES; i++)
        m_pInstructions[i] = 0;
}

SceneGame::~SceneGame()
{
        delete list;
    delete pathmaker;
    delete m_pCentre;
    m_pCentre = 0;

    GameData::Destroy();
    for (int i = 0; i < (int)m_enemies.size(); i++)
    {
        delete m_enemies[i];
        m_enemies[i] = 0;
    }
    m_enemies.clear();

    for (int i = 0; i < (int)m_towers.size(); i++)
    {
        delete m_towers[i];
        m_towers[i] = 0;
    }
    m_towers.clear();

    for (int i = 0; i < (int)m_projectiles.size(); i++)
    {
        delete m_projectiles[i];
        m_projectiles[i] = 0;
    }
    m_projectiles.clear();

    delete m_pLivesText;   m_pLivesText   = 0;
    delete m_pWaveText;    m_pWaveText    = 0;
    delete m_pGoldText;    m_pGoldText    = 0;
    delete m_pGameOverSprite; m_pGameOverSprite = 0;
    delete m_pRestartText; m_pRestartText = 0;
    delete m_pParticleSprite; m_pParticleSprite = 0;

    for (int i = 0; i < NUM_INSTRUCTION_LINES; i++)
    {
        delete m_pInstructions[i];
        m_pInstructions[i] = 0;
    }

    if (m_pMusicBG)        { m_pMusicBG->release();        m_pMusicBG        = 0; }
    if (m_pSoundGameOver)  { m_pSoundGameOver->release();  m_pSoundGameOver  = 0; }
    if (m_pSoundWaveStart) { m_pSoundWaveStart->release(); m_pSoundWaveStart = 0; }

    UIShopManager::DestroyInstance();
    b2DestroyWorld(WorldPointer);
    delete World;
    std::cout << "WORLD DESTROYED\n";
}

bool SceneGame::Initialise(Renderer& renderer)
{
    srand(time(NULL));
    const int W = renderer.GetWidth();
    const int H = renderer.GetHeight();

    // Box2D
    World = new b2WorldDef();
    *World = b2DefaultWorldDef();
    WorldPointer = b2CreateWorld(World);
    b2World_SetGravity(WorldPointer, { 0, 0 });
    ScenesubStepCount = 16;

    list      = new Tilelist();
    pathmaker = new Pathmaker();
    moving    = true;
    columns   = W / 40;
    rows      = H / 40;
    list->Initialise(renderer, rows, columns);
    pathmaker->Initialise(renderer, list->Startpos);

    m_pRenderer = &renderer;
    m_fTileSize = (float)W / (float)columns;

    // Game over image
    m_pGameOverSprite = renderer.CreateSprite(GAMEOVER_IMAGE_PATH);
    if (m_pGameOverSprite)
    {
        float sx = (float)W / m_pGameOverSprite->GetWidth();
        float sy = (float)H / m_pGameOverSprite->GetHeight();
        m_pGameOverSprite->SetScale(sx < sy ? sx : sy);
        m_pGameOverSprite->SetX(W / 2);
        m_pGameOverSprite->SetY(H / 2);
    }


    // Particles -- pre-initialise pool with shared explosion sprite
    m_pParticleSprite = renderer.CreateSprite(EXPLOSION_PATH);
    if (m_pParticleSprite)
    {
        m_pParticleSprite->SetScale(0.3f);
        for (int i = 0; i < PARTICLE_POOL_SIZE; i++)
        {
            m_particlePool[i].Initialise(*m_pParticleSprite);
        }
    }

    // Instructions overlay -- lines spaced 40px apart, centred on screen
    int startY = H / 2 - (NUM_INSTRUCTION_LINES * 40) / 2;
    for (int i = 0; i < NUM_INSTRUCTION_LINES; i++)
    {
        m_pInstructions[i] = new DynamicText();
        m_pInstructions[i]->Initialise(renderer, FONT_PATH, 24, true);
        // Use a placeholder space for blank lines so DynamicText doesn't fail
        const char* text = (INSTRUCTION_LINES[i][0] == '\0') ? " " : INSTRUCTION_LINES[i];
        m_pInstructions[i]->SetText(renderer, text);
        m_pInstructions[i]->SetPosition((float)(W / 2), (float)(startY + i * 40));
    }

    m_pRestartText = new DynamicText();
    m_pRestartText->Initialise(renderer, FONT_PATH, 32, true);
    m_pRestartText->SetText(renderer, "Press R to Restart");
    m_pRestartText->SetPosition((float)(W / 2), (float)(H - 80));
    // HUD
    m_pLivesText = new DynamicText();
    m_pLivesText->Initialise(renderer, FONT_PATH, 24, false);
    m_pLivesText->SetText(renderer, "Lives: 20");
    m_pLivesText->SetPosition(16, 16);

    m_pWaveText = new DynamicText();
    m_pWaveText->Initialise(renderer, FONT_PATH, 24, false);
    m_pWaveText->SetText(renderer, "Wave: 1");
    m_pWaveText->SetPosition(16, 48);

    // Gold counter -- sits just under the lives/wave HUD
    m_pGoldText = new DynamicText();
    m_pGoldText->Initialise(renderer, FONT_PATH, 24, false);
    m_pGoldText->SetText(renderer, "Gold: " + std::to_string(m_iGold));
    m_pGoldText->SetPosition(16, 80);

    // Tower and UI
    GameData::Get().Initialise();
	UIShopManager().GetInstance().Initialise(renderer);
    // FMOD
    FMOD::System* fmod = Game::GetInstance().GetSoundSystem();
    if (fmod)
    {
        fmod->createSound(MUSIC_BG_PATH, FMOD_LOOP_NORMAL | FMOD_CREATESTREAM, 0, &m_pMusicBG);
        fmod->createSound(SOUND_GAMEOVER_PATH, FMOD_DEFAULT, 0, &m_pSoundGameOver);
        fmod->createSound(SOUND_WAVESTART_PATH, FMOD_DEFAULT, 0, &m_pSoundWaveStart);
        if (m_pMusicBG) fmod->playSound(m_pMusicBG, 0, false, &m_pMusicChannel);
    }

    return true;
}

// -------------------------------------------------------
// SpawnBurst: activates a burst of particles at (x, y)
// -------------------------------------------------------
void SceneGame::SpawnBurst(float x, float y)
{
    if (!m_pParticleSprite) return;

    int spawned = 0;
    for (int i = 0; i < PARTICLE_POOL_SIZE && spawned < 8; i++)
    {
        if (!m_particlePool[i].m_bAlive)
        {
            // Random outward direction
            float angle = ((float)rand() / RAND_MAX) * 6.2832f; // 0 to 2pi
            float speed = 60.0f + ((float)rand() / RAND_MAX) * 80.0f;

            m_particlePool[i].m_bAlive = true;
            m_particlePool[i].m_fCurrentAge = 0.0f;
            m_particlePool[i].m_fMaxLifespan = 0.5f + ((float)rand() / RAND_MAX) * 0.3f;
            m_particlePool[i].m_postion = { x, y };
            m_particlePool[i].m_velocity = { cosf(angle) * speed, sinf(angle) * speed };
            m_particlePool[i].m_acceleration = { 0.0f, 40.0f }; // slight gravity
            m_particlePool[i].m_fColour[0] = 1.0f;            // red-orange tint
            m_particlePool[i].m_fColour[1] = 0.5f;
            m_particlePool[i].m_fColour[2] = 0.0f;
            spawned++;
        }
    }
}

// -------------------------------------------------------
// Economy (Gold) helpers
// All the actual numbers live in EconomyConfig.h -- tune balance there.
// -------------------------------------------------------

// Gold given for killing one normal enemy on the given wave.
// The reward shrinks a bit every wave (BOUNTY_DECAY) but never goes under
// BOUNTY_FLOOR. Without the decay the late waves would print way too much
// money, since the enemy count keeps climbing each wave.
int SceneGame::KillBounty(int wave) const
{
    float reward = KILL_BOUNTY_BASE * powf(BOUNTY_DECAY, (float)(wave - 1));
    int   rounded = (int)(reward + 0.5f); // round to nearest
    if (rounded < BOUNTY_FLOOR) rounded = BOUNTY_FLOOR;
    return rounded;
}

// Kills only pay out for the first PAY_WINDOW seconds of a wave.
// If a player draws a really long path, enemies take longer to clear, so they
// run out of paying time -- long paths end up punishing themselves (this is
// the "tunnel" problem from the design doc).
bool SceneGame::IsPayWindowOpen() const
{
    if (!m_bWaveTimerStarted) return true; // window hasn't started yet
    return m_fWaveTimer < PAY_WINDOW;
}

void SceneGame::RefreshGoldText()
{
    if (m_pGoldText && m_pRenderer)
    {
        m_pGoldText->SetText(*m_pRenderer, "Gold: " + std::to_string(m_iGold));
        m_iGoldPrev = m_iGold;
    }
}

// Towers / upgrades call this to pay for things. If the player can't afford it
// nothing is bought and we return false.
bool SceneGame::TrySpend(int cost)
{
    if (cost < 0)       return false;
    if (m_iGold < cost) return false;
    m_iGoldPrev = m_iGold;
    m_iGold -= cost;
    RefreshGoldText();
    return true;
}

// Generic reward -- relics, events, anything that just hands the player gold.
void SceneGame::AddGold(int amount)
{
    if (amount <= 0) return;
    m_iGoldPrev = m_iGold;
    m_iGold += amount;
    RefreshGoldText();
}

// Pickaxe "Gold Striker" upgrade: a gold-pickaxe kill gives a flat bonus.
// It's flat (no wave decay) so it stays worth taking late game, but it still
// has to be inside the pay window so you can't farm gold by stalling a wave.
void SceneGame::AddGoldStrikeBonus()
{
    if (!IsPayWindowOpen()) return;
    m_iGoldPrev = m_iGold;
    m_iGold += GOLD_STRIKER_BONUS;
    RefreshGoldText();
}

void SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{
    // --- Instructions overlay ---
    if (m_bShowInstructions)
    {
        if (inputSystem.GetKeyState(SDL_SCANCODE_SPACE) == BS_PRESSED)
        {
            m_bShowInstructions = false;
        }
        return; // hold everything until instructions are dismissed
    }
    // H to toggle shop/upgrades UI
    if (inputSystem.GetKeyState(SDL_SCANCODE_H) == BS_PRESSED)
    {
        m_bHideUI = !m_bHideUI;
	}
    // --- Game over ---
    if (m_bGameOver)
    {
        if (inputSystem.GetKeyState(SDL_SCANCODE_R) == BS_PRESSED)
        {
            RestartGame(*m_pRenderer);
        }
        return;
    }

    // --- Particles ---
    for (int i = 0; i < PARTICLE_POOL_SIZE; i++)
    {
        if (m_particlePool[i].m_bAlive)
            m_particlePool[i].Process(deltaTime);
    }

    // --- Normal game ---
    list->Process(deltaTime, inputSystem);

    if (moving)
    {
        // MovePosition(xoffset, yoffset): xoffset moves COLUMN (x), yoffset moves ROW (y).
        if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) MovePosition(0, -1); // up    = row - 1
        else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED) MovePosition(0, 1);  // down  = row + 1
        else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED) MovePosition(-1, 0); // left  = col - 1
        else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED) MovePosition(1, 0);  // right = col + 1

        if (inputSystem.GetKeyState(SDL_SCANCODE_B) == BS_PRESSED)
        {
            // Undo() returns a Tile::Position, which now uses the SAME convention
            // (x = column, y = row), so no more flipping is needed.
            pathmaker->pos = list->Undo();
        }
       
      
    }
    else
    {
        // tower interactions
        if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED) {
            // try placing tower
            if (list->Hovered->hastower == false && !list->Hovered->isPath && UIShopManager::GetInstance().IsTowerSelected() && !UIShopManager::GetInstance().IsAnyElementHovered(inputSystem)) {
                if (TrySpend(GameData::Get().Tower[UIShopManager::GetInstance().GetSelectedTowerType()].Price))
                {
                    Tower* newTower = new Tower();
                    newTower->Initialise(*m_pRenderer, list->Hovered, m_fTileSize, WorldPointer, m_projectiles, UIShopManager::GetInstance().GetSelectedTowerType());
                    m_towers.push_back(newTower);
					UIShopManager::GetInstance().UpdateSelection(-1); // deselect
                    UIShopManager::GetInstance().SetSidepanelTower(*m_pRenderer, newTower);
                }
                else
                {
					cout << "Not enough gold to build " << UIShopManager::GetInstance().GetSelectedTowerType() << "\n";
                }
            }
            else if (list->Hovered->hastower)
            {
                std::vector<Tower*>::iterator iter = std::find_if(m_towers.begin(), m_towers.end(), [this](Tower* tower) {
                    return tower->GetCurrentTile() == list->Hovered; // return tower's curerent tile is same as hovered
                });
                if (iter != m_towers.end()) UIShopManager::GetInstance().SetSidepanelTower(*m_pRenderer, *iter); // making sure it was found
            }
            else if (!list->Hovered->hastower && !UIShopManager::GetInstance().IsTowerSelected() && !UIShopManager::GetInstance().IsAnyElementHovered(inputSystem))
            {
                // Deselects sidepanel if you clicked on anything
                UIShopManager::GetInstance().SetSidepanelTower(*m_pRenderer, 0);
            }
        }

        if (m_iGold != m_iGoldPrev)
        {
            RefreshGoldText();
        }

        UIShopManager::GetInstance().Process(deltaTime, inputSystem, &m_iGold);
        m_fSpawnTimer += deltaTime;
        b2World_Step(WorldPointer, deltaTime, ScenesubStepCount);

        // Once enemies start spawning, count down the pay window for this wave.
        if (m_bWaveTimerStarted) m_fWaveTimer += deltaTime;

        if (m_iEnemiesToSpawn > 0 && m_fSpawnTimer >= SPAWN_INTERVAL && m_pRenderer)
        {
            m_fSpawnTimer = 0.0f;
            Enemy* e = new Enemy();
            e->Initialise(*m_pRenderer, list->GetStart(), m_fTileSize, WorldPointer, m_iWave, "Basic");
            m_enemies.push_back(e);
            m_iEnemiesToSpawn--;

            // Start the pay window timer on the first enemy of the wave.
            if (!m_bWaveTimerStarted) m_bWaveTimerStarted = true;
        }

        for (int i = (int)m_towers.size() - 1; i >= 0; i--)//tower process
        {
            // Selling
            if (m_towers[i]->IsSold())
            {
                m_towers[i]->GetCurrentTile()->hastower = false;
                AddGold(m_towers[i]->GetSellValue()); // refund half the tower's price
                delete m_towers[i];
                m_towers.erase(m_towers.begin() + i);
				continue;
            }
            m_towers[i]->Process(deltaTime);
        }


        for (int i = (int)m_projectiles.size() - 1; i >= 0; i--)//tower process
        {
            m_projectiles[i]->Process(deltaTime);


            if (!m_projectiles.at(i)->GetAlive()) {//if projectile is dead
                delete m_projectiles[i];
                m_projectiles.erase(m_projectiles.begin() + i);
                continue;
            }
        }

        for (int i = (int)m_enemies.size() - 1; i >= 0; i--)
        {
            m_enemies[i]->Process(deltaTime);

            // Enemy killed by a tower (HP reached 0)
            if (m_enemies[i]->IsDead())
            {
                SpawnBurst(m_enemies[i]->GetX(), m_enemies[i]->GetY());

                // Reward gold for the kill -- but only while the pay window is
                // still open (see IsPayWindowOpen). Note: boss enemies on every
                // 10th wave are meant to drop a Relic instead of gold, so the
                // boss/relic system should skip this for the boss itself.
                if (IsPayWindowOpen())
                {
                    m_iGold += KillBounty(m_iWave);
                    RefreshGoldText();
                }

                delete m_enemies[i];
                m_enemies.erase(m_enemies.begin() + i);
                continue;
            }

            // Enemy reached the end -- lose a life
            if (m_enemies[i]->HasReachedEnd())
            {
                SpawnBurst(m_enemies[i]->GetX(), m_enemies[i]->GetY());
                m_iLives-= m_enemies[i]->GetDamage();
                delete m_enemies[i];
                m_enemies.erase(m_enemies.begin() + i);
                
                m_pLivesText->SetText(*m_pRenderer, "Lives: " + std::to_string(m_iLives));

                if (m_iLives <= 0)
                {
                    m_bGameOver = true;
                    std::cout << "GAME OVER\n";
                    if (m_pMusicChannel) { m_pMusicChannel->stop(); m_pMusicChannel = 0; }
                    FMOD::System* fmod = Game::GetInstance().GetSoundSystem();
                    if (fmod && m_pSoundGameOver) fmod->playSound(m_pSoundGameOver, 0, false, 0);
                    return;
                }
            }
        }

        if (m_iEnemiesToSpawn == 0 && m_enemies.empty())
        {
            m_iWave++;
            m_iEnemiesToSpawn = m_iWave * ENEMIES_PER_WAVE;
            m_fSpawnTimer = 0.0f;
            m_pWaveText->SetText(*m_pRenderer, "Wave: " + std::to_string(m_iWave));
            std::cout << "Wave " << m_iWave << " starting!\n";

            // Fresh pay window for the new wave.
            m_fWaveTimer        = 0.0f;
            m_bWaveTimerStarted = false;

            FMOD::System* fmod = Game::GetInstance().GetSoundSystem();
            if (fmod && m_pSoundWaveStart) fmod->playSound(m_pSoundWaveStart, 0, false, 0);
        }
    }
}

bool SceneGame::MovePosition(int xoffset, int yoffset)
{
    Vector2 position = pathmaker->pos;

    // x = column (bounded by columns), y = row (bounded by rows)
    if ((position.x + xoffset >= columns) || (position.x + xoffset < 0)
    ||  (position.y + yoffset < 0)        || (position.y + yoffset >= rows))
        return false;

    if (list->GetTile({ pathmaker->pos.x + xoffset, pathmaker->pos.y + yoffset })->isPath)
        return false;

    Tile* cur  = list->GetTile(pathmaker->pos);
    pathmaker->pos.x += xoffset;
    pathmaker->pos.y += yoffset;
    Tile* next = list->GetTile(pathmaker->pos);
    cur->setNext(next);
    next->setPrevious(cur);
    next->setPath();
    list->path.push_back(next);

    if (list->isEnd(pathmaker->pos))
    {
        moving = false;

        // Short-path reward (paid once, the moment the path is finished).
        // The fewer tiles the player used, the bigger the one-off bonus. This
        // rewards efficient paths and works alongside the pay window to make
        // long "tunnel" paths a bad idea.
        int pathTiles = (int)list->path.size();
        int bonus     = (PATH_PAR - pathTiles) * SHORT_PATH_BONUS_PER_TILE;
        if (bonus < 0)                    bonus = 0;
        if (bonus > SHORT_PATH_BONUS_CAP) bonus = SHORT_PATH_BONUS_CAP;
        AddGold(bonus);
    }
    return true;
}

void SceneGame::Draw(Renderer& renderer)
{

    // Instructions overlay
    if (m_bShowInstructions)
    {
        list->Draw(renderer); // show the grid behind the text
        for (int i = 0; i < NUM_INSTRUCTION_LINES; i++)
        {
            if (m_pInstructions[i]) m_pInstructions[i]->Draw(renderer);
        }
        return;
    }

    // Game over screen
    if (m_bGameOver)
    {
        if (m_pGameOverSprite) m_pGameOverSprite->Draw(renderer);
        if (m_pRestartText)    m_pRestartText->Draw(renderer);
        return;
    }

    // Normal game
    list->Draw(renderer);

    for (int i = 0; i < (int)m_enemies.size(); i++)
        m_enemies[i]->Draw(renderer);


    for (int i = 0; i < (int)m_towers.size(); i++)
        m_towers[i]->Draw(renderer);

    for (int i = 0; i < (int)m_projectiles.size(); i++)
        m_projectiles[i]->Draw(renderer);
    // Particles drawn on top of entities
    for (int i = 0; i < PARTICLE_POOL_SIZE; i++)
        if (m_particlePool[i].m_bAlive) m_particlePool[i].Draw(renderer);

    

    if (!moving)
    {
        m_pLivesText->Draw(renderer);
        m_pWaveText->Draw(renderer);
        m_pGoldText->Draw(renderer);
        if (!m_bHideUI)
        {
            UIShopManager::GetInstance().Draw(renderer);
        }
    }
}

void SceneGame::DebugDraw()
{
    list->DebugDraw();

    ImGui::Text("Scene: Grid");
    ImGui::InputFloat("Rotation speed", &m_rotationSpeed);
    ImGui::SliderInt("Start row",    &x, 0, rows    - 1, "%d");
    ImGui::SliderInt("Start column", &y, 0, columns - 1, "%d");
    list->GetTile(y, x)->isPath = true; // GetTile(column, row): y is the column slider, x the row slider

    ImGui::Text("Lives: %d",             m_iLives);
    ImGui::Text("Wave: %d",              m_iWave);
    ImGui::Text("Gold: %d",              m_iGold);
    ImGui::Text("Pay window: %s (%.1fs)", IsPayWindowOpen() ? "OPEN" : "CLOSED", m_fWaveTimer);
    ImGui::Text("Enemies to spawn: %d",  m_iEnemiesToSpawn);
    ImGui::Text("Enemies on screen: %d", (int)m_enemies.size());
    ImGui::Text("Game Over: %s",         m_bGameOver ? "YES" : "NO");
    ImGui::Text("Instructions: %s",      m_bShowInstructions ? "ON" : "OFF");

    if (ImGui::Button("Force Game Over"))
    {
        m_iLives = 0; m_bGameOver = true;
        if (m_pMusicChannel) { m_pMusicChannel->stop(); m_pMusicChannel = 0; }
        FMOD::System* fmod = Game::GetInstance().GetSoundSystem();
        if (fmod && m_pSoundGameOver) fmod->playSound(m_pSoundGameOver, 0, false, 0);
    }
    if (ImGui::Button("Skip Instructions"))
    {
        m_bShowInstructions = false;
    }
    if (ImGui::Button("Test Particles"))
    {
        SpawnBurst(640.0f, 360.0f);
    }
    if (ImGui::Button("Add 100 Gold"))
    {
        AddGold(100);
    }
    if (ImGui::Button("Hide UI"))
    {
        m_bHideUI = !m_bHideUI;
    }
}

void SceneGame::RestartGame(Renderer& renderer)
{
    std::cout << "Restarting...\n";

    for (int i = 0; i < (int)m_enemies.size(); i++) { delete m_enemies[i]; m_enemies[i] = 0; }
    m_enemies.clear();
    for (int i = 0; i < (int)m_towers.size(); i++) { delete m_towers[i]; m_towers[i] = 0; }
    m_towers.clear();
    for (int i = 0; i < (int)m_projectiles.size(); i++) { delete m_projectiles[i]; m_projectiles[i] = 0; }
    m_projectiles.clear();

    b2DestroyWorld(WorldPointer);
    delete World;
    World = new b2WorldDef();
    *World = b2DefaultWorldDef();
    WorldPointer = b2CreateWorld(World);
    b2World_SetGravity(WorldPointer, { 0, 0 });

    delete list;    list    = new Tilelist();  list->Initialise(renderer, rows, columns);
    delete pathmaker; pathmaker = new Pathmaker(); pathmaker->Initialise(renderer, list->Startpos);

    m_iLives           = 20;
    m_iWave            = 1;
    m_iEnemiesToSpawn  = ENEMIES_PER_WAVE;
    m_bWaveComplete    = false;
    m_fSpawnTimer      = 0.0f;
    moving             = true;
    m_bGameOver        = false;
    m_bShowInstructions= true; // show instructions again on restart

    // Reset the economy too -- the old restart only reset lives/wave and would
    // otherwise leave the player with their leftover gold from last run.
    m_iGold            = START_GOLD;
    m_fWaveTimer       = 0.0f;
    m_bWaveTimerStarted= false;

    m_pLivesText->SetText(renderer, "Lives: 20");
    m_pWaveText->SetText(renderer,  "Wave: 1");
    m_pGoldText->SetText(renderer,  "Gold: " + std::to_string(START_GOLD));

    // Kill all particles
    for (int i = 0; i < PARTICLE_POOL_SIZE; i++) m_particlePool[i].m_bAlive = false;

    FMOD::System* fmod = Game::GetInstance().GetSoundSystem();
    if (fmod && m_pMusicBG) fmod->playSound(m_pMusicBG, 0, false, &m_pMusicChannel);

    std::cout << "Game restarted.\n";
}
