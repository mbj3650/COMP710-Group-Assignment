#include "soundsystem.h"
#include "fmod.hpp"
#include "logmanager.h"
#include <iostream>

SoundSystem::SoundSystem()
{

}

SoundSystem::~SoundSystem()
{
	std::map<std::string, FMOD::Sound*>::iterator iter = m_pLoadedSounds.begin();
	while (iter != m_pLoadedSounds.end())
	{
		FMOD::Sound* pSound = iter->second;
		pSound->release();
		++iter;
	}
	m_pLoadedSounds.clear();
	m_pSystem->release();
}

void SoundSystem::Init()
{
	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(512, FMOD_INIT_NORMAL, 0);
	

}

void SoundSystem::Process()
{
	m_pSystem->update();
}

void SoundSystem::CreateSound(const char* filename)
{
	m_pSystem->createSound(filename, FMOD_DEFAULT, 0, &m_pLoadedSounds[filename]);
}

void SoundSystem::PlaySound(const char* filename)
{
	
	if (m_pLoadedSounds.find(filename) == m_pLoadedSounds.end())
	{
		LogManager::GetInstance().Log("SoundSystem: Couldn't find sound");
	}
	else
	{
		FMOD_RESULT result = m_pSystem->playSound(m_pLoadedSounds[filename], 0, false, 0);
		if (result != FMOD_OK)
		{
			std::cout << "FMOD:" << result << std::endl;
		}
	}
}