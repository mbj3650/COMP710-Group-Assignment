// COMP710 GP Framework 2025
// SceneGame.cpp
// Modified by: MartinYan12138y
// Changes: Added game over detection, game over screen (defeated_gameover.png),
//          full restart system, and FMOD background music + sound effects.

// This include:
#include "SceneGame.h"
// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "AnimatedSprite.h"
#include "Pathmaker.h"
#include "Tile.h"
#include "Enemy.h"
#include "DynamicText.h"
#include "game.h"
// Library includes:
#include <cassert>
#include "lib/imgui/imgui.h"
#include <time.h>
#include "inlinehelpers.h"
#include "Tilelist.h"
#include <iostream>
#include <string>
#include <box2d.h>

// Each wave spawns this many enemies per wave level
// wave 1 = 3 enemies, wave 2 = 6 enemies, and so on
const int ENEMIES_PER_WAVE = 3;

// Seconds between each enemy spawn
const float SPAWN_INTERVAL = 1.5f;

// Windows system font for HUD text
const char* FONT_PATH = "C:\\Windows\\Fonts\\arial.ttf";

// -------------------------------------------------------
// Asset paths -- place these files in your assets/ folder
// -------------------------------------------------------
const char* GAMEOVER_IMAGE_PATH  = "..\\assets\\defeated_gameover.png";
const char* MUSIC_BG_PATH        = "..\\assets\\music_bg.mp3";
const char* SOUND_GAMEOVER_PATH  = "..\\assets\\sound_gameover.wav";
const char* SOUND_WAVESTART_PATH = "..\\assets\\sound_wavestart.wav";

SceneGame::SceneGame()
    : m_pCentre(0)
    , m_angle(0.0f)
    , m_rotationSpeed(1.0f)
{
    m_pRenderer       = 0;
    m_fSpawnTimer     = 0.0f;
    m_fTileSize       = 40.0f;

    m_iLives          = 20;
    m_iWave           = 1;
    m_iEnemiesToSpawn = ENEMIES_PER_WAVE;
    m_bWaveComplete   = false;

    m_pLivesText      = 0;
    m_pWaveText       = 0;

    m_bGameOver       = false;
    m_pGameOverSprite = 0;
    m_pRestartText    = 0;

    // Sound pointers start null; loaded in Initialise
    m_pMusicBG        = 0;
    m_pSoundGameOver  = 0;
    m_pSoundWaveStart = 0;
    m_pMusicChannel   = 0;
}

SceneGame::~SceneGame()
{
    delete list;
    delete m_pCentre;
    delete pathmaker;
    m_pCentre = 0;

    for (int i = 0; i < (int)m_enemies.size(); i++)
    {
        delete m_enemies[i];
        m_enemies[i] = 0;
    }
    m_enemies.clear();

    delete m_pLivesText;
    m_pLivesText = 0;

    delete m_pWaveText;
    m_pWaveText = 0;

    delete m_pGameOverSprite;
    m_pGameOverSprite = 0;

    delete m_pRestartText;
    m_pRestartText = 0;

    // Release FMOD sound objects
    // The FMOD::System itself is owned by Game and released there
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
    const int SCREEN_WIDTH  = renderer.GetWidth();
    const int SCREEN_HEIGHT = renderer.GetHeight();

    // Box2D setup
    World = new b2WorldDef();
    *World = b2DefaultWorldDef();
    WorldPointer = b2CreateWorld(World);
    b2World_SetGravity(WorldPointer, { 0, 0 });
    ScenesubStepCount = 16;

    list      = new Tilelist();
    pathmaker = new Pathmaker();
    moving    = true;
    columns   = SCREEN_WIDTH  / 40;
    rows      = SCREEN_HEIGHT / 40;
    list->Initialise(renderer, rows, columns);
    pathmaker->Initialise(renderer, list->Startpos);

    m_pRenderer = &renderer;
    m_fTileSize = (float)SCREEN_WIDTH / (float)columns;

    // HUD text
    m_pLivesText = new DynamicText();
    m_pLivesText->Initialise(renderer, FONT_PATH, 24, false);
    m_pLivesText->SetText(renderer, "Lives: 20");
    m_pLivesText->SetPosition(16, 16);

    m_pWaveText = new DynamicText();
    m_pWaveText->Initialise(renderer, FONT_PATH, 24, false);
    m_pWaveText->SetText(renderer, "Wave: 1");
    m_pWaveText->SetPosition(16, 48);

    // Game over image
    m_pGameOverSprite = renderer.CreateSprite(GAMEOVER_IMAGE_PATH);
    if (m_pGameOverSprite)
    {
        float scaleX = (float)SCREEN_WIDTH  / (float)m_pGameOverSprite->GetWidth();
        float scaleY = (float)SCREEN_HEIGHT / (float)m_pGameOverSprite->GetHeight();
        float scale  = (scaleX < scaleY) ? scaleX : scaleY;
        m_pGameOverSprite->SetScale(scale);
        m_pGameOverSprite->SetX(SCREEN_WIDTH  / 2);
        m_pGameOverSprite->SetY(SCREEN_HEIGHT / 2);
    }

    // "Press R to Restart" prompt
    m_pRestartText = new DynamicText();
    m_pRestartText->Initialise(renderer, FONT_PATH, 32, true);
    m_pRestartText->SetText(renderer, "Press R to Restart");
    m_pRestartText->SetPosition((float)(SCREEN_WIDTH / 2), (float)(SCREEN_HEIGHT - 80));

    // -------------------------------------------------------
    // FMOD: load sounds
    // Get the FMOD system from Game so we share the same instance
    // -------------------------------------------------------
    FMOD::System* fmodSystem = Game::GetInstance().GetSoundSystem();

    if (fmodSystem)
    {
        // Background music: loops continuously
        fmodSystem->createSound(MUSIC_BG_PATH, FMOD_LOOP_NORMAL | FMOD_CREATESTREAM, 0, &m_pMusicBG);

        // Game over sting: plays once
        fmodSystem->createSound(SOUND_GAMEOVER_PATH, FMOD_DEFAULT, 0, &m_pSoundGameOver);

        // Wave start jingle: plays once per wave
        fmodSystem->createSound(SOUND_WAVESTART_PATH, FMOD_DEFAULT, 0, &m_pSoundWaveStart);

        // Start background music immediately, keep channel reference so we can stop it later
        if (m_pMusicBG)
        {
            fmodSystem->playSound(m_pMusicBG, 0, false, &m_pMusicChannel);
            std::cout << "Background music started\n";
        }
    }
    else
    {
        std::cout << "Warning: FMOD system not available, sounds will not play\n";
    }

    return true;
}

void SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{
    // -------------------------------------------------------
    // GAME OVER STATE: only listen for restart key
    // -------------------------------------------------------
    if (m_bGameOver)
    {
        if (inputSystem.GetKeyState(SDL_SCANCODE_R) == BS_PRESSED)
        {
            RestartGame(*m_pRenderer);
        }
        return;
    }

    // -------------------------------------------------------
    // NORMAL GAME LOGIC
    // -------------------------------------------------------
    list->Process(deltaTime);

    if (moving)
    {
        if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED)
        {
            std::cout << "w\n";
            MovePosition(-1, 0);
        }
        else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED)
        {
            std::cout << "s\n";
            MovePosition(1, 0);
        }
        else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED)
        {
            std::cout << "a\n";
            MovePosition(0, -1);
        }
        else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED)
        {
            std::cout << "d\n";
            MovePosition(0, 1);
        }

        if (inputSystem.GetKeyState(SDL_SCANCODE_B) == BS_PRESSED)
        {
            std::cout << "undo\n";
            Vector2 pos = list->Undo();
            pathmaker->pos.x = pos.x;
            pathmaker->pos.y = pos.y;
        }
    }
    else
    {
        m_fSpawnTimer += deltaTime;

        b2World_Step(WorldPointer, deltaTime, ScenesubStepCount);

        if (m_iEnemiesToSpawn > 0 && m_fSpawnTimer >= SPAWN_INTERVAL && m_pRenderer != 0)
        {
            m_fSpawnTimer = 0.0f;

            Enemy* newEnemy = new Enemy();
            newEnemy->Initialise(*m_pRenderer, list->GetStart(), m_fTileSize, WorldPointer);
            m_enemies.push_back(newEnemy);

            m_iEnemiesToSpawn--;
            std::cout << "Spawned enemy, " << m_iEnemiesToSpawn << " left this wave\n";
        }

        for (int i = (int)m_enemies.size() - 1; i >= 0; i--)
        {
            m_enemies[i]->Process(deltaTime);

            if (m_enemies[i]->HasReachedEnd())
            {
                delete m_enemies[i];
                m_enemies.erase(m_enemies.begin() + i);
                m_iLives--;
                std::cout << "Enemy got through! Lives left: " << m_iLives << "\n";

                m_pLivesText->SetText(*m_pRenderer, "Lives: " + std::to_string(m_iLives));

                if (m_iLives <= 0)
                {
                    m_bGameOver = true;
                    std::cout << "GAME OVER\n";

                    // Stop background music and play the game over sound
                    if (m_pMusicChannel)
                    {
                        m_pMusicChannel->stop();
                        m_pMusicChannel = 0;
                    }
                    FMOD::System* fmodSystem = Game::GetInstance().GetSoundSystem();
                    if (fmodSystem && m_pSoundGameOver)
                    {
                        fmodSystem->playSound(m_pSoundGameOver, 0, false, 0);
                    }
                    return;
                }
            }
        }

        // Wave complete: start the next wave
        if (m_iEnemiesToSpawn == 0 && m_enemies.empty())
        {
            m_iWave++;
            m_iEnemiesToSpawn = m_iWave * ENEMIES_PER_WAVE;
            m_fSpawnTimer     = 0.0f;

            std::cout << "Wave " << m_iWave << " starting! Enemies: " << m_iEnemiesToSpawn << "\n";
            m_pWaveText->SetText(*m_pRenderer, "Wave: " + std::to_string(m_iWave));

            // Play the wave start sound
            FMOD::System* fmodSystem = Game::GetInstance().GetSoundSystem();
            if (fmodSystem && m_pSoundWaveStart)
            {
                fmodSystem->playSound(m_pSoundWaveStart, 0, false, 0);
            }
        }
    }
}

bool SceneGame::MovePosition(int xoffset, int yoffset)
{
    Vector2 position = pathmaker->pos;

    if ((position.x + xoffset >= rows)
    ||  (position.x + xoffset < 0)
    ||  (position.y + yoffset < 0)
    ||  (position.y + yoffset >= columns))
    {
        return false;
    }

    if (list->GetTile({ pathmaker->pos.x + xoffset, pathmaker->pos.y + yoffset })->isPath)
    {
        return false;
    }
    else
    {
        Tile* CurrentTile = list->GetTile(pathmaker->pos);
        pathmaker->pos.x += xoffset;
        pathmaker->pos.y += yoffset;
        Tile* NextTile = list->GetTile(pathmaker->pos);
        CurrentTile->setNext(NextTile);
        NextTile->setPrevious(CurrentTile);
        NextTile->setPath();
        list->path.push_back(NextTile);

        if (list->isEnd(pathmaker->pos))
        {
            moving = false;
        }
        return true;
    }
}

void SceneGame::Draw(Renderer& renderer)
{
    // -------------------------------------------------------
    // GAME OVER SCREEN
    // -------------------------------------------------------
    if (m_bGameOver)
    {
        if (m_pGameOverSprite)
        {
            m_pGameOverSprite->Draw(renderer);
        }
        if (m_pRestartText)
        {
            m_pRestartText->Draw(renderer);
        }
        return;
    }

    // -------------------------------------------------------
    // NORMAL GAME DRAW
    // -------------------------------------------------------
    list->Draw(renderer);

    for (int i = 0; i < (int)m_enemies.size(); i++)
    {
        m_enemies[i]->Draw(renderer);
    }

    if (!moving)
    {
        m_pLivesText->Draw(renderer);
        m_pWaveText->Draw(renderer);
    }
}

void SceneGame::DebugDraw()
{
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

    if (ImGui::Button("Force Game Over"))
    {
        m_iLives    = 0;
        m_bGameOver = true;
        if (m_pMusicChannel) { m_pMusicChannel->stop(); m_pMusicChannel = 0; }
        FMOD::System* fmodSystem = Game::GetInstance().GetSoundSystem();
        if (fmodSystem && m_pSoundGameOver)
        {
            fmodSystem->playSound(m_pSoundGameOver, 0, false, 0);
        }
    }
}

// -------------------------------------------------------
// RestartGame: wipes all state and starts fresh
// -------------------------------------------------------
void SceneGame::RestartGame(Renderer& renderer)
{
    std::cout << "Restarting game...\n";

    // Clear enemies
    for (int i = 0; i < (int)m_enemies.size(); i++)
    {
        delete m_enemies[i];
        m_enemies[i] = 0;
    }
    m_enemies.clear();

    // Rebuild Box2D world
    b2DestroyWorld(WorldPointer);
    delete World;
    World = new b2WorldDef();
    *World = b2DefaultWorldDef();
    WorldPointer = b2CreateWorld(World);
    b2World_SetGravity(WorldPointer, { 0, 0 });

    // Rebuild tile grid and path
    delete list;
    list = new Tilelist();
    list->Initialise(renderer, rows, columns);

    delete pathmaker;
    pathmaker = new Pathmaker();
    pathmaker->Initialise(renderer, list->Startpos);

    // Reset game state
    m_iLives          = 20;
    m_iWave           = 1;
    m_iEnemiesToSpawn = ENEMIES_PER_WAVE;
    m_bWaveComplete   = false;
    m_fSpawnTimer     = 0.0f;
    moving            = true;
    m_bGameOver       = false;

    m_pLivesText->SetText(renderer, "Lives: 20");
    m_pWaveText->SetText(renderer,  "Wave: 1");

    // Restart background music
    FMOD::System* fmodSystem = Game::GetInstance().GetSoundSystem();
    if (fmodSystem && m_pMusicBG)
    {
        fmodSystem->playSound(m_pMusicBG, 0, false, &m_pMusicChannel);
        std::cout << "Background music restarted\n";
    }

    std::cout << "Game restarted.\n";
}
