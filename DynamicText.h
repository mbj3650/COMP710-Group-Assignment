// DynamicText.h
#ifndef DYNAMICTEXT_H
#define DYNAMICTEXT_H

#include <string>

class Texture;
class Sprite;
class Renderer;

class DynamicText
{
public:
    DynamicText();
    ~DynamicText();

    bool Initialise(Renderer& renderer,const char* fontPath, int pointSize, bool centered);

    void SetText(Renderer& renderer, const std::string& text);

    void Draw(Renderer& renderer);

    void SetPosition(float x, float y);

private:
    // Disable copy
    DynamicText(const DynamicText&);
    DynamicText& operator=(const DynamicText&);

private:
    Texture* m_pTexture;
    Sprite* m_pSprite;

    std::string m_text;
    std::string m_fontPath;

    int m_pointSize;

    float m_x;
    float m_y;

    bool m_bCentre;
};

#endif