#include "UIShopSlot.h"
#include "UIShopManager.h"
#include "renderer.h"
#include "sprite.h"
#include "DynamicText.h"
#include "inputsystem.h"
#include <iostream>
#include "game.h"
UIShopSlot::UIShopSlot()
{
	m_pSlotSprite = 0;
	m_pSlotSelectedSprite = 0;
	m_pIconSprite = 0;
	m_iSpriteIndex = 0;
	m_pPriceText = 0;
	m_iSlot = 0;
}

UIShopSlot::~UIShopSlot()
{
	delete m_pSlotSprite;
	m_pSlotSprite = 0;
	delete m_pSlotSelectedSprite;
	m_pSlotSelectedSprite = 0;
	delete m_pSlotHoverSprite;
	m_pSlotHoverSprite = 0;
	delete m_pIconSprite;
	m_pIconSprite = 0;
	delete m_pPriceText;
	m_pPriceText = 0;
}

void UIShopSlot::Initialise(Renderer& renderer, int index, std::string towerPath, int price)
{
	m_pSlotSprite = renderer.CreateSprite("..\\assets\\ui\\shopicon.png");
	m_pSlotHoverSprite = renderer.CreateSprite("..\\assets\\ui\\shopicon_hover.png");
	m_pSlotSelectedSprite = renderer.CreateSprite("..\\assets\\ui\\shopicon_sel.png");
	m_pIconSprite = renderer.CreateSprite(towerPath.c_str());
	m_pSprites[SLOT_NOTHING] = m_pSlotSprite;
	m_pSprites[SLOT_HOVER] = m_pSlotHoverSprite;
	m_pSprites[SLOT_SELECTED] = m_pSlotSelectedSprite;

	int x = index * (m_pSlotSprite->GetWidth() + 10) + 50;
	int y = renderer.GetHeight() - m_pSlotSprite->GetHeight() / 2 - 10;
	m_pSlotSprite->SetX(x);
	m_pSlotSprite->SetY(y);

	m_pSlotHoverSprite->SetX(x);
	m_pSlotHoverSprite->SetY(y);

	m_pSlotSelectedSprite->SetX(x);
	m_pSlotSelectedSprite->SetY(y);

	m_pIconSprite->SetX(x);
	m_pIconSprite->SetY(y);
	m_pIconSprite->SetScale(64 / m_pIconSprite->GetWidth());

	m_pPriceText = new DynamicText();
	m_pPriceText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 18, false);
	m_pPriceText->SetText(renderer, std::to_string(price));
	m_pPriceText->SetPosition(x-35, y+15);

	m_iSlot = index;
}

void UIShopSlot::SetState(int state)
{
	m_iSpriteIndex = state;
}

int UIShopSlot::GetState() const
{
	return m_iSpriteIndex;
}

void UIShopSlot::Process(float deltaTime, InputSystem& input)
{
	if (GetState() != SLOT_SELECTED)
	{
		if (IsElementHovered(input))
		{
			SetState(SLOT_HOVER);
			if (input.GetMouseButtonState(1) == BS_PRESSED)
			{
				UIShopManager::GetInstance().UpdateSelection(m_iSlot);
				Game::GetInstance().GetSoundSystem()->PlaySound("..\\assets\\sounds\\select.wav");
			}
		}
		else
		{
			SetState(SLOT_NOTHING);
		}
	}

}

void UIShopSlot::Draw(Renderer& renderer)
{
	m_pSprites[m_iSpriteIndex]->Draw(renderer);
	m_pIconSprite->Draw(renderer);
	m_pPriceText->Draw(renderer);
}

bool UIShopSlot::IsElementHovered(InputSystem& input) const
{
	Vector2 mousePos = input.GetMousePosition();
	if (mousePos.x >= m_pSlotSprite->GetX() - m_pSlotSprite->GetWidth() / 2 &&
		mousePos.x <= m_pSlotSprite->GetX() + m_pSlotSprite->GetWidth() / 2 &&
		mousePos.y >= m_pSlotSprite->GetY() - m_pSlotSprite->GetHeight() / 2 &&
		mousePos.y <= m_pSlotSprite->GetY() + m_pSlotSprite->GetHeight() / 2)
		return true;
	else return false;
}