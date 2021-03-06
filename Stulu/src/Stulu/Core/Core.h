#pragma once

#pragma warning(disable:4251)

#include "CoreConfig.h"
//#include "PlatformConfig.h" in Log.h
#include "Log.h"

#if ST_ENABLE_ASSERTS
	#define ST_ASSERT(x, ...) {if(!(x)){ST_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); __debugbreak(); } }
	#define CORE_ASSERT(x, ...) {if(!(x)){CORE_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); __debugbreak(); } }
#elif ST_ENABLE_ASSERTS_ONLY_LOGGING
	#define ST_ASSERT(x, ...)	{if(!(x)){ST_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); } }
	#define CORE_ASSERT(x, ...)	{if(!(x)){CORE_ERROR("Assertion failed in {1} at line {2}:\n{0}", __VA_ARGS__, __FILE__, __LINE__); } }
#else
#define ST_ASSERT(x, ...)	
#define CORE_ASSERT(x, ...)	
#endif // ST_ENABLE_ASSERT

#define ST_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
#define ST_BIT(x) (1 << x)
#define ST_CONCAT_INNER(a, b) a ## b
#define ST_CONCAT(a, b) ST_CONCAT_INNER(a, b)


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