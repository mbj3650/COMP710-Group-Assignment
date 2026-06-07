#ifndef UIRELICSLOT_H
#define UIRELICSLOT_H
#include <string>
class Renderer;
class Sprite;
class Tower;
class DynamicText;
class InputSystem;

class UIRelicSlot
{
public:
    UIRelicSlot();
    ~UIRelicSlot();

    void Initialise(Renderer& renderer);
    void Update(Renderer& renderer, int baseX, int baseY, int index, std::string relic, int amount);
    void Clear(Renderer& renderer);
    void Draw(Renderer& renderer);
    bool IsElementHovered(InputSystem& input) const;
    std::string GetStoredRelic() const;
private:
    UIRelicSlot(const UIRelicSlot&);
    UIRelicSlot& operator=(const UIRelicSlot&);
    Sprite* m_pSlotSprite;
    int m_iAmount;
    DynamicText* m_pAmountText;
    std::string storedRelic;
};

#endif // UIRelicSlot_H
