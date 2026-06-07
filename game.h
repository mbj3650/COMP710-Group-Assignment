// COMP710 GP Framework
// Library includes:
#include <vector>
#include "inputsystem.h"
#include "soundsystem.h"
#include "fmod.h"

#include "fmod_common.h"
#include "fmod.hpp"
#ifndef GAME_H
#define GAME_H


// Forward declarations:
class Sprite;
class Renderer;
class Scene;
class LogManager;
class Game
{
	// Member methods:
public:
	static Game& GetInstance();
	static void DestroyInstance();
	bool Initialise();
	void NextScene();
	bool DoGameLoop();
	void DebugDraw();
	void ToggleDebugWindow();
	void Quit();
	SoundSystem* GetSoundSystem();

protected:
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void ProcessFrameCounting(float deltaTime);
private:
	Game();
	~Game();
	Game(const Game& game);
	Game& operator=(const Game& game);
	// Member data:
public:
protected:
	bool m_bPauseSimulation;
	bool m_bShowDebugWindow;
	std::vector<Scene*> m_scenes;
	int m_iCurrentScene;
	static Game* sm_pInstance;
	Renderer* m_pRenderer;
	InputSystem* m_pInputSystem;
	_int64 m_iLastTime;
	float m_fExecutionTime;
	float m_fElapsedSeconds;
	int m_iFrameCount;
	int m_iFPS;
	SoundSystem* m_pSoundSystem;

#ifdef USE_LAG
	float m_fLag;
	int m_iUpdateCount;
#endif // USE_LAG
	bool m_bLooping;
private:
};
#endif // GAME_H
