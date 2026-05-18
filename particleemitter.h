// COMP710 GP Framework
#ifndef __PARTICLEEMITTER_H__
#define __PARTICLEEMITTER_H__
// Library includes:
#include <vector>
// Forward declarations:
class Renderer;
class Sprite;
class Texture;
class Particle;
class Vector2;
class b2Vec2;
// Class declaration:
class ParticleEmitter
{
	// Member methods:
public:
	ParticleEmitter();
	~ParticleEmitter();
	bool Initialise(Renderer& renderer,
		const char* texture,
		float m_fEmitRate,
		float m_fMaxLifespan,
		float m_fAccelerationScalar,
		float m_fColour[3],
		float m_fMinAngle,
		float m_fMaxAngle,
		float m_iSpawnBatchSize);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void Spawn();

	void SetParticlePosition(b2Vec2 position);
	void SetParticleAngle(float min, float max);
	void SetParticlePosition(Vector2 position);
	void DebugDraw();
	void turnoff();
	void turnon();
protected:
private:
	ParticleEmitter(const ParticleEmitter& particleemitter);
	ParticleEmitter& operator=(const ParticleEmitter& particleemitter);
	// Member data:
public:
protected:
	Sprite* m_pSharedSprite;
	std::vector<Particle*> m_particles;
	float m_fTimeElapsed;
	int m_iSpawnBatchSize;
	float m_fEmitRate;
	float m_fMaxLifespan;
	float m_fAccelerationScalar;
	float m_fColour[3];
	float m_fMinAngle;
	float m_fMaxAngle;
	float m_fX;
	float m_fY;
	bool isemitting;
private:
};
#endif // __PARTICLEEMITTER_H__