// COMP710 GP Framework 2025
// This include:
#include "SceneGame.h"

// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "AnimatedSprite.h"
#include "Pathmaker.h"
#include "Tile.h"
#include "Tilelist.h"
#include "inputsystem.h"
#include "Enemy.h"
#include "ProjectilePool.h"

// Library includes:
#include <cassert>
#include "lib/imgui/imgui.h"
#include <time.h>
#include "inlinehelpers.h"
#include <iostream>

SceneGame::SceneGame()
	: m_pCentre(0)
	, m_angle(0.0f)
	, m_rotationSpeed(1.0f)
	, m_pProjectilePool(nullptr)
	, m_iPlayerHealth(100)
	, m_iPlayerGold(0)
	, m_pCachedRenderer(nullptr)
{
}

SceneGame::~SceneGame()
{
	delete list;
	delete m_pCentre;
	delete pathmaker;

	// unique_ptr handles m_enemyPool automatically
	delete m_pProjectilePool;

	m_pCentre = 0;
	m_pProjectilePool = nullptr;
}

bool SceneGame::Initialise(Renderer& renderer)
{
	srand(static_cast<unsigned int>(time(NULL)));
	m_pCachedRenderer = &renderer; // Cache it for Reset routines

	// Prevents double-allocation memory leaks during Reset() hot-reloads
	if (!list) list = new Tilelist();
	if (!pathmaker) pathmaker = new Pathmaker();

	// Instantiate Martin's High-Performance Projectile Pipeline if not existing
	if (!m_pProjectilePool)
	{
		m_pProjectilePool = new ProjectilePool();
		m_pProjectilePool->Initialise(renderer);
	}

	const int SCREEN_WIDTH = renderer.GetWidth();
	const int SCREEN_HEIGHT = renderer.GetHeight();

	columns = SCREEN_WIDTH / 40;
	rows = SCREEN_HEIGHT / 40;

	list->Initialise(renderer, rows, columns); // this holds all the tiles that the map needs
	pathmaker->Initialise(renderer, list->GetStartTile());

	m_pCentre = renderer.CreateAnimatedSprite("assets\\animatedship.png");
	m_pCentre->SetPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	m_pCentre->SetLooping(true);
	m_pCentre->SetFrameSpeed(0.1f);

	return true;
}

void SceneGame::Reset(Renderer& renderer)
{
	// 1. Physically ultra-wipe all active allocated enemies
	m_enemyPool.clear();

	// 2. Clear out all in-flight projectiles in continuous cache packed array
	if (m_pProjectilePool)
	{
		m_pProjectilePool->Clear();
	}

	// 3. Reset player parameters back to baseline
	m_iPlayerHealth = 100;
	m_iPlayerGold = 0;

	// 4. Wipe tiles and structural grids safely before re-seeding
	delete list;
	delete pathmaker;
	delete m_pCentre;

	list = nullptr;
	pathmaker = nullptr;
	m_pCentre = nullptr;

	Path.clear();

	// 5. Hot re-run initialization layout
	Initialise(renderer);
}

void SceneGame::Process(float deltaTime, InputSystem& inputSystem)
{
	// ─── MARTIN'S SYSTEM RESET INTERCEPTOR ──────────────────────────────────
	if (inputSystem.GetKeyState(SDL_SCANCODE_R) == BS_PRESSED)
	{
		if (m_pCachedRenderer)
		{
			Reset(*m_pCachedRenderer);
			std::cout << "Scene Game successfully reset by Martin.\n";
		}
		return; // HARD STOP: prevents stale scene data loops from ticking this frame
	}
	// ────────────────────────────────────────────────────────────────────────

	m_pCentre->Process(deltaTime);
	m_angle += m_rotationSpeed * deltaTime;

	// MOVEMENT FOR MAKING PATHS
	if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) {
		std::cout << "w\n";
		if (list->GetTile({ pathmaker->pos.y, pathmaker->pos.x - 1 }) != NULL) {
			pathmaker->pos.x -= 1;
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}
	else if (inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED) {
		std::cout << "s\n";
		if (list->GetTile({ pathmaker->pos.y, pathmaker->pos.x + 1 }) != NULL) {
			pathmaker->pos.x += 1;
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}
	else if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED) {
		std::cout << "a\n";
		if (list->GetTile({ pathmaker->pos.y - 1, pathmaker->pos.x }) != NULL) {
			pathmaker->pos.y -= 1;
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}
	else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED) {
		std::cout << "d\n";
		if (list->GetTile({ pathmaker->pos.y + 1, pathmaker->pos.x }) != NULL) {
			pathmaker->pos.y += 1;
			list->GetTile(pathmaker->pos)->isPath = true;
		}
	}

	// Update active gameplay elements
	for (auto& enemy : m_enemyPool)
	{
		if (enemy && enemy->IsAlive())
		{
			enemy->Update(deltaTime);
		}
	}
}

void SceneGame::Draw(Renderer& renderer)
{
	list->Draw(renderer);
	pathmaker->Draw(renderer);
	m_pCentre->Draw(renderer);

	// Render dynamic layer elements
	for (auto& enemy : m_enemyPool)
	{
		if (enemy && enemy->IsAlive())
		{
			enemy->Draw();
		}
	}
}

void SceneGame::Move(SDL_Scancode key)
{
	(void)key;
}

void SceneGame::DebugDraw()
{
	ImGui::Text("Scene Game Debug Console");
	ImGui::Value("Player Health", m_iPlayerHealth);
	ImGui::Value("Player Gold", m_iPlayerGold);
	ImGui::Value("Active Enemies Count", static_cast<int>(m_enemyPool.size()));
}