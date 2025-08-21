#include "st_pch.h"
#include "Log.h"

#include <ctime>
#include <filesystem>

namespace Stulu {
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::vector<std::shared_ptr<spdlog::sinks::sink>> Log::s_sinks;

	void Log::init() {
		spdlog::set_pattern("%^[%T][%n]: %v%$");
		if (!s_CoreLogger) {
			s_CoreLogger = spdlog::stdout_color_mt("STULU");
			s_CoreLogger->set_level(spdlog::level::trace);
		}
		if (!s_ClientLogger) {
			s_ClientLogger = spdlog::stdout_color_mt("APP");
			s_ClientLogger->set_level(spdlog::level::trace);
		}
		s_sinks.clear();
		CORE_INFO("Created Core and Client Logger");
	}

	void Log::AddFileSink(const std::string& logFile, Level level) {
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
		AddSink(sink, level);
		CORE_INFO("Added file sink: {0}", logFile);
	}

	void Log::AddSink(const std::shared_ptr<spdlog::sinks::sink> sink, Level level) {
		sink->set_level((spdlog::level::level_enum)level);
		s_CoreLogger->flush_on((spdlog::level::level_enum)level);
		s_ClientLogger->flush_on((spdlog::level::level_enum)level);
		s_CoreLogger->sinks().push_back(sink);
		s_ClientLogger->sinks().push_back(sink);
		s_sinks.push_back(sink);
	}

	std::string Log::generateTimeString() {
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		// will always be 19 chars long since formating
		std::string result = std::string(19, ' ');
		sprintf(&result[0], "%04d-%02d-%02d_%02d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
		return result;
	}
}