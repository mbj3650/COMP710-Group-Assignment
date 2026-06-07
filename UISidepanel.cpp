#include "UISidepanel.h"
#include "UIShopManager.h"
#include "UIUpgradeButton.h"
#include "renderer.h"
#include "sprite.h"
#include "Tower.h"
#include "DynamicText.h"
#include "inputsystem.h"
#include "UIRelicSlot.h"
#include "GameData.h"
#include "RelicData.h"
#include <iostream>
#include <string>
#include "game.h"
#include "RelicManager.h"
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
	m_pRelicNameText = 0;
	m_pRelicDescText = 0;
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

	for (int i = 0; i < (int)m_pRelicSlots.size(); i++)
	{
		delete m_pRelicSlots[i];
		m_pRelicSlots[i] = 0;
	}
	m_pRelicSlots.clear();

	delete m_pRelicNameText;
	m_pRelicNameText = 0;
	delete m_pRelicDescText;
	m_pRelicDescText = 0;
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
	m_pTowerTitle->SetPosition(x, y - 450);

	m_pSellSprite = renderer.CreateSprite("..\\assets\\ui\\sellbutton.png");
	m_pSellSprite->SetX(x - 125);
	m_pSellSprite->SetY(y + 470);

	m_pSellText = new DynamicText();
	m_pSellText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 36, true);
	m_pSellText->SetText(renderer, "Sell");
	m_pSellText->SetPosition(x - 125, y + 470);

	m_pUpgrade1 = new UIUpgradeButton();
	m_pUpgrade1->Initialise(renderer, x, y, -1);

	m_pUpgrade2 = new UIUpgradeButton();
	m_pUpgrade2->Initialise(renderer, x, y, 0);

	m_pUpgrade3 = new UIUpgradeButton();
	m_pUpgrade3->Initialise(renderer, x, y, 1);

	m_pTargetFirstSprite = renderer.CreateSprite("..\\assets\\ui\\target_first.png");
	m_pTargetFirstSprite->SetX(x);
	m_pTargetFirstSprite->SetY(y - 250);

	m_pTargetLastSprite = renderer.CreateSprite("..\\assets\\ui\\target_last.png");
	m_pTargetLastSprite->SetX(x);
	m_pTargetLastSprite->SetY(y - 250);

	m_pSprites[0] = m_pTargetFirstSprite;
	m_pSprites[1] = m_pTargetLastSprite;
	m_iSpriteIndex = 0;

	m_pTargetText = new DynamicText();
	m_pTargetText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 32, true);
	m_pTargetText->SetText(renderer, "Target: First");
	m_pTargetText->SetPosition(x, y - 320);
	
	m_pRelicNameText = new DynamicText();
	m_pRelicNameText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 48, true);
	m_pRelicNameText->SetText(renderer, " ");
	m_pRelicNameText->SetPosition(renderer.GetWidth()/2, 50);

	m_pRelicDescText = new DynamicText();
	m_pRelicDescText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 30, true);
	m_pRelicDescText->SetText(renderer, " ");
	m_pRelicDescText->SetPosition(renderer.GetWidth() / 2, 100);
}

void UISidepanel::Process(float deltaTime, InputSystem& input, int* gold, Renderer& renderer)
{
	// if we have a tower in sidepanel, and mouse clicks on sell button
	if (m_pSidepanelTower)
	{
		if (IsSpriteHovered(m_pTargetFirstSprite, input) && input.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			std::cout << "toggled targeting\n";
			m_pSidepanelTower->SwapTargeting();
			Game::GetInstance().GetSoundSystem()->PlaySound("..\\assets\\sounds\\select.wav");
		}

		if (m_pSidepanelTower->IsTargetingLast())
		{
			m_iSpriteIndex = 1;
		}
		else
		{
			m_iSpriteIndex = 0;
		}

		// Selling tower has to be done last
		if (IsSpriteHovered(m_pSellSprite, input) && input.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
		{
			m_pSidepanelTower->Sell();
			m_pSidepanelTower = 0;
			Game::GetInstance().GetSoundSystem()->PlaySound("..\\assets\\sounds\\bump.wav");
		}
			
	}

	m_pUpgrade1->Process(deltaTime, input, gold);
	m_pUpgrade2->Process(deltaTime, input, gold);
	m_pUpgrade3->Process(deltaTime, input, gold);
	
	m_bDisplayRelicText = false;
	for (UIRelicSlot* slot : m_pRelicSlots)
	{
		if (slot->IsElementHovered(input))
		{
			m_bDisplayRelicText = true;
			const auto& data = GameData::Get().Relic[slot->GetStoredRelic()];
			m_pRelicNameText->SetText(renderer, data.Name);
			m_pRelicDescText->SetText(renderer, data.Description);
		}
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
	else
	{
		for (UIRelicSlot* slot : m_pRelicSlots)
		{
			slot->Draw(renderer);
		}
		if (m_bDisplayRelicText)
		{
			m_pRelicNameText->Draw(renderer);
			m_pRelicDescText->Draw(renderer);
		}
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
		Game::GetInstance().GetSoundSystem()->PlaySound("..\\assets\\sounds\\select.wav");
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

void UISidepanel::RefreshRelicSlots(Renderer& renderer)
{
	
	for (int i = 0; i < (int)m_pRelicSlots.size(); i++)
	{
		delete m_pRelicSlots[i];
		m_pRelicSlots[i] = 0;
	}
	m_pRelicSlots.clear();

	int x = renderer.GetWidth() - m_pPanelSprite->GetWidth() / 2 - 50;
	int y = renderer.GetHeight() / 2;
	int index = 0;

	for (const auto& pair : RelicManager::Get().TowerRelics)
	{
		UIRelicSlot* newSlot = new UIRelicSlot();
		newSlot->Initialise(renderer);
		newSlot->Update(renderer, x - 160, y - 460, index, pair.first, pair.second);
		index++;
		m_pRelicSlots.push_back(newSlot);
	}

	for (const auto& pair : RelicManager::Get().ProjectileRelics)
	{
		UIRelicSlot* newSlot = new UIRelicSlot();
		newSlot->Initialise(renderer);
		newSlot->Update(renderer, x - 160, y - 460, index, pair.first, pair.second);
		index++;
		m_pRelicSlots.push_back(newSlot);
	}
}