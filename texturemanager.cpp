// COMP710 GP Framework 2025
// This include:
#include "texturemanager.h"
// Local includes:
#include "texture.h"
#include "logmanager.h"
// Library includes:
#include <cassert>
#include <SDL.h>
#include "lib/imgui/imgui.h"
#include <iostream>
TextureManager::TextureManager()
{
}
TextureManager::~TextureManager()
{
	std::map<std::string, Texture*>::iterator iter = m_pLoadedTextures.begin();
	std::cout << "TEXTURE LOOP STARTED!\n";

	while (iter != m_pLoadedTextures.end())
	{
		std::cout << "DELETING: "<< iter->first << "\n";
		Texture* pTexture = iter->second;
		delete pTexture;
		++iter;
	}
	std::cout << "TEXTURE LOOP ENDED!\n";
	m_pLoadedTextures.clear();
}
bool TextureManager::Initialise
()
{
	LogManager::GetInstance().Log("TextureManager starting...");
	return true;
}
Texture* TextureManager::GetTexture(const char* pcFilename)
{
	Texture* pTexture = 0;
	if (m_pLoadedTextures.find(pcFilename) == m_pLoadedTextures.end())
	{
		// Not already loaded... so load...
		pTexture = new Texture();
		if (!pTexture->Initialise(pcFilename))
		{
			LogManager::GetInstance().Log("Texture failed to initialise!");
			assert(0);
		}
		m_pLoadedTextures[pcFilename] = pTexture;
	}
	else
	{
		// It has already been loaded...
		pTexture = m_pLoadedTextures[pcFilename];
	}
	return pTexture;
}
void
TextureManager::AddTexture(const char* key, Texture* pTexture)
{
	m_pLoadedTextures[key] = pTexture;
}
void
TextureManager::DebugDraw()
{
	ImGui::Text("TextureManager");
	ImGui::Text("Stores %d textures", m_pLoadedTextures.size());
	std::map<std::string, Texture*>::iterator iter = m_pLoadedTextures.begin();
	while (iter != m_pLoadedTextures.end())
	{
		Texture* pTexture = iter->second;
		pTexture->DebugDraw();
		++iter;
	}
}