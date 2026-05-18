// COMP710 GP Framework
// This include:
#include "particle.h"
// Local includes:
#include "renderer.h"
#include "sprite.h"
Particle::Particle()
	: m_bAlive(false)
	, m_fCurrentAge(0.0f)
{
}
Particle::~Particle()
{
	m_pSharedSprite = 0;
}
bool
Particle::Initialise(Sprite& sprite)
{
	m_pSharedSprite = &sprite;
	return true;
}
void
Particle::Process(float deltaTime)
{
	if (m_bAlive)
	{
		m_fCurrentAge += deltaTime;
		m_velocity.x += m_acceleration.x * deltaTime;
		m_velocity.y += m_acceleration.y * deltaTime;
		m_postion += m_velocity * deltaTime;
		if (m_fCurrentAge > m_fMaxLifespan)
		{
			m_bAlive = false;
		}
	}
}
void
Particle::Draw(Renderer& renderer)
{
	if (m_bAlive)
	{
		m_pSharedSprite->SetRedTint(m_fColour[0]);
		m_pSharedSprite->SetGreenTint(m_fColour[1]);
		m_pSharedSprite->SetBlueTint(m_fColour[2]);
		float alpha = 1.0f - (m_fCurrentAge / m_fMaxLifespan);
		m_pSharedSprite->SetAlpha(alpha);
		m_pSharedSprite->SetX(m_postion.x);
		m_pSharedSprite->SetY(m_postion.y);
		m_pSharedSprite->Draw(renderer);
	}
}