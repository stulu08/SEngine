#pragma once

#include "Stulu/Core/Core.h"
#include "Stulu/Math/Math.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>


namespace Stulu {
	class Log {
	public:
		static void init();


		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
//Core Log
#define CORE_ERROR(...)  ::Stulu::Log::GetCoreLogger()->error(__VA_ARGS__);
#define CORE_WARN(...)   ::Stulu::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define CORE_INFO(...)   ::Stulu::Log::GetCoreLogger()->info(__VA_ARGS__);
#define CORE_TRACE(...)  ::Stulu::Log::GetCoreLogger()->trace(__VA_ARGS__);

//ClientLog
#define ST_ERROR(...)    ::Stulu::Log::GetClientLogger()->error(__VA_ARGS__);
#define ST_WARN(...)     ::Stulu::Log::GetClientLogger()->warn(__VA_ARGS__);
#define ST_INFO(...)     ::Stulu::Log::GetClientLogger()->info(__VA_ARGS__);
#define ST_TRACE(...)    ::Stulu::Log::GetClientLogger()->trace(__VA_ARGS__);