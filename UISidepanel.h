#ifndef UI_SIDEPANEL_H
#define UI_SIDEPANEL_H
#include <string>
class Renderer;
class Sprite;
class Tower;
class DynamicText;
class InputSystem;
class UIUpgradeButton;

class UISidepanel
{
public:
    UISidepanel();
    ~UISidepanel();

    void Initialise(Renderer& renderer);
    void Process(float deltaTime, InputSystem& input, int* gold);
    void Draw(Renderer& renderer);
    bool IsElementHovered(InputSystem& input) const;
    bool IsSpriteHovered(Sprite* sprite, InputSystem& input) const;
    void SetTower(Renderer& renderer, Tower* tower);
private:
    UISidepanel(const UISidepanel&);
    UISidepanel& operator=(const UISidepanel&);
    Sprite* m_pPanelSprite;

    // tower related
    Tower* m_pSidepanelTower;
	DynamicText* m_pTowerTitle;

    // sell button
    Sprite* m_pSellSprite;
	DynamicText* m_pSellText;

    // upgrade buttons
    UIUpgradeButton* m_pUpgrade1;
    UIUpgradeButton* m_pUpgrade2;
    UIUpgradeButton* m_pUpgrade3;
};

#endif // UI_SIDEPANEL_H
