// COMP710 GP Framework 2025
// SceneSplash.h
// Author: MartinYan12138y
// Displays the splash screen for SPLASH_DURATION seconds,
// then automatically advances to the next scene (SceneGame).
// Press SPACE or ENTER to skip.

#ifndef __SCENESPLASH_H__
#define __SCENESPLASH_H__

#include "scene.h"

class Sprite;
class Renderer;

class SceneSplash : public Scene
{
public:
    SceneSplash();
    virtual ~SceneSplash();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime, InputSystem& inputSystem);
    virtual void Draw(Renderer& renderer);
    virtual void DebugDraw();

private:
    SceneSplash(const SceneSplash&);
    SceneSplash& operator=(const SceneSplash&);

    Sprite* m_pSplashSprite;   // the splash image
    float   m_fTimer;          // counts up to SPLASH_DURATION
    bool    m_bDone;           // true once we have advanced
};

#endif // __SCENESPLASH_H__
