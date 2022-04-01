#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

#include "Stulu/Core/CoreConfig.h"
#include "Stulu/Core/PlatformConfig.h"

namespace Stulu {
	class STULU_API Log {
	public:
		static void init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger();
		static std::shared_ptr<spdlog::logger>& GetClientLogger();
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
//Core Log
#define CORE_CRITICAL(...)	::Stulu::Log::GetCoreLogger()->critical(__VA_ARGS__);
#define CORE_ERROR(...)		::Stulu::Log::GetCoreLogger()->error(__VA_ARGS__);
#define CORE_WARN(...)		::Stulu::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define CORE_INFO(...)		::Stulu::Log::GetCoreLogger()->info(__VA_ARGS__);

//Client Log
#define ST_CRITICAL(...)	::Stulu::Log::GetClientLogger()->critical(__VA_ARGS__);
#define ST_ERROR(...)		::Stulu::Log::GetClientLogger()->error(__VA_ARGS__);
#define ST_WARN(...)		::Stulu::Log::GetClientLogger()->warn(__VA_ARGS__);
#define ST_INFO(...)		::Stulu::Log::GetClientLogger()->info(__VA_ARGS__);

#if ST_ENABLE_TRACE_LOGGING
#define CORE_TRACE(...)		::Stulu::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define ST_TRACE(...)		::Stulu::Log::GetClientLogger()->trace(__VA_ARGS__);
#else
#define CORE_TRACE(...)
#define ST_TRACE(...)
#endif
