#ifndef UI_SIDEPANEL_H
#define UI_SIDEPANEL_H
#include <string>
class Renderer;
class Sprite;
class Tower;
class DynamicText;
class InputSystem;

class UISidepanel
{
public:
    UISidepanel();
    ~UISidepanel();

    void Initialise(Renderer& renderer);
    void Process(float deltaTime, InputSystem& input);
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
};

#endif // UI_SIDEPANEL_H
