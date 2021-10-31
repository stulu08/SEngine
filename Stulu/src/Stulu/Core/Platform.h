#pragma once

namespace Stulu {
	/// <summary>
	/// in bytes
	/// </summary>
	struct MemoryUsageInfo {
		uint64_t totalVirtual;
		uint64_t virtualUsed;
		uint64_t virtualUsedByProcess;

		uint64_t totalPhysical;
		uint64_t physicalUsed;
		uint64_t physicalUsedByProcess;
	};
	class Platform {
	public:
		static float getTime();

		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);

		static bool createDirectory(const char* directory);

		static MemoryUsageInfo getMemoryUsage();
	};
}