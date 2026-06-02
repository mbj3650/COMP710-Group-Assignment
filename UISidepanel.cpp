#include "UISidepanel.h"
#include "UIShopManager.h"
#include "renderer.h"
#include "sprite.h"
#include "Tower.h"
#include "DynamicText.h"
#include "inputsystem.h"
#include <iostream>
UISidepanel::UISidepanel()
{
	m_pPanelSprite = 0;
	m_pTowerTitle = 0;
	m_pSidepanelTower = 0;
	m_pSellSprite = 0;
	m_pSellText = 0;
}

UISidepanel::~UISidepanel()
{
	delete m_pPanelSprite;
	m_pPanelSprite = 0;
	delete m_pTowerTitle;
	m_pTowerTitle = 0;
	delete m_pSellSprite;
	m_pSellSprite = 0;
	delete m_pSellText;
	m_pSellText = 0;
}

void UISidepanel::Initialise(Renderer& renderer)
{
	m_pPanelSprite = renderer.CreateSprite("..\\assets\\ui\\sidepanel.png");
	m_pPanelSprite->SetScale(1.5f);
	int x = renderer.GetWidth() - m_pPanelSprite->GetWidth() / 2 - 50;
	int y = renderer.GetHeight() / 2;
	m_pPanelSprite->SetX(x);
	m_pPanelSprite->SetY(y);

	m_pTowerTitle = new DynamicText();
	m_pTowerTitle->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 108, true);
	m_pTowerTitle->SetText(renderer, " ");
	m_pTowerTitle->SetPosition(x, y - 500);

	m_pSellSprite = renderer.CreateSprite("..\\assets\\ui\\sellbutton.png");
	m_pSellSprite->SetX(x - 125);
	m_pSellSprite->SetY(y + 525);

	m_pSellText = new DynamicText();
	m_pSellText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 36, true);
	m_pSellText->SetText(renderer, "Sell");
	m_pSellText->SetPosition(x - 125, y + 525);
}

void UISidepanel::Process(float deltaTime, InputSystem& input)
{
	// if we have a tower in sidepanel, and mouse clicks on sell button
	if (m_pSidepanelTower && IsSpriteHovered(m_pSellSprite, input) && input.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
	{
		m_pSidepanelTower->Sell();
		m_pSidepanelTower = 0;
	}
}

void UISidepanel::Draw(Renderer& renderer)
{
	m_pPanelSprite->Draw(renderer);


	if (m_pSidepanelTower)
	{
		m_pTowerTitle->Draw(renderer);
		m_pSellSprite->Draw(renderer);
		m_pSellText->Draw(renderer);
	}
}

bool UISidepanel::IsElementHovered(InputSystem& input) const
{
	return IsSpriteHovered(m_pPanelSprite, input);
}

bool UISidepanel::IsSpriteHovered(Sprite* sprite, InputSystem& input) const
{
	Vector2 mousePos = input.GetMousePosition();
	if (mousePos.x >= sprite->GetX() - sprite->GetWidth() / 2 &&
		mousePos.x <= sprite->GetX() + sprite->GetWidth() / 2 &&
		mousePos.y >= sprite->GetY() - sprite->GetHeight() / 2 &&
		mousePos.y <= sprite->GetY() + sprite->GetHeight() / 2)
		return true;
	else return false;
}

void UISidepanel::SetTower(Renderer& renderer, Tower* tower)
{
	if (tower)
	{
		m_pSidepanelTower = tower;
		m_pTowerTitle->SetText(renderer, tower->GetTowerID());
	}
	else
	{
		m_pSidepanelTower = 0;
		m_pTowerTitle->SetText(renderer, " ");
	}
	

	
}