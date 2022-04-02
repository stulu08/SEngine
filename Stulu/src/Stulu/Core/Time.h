#pragma once
#include "Stulu/Core/Timestep.h"
namespace Stulu {
	class Time {
	public:
		static inline Timestep frameTime = .0f;
		//frame time scaled by Scale
		static inline Timestep deltaTime = .0f;
		//time since scene runtime started
		static inline Timestep time = .0f;
		//time since application startup
		static inline Timestep applicationRuntime = .0f;
		//deltatime is multiplied with this but not runtime
		static inline float Scale = 1.0f;
	};
}