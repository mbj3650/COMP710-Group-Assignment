#ifndef UISHOPMANAGER_H
#define UISHOPMANAGER_H
#include <vector>
#include <string>
#include "UIShopSlot.h"
class Renderer;
class InputSystem;
class UISidepanel;
class UIShopManager
{
public:
    UIShopManager();
    ~UIShopManager();

	static UIShopManager& GetInstance();
	static void DestroyInstance();

	void Initialise(Renderer& renderer);
	void Process(float deltaTime, InputSystem& input, int* m_iGold, Renderer& renderer);
	void Draw(Renderer& renderer);
	void UpdateSelection(int slot);
	std::string GetSelectedTowerType();
	bool IsTowerSelected() const;
	bool IsAnyElementHovered(InputSystem& input) const;
	void SetSidepanelTower(Renderer& renderer, Tower* tower);
	void RefreshRelicSlots(Renderer& renderer);
protected:
	static UIShopManager* sm_pInstance;
private:
    UIShopManager(const UIShopManager&);
    UIShopManager& operator=(const UIShopManager&);
	std::vector<UIShopSlot*> m_shopSlots;
	std::vector<std::string> m_towerIDs;
	int m_iSelectedSlot;

	UISidepanel* m_pSidePanel;
};

#endif // UISHOPMANAGER_H
