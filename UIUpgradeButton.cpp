#include "UIUpgradeButton.h"
#include "UIShopManager.h"
#include "renderer.h"
#include "sprite.h"
#include "DynamicText.h"
#include "inputsystem.h"
#include "tower.h"
#include "TowerData.h"
#include "GameData.h"
#include <iostream>
UIUpgradeButton::UIUpgradeButton()
{
	m_pButtonSprite = 0;
	m_pButtonUsedSprite = 0;
	m_pArtSprite = 0;
	m_iSpriteIndex = 0;

	m_pUpgradeText = 0;
	m_pDescriptionText = 0;
	m_pPriceText = 0;

	m_associatedTower = 0;
	m_iUpgradeIndex = 0;

	m_iArtX = 0;
	m_iArtY = 0;
}

UIUpgradeButton::~UIUpgradeButton()
{
	delete m_pButtonSprite;
	m_pButtonSprite = 0;
	delete m_pButtonUsedSprite;
	m_pButtonUsedSprite = 0;
	m_pArtSprite = 0;

	delete m_pUpgradeText;
	m_pUpgradeText = 0;
	delete m_pDescriptionText;
	m_pDescriptionText = 0;
	delete m_pPriceText;
	m_pPriceText = 0;

	m_associatedTower = 0;
}

void UIUpgradeButton::Initialise(Renderer& renderer, int x, int y, int index)
{
	m_pButtonSprite = renderer.CreateSprite("..\\assets\\ui\\upgrade.png");
	m_pButtonUsedSprite = renderer.CreateSprite("..\\assets\\ui\\upgrade_bought.png");
	m_pSprites[0] = m_pButtonSprite;
	m_pSprites[1] = m_pButtonUsedSprite;
	m_iUpgradeIndex = index + 2;
	int btnX = x;
	int btnY = y + 120 + index * (m_pButtonSprite->GetHeight() + 75);
	m_pButtonSprite->SetX(btnX);
	m_pButtonSprite->SetY(btnY);
	m_pButtonSprite->SetScale(1.5f);

	m_pButtonUsedSprite->SetX(btnX);
	m_pButtonUsedSprite->SetY(btnY);
	m_pButtonUsedSprite->SetScale(1.5f);

	m_iArtX = btnX - 100;
	m_iArtY = btnY - 10;

	m_pUpgradeText = new DynamicText();
	m_pUpgradeText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 40, false);
	m_pUpgradeText->SetText(renderer, "Rapid Fire");
	m_pUpgradeText->SetPosition(btnX - 40, btnY - 40);

	m_pDescriptionText = new DynamicText();
	m_pDescriptionText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 24, false);
	m_pDescriptionText->SetText(renderer, "Attacks way faster");
	m_pDescriptionText->SetPosition(btnX - 40, btnY + 10);

	m_pPriceText = new DynamicText();
	m_pPriceText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 20, false);
	m_pPriceText->SetText(renderer, "Cost: 100");
	m_pPriceText->SetPosition(btnX - 150, btnY + 50);
}

void UIUpgradeButton::SetState(int state)
{
	m_iSpriteIndex = state;
}

int UIUpgradeButton::GetState() const
{
	return m_iSpriteIndex;
}

void UIUpgradeButton::Process(float deltaTime, InputSystem& input, int* gold)
{
	if (m_associatedTower)
	{
		if (GetState() != 1)
		{
			if (IsElementHovered(input) && input.GetMouseButtonState(1) == BS_PRESSED)
			{

				
				switch (m_iUpgradeIndex)
				{
				case 1:
					if (m_associatedTower->Upgrade(1, gold)) SetState(1);
					break;
				case 2:
					if (m_associatedTower->Upgrade(2, gold)) SetState(1);
					break;
				case 3:
					if (m_associatedTower->Upgrade(3, gold)) SetState(1);
					break;
				}
			}
		}
	}
}

void UIUpgradeButton::Draw(Renderer& renderer)
{
	m_pSprites[m_iSpriteIndex]->Draw(renderer);
	if (m_pArtSprite) m_pArtSprite->Draw(renderer);

	m_pUpgradeText->Draw(renderer);
	m_pDescriptionText->Draw(renderer);
	m_pPriceText->Draw(renderer);
}

bool UIUpgradeButton::IsElementHovered(InputSystem& input) const
{
	Vector2 mousePos = input.GetMousePosition();
	if (mousePos.x >= m_pButtonSprite->GetX() - m_pButtonSprite->GetWidth() / 2 &&
		mousePos.x <= m_pButtonSprite->GetX() + m_pButtonSprite->GetWidth() / 2 &&
		mousePos.y >= m_pButtonSprite->GetY() - m_pButtonSprite->GetHeight() / 2 &&
		mousePos.y <= m_pButtonSprite->GetY() + m_pButtonSprite->GetHeight() / 2)
		return true;
	else return false;
}

void UIUpgradeButton::SetTower(Renderer& renderer, Tower* tower)
{
	if (tower)
	{
		m_associatedTower = tower;

		m_pArtSprite = tower->GetUpgradeSprite(m_iUpgradeIndex);
		m_pArtSprite->SetX(m_iArtX);
		m_pArtSprite->SetY(m_iArtY);

		TowerData data = GameData::Get().Tower[tower->GetTowerID()];
		switch (m_iUpgradeIndex)
		{
		case 1:
			m_pUpgradeText->SetText(renderer, data.Upgrade1Name);
			m_pDescriptionText->SetText(renderer, data.Upgrade1Desc);
			m_pPriceText->SetText(renderer, std::to_string(data.Upgrade1Price));
			if (tower->CanUpgrade(1)) SetState(0);
			else SetState(1);
			break;
		case 2:
			m_pUpgradeText->SetText(renderer, data.Upgrade2Name);
			m_pDescriptionText->SetText(renderer, data.Upgrade2Desc);
			m_pPriceText->SetText(renderer, std::to_string(data.Upgrade2Price));
			if (tower->CanUpgrade(2)) SetState(0);
			else SetState(1);
			break;
		case 3:
			m_pUpgradeText->SetText(renderer, data.Upgrade3Name);
			m_pDescriptionText->SetText(renderer, data.Upgrade3Desc);
			m_pPriceText->SetText(renderer, std::to_string(data.Upgrade3Price));
			if (tower->CanUpgrade(3)) SetState(0);
			else SetState(1);
			break;
		}
		
	}
	else
	{
		m_associatedTower = 0;
	}
}