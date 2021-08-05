#pragma once

#include <memory>

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

#ifdef ST_PLATFORM_WINDOWS
	#if ST_DYNAMIC_LINK
		#ifdef ST_DLL_BUILD
			#define STULU_API __declspec(dllexport)
		#else
			#define STULU_API __declspec(dllimport)
		#endif // ST_DLL_BUILD
	#else 
		#define STULU_API
	#endif
#else
	#error Only Winodws is currently supported in Stulu
#endif // ST_PLATFORM_WINDOWS

#ifdef ST_DEBUG
	#define ST_ENABLE_ASSERTS
#endif

#ifdef ST_ENABLE_ASSERTS
	#define ST_ASSERT(x, ...) {if(!(x)){ST_ERROR("Asserion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CORE_ASSERT(x, ...) {if(!(x)){CORE_ERROR("Asserion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ST_ASSERT(x, ...)
	#define CORE_ASSERT(x, ...)
#endif // ST_ENABLE_ASSERT


#define ST_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
#define ST_BIT(x) (1 << x)
namespace Stulu {
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}