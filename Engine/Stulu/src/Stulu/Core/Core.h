#pragma once

#include "Log.h"

#define ST_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
#define ST_BIT(x) (1 << x)
#define ST_CONCAT_INNER(a, b) a ## b
#define ST_CONCAT(a, b) ST_CONCAT_INNER(a, b)
#define ST_ENUM_TYPE uint16_t
#define ST_ENUM(name) enum class name : ST_ENUM_TYPE
#define ST_STRINGIFY(x) ST_MAKESTRING(x)
#define ST_MAKESTRING(x) #x

#ifdef ST_PLATFORM_WINDOWS
#define ST_DEBUGBREAK() __debugbreak()
#elif ST_PLATFORM_LINUX
#include <signal.h>
#define ST_DEBUGBREAK() raise(SIGTRAP)
#endif


#if ST_ENABLE_ASSERTS
	#define ST_ASSERT(x, ...) {if(!(x)){ST_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); ST_DEBUGBREAK(); } }
	#define CORE_ASSERT(x, ...) {if(!(x)){CORE_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); ST_DEBUGBREAK(); } }
#elif ST_ENABLE_ASSERTS_ONLY_LOGGING
	#define ST_ASSERT(x, ...)	{if(!(x)){ST_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); } }
	#define CORE_ASSERT(x, ...)	{if(!(x)){CORE_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); } }
#else
#define ST_ASSERT(x, ...)	
#define CORE_ASSERT(x, ...)	
#endif // ST_ENABLE_ASSERT


namespace Stulu {
	template<typename T>
	using Scope = STULU_API std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
	template<typename T>
	using Ref = STULU_API std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
#include "Utils.h"