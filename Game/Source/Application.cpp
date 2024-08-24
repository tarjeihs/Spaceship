#include "Core/Application.h"
#include "Core/Entrypoint.h"

class PSpaceshipApplication : public PApplication
{
};

PApplication* CreateApplication()
{
	return new PSpaceshipApplication();
}