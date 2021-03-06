#pragma once
//define OPENGL or Vulkan, only for debugging
#define OPENGL 1

#if OPENGL
	#define VULKAN 0
#else
	#define VULKAN 1
#endif
#ifdef ST_DEBUG
	#define ST_SHOWCONSOLE 1

	#define ST_PROFILING 1
	#define ST_PROFILING_RENDERDATA 1

	#define ST_LOG_FPS 0
	#define ST_ENABLE_ASSERTS 1
	#define ST_ENABLE_ASSERTS_ONLY_LOGGING 1
#if VULKAN
	#define ST_VULKAN_VERBOSE_LOGGING 0
	#define ST_VULKAN_INFO_LOGGING 0
#endif
#elif ST_RELEASE
	#define ST_SHOWCONSOLE 1
	#define ST_ENABLE_ASSERTS_ONLY_LOGGING 1
	#define ST_PROFILING_RENDERDATA 1

#elif ST_DIST
	#define ST_PROFILING_RENDERDATA 1
#endif
//for physX
#ifndef ST_DEBUG
	#define NDEBUG
#endif
