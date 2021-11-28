#pragma once
#ifdef _WIN32
	#ifdef _WIN64
		#define ST_PLATFORM_WINDOWS
	#else
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define ST_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define ST_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__ANDROID__)
	#define ST_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define ST_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	#error "Unknown platform!"
#endif
#ifdef _WIN32
	#if ST_DYNAMIC_LINK
		#ifdef ST_DLL_BUILD
			#define STULU_API __declspec(dllexport)
		#else
			#define STULU_API __declspec(dllimport)
		#endif // ST_DLL_BUILD
	#else
		#define ST_STATIC_BUILD
		#define STULU_API
	#endif
#else
	#error Only Winodws is currently supported in Stulu
#endif // ST_PLATFORM_WINDOWS