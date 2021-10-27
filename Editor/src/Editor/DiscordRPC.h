#pragma once
#include <Stulu.h>

namespace Stulu {
	class DiscordRPC {
	public:
		static void init(const char* applicationID);
		static void shutdown();

		static inline void setState(std::string state) { s_state = state; }
		static inline void setDetails(std::string details) { s_details = details; }
		static inline void setLargeImageKey(std::string largeImageKey) { s_largeImageKey = largeImageKey; }
	private:
		static void create(const char* applicationID);
		static void threadLoop(const char* applicationID);

		static bool s_running;
		static bool s_activated;

		static std::string s_state;
		static std::string s_details;
		static std::string s_largeImageKey;
	};
}