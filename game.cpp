// COMP710 GP Framework 2022
// This include:
#include "game.h"
// Library includes:
#include "renderer.h"
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
#include "../SKRT/lib/FMOD/include/fmod_errors.h"
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
	
}
void Game::Quit()
{
	m_bLooping = false;
}

bool Game::Initialise()
{

	result = FMOD::System_Create(&system);      // Create the main system object.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	FMOD::Sound* newSound = nullptr;

	int bbWidth = 1920;
	int bbHeight = 1200;
	m_pRenderer = new Renderer();
	m_pInputSystem = new InputSystem();
	if (!m_pRenderer->Initialise(false, bbWidth, bbHeight))
	{
		LogManager::GetInstance().Log("Renderer failed to initialise!");
		return false;
	}
	m_pInputSystem->Initialise();
	bbWidth = m_pRenderer->GetWidth();
	bbHeight = m_pRenderer->GetHeight();
	m_iLastTime = SDL_GetPerformanceCounter();
	m_pRenderer->SetClearColour(0, 0, 0);

	

	//// Load static text textures into the Texture Manager...
	//m_pRenderer->CreateStaticText("Auckland University of Technology", 50);

	//// Generate sprites that use the static text textures...
	//m_pWelcomeText = m_pRenderer->CreateSprite("Auckland University of Technology");

	//m_pWelcomeText->SetX(bbWidth - (m_pWelcomeText->GetWidth() / 1.05));
	//m_pWelcomeText->SetY(bbHeight / 1.5);
	m_iCurrentScene = 0;

	return true;
}
bool Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f;
	// TODO: Process input here!
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
#endif //USE_LAG
		if (m_fExecutionTime > 6 && m_iCurrentScene == 0) {
			m_iCurrentScene = 1;
		}
		
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
	else {
		m_pInputSystem->ProcessInput();
		int result = m_pInputSystem->GetMouseButtonState(SDL_BUTTON_LEFT);
		if (result == BS_PRESSED)
		{
			LogManager::GetInstance().Log("Left mouse button pressed.");
		}
		else if (result == BS_RELEASED)
		{
			LogManager::GetInstance().Log("Left mouse button released.");
		}
		//ButtonState xboxA = m_pInputSystem->GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_A);
		//ButtonState xboxX = m_pInputSystem->GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_X);
		//ButtonState xboxLeft = m_pInputSystem->GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
		//ButtonState xboxRight = m_pInputSystem->GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		//if (xboxA == BS_PRESSED)
		//{
		//	LogManager::GetInstance().Log("Xbox A Pressed");
		//}
		//if (xboxLeft == BS_PRESSED)
		//{
		//	LogManager::GetInstance().Log("Xbox Left Pressed");
		//	ProcessFrameCounting(deltaTime);
		//	// TODO: Add game objects to process here!
		//	
		//}
		m_scenes[m_iCurrentScene]->Process(deltaTime, *m_pInputSystem);
		if (m_pInputSystem->GetKeyState(SDL_SCANCODE_ESCAPE)== BS_HELD) {
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
		ImGui::SliderInt("Active scene", &m_iCurrentScene, 0, m_scenes.size() - 1, "%d");
		m_scenes[m_iCurrentScene]->DebugDraw();
		ImGui::End();
	}
}
void Game::Draw(Renderer& renderer)
{
	++m_iFrameCount;
	renderer.Clear();
	/*if (m_iCurrentScene == 0) {
		m_pWelcomeText->Draw(renderer);
	}*/
	
	// TODO: Add game objects to draw here!
	m_scenes[m_iCurrentScene]->Draw(renderer);
	DebugDraw();
	renderer.Present();
}

void
Game::ProcessFrameCounting(float deltaTime)
{
	// Count total simulation time elapsed:
	m_fElapsedSeconds += deltaTime;
	// Frame Counter:
	if (m_fElapsedSeconds > 100.0f)
	{
		m_fElapsedSeconds -= 1.0f;
		m_iFPS = m_iFrameCount;
		m_iFrameCount = 0;
	}
}
void Game::ToggleDebugWindow
()
{
	m_bShowDebugWindow = !m_bShowDebugWindow;
	m_pInputSystem->ShowMouseCursor(m_bShowDebugWindow);
}