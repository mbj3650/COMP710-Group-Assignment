// COMP710 JESSE
// This include:
#include "Pathmaker.h"

// Local includes:
#include "renderer.h"

// Library includes:
#include <cassert>

Pathmaker::Pathmaker()
{

};
Pathmaker::~Pathmaker()
{

};

bool
Pathmaker::Initialise(Renderer& renderer, Vector2 Startpos)
{
	this->pos = Startpos;//literally just holds the position at this time, i expected to add more features but if not we can remove this from the program
	return true;
};

void
Pathmaker::Process(float deltaTime)
{
	assert(0);
};



void
Pathmaker::Draw(Renderer& renderer)
{
	assert(0);
};
