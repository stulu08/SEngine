#pragma once

namespace Stulu {
	class Platform {
	public:
		static float getTime();

		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	};
}