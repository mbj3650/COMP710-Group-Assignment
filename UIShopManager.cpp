#include "UIShopManager.h"
#include "UIShopSlot.h"
#include "GameData.h"
#include "TowerData.h"
#include "UISidepanel.h"
#include "UIUpgradeButton.h"
#include <vector>
#include <string>
#include <iostream>
#include <SDL.h>
#include "inputsystem.h"
UIShopManager::UIShopManager() :
	m_iSelectedSlot(-1),
	m_pSidePanel(0)
{
}

UIShopManager::~UIShopManager()
{
	for (UIShopSlot* slot : m_shopSlots) {
		delete slot;
		slot = 0;
	}
	delete m_pSidePanel;
	m_pSidePanel = 0;
}

UIShopManager* UIShopManager::sm_pInstance = 0;
UIShopManager& UIShopManager::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new UIShopManager();
	}
	return (*sm_pInstance);
}

void UIShopManager::DestroyInstance()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

void UIShopManager::Initialise(Renderer& renderer)
{
	const auto& towers = GameData::Get().Tower;
	for (int i = 0; i < towers.size(); i++)
	{
		for (const auto& p : towers)
		{
			if (p.second.ID == i)
			{
				m_towerIDs.push_back(p.first);
				UIShopSlot* slot = new UIShopSlot();
				slot->Initialise(
					renderer,
					p.second.ID,
					"..\\assets\\towers\\" + p.second.Sprite + ".png",
					p.second.Price
				);
				m_shopSlots.push_back(slot);
				break;
			}
		}
	}
	m_iSelectedSlot = -1;

	m_pSidePanel = new UISidepanel();
	m_pSidePanel->Initialise(renderer);
}

void UIShopManager::Process(float deltaTime, InputSystem& input, int* gold)
{
	for (UIShopSlot* slot : m_shopSlots) {
		slot->Process(deltaTime, input);
	}
	m_pSidePanel->Process(deltaTime, input, gold);


	if (input.GetMouseButtonState(SDL_BUTTON_RIGHT) == BS_PRESSED)
	{
		UpdateSelection(-1);
	}
}
void UIShopManager::Draw(Renderer& renderer)
{
	for (UIShopSlot* slot : m_shopSlots) {
		slot->Draw(renderer);
	}
	m_pSidePanel->Draw(renderer);
}

void UIShopManager::UpdateSelection(int slot)
{
	for (int i = 0; i < m_shopSlots.size(); i++) {
		if (i == slot) 
		{
			m_shopSlots[i]->SetState(SLOT_SELECTED);
			m_iSelectedSlot = slot;
		}
		else 
		{
			m_shopSlots[i]->SetState(SLOT_NOTHING);
		}
	}
	if (slot == -1) m_iSelectedSlot = slot;
}

std::string UIShopManager::GetSelectedTowerType()
{
	if (m_iSelectedSlot >= 0 && m_iSelectedSlot < m_towerIDs.size()) {
		return m_towerIDs[m_iSelectedSlot];
	}
	return "";
}

bool UIShopManager::IsTowerSelected() const
{
	return m_iSelectedSlot >= 0 && m_iSelectedSlot < m_towerIDs.size();
}

bool UIShopManager::IsAnyElementHovered(InputSystem& input) const
{
	for (int i = 0; i < m_shopSlots.size(); i++) {
		if (m_shopSlots[i]->IsElementHovered(input))
		{
			return true;
		}
	}
	if (m_pSidePanel->IsElementHovered(input)) return true;
	return false;
}

void UIShopManager::SetSidepanelTower(Renderer& renderer, Tower* tower)
{
	m_pSidePanel->SetTower(renderer, tower);
}