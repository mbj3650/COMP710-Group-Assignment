// Enemy.cpp

#include "Enemy.h"
#include "Tile.h"
#include "sprite.h"
#include "renderer.h"
#include <cmath>
#include <cassert>
#include <box2d.h>

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
	//destroy its body so that we dont get memory leaks
	if (b2Body_IsValid(ID)) {
		b2DestroyBody(ID);//destroy impact body
	}
	delete m_pSprite;
	m_pSprite = 0;
}

bool Enemy::Initialise(Renderer& renderer, Tile* startTile, float tileSize, b2WorldId WorldID)
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


	//SETTING UP BASIC BOX2D 
	b2BodyDef WorldObj = b2DefaultBodyDef();
	WorldObj.position = { m_x,m_y };//set initial positoin

	//setup the hitbox
	ID = b2CreateBody(WorldID, &WorldObj);
	b2Body_SetType(ID, b2_dynamicBody);
	b2Body_SetUserData(ID, this);
	b2Polygon box = b2MakeRoundedBox(6* scale, 6* scale, 5.0f);

	//all this does is basically give the physics objects its physics attributes
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.friction = 0.1f;

	shapeDef.filter.categoryBits = 0x0002;//i am this ID
	shapeDef.filter.maskBits =  0x0002 ;//i collide with all things of this ID

	//finally put it all together to create this object
	shapeId = b2CreatePolygonShape(ID, &shapeDef, &box);

	//get the position of our new box2d object, and then tell the sprite to appear there
	m_pSprite->SetX(b2Body_GetPosition(ID).x);
	m_pSprite->SetY(b2Body_GetPosition(ID).y);

	return true;
}

void Enemy::Process(float deltaTime)
{
	//set m_x and m_y to the box2d object's position
	m_x = b2Body_GetPosition(ID).x;
	m_y = b2Body_GetPosition(ID).y;
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
		b2Vec2 velocityVec = { (dx / dist) * m_speed , (dy / dist) * m_speed };
		b2Body_SetLinearVelocity(ID, velocityVec);
		//m_x += (dx / dist) * step;
		//m_y += (dy / dist) * step;
	}

	m_pSprite->SetX(b2Body_GetPosition(ID).x);
	m_pSprite->SetY(b2Body_GetPosition(ID).y);
}

void Enemy::Draw(Renderer& renderer)
{
	// dont draw if the enemy has already left the screen
	if (!m_bReachedEnd)
	{
		m_pSprite->Draw(renderer);
	}
}
