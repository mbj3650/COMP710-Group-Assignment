// COMP710 GP Framework 2025
// SceneGame.cpp
// Modified by: MartinYan12138y
// Changes: Added game over detection, game over screen (defeated_gameover.png),
//          and full restart system that resets all game state.

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
// Library includes:
#include <cassert>
#include "lib/imgui/imgui.h"
#include <time.h>
#include "inlinehelpers.h"
#include "Tilelist.h"
#include <iostream>
#include <string>
#include <box2d.h>

// Each wave will spawn this many enemies per wave level
// so wave 1 = 3 enemy, wave 2 = 6 enemy, and so on
const int ENEMIES_PER_WAVE = 3;

// How many seconds between each enemy spawn
const float SPAWN_INTERVAL = 1.5f;

// Use Windows system font because we don't have a font file in assets folder
// If this doesn't work, change to any TTF path on your computer
const char* FONT_PATH = "C:\\Windows\\Fonts\\arial.ttf";

// Path to the game over image -- make sure defeated_gameover.png is in your assets folder
const char* GAMEOVER_IMAGE_PATH = "..\\assets\\defeated_gameover.png";

SceneGame::SceneGame()
    : m_pCentre(0)
    , m_angle(0.0f)
    , m_rotationSpeed(1.0f)
{
    m_pRenderer = 0;
    m_fSpawnTimer = 0.0f;
    m_fTileSize = 40.0f;

    // Player starts with 20 lives
    m_iLives = 20;

    // Wave starts from 1
    m_iWave = 1;
    m_iEnemiesToSpawn = ENEMIES_PER_WAVE;
    m_bWaveComplete = false;

    // Set to null first, will create in Initialise
    m_pLivesText  = 0;
    m_pWaveText   = 0;

    // Game over starts as false
    m_bGameOver       = false;
    m_pGameOverSprite = 0;
    m_pRestartText    = 0;
}

SceneGame::~SceneGame()
{
    delete list;
    delete m_pCentre;
    delete pathmaker;
    m_pCentre = 0;

    // Delete all enemies still alive on screen
    for (int i = 0; i < (int)m_enemies.size(); i++)
    {
        delete m_enemies[i];
        m_enemies[i] = 0;
    }
    m_enemies.clear();

    // Delete HUD text objects
    delete m_pLivesText;
    m_pLivesText = 0;

    delete m_pWaveText;
    m_pWaveText = 0;

    // Delete game over objects
    delete m_pGameOverSprite;
    m_pGameOverSprite = 0;

    delete m_pRestartText;
    m_pRestartText = 0;

    // Destroy the Box2D world
    b2DestroyWorld(WorldPointer);
    delete World;
    std::cout << "WORLD DESTROYED\n";
}

bool SceneGame::Initialise(Renderer& renderer)
{
    srand(time(NULL));
    const int SCREEN_WIDTH  = renderer.GetWidth();
    const int SCREEN_HEIGHT = renderer.GetHeight();

    // Set up the Box2D world
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

    // Save renderer pointer so we can use it later when spawning enemies
    m_pRenderer = &renderer;
    m_fTileSize = (float)SCREEN_WIDTH / (float)columns;

    // Create the lives text, anchored to the top-left corner
    m_pLivesText = new DynamicText();
    m_pLivesText->Initialise(renderer, FONT_PATH, 24, false);
    m_pLivesText->SetText(renderer, "Lives: 20");
    m_pLivesText->SetPosition(16, 16);

    // Wave text goes just below the lives text
    m_pWaveText = new DynamicText();
    m_pWaveText->Initialise(renderer, FONT_PATH, 24, false);
    m_pWaveText->SetText(renderer, "Wave: 1");
    m_pWaveText->SetPosition(16, 48);

    // --- Load the game over sprite ---
    // The sprite is loaded here and scaled to fill the full screen.
    // Make sure defeated_gameover.png is inside your assets folder.
    m_pGameOverSprite = renderer.CreateSprite(GAMEOVER_IMAGE_PATH);
    if (m_pGameOverSprite)
    {
        // Scale image to cover the whole screen
        float scaleX = (float)SCREEN_WIDTH  / (float)m_pGameOverSprite->GetWidth();
        float scaleY = (float)SCREEN_HEIGHT / (float)m_pGameOverSprite->GetHeight();

        // Use the smaller scale so image fills without distortion,
        // or just use scaleX if you want a full stretch
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        m_pGameOverSprite->SetScale(scale);

        // Centre it on screen
        m_pGameOverSprite->SetX(SCREEN_WIDTH  / 2);
        m_pGameOverSprite->SetY(SCREEN_HEIGHT / 2);
    }

    // "Press R to restart" text shown on the game over screen
    m_pRestartText = new DynamicText();
    m_pRestartText->Initialise(renderer, FONT_PATH, 32, true);
    m_pRestartText->SetText(renderer, "Press R to Restart");
    m_pRestartText->SetPosition((float)(SCREEN_WIDTH / 2), (float)(SCREEN_HEIGHT - 80));

    return true;
}

void SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{
    // -------------------------------------------------------
    // GAME OVER STATE: only listen for restart key
    // -------------------------------------------------------
    if (m_bGameOver)
    {
        // Press R to restart the whole game
        if (inputSystem.GetKeyState(SDL_SCANCODE_R) == BS_PRESSED)
        {
            RestartGame(*m_pRenderer);
        }
        return; // Skip all normal game logic while game over screen is showing
    }

    // -------------------------------------------------------
    // NORMAL GAME LOGIC
    // -------------------------------------------------------
    list->Process(deltaTime);

    if (moving)
    {
        // Player is still drawing the path -- handle movement input
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

        // B key to undo the last step
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
        // Path drawing is finished -- run the enemy wave logic
        m_fSpawnTimer += deltaTime;

        b2World_Step(WorldPointer, deltaTime, ScenesubStepCount);

        // If there are still enemies to spawn and the timer is ready, spawn one
        if (m_iEnemiesToSpawn > 0 && m_fSpawnTimer >= SPAWN_INTERVAL && m_pRenderer != 0)
        {
            m_fSpawnTimer = 0.0f;

            Enemy* newEnemy = new Enemy();
            newEnemy->Initialise(*m_pRenderer, list->GetStart(), m_fTileSize, WorldPointer);
            m_enemies.push_back(newEnemy);

            m_iEnemiesToSpawn--;
            std::cout << "Spawned enemy, " << m_iEnemiesToSpawn << " left this wave\n";
        }

        // Update all enemies; if an enemy reaches the end, remove it and subtract a life
        for (int i = (int)m_enemies.size() - 1; i >= 0; i--)
        {
            m_enemies[i]->Process(deltaTime);

            if (m_enemies[i]->HasReachedEnd())
            {
                delete m_enemies[i];
                m_enemies.erase(m_enemies.begin() + i);
                m_iLives--;
                std::cout << "Enemy got through! Lives left: " << m_iLives << "\n";

                // Update the lives display
                m_pLivesText->SetText(*m_pRenderer, "Lives: " + std::to_string(m_iLives));

                // Check if the player has run out of lives
                if (m_iLives <= 0)
                {
                    m_bGameOver = true;
                    std::cout << "GAME OVER\n";
                    return; // Stop processing this frame immediately
                }
            }
        }

        // Check if the current wave is finished
        // (no more enemies to spawn AND no enemies left on screen)
        if (m_iEnemiesToSpawn == 0 && m_enemies.empty())
        {
            m_iWave++;
            m_iEnemiesToSpawn = m_iWave * ENEMIES_PER_WAVE;
            m_fSpawnTimer = 0.0f;

            std::cout << "Wave " << m_iWave << " starting! Enemies this wave: " << m_iEnemiesToSpawn << "\n";
            m_pWaveText->SetText(*m_pRenderer, "Wave: " + std::to_string(m_iWave));
        }
    }
}

bool SceneGame::MovePosition(int xoffset, int yoffset)
{
    Vector2 position = pathmaker->pos;

    // Check boundary so the player can't walk off the grid
    if ((position.x + xoffset >= rows)
    ||  (position.x + xoffset < 0)
    ||  (position.y + yoffset < 0)
    ||  (position.y + yoffset >= columns))
    {
        return false;
    }

    // Also check if that tile is already part of the path
    if (list->GetTile({ pathmaker->pos.x + xoffset, pathmaker->pos.y + yoffset })->isPath)
    {
        return false;
    }
    else
    {
        // Move is valid: update the tile linked list and mark as path
        Tile* CurrentTile = list->GetTile(pathmaker->pos);
        pathmaker->pos.x += xoffset;
        pathmaker->pos.y += yoffset;
        Tile* NextTile = list->GetTile(pathmaker->pos);
        CurrentTile->setNext(NextTile);
        NextTile->setPrevious(CurrentTile);
        NextTile->setPath();
        list->path.push_back(NextTile);

        // If the player reaches the end tile, stop movement
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
    // GAME OVER SCREEN: draw the image and restart prompt
    // -------------------------------------------------------
    if (m_bGameOver)
    {
        // Draw the game over image (fills screen)
        if (m_pGameOverSprite)
        {
            m_pGameOverSprite->Draw(renderer);
        }

        // Draw "Press R to Restart" below the image
        if (m_pRestartText)
        {
            m_pRestartText->Draw(renderer);
        }
        return;
    }

    // -------------------------------------------------------
    // NORMAL GAME DRAW
    // -------------------------------------------------------

    // Draw tile grid first
    list->Draw(renderer);

    // Draw enemies on top of tiles
    for (int i = 0; i < (int)m_enemies.size(); i++)
    {
        m_enemies[i]->Draw(renderer);
    }

    // Draw HUD text last so it appears on top of everything
    // Only show after path is drawn and waves have started
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

    // Show game state info in the debug window
    ImGui::Text("Lives: %d",             m_iLives);
    ImGui::Text("Wave: %d",              m_iWave);
    ImGui::Text("Enemies to spawn: %d",  m_iEnemiesToSpawn);
    ImGui::Text("Enemies on screen: %d", (int)m_enemies.size());
    ImGui::Text("Game Over: %s",         m_bGameOver ? "YES" : "NO");

    // Debug button to manually trigger game over (useful for testing)
    if (ImGui::Button("Force Game Over"))
    {
        m_iLives   = 0;
        m_bGameOver = true;
    }
}

// -------------------------------------------------------
// RestartGame: wipes all state and starts fresh
// -------------------------------------------------------
void SceneGame::RestartGame(Renderer& renderer)
{
    std::cout << "Restarting game...\n";

    // --- Clear enemies ---
    for (int i = 0; i < (int)m_enemies.size(); i++)
    {
        delete m_enemies[i];
        m_enemies[i] = 0;
    }
    m_enemies.clear();

    // --- Destroy old Box2D world and create a new one ---
    b2DestroyWorld(WorldPointer);
    delete World;
    World = new b2WorldDef();
    *World = b2DefaultWorldDef();
    WorldPointer = b2CreateWorld(World);
    b2World_SetGravity(WorldPointer, { 0, 0 });

    // --- Reset tile grid and path ---
    delete list;
    list = new Tilelist();
    list->Initialise(renderer, rows, columns);

    delete pathmaker;
    pathmaker = new Pathmaker();
    pathmaker->Initialise(renderer, list->Startpos);

    // --- Reset game state ---
    m_iLives          = 20;
    m_iWave           = 1;
    m_iEnemiesToSpawn = ENEMIES_PER_WAVE;
    m_bWaveComplete   = false;
    m_fSpawnTimer     = 0.0f;
    moving            = true;
    m_bGameOver       = false;

    // --- Refresh HUD text ---
    m_pLivesText->SetText(renderer, "Lives: 20");
    m_pWaveText->SetText(renderer,  "Wave: 1");

    std::cout << "Game restarted.\n";
}
