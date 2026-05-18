// COMP710 GP Framework 2025
// This include:
#include "particleemitter.h"
// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "Particle.h"
#include "vector2.h"
// Library includes:
#include <cassert>
#include "lib/imgui/imgui.h"
#include "box2d.h"
#include "inlinehelpers.h"
#include <iostream>
//Sprite* m_pSharedSprite;
//std::vector<Particle*> m_particles;
//float m_fTimeElapsed;
//int m_iSpawnBatchSize;
//float m_fEmitRate;
//float m_fMaxLifespan;
//float m_fAccelerationScalar;
//float m_fColour[3];
//float m_fMinAngle;
//float m_fMaxAngle;
//float m_fX;
//float m_fY;
ParticleEmitter::ParticleEmitter() :
	m_fTimeElapsed(0)
{

}
ParticleEmitter::~ParticleEmitter() {
	std::cout << "DELETING PARTICLES\n";
	for (int z = 0; z < m_particles.size(); z++) {
		if (m_particles.at(z) != NULL) {
			delete m_particles.at(z);
			m_particles.at(z) = 0;
		}

	}
	m_particles.clear();
	std::cout << "DELETED PARTICLES\n";
	if(m_pSharedSprite != NULL){
		try {
			delete(m_pSharedSprite);
			m_pSharedSprite = 0;
		}
		catch (...) {

		}
		
	}
	std::cout << "DELETING PARTICLE SPRITE\n";
}
bool ParticleEmitter::Initialise(Renderer& renderer, 
	const char* texture, 
	float m_fEmitRate,
	float m_fMaxLifespan, 
	float m_fAccelerationScalar, 
	float m_fColour[3],
	float m_fMinAngle,
	float m_fMaxAngle,
	float m_iSpawnBatchSize) {
	m_pSharedSprite = renderer.CreateSprite(texture);
	this->m_fEmitRate = m_fEmitRate;
	this->m_fMaxLifespan = m_fMaxLifespan;
	this->m_fAccelerationScalar = m_fAccelerationScalar;
	this->m_fColour[0] = m_fColour[0];
	this->m_fColour[1] = m_fColour[1];
	this->m_fColour[2] = m_fColour[2];
	this->m_fMinAngle = m_fMinAngle;
	this->m_fMaxAngle = m_fMaxAngle;
	this->m_iSpawnBatchSize = m_iSpawnBatchSize;
	return true;
}
void ParticleEmitter::Process(float deltaTime) {
	if (isemitting && m_fEmitRate != -1) {//if -1, this means that the game will manually emit for the particle emitter
		m_fTimeElapsed += 1 * deltaTime;
		if (m_fTimeElapsed > m_fEmitRate) {
			m_fTimeElapsed = 0;
			for (int b = 0; b < m_iSpawnBatchSize; b++) {
				Spawn();
			}
		}
	}

	for (int i = 0; i < m_particles.size(); i++) {
		if (m_particles.at(i)->m_bAlive == false) {
			delete m_particles.at(i);
			m_particles.at(i) = 0;
			m_particles.erase(m_particles.begin() + i);//erase particle at a given position if dead
		}
		else {
			m_particles.at(i)->Process(deltaTime);//else process it
		}
		
	}
}
void ParticleEmitter::Draw(Renderer& renderer) {
	for (int i = 0; i < m_particles.size(); i++) {
		m_particles.at(i)->Draw(renderer);
	}
}
void ParticleEmitter::Spawn() {
	Particle* pParticle = new Particle();
	pParticle->Initialise(*m_pSharedSprite);
	pParticle->m_bAlive = true;
	pParticle->m_fMaxLifespan = m_fMaxLifespan;
	pParticle->m_postion.x = m_fX;
	pParticle->m_postion.y = m_fY;
	float angle = GetRandom(m_fMinAngle, m_fMaxAngle);
	pParticle->m_acceleration.x += m_fAccelerationScalar * cos(angle);
	pParticle->m_acceleration.y += m_fAccelerationScalar * sin(angle);
	for (int i = 0; i < 3; i++) {
		pParticle->m_fColour[i] = m_fColour[i];
	}
	m_particles.push_back(pParticle);
}

void ParticleEmitter::SetParticlePosition(Vector2 position) {
	m_fX = position.x;
	m_fY = position.y;
}

void ParticleEmitter::SetParticlePosition(b2Vec2 position) {
	m_fX = position.x;
	m_fY = position.y;
}

void ParticleEmitter::SetParticleAngle(float min, float max) {
	m_fMaxAngle = max;
	m_fMinAngle = min;
}

void ParticleEmitter::DebugDraw() {
	ImGui::Text("PARTICLE INFORMATION:");
	ImGui::Text("Particles %f", m_particles.size());
	ImGui::Text("Time %f",m_fTimeElapsed);
	ImGui::Text("Emit Rate %f", m_fEmitRate);
}

void ParticleEmitter::turnoff() {
	isemitting = false;
}

void ParticleEmitter::turnon() {
	isemitting = true;
}