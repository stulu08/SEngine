#pragma once

namespace StuluBindings {
	class Log {
	public:
		static inline void engine_log(int32_t level, Stulu::Mono::String msg) {
			Stulu::Log::engine_log(level, msg.ToUtf8());
		}
		static inline void client_log(int32_t level, Stulu::Mono::String msg) {
			Stulu::Log::client_log(level, msg.ToUtf8());
		}
	};
}