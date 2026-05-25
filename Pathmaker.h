// COMP710 JESSE
#include <vector>
#include "vector2.h"
#ifndef __PATHMAKER_H__
#define __PATHMAKER_H__

// Forward declarations:
class Renderer;
class Tile;
// Class declaration:
class Pathmaker
{
	// Member methods:
public:
	Pathmaker();
	~Pathmaker();

	bool Initialise(Renderer& renderer, Vector2 Startpos);
	void Process(float deltaTime);

	void Draw(Renderer& renderer);

protected:

private:
	Pathmaker(const Pathmaker& Pathmaker);
	Pathmaker& operator=(const Pathmaker& Pathmaker);

	// Member data:
public:
	Vector2 pos;

protected:

private:

};

#endif // __PATHMAKER_H__
