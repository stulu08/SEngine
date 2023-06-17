#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

#include "Stulu/Core/CoreConfig.h"
#include "Stulu/Core/PlatformConfig.h"

namespace Stulu {
	class STULU_API Log {
	public:
		enum class Level {
			trace=0,info=1,warn=2,error=3,critical=4
		};

		static void init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger();
		static std::shared_ptr<spdlog::logger>& GetClientLogger();

		static inline void engine_log(int32_t level, const char* msg) {
			switch (level)
			{
#if ST_ENABLE_TRACE_LOGGING
			case (int32_t)Stulu::Log::Level::trace:
				GetCoreLogger()->trace(msg);
				break;
#endif
			case (int32_t)Stulu::Log::Level::info:
				GetCoreLogger()->info(msg);
				break;
			case (int32_t)Stulu::Log::Level::warn:
				GetCoreLogger()->warn(msg);
				break;
			case (int32_t)Stulu::Log::Level::error:
				GetCoreLogger()->error(msg);
				break;
			case (int32_t)Stulu::Log::Level::critical:
				GetCoreLogger()->critical(msg);
				break;
			}
		}
		static inline void client_log(int32_t level, const char* msg) {
			switch (level)
			{
#if ST_ENABLE_TRACE_LOGGING
			case (int32_t)Stulu::Log::Level::trace:
				GetClientLogger()->trace(msg);
				break;
#endif
			case (int32_t)Stulu::Log::Level::info:
				GetClientLogger()->info(msg);
				break;
			case (int32_t)Stulu::Log::Level::warn:
				GetClientLogger()->warn(msg);
				break;
			case (int32_t)Stulu::Log::Level::error:
				GetClientLogger()->error(msg);
				break;
			case (int32_t)Stulu::Log::Level::critical:
				GetClientLogger()->critical(msg);
				break;
			}
		}
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