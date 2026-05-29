// COMP710 GP Framework 2025
// SceneGame.cpp
// Modified by: MartinYan12138y
// Changes: Game over screen, restart system, FMOD audio,
//          instructions overlay, particle burst effects, enemy HP integration.

#include "SceneGame.h"
#include "renderer.h"
#include "sprite.h"
#include "AnimatedSprite.h"
#include "Pathmaker.h"
#include "Tile.h"
#include "Enemy.h"
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
    "SPACE    Start game"
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

    delete m_pLivesText;   m_pLivesText   = 0;
    delete m_pWaveText;    m_pWaveText    = 0;
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
        if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) MovePosition(-1, 0);
        else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED) MovePosition(1, 0);
        else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED) MovePosition(0, -1);
        else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED) MovePosition(0, 1);

        if (inputSystem.GetKeyState(SDL_SCANCODE_B) == BS_PRESSED)
        {
            Vector2 pos = list->Undo();
            pathmaker->pos.x = pos.y;//ok so i kind of goofed up with the implementation here where the pathmaker x and y are flipped and wrong
            pathmaker->pos.y = pos.x;
        }
        if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED) {
            if (list->Hovered->hastower == false) {
                Tower* newTower = new Tower();
                newTower->Initialise(*m_pRenderer, list->Hovered, m_fTileSize, WorldPointer);
                m_towers.push_back(newTower);
            }
        }
      
    }
    else
    {

       
        m_fSpawnTimer += deltaTime;
        b2World_Step(WorldPointer, deltaTime, ScenesubStepCount);

        if (m_iEnemiesToSpawn > 0 && m_fSpawnTimer >= SPAWN_INTERVAL && m_pRenderer)
        {
            m_fSpawnTimer = 0.0f;
            Enemy* e = new Enemy();
            e->Initialise(*m_pRenderer, list->GetStart(), m_fTileSize, WorldPointer, m_iWave);
            m_enemies.push_back(e);
            m_iEnemiesToSpawn--;
        }

        for (int i = (int)m_towers.size() - 1; i >= 0; i--)//tower process
        {
            m_towers[i]->Process(deltaTime);
        }

        for (int i = (int)m_enemies.size() - 1; i >= 0; i--)
        {
            m_enemies[i]->Process(deltaTime);

            // Enemy killed by a tower (HP reached 0)
            if (m_enemies[i]->IsDead())
            {
                SpawnBurst(m_enemies[i]->GetX(), m_enemies[i]->GetY());
                delete m_enemies[i];
                m_enemies.erase(m_enemies.begin() + i);
                continue;
            }

            // Enemy reached the end -- lose a life
            if (m_enemies[i]->HasReachedEnd())
            {
                SpawnBurst(m_enemies[i]->GetX(), m_enemies[i]->GetY());
                delete m_enemies[i];
                m_enemies.erase(m_enemies.begin() + i);
                m_iLives--;
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

            FMOD::System* fmod = Game::GetInstance().GetSoundSystem();
            if (fmod && m_pSoundWaveStart) fmod->playSound(m_pSoundWaveStart, 0, false, 0);
        }
    }
}

bool SceneGame::MovePosition(int xoffset, int yoffset)
{
    Vector2 position = pathmaker->pos;

    if ((position.x + xoffset >= rows) || (position.x + xoffset < 0)
    ||  (position.y + yoffset < 0)     || (position.y + yoffset >= columns))
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

    if (list->isEnd(pathmaker->pos)) moving = false;
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
    // Particles drawn on top of enemies
    for (int i = 0; i < PARTICLE_POOL_SIZE; i++)
        if (m_particlePool[i].m_bAlive) m_particlePool[i].Draw(renderer);

    if (!moving)
    {
        m_pLivesText->Draw(renderer);
        m_pWaveText->Draw(renderer);
    }
}

void SceneGame::DebugDraw()
{
    list->DebugDraw();

    ImGui::Text("Scene: Grid");
    ImGui::InputFloat("Rotation speed", &m_rotationSpeed);
    ImGui::SliderInt("Start row",    &x, 0, rows    - 1, "%d");
    ImGui::SliderInt("Start column", &y, 0, columns - 1, "%d");
    list->GetTile(x, y)->isPath = true;

    ImGui::Text("Lives: %d",             m_iLives);
    ImGui::Text("Wave: %d",              m_iWave);
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
}

void SceneGame::RestartGame(Renderer& renderer)
{
    std::cout << "Restarting...\n";

    for (int i = 0; i < (int)m_enemies.size(); i++) { delete m_enemies[i]; m_enemies[i] = 0; }
    m_enemies.clear();

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

    m_pLivesText->SetText(renderer, "Lives: 20");
    m_pWaveText->SetText(renderer,  "Wave: 1");

    // Kill all particles
    for (int i = 0; i < PARTICLE_POOL_SIZE; i++) m_particlePool[i].m_bAlive = false;

    FMOD::System* fmod = Game::GetInstance().GetSoundSystem();
    if (fmod && m_pMusicBG) fmod->playSound(m_pMusicBG, 0, false, &m_pMusicChannel);

    std::cout << "Game restarted.\n";
}
