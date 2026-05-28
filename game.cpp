// COMP710 GP Framework 2022
// This include:
#include "game.h"
// Local includes:
#include "renderer.h"
#include "SceneSplash.h"
#include "SceneGame.h"
#include "logmanager.h"
#include "sprite.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_sdl2.h"
#include "inputsystem.h"
#include "xboxController.h"
#include "scene.h"
#include "fmod.h"
#include "fmod.hpp"
#include "box2d.h"
#include "lib/FMOD//include/fmod_errors.h"

// Static Members:
Game* Game::sm_pInstance = 0;

Game& Game::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new Game();
	}
	return (*sm_pInstance);
}

void Game::DestroyInstance()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

Game::Game() : m_pRenderer(0), m_bLooping(true)
{
}

Game::~Game()
{
	system->release();
	std::cout << "SYSTEM DESTROYED!\n";
	for (int i = 0; i < (int)m_scenes.size(); i++) {
		delete m_scenes.at(i);
		m_scenes.at(i) = 0;
	}
	std::cout << "SCENES DESTROYED!\n";
	m_pSounds.clear();
	delete m_pInputSystem;
	m_pInputSystem = 0;
	std::cout << "INPUT SYSTEM DESTROYED!\n";
	m_scenes.clear();
	std::cout << "SOUND SYSTEM AND SCENES CLEARED!";
	delete m_pRenderer;
	m_pRenderer = 0;
	std::cout << "RENDERER DELETED!\n";
}

void Game::Quit()
{
	m_bLooping = false;
}

bool Game::Initialise()
{
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	result = system->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	int bbWidth  = 1280;
	int bbHeight = 720;
	m_pRenderer    = new Renderer();
	m_pInputSystem = new InputSystem();

	// false = fullscreen mode (required by assignment spec)
	if (!m_pRenderer->Initialise(false, bbWidth, bbHeight))
	{
		LogManager::GetInstance().Log("Renderer failed to initialise!");
		return false;
	}

	m_pInputSystem->Initialise();
	bbWidth  = m_pRenderer->GetWidth();
	bbHeight = m_pRenderer->GetHeight();
	m_iLastTime = SDL_GetPerformanceCounter();
	m_pRenderer->SetClearColour(0, 0, 0);

	// Scene 0: Splash screen (shows first)
	Scene* pSplash = new SceneSplash();
	pSplash->Initialise(*m_pRenderer);
	m_scenes.push_back(pSplash);

	// Scene 1: Main game
	Scene* pGame = new SceneGame();
	pGame->Initialise(*m_pRenderer);
	m_scenes.push_back(pGame);

	m_iCurrentScene = 0;

	return true;
}

// Advances to the next scene -- called by SceneSplash when it finishes
void Game::NextScene()
{
	if (m_iCurrentScene < (int)m_scenes.size() - 1)
	{
		m_iCurrentScene++;
	}
}

bool Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f;
	m_pInputSystem->ProcessInput();

	if (m_bLooping)
	{
		Uint64 current = SDL_GetPerformanceCounter();
		float deltaTime = (current - m_iLastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
		m_iLastTime = current;
		m_fExecutionTime += deltaTime;

		Process(deltaTime);

#ifdef USE_LAG
		m_fLag += deltaTime;
		int innerLag = 0;
		while (m_fLag >= stepSize)
		{
			Process(stepSize);
			m_fLag -= stepSize;
			++m_iUpdateCount;
			++innerLag;
		}
#endif // USE_LAG

		Draw(*m_pRenderer);
	}

	return m_bLooping;
}

void Game::Process(float deltaTime)
{
	if (m_bPauseSimulation)
	{
		deltaTime = 0.0f;
	}
	else
	{
		int result = m_pInputSystem->GetMouseButtonState(SDL_BUTTON_LEFT);
		if (result == BS_PRESSED)
		{
			LogManager::GetInstance().Log("Left mouse button pressed.");
		}
		else if (result == BS_RELEASED)
		{
			LogManager::GetInstance().Log("Left mouse button released.");
		}

		m_scenes[m_iCurrentScene]->Process(deltaTime, *m_pInputSystem);

		if (m_pInputSystem->GetKeyState(SDL_SCANCODE_ESCAPE) == BS_HELD)
		{
			Quit();
		}
	}
}

void Game::DebugDraw()
{
	if (m_bShowDebugWindow)
	{
		ImGui::Separator();
		m_pRenderer->DebugDraw();
		ImGui::Separator();
		LogManager::GetInstance().DebugDraw();
		if (ImGui::Button("Pause simulation"))
		{
			m_bPauseSimulation = !m_bPauseSimulation;
		}
		bool open = true;
		ImGui::Begin("Debug Window", &open, ImGuiWindowFlags_MenuBar);
		ImGui::Text("COMP710 GP Framework (%s)", "2024, S2");
		if (ImGui::Button("Quit"))
		{
			Quit();
		}
		ImGui::SliderInt("Active scene", &m_iCurrentScene, 0, (int)m_scenes.size() - 1, "%d");
		m_scenes[m_iCurrentScene]->DebugDraw();
		ImGui::End();
	}
}

void Game::Draw(Renderer& renderer)
{
	++m_iFrameCount;
	renderer.Clear();
	m_scenes[m_iCurrentScene]->Draw(renderer);
	DebugDraw();
	renderer.Present();
}

void Game::ProcessFrameCounting(float deltaTime)
{
	m_fElapsedSeconds += deltaTime;
	if (m_fElapsedSeconds > 100.0f)
	{
		m_fElapsedSeconds -= 1.0f;
		m_iFPS = m_iFrameCount;
		m_iFrameCount = 0;
	}
}

void Game::ToggleDebugWindow()
{
	m_bShowDebugWindow = !m_bShowDebugWindow;
	m_pInputSystem->ShowMouseCursor(m_bShowDebugWindow);
}
