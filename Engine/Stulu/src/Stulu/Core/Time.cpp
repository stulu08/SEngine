#include "st_pch.h"
#include "Time.h"

namespace Stulu {
	Timestep Time::frameTime = 0.0f;
	Timestep Time::deltaTime = 0.0f;
	Timestep Time::applicationRuntime = 0.0f;
	float Time::Scale = 1.0f;
}