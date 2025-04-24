#pragma once
#include "Stulu/Core/Time.h"

namespace StuluBindings {
	class Time {
	public:
		static inline float time_delta() {
			return Stulu::Time::deltaTime;
		}
		static inline float time_frame() {
			return Stulu::Time::frameTime;
		}
		static inline float time_runtime() {
			return Stulu::Time::applicationRuntime;
		}
		static inline float time_time() {
			if(GetCurrentRegistry()->IsScene())
				return GetCurrentRegistry()->GetAsScene()->GetSceneRuntime();
			return 0.0f;
		}
		static inline float time_scale() {
			return Stulu::Time::Scale;
		}
		static inline void time_setScale(float v) {
			Stulu::Time::Scale = v;
		}
	};
}