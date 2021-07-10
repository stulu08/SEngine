#pragma once


#ifdef ST_PLATFORM_WINDOWS
	#ifdef ST_DLL_BUILD
		#define STULU_API __declspec(dllexport)
	#else
		#define STULU_API __declspec(dllimport)
	#endif // ST_DLL_BUILD
#else
	#error Only Winodws is currently supported in Stulu
#endif // ST_PLATFORM_WINDOWS


#ifdef ST_ENABLE_ASSERTS
	#define ST_ASSERT(x, ...) {if(!(x)){ST_ERROR("Asserion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CORE_ASSERT(x, ...) {if(!(x)){CORE_ERROR("Asserion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ST_ASSERT(x, ...)
	#define CORE_ASSERT(x, ...)
#endif // ST_ENABLE_ASSERT



#define ST_BIT(x) (1 << x)
