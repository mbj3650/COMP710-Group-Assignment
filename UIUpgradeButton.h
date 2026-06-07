#ifndef UIUPGRADEBUTTON_H
#define UIUPGRADEBUTTON_H
#include <string>
class Renderer;
class Sprite;
class Tower;
class DynamicText;
class InputSystem;

class UIUpgradeButton
{
public:
    UIUpgradeButton();
    ~UIUpgradeButton();

    void Initialise(Renderer& renderer, int x, int y, int index);
    void SetState(int state);
    int GetState() const;
    void Process(float deltaTime, InputSystem& input, int* gold);
    void Draw(Renderer& renderer);
    bool IsElementHovered(InputSystem& input) const;
    void SetTower(Renderer& renderer, Tower* tower);
private:
    UIUpgradeButton(const UIUpgradeButton&);
    UIUpgradeButton& operator=(const UIUpgradeButton&);
    Sprite* m_pButtonSprite;
    Sprite* m_pButtonUsedSprite;
    Sprite* m_pArtSprite;
    Sprite* m_pSprites[2];
    int m_iSpriteIndex;

    DynamicText* m_pUpgradeText;
    DynamicText* m_pDescriptionText;
    DynamicText* m_pPriceText;

    Tower* m_associatedTower;
    int m_iUpgradeIndex;
    int m_iArtX;
    int m_iArtY;
};

#endif // UIUPGRADEBUTTON_H
