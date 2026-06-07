// COMP710 JESSE
// This include:
#include "Tile.h"

// Local includes:
#include "renderer.h"
#include "sprite.h"
// Library includes:
#include <cassert>
#include "inlinehelpers.h"

Tile::Tile()
{
	m_pSprite = 0;
	m_pSpriteSand = 0;
	m_pSpriteOutline = 0;
	// initialise everything so we dont get random garbage values on startup
	isPath = false;
	isStart = false;
	isEnd = false;
	Connections = 0;
	PrevPosition = 0;
	NextPosition = 0;
};

Tile::~Tile()
{
	delete m_pSprite;
	delete m_pSpriteSand;
	delete m_pSpriteOutline;
};

bool
Tile::Initialise(Renderer& renderer, Vector2 Pos, int rows, int columns)
{
	Position = Pos; // set position
	
	int obstaclecheck = GetRandom(0, 50);
	if (obstaclecheck == 1) {
		isObstacle = true;
	}
	

	if (!isObstacle) {
		m_pSprite = renderer.CreateSprite("..\\assets\\stone4.png"); // set sprite
	}
	else {
		m_pSprite = renderer.CreateSprite("..\\assets\\stone_obstacle.png"); // set sprite
	}
	
	m_pSpriteSand = renderer.CreateSprite("..\\assets\\stone5.png"); // set sprite
	m_pSpriteOutline = renderer.CreateSprite("..\\assets\\stone5_outline.png"); // set sprite


	m_pSprite->SetScale( // set scale to make sure it fits in screen
		renderer.GetWidth() / (m_pSprite->GetWidth() * (1.0 * columns))
	);

	m_pSprite->SetX(Position.x * m_pSprite->GetWidth() + (m_pSprite->GetWidth() / 2));
	m_pSprite->SetY(Position.y * m_pSprite->GetWidth() + (m_pSprite->GetWidth() / 2));


	m_pSpriteSand->SetScale( // set scale to make sure it fits in screen
		renderer.GetWidth() / (m_pSpriteSand->GetWidth() * (1.0 * columns))
	);
	m_pSpriteSand->SetX(Position.x * m_pSpriteSand->GetWidth() + (m_pSpriteSand->GetWidth() / 2));//sand sprite for if its a path
	m_pSpriteSand->SetY(Position.y * m_pSpriteSand->GetWidth() + (m_pSpriteSand->GetWidth() / 2));
	m_pSpriteSand->SetScale(m_pSpriteSand->GetScale() * 1.25);


	m_pSpriteOutline->SetScale( // set scale to make sure it fits in screen
		renderer.GetWidth() / (m_pSpriteOutline->GetWidth() * (1.0 * columns))
	);
	m_pSpriteOutline->SetX(Position.x * m_pSpriteOutline->GetWidth() + (m_pSpriteOutline->GetWidth() / 2));//sand sprite for if its a path
	m_pSpriteOutline->SetY(Position.y * m_pSpriteOutline->GetWidth() + (m_pSpriteOutline->GetWidth() / 2));
	m_pSpriteOutline->SetScale(m_pSpriteOutline->GetScale() * 1.25);
	Sprites[0] = m_pSprite;
	Sprites[1] = m_pSpriteSand;
	Sprites[2] = m_pSpriteOutline;
	currentsprite = 0;
	return true;
};

void
Tile::Process(float deltaTime)
{
	UpdateDraw();
};

void Tile::setStart()
{
	isStart = true;
	isObstacle = false;
	//make sure it can be pathed to
}

void Tile::setEnd()
{
	isEnd = true;
	isObstacle = false;//make sure it can be pathed to
}

void Tile::setNext(Tile* NextPosition)
{
	this->NextPosition = NextPosition;
}

void Tile::setPath()
{
	this->isPath = true;
}

void Tile::setPrevious(Tile* PrevPosition)
{
	// fixed: was accidentally storing NextPosition here instead of the parameter
	// this meant the backwards link in the path was always null
	this->PrevPosition = PrevPosition;
}

void Tile::Undo() // reset the address of the next tile
{
	NextPosition->isPath = false; // remove path status from it
	NextPosition->currentsprite = 0;
	this->NextPosition = 0;
}

void
Tile::UpdateDraw() // changes tile color based on its status
{
	if (isPath || isStart)
	{
		currentsprite = 1;
	}
	else if (hovered)
	{
		Sprites[currentsprite]->SetBlueTint(0.0);
		Sprites[currentsprite]->SetRedTint(0.0);
		Sprites[currentsprite]->SetGreenTint(1.0);
	}
	else
	{
		Sprites[currentsprite]->SetBlueTint(1);
		Sprites[currentsprite]->SetRedTint(1);
		Sprites[currentsprite]->SetGreenTint(1);
	}
	if (static_cast<int>(Position.x + Position.y) % 2 == 0 && isPath == false)
	{
		Sprites[currentsprite]->SetAlpha(0.95f);
	}
	else
	{
		Sprites[currentsprite]->SetAlpha(1.0f);
	}
}

void
Tile::Draw(Renderer& renderer)
{
	Sprites[currentsprite]->Draw(renderer);	
};

void
Tile::DrawPaths(Renderer& renderer)
{
	Sprites[2]->Draw(renderer);
};

int Tile::GetWidth() {
	return Sprites[currentsprite]->GetWidth();
}