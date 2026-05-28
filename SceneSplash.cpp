// COMP710 GP Framework 2025
// SceneSplash.cpp
// Author: MartinYan12138y
// Shows splash.png for SPLASH_DURATION seconds, then advances to SceneGame.
// Press SPACE or ENTER to skip.

#include "SceneSplash.h"
#include "renderer.h"
#include "sprite.h"
#include "game.h"
#include "lib/imgui/imgui.h"
#include <iostream>

// How long (in seconds) to display the splash before auto-advancing
const float SPLASH_DURATION = 3.0f;

SceneSplash::SceneSplash()
    : m_pSplashSprite(0)
    , m_fTimer(0.0f)
    , m_bDone(false)
{
}

SceneSplash::~SceneSplash()
{
    delete m_pSplashSprite;
    m_pSplashSprite = 0;
}

bool SceneSplash::Initialise(Renderer& renderer)
{
    const int W = renderer.GetWidth();
    const int H = renderer.GetHeight();

    m_pSplashSprite = renderer.CreateSprite("..\\assets\\splash.png");
    if (m_pSplashSprite)
    {
        // Scale to fill the screen, maintaining aspect ratio
        float sx = (float)W / (float)m_pSplashSprite->GetWidth();
        float sy = (float)H / (float)m_pSplashSprite->GetHeight();
        m_pSplashSprite->SetScale(sx < sy ? sx : sy);
        m_pSplashSprite->SetX(W / 2);
        m_pSplashSprite->SetY(H / 2);
    }

    return true;
}

void SceneSplash::Process(float deltaTime, InputSystem& inputSystem)
{
    if (m_bDone) return;

    m_fTimer += deltaTime;

    // Skip on SPACE or ENTER, or auto-advance after SPLASH_DURATION
    bool skip = (inputSystem.GetKeyState(SDL_SCANCODE_SPACE)  == BS_PRESSED)
             || (inputSystem.GetKeyState(SDL_SCANCODE_RETURN) == BS_PRESSED)
             || (m_fTimer >= SPLASH_DURATION);

    if (skip)
    {
        m_bDone = true;
        std::cout << "Splash done, advancing to game\n";
        Game::GetInstance().NextScene();
    }
}

void SceneSplash::Draw(Renderer& renderer)
{
    if (m_pSplashSprite)
    {
        m_pSplashSprite->Draw(renderer);
    }
}

void SceneSplash::DebugDraw()
{
    ImGui::Text("Scene: Splash");
    ImGui::Text("Timer: %.2f / %.2f", m_fTimer, SPLASH_DURATION);
    if (ImGui::Button("Skip Splash"))
    {
        m_bDone = true;
        Game::GetInstance().NextScene();
    }
}
