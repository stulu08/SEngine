#pragma once
#include "Stulu/Core/Log.h"
#include <mono/metadata/object.h>

namespace StuluBindings {
	class Log {
	public:
		static inline void engine_log(int32_t level, MonoString* msg) {
			Stulu::Log::engine_log(level, mono_string_to_utf8(msg));
		}
		static inline void client_log(int32_t level, MonoString* msg) {
			Stulu::Log::client_log(level, mono_string_to_utf8(msg));
		}
	};
}