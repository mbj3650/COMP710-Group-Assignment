// DynamicText.cpp

#include "dynamictext.h"

#include "renderer.h"
#include "texture.h"
#include "sprite.h"

DynamicText::DynamicText()
    : m_pTexture(0)
    , m_pSprite(0)
    , m_pointSize(0)
    , m_x(0.0f)
    , m_y(0.0f)
    , m_bCentre(false)
{
}

DynamicText::~DynamicText()
{
    delete m_pSprite;
    m_pSprite = 0;

    delete m_pTexture;
    m_pTexture = 0;
}

bool
DynamicText::Initialise(Renderer& renderer, const char* fontPath, int pointSize, bool center)
{
    m_fontPath = fontPath;
    m_pointSize = pointSize;
    m_bCentre = center;
    return true;
}

void
DynamicText::SetText(Renderer& renderer, const std::string& text)
{
    // Prevent unnecessary rebuilds
    if (text == m_text)
    {
        return;
    }

    m_text = text;

    delete m_pSprite;
    m_pSprite = 0;
    delete m_pTexture;
    m_pTexture = 0;

    m_pTexture = new Texture();

    m_pTexture->LoadTextTexture(m_text.c_str(), m_fontPath.c_str(), m_pointSize);

    // Create sprite directly
    m_pSprite = new Sprite();
    m_pSprite->Initialise(*m_pTexture);

    if (m_pSprite)
    {
        m_pSprite->SetX(static_cast<int>(m_x));
        m_pSprite->SetY(static_cast<int>(m_y));
    }
}

void
DynamicText::Draw(Renderer& renderer)
{
    if (m_pSprite)
    {
        m_pSprite->Draw(renderer);
    }
}

void
DynamicText::SetPosition(float x, float y)
{
    m_x = x;
    m_y = y;

    if (m_pSprite)
    { 
        if (m_bCentre)
        {
            m_pSprite->SetX(static_cast<int>(m_x));
            m_pSprite->SetY(static_cast<int>(m_y));
        }
        else
        {
            m_pSprite->SetX(static_cast<int>(m_x) + m_pSprite->GetWidth() / 2);
            m_pSprite->SetY(static_cast<int>(m_y) + m_pSprite->GetHeight() / 2);
        }

    }
}