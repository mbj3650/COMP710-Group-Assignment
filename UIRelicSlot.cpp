#include "UIRelicSlot.h"
#include "UIShopManager.h"
#include "renderer.h"
#include "sprite.h"
#include "DynamicText.h"
#include "inputsystem.h"
#include <iostream>
#include <string>
#include "game.h"
#include "GameData.h"
#include "RelicData.h"
UIRelicSlot::UIRelicSlot()
{
	m_pSlotSprite = 0;
	m_pAmountText = 0;
	m_iAmount = 0;
}

UIRelicSlot::~UIRelicSlot()
{
	delete m_pSlotSprite;
	m_pSlotSprite = 0;
	delete m_pAmountText;
	m_pAmountText = 0;
}

void UIRelicSlot::Initialise(Renderer& renderer)
{
	m_pAmountText = new DynamicText();
	m_pAmountText->Initialise(renderer, "C:\\Windows\\Fonts\\arial.ttf", 24, false);
	m_pAmountText->SetText(renderer, "Placeholder");
	m_iAmount = 0;
	storedRelic = "";
}

void UIRelicSlot::Update(Renderer& renderer, int baseX, int baseY, int index, std::string relic, int amount)
{
	if (m_pSlotSprite) 
	{ 
		delete m_pSlotSprite; 
		m_pSlotSprite = 0; 
	}
	storedRelic = relic;
	const auto& data = GameData::Get().Relic[relic];
	string SpritePath = "..\\assets\\relics\\" + data.Sprite + ".png";
	m_pSlotSprite = renderer.CreateSprite(SpritePath.c_str());

	m_pAmountText->SetText(renderer, std::to_string(amount));
	
	int col = index % 5;
	int row = index / 5;
	int finalX = baseX + col*80;
	int finalY = baseY + row*80;
	
	m_pSlotSprite->SetX(finalX);
	m_pSlotSprite->SetY(finalY);

	m_pAmountText->SetPosition(finalX+20, finalY+20);
}


void UIRelicSlot::Clear(Renderer& renderer)
{
	if (m_pSlotSprite) delete m_pSlotSprite; m_pSlotSprite = 0;
	m_pAmountText->SetText(renderer, " ");
	m_iAmount = 0;
	storedRelic = "";
}

void UIRelicSlot::Draw(Renderer& renderer)
{
	if (m_pSlotSprite) m_pSlotSprite->Draw(renderer);
	if (m_pAmountText) m_pAmountText->Draw(renderer);
}

bool UIRelicSlot::IsElementHovered(InputSystem& input) const
{
	Vector2 mousePos = input.GetMousePosition();
	if (mousePos.x >= m_pSlotSprite->GetX() - m_pSlotSprite->GetWidth() / 2 &&
		mousePos.x <= m_pSlotSprite->GetX() + m_pSlotSprite->GetWidth() / 2 &&
		mousePos.y >= m_pSlotSprite->GetY() - m_pSlotSprite->GetHeight() / 2 &&
		mousePos.y <= m_pSlotSprite->GetY() + m_pSlotSprite->GetHeight() / 2)
		return true;
	else return false;
}

std::string UIRelicSlot::GetStoredRelic() const
{
	return storedRelic;
}