#pragma once
#include "Stulu/Types/Timestep.h"

namespace Stulu {
	class STULU_API Time {
	public:
		// time of the last application update loop
		static Timestep frameTime;
		// frame time scaled by Scale
		static Timestep deltaTime;
		// time since application startup
		static Timestep applicationRuntime;
		// deltatime is multiplied with this but not runtime
		static float Scale;
	};
}