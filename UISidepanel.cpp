#include "UISidepanel.h"
#include "UIShopManager.h"
#include "UIUpgradeButton.h"
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
	m_pUpgrade1 = 0;
	m_pUpgrade2 = 0;
	m_pUpgrade3 = 0;
	m_pTargetFirstSprite = 0;
	m_pTargetLastSprite = 0;
	m_pTargetText = 0;
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

	delete m_pUpgrade1;
	m_pUpgrade1 = 0;
	delete m_pUpgrade2;
	m_pUpgrade2 = 0;
	delete m_pUpgrade3;
	m_pUpgrade3 = 0;

	delete m_pTargetFirstSprite;
	m_pTargetFirstSprite = 0;
	delete m_pTargetLastSprite;
	m_pTargetLastSprite = 0;
	delete m_pTargetText;
	m_pTargetText = 0;

	m_pSidepanelTower = 0;
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
	m_pTowerTitle->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 84, true);
	m_pTowerTitle->SetText(renderer, " ");
	m_pTowerTitle->SetPosition(x, y - 500);

	m_pSellSprite = renderer.CreateSprite("..\\assets\\ui\\sellbutton.png");
	m_pSellSprite->SetX(x - 125);
	m_pSellSprite->SetY(y + 525);

	m_pSellText = new DynamicText();
	m_pSellText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 36, true);
	m_pSellText->SetText(renderer, "Sell");
	m_pSellText->SetPosition(x - 125, y + 525);

	m_pUpgrade1 = new UIUpgradeButton();
	m_pUpgrade1->Initialise(renderer, x, y, -1);

	m_pUpgrade2 = new UIUpgradeButton();
	m_pUpgrade2->Initialise(renderer, x, y, 0);

	m_pUpgrade3 = new UIUpgradeButton();
	m_pUpgrade3->Initialise(renderer, x, y, 1);

	m_pTargetFirstSprite = renderer.CreateSprite("..\\assets\\ui\\target_first.png");
	m_pTargetFirstSprite->SetX(x);
	m_pTargetFirstSprite->SetY(y - 300);

	m_pTargetLastSprite = renderer.CreateSprite("..\\assets\\ui\\target_last.png");
	m_pTargetLastSprite->SetX(x);
	m_pTargetLastSprite->SetY(y - 300);

	m_pSprites[0] = m_pTargetFirstSprite;
	m_pSprites[1] = m_pTargetLastSprite;
	m_iSpriteIndex = 0;

	m_pTargetText = new DynamicText();
	m_pTargetText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 32, true);
	m_pTargetText->SetText(renderer, "Target: First");
	m_pTargetText->SetPosition(x, y - 370);
}

void UISidepanel::Process(float deltaTime, InputSystem& input, int* gold)
{
	// if we have a tower in sidepanel, and mouse clicks on sell button
	if (m_pSidepanelTower)
	{
		if (IsSpriteHovered(m_pSellSprite, input) && input.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSidepanelTower->Sell();
			m_pSidepanelTower = 0;
		}
		else if (IsSpriteHovered(m_pTargetFirstSprite, input) && input.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			std::cout << "toggled targeting\n";
			m_pSidepanelTower->SwapTargeting();
		}

		if (m_pSidepanelTower->IsTargetingLast())
		{
			m_iSpriteIndex = 1;
		}
		else
		{
			m_iSpriteIndex = 0;
		}
			
	}

	m_pUpgrade1->Process(deltaTime, input, gold);
	m_pUpgrade2->Process(deltaTime, input, gold);
	m_pUpgrade3->Process(deltaTime, input, gold);
}

void UISidepanel::Draw(Renderer& renderer)
{
	m_pPanelSprite->Draw(renderer);


	if (m_pSidepanelTower)
	{
		m_pTowerTitle->Draw(renderer);
		m_pSellSprite->Draw(renderer);
		m_pSellText->Draw(renderer);

		m_pUpgrade1->Draw(renderer);
		m_pUpgrade2->Draw(renderer);
		m_pUpgrade3->Draw(renderer);

		if (m_iSpriteIndex == 1)
		{
			m_pTargetText->SetText(renderer, "Target: Last");
		}
		else
		{
			m_pTargetText->SetText(renderer, "Target: First");
		}
		m_pSprites[m_iSpriteIndex]->Draw(renderer);
		m_pTargetText->Draw(renderer);
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
		m_pUpgrade1->SetTower(renderer, tower);
		m_pUpgrade2->SetTower(renderer, tower);
		m_pUpgrade3->SetTower(renderer, tower);
	}
	else
	{
		m_pSidepanelTower = 0;
		m_pTowerTitle->SetText(renderer, " ");
		m_pUpgrade1->SetTower(renderer, 0);
		m_pUpgrade2->SetTower(renderer, 0);
		m_pUpgrade3->SetTower(renderer, 0);
	}
}