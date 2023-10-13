#pragma once
#include "Core.h"


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
	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
	enum class MessageBoxType : uint32_t {
		Abort_Retry_Ignore = 0x00000002L,
		Cancel_TryAgain_Continue = 0x00000006L,
		Help = 0x00004000L,
		Ok = 0x00000000L,
		Ok_Cancel = 0x00000001L,
		Retry_Cancel = 0x00000005L,
		Yes_No = 0x00000004L,
		Yes_No_Cancel = 0x00000003L,

		Icon_Exclemation = 0x00000030L,
		Icon_Warning = 0x00000030L,
		Icon_Information = 0x00000040L,
		Icon_Question = 0x00000020L,
		Icon_Stop = 0x00000010L,
		Icon_Error = 0x00000010L,
		Icon_Hand = 0x00000010L,
	};
	enum class MessageBoxReturn : uint32_t {
		Ok = 1,
		Cancel = 2,
		Abort = 3,
		Retry = 4,
		Ignore = 5,
		Yes = 6,
		No = 7,
		TryAgain = 10,
		Continue = 11,
	};
	class STULU_API Platform {
	public:
		static float getTime();

		static std::string openFile(const char* filter, const char* initialDir = NULL);
		static std::string saveFile(const char* filter, const char* initialDir = NULL);
		static std::string browseFolder();

		static int Message(const std::string& title, const std::string& msg, uint32_t type = (uint32_t)MessageBoxType::Icon_Error | (uint32_t)MessageBoxType::Ok);

		static bool createDirectory(const char* directory);

		static int changeWorkingDirectory(const char* dir = NULL);
		static std::string getCurrentWorkingDirectory();

		static MemoryUsageInfo getMemoryUsage();
	};
}