// Enemy.cpp

#include "Enemy.h"
#include "Tile.h"
#include "sprite.h"
#include "renderer.h"
#include <cmath>
#include <cassert>

Enemy::Enemy()
{
	m_pSprite = 0;
	m_pCurrentTile = 0;
	m_x = 0;
	m_y = 0;
	m_speed = 100.0f;
	m_tileSize = 40.0f;
	m_bReachedEnd = false;
}

Enemy::~Enemy()
{
	delete m_pSprite;
	m_pSprite = 0;
}

bool Enemy::Initialise(Renderer& renderer, Tile* startTile, float tileSize)
{
	assert(startTile);

	m_tileSize = tileSize;
	m_pCurrentTile = startTile;

	// work out where the start tile is on screen
	// tile Position is in grid coords so multiply by tileSize to get pixels
	// add half a tile to put it in the centre
	m_x = startTile->Position.x * tileSize + tileSize * 0.5f;
	m_y = startTile->Position.y * tileSize + tileSize * 0.5f;

	// using the ball sprite since it fits the enemy role well enough for now
	// TODO: swap this out for a proper enemy sprite later
	m_pSprite = renderer.CreateSprite("..\\assets\\ball.png");

	// scale it down so it fits inside a tile with a bit of space around it
	float scale = (tileSize * 0.65f) / m_pSprite->GetWidth();
	m_pSprite->SetScale(scale);

	m_pSprite->SetX((int)m_x);
	m_pSprite->SetY((int)m_y);

	return true;
}

void Enemy::Process(float deltaTime)
{
	// dont do anything if already done
	if (m_bReachedEnd || m_pCurrentTile == 0)
	{
		return;
	}

	// check where we need to go next using the linked list Jesse set up
	Tile* nextTile = m_pCurrentTile->NextPosition;

	if (nextTile == 0)
	{
		// no next tile = reached the end of the path
		m_bReachedEnd = true;
		return;
	}

	// get the screen position of the centre of the next tile
	float targetX = nextTile->Position.x * m_tileSize + m_tileSize * 0.5f;
	float targetY = nextTile->Position.y * m_tileSize + m_tileSize * 0.5f;

	float dx = targetX - m_x;
	float dy = targetY - m_y;
	float dist = sqrtf(dx * dx + dy * dy);

	// close enough - snap to the tile and move on to the next one
	if (dist < 1.5f)
	{
		m_x = targetX;
		m_y = targetY;
		m_pCurrentTile = nextTile;

		if (nextTile->isEnd)
		{
			m_bReachedEnd = true;
		}
	}
	else
	{
		// move toward the target tile this frame
		float step = m_speed * deltaTime;
		m_x += (dx / dist) * step;
		m_y += (dy / dist) * step;
	}

	m_pSprite->SetX((int)m_x);
	m_pSprite->SetY((int)m_y);
}

void Enemy::Draw(Renderer& renderer)
{
	// dont draw if the enemy has already left the screen
	if (!m_bReachedEnd)
	{
		m_pSprite->Draw(renderer);
	}
}
