#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H
// Local includes:
#include "fmod.hpp"
// Library Includes:
#include <string>
#include <map>
// Forward declarations:
// Class declaration:
class SoundSystem
{
	// Member methods:
public:
	SoundSystem();
	~SoundSystem();
	void Init();
	void Process();
	void CreateSound(const char* filename);
	void PlaySound(const char* filename);
protected:
	std::map<std::string, FMOD::Sound*> m_pLoadedSounds;
private:
	// Member data:
	FMOD::System* m_pSystem;
public:
protected:
private:
};
#endif
