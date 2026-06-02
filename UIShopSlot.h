#ifndef UISHOPSLOT_H
#define UISHOPSLOT_H
#include <string>
class Renderer;
class Sprite;
class Tower;
class DynamicText;
class InputSystem;

enum SlotState
{
    SLOT_NOTHING = 0,
    SLOT_HOVER = 1,
    SLOT_SELECTED = 2
};

class UIShopSlot
{
public:
    UIShopSlot();
    ~UIShopSlot();

    void Initialise(Renderer& renderer, int x, std::string path, int price);
	void SetState(int state);
    int GetState() const;
	void Process(float deltaTime, InputSystem& input);
	void Draw(Renderer& renderer);
	bool IsElementHovered(InputSystem& input) const;
private:
    UIShopSlot(const UIShopSlot&);
    UIShopSlot& operator=(const UIShopSlot&);
    Sprite* m_pSlotSprite;
    Sprite* m_pSlotHoverSprite;
    Sprite* m_pSlotSelectedSprite;
	Sprite* m_pIconSprite;
    Sprite* m_pSprites[3];
    int m_iSpriteIndex;
    int m_iSlot;
    int m_iPrice;
	DynamicText* m_pPriceText;
};

#endif // UISHOPSLOT_H
