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
};

bool
Tile::Initialise(Renderer& renderer, Vector2 Pos, int rows, int columns)
{
	Position = Pos; // set position
	m_pSprite = renderer.CreateSprite("..\\assets\\stone4.png"); // set sprite

	m_pSprite->SetScale( // set scale to make sure it fits in screen
		renderer.GetWidth() / (m_pSprite->GetWidth() * (1.0 * columns))
	);
	m_pSprite->SetX(Position.x * m_pSprite->GetWidth() + (m_pSprite->GetWidth() / 2));
	m_pSprite->SetY(Position.y * m_pSprite->GetWidth() + (m_pSprite->GetWidth() / 2));

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
}

void Tile::setEnd()
{
	isEnd = true;
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
	this->NextPosition = 0;
}

void
Tile::UpdateDraw() // changes tile color based on its status
{
	if (isEnd)
	{
		m_pSprite->SetBlueTint(0);
		m_pSprite->SetGreenTint(0);
	}
	else if (isStart)
	{
		m_pSprite->SetRedTint(0);
		m_pSprite->SetGreenTint(0);
	}
	else if (isPath)
	{
		m_pSprite->SetBlueTint(0.2);
		m_pSprite->SetRedTint(0.5);
		m_pSprite->SetGreenTint(0.35);
	}
	else
	{
		m_pSprite->SetBlueTint(1);
		m_pSprite->SetRedTint(1);
		m_pSprite->SetGreenTint(1);
	}
	if (static_cast<int>(Position.x + Position.y) % 2 == 0)
	{
		m_pSprite->SetAlpha(0.5f);
	}
	else
	{
		m_pSprite->SetAlpha(1.0f);
	}
}

void
Tile::Draw(Renderer& renderer)
{
	m_pSprite->Draw(renderer);
};
