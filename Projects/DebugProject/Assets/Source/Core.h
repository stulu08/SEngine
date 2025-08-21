#pragma once
#include <Stulu/Core/Core.h>

#ifdef _WIN32
	#if APP_DYNAMIC_LINK
			#ifdef APP_DLL_BUILD
			#define APP_API __declspec(dllexport)
		#else
			#define APP_API __declspec(dllimport)
		#endif // APP_DLL_BUILD
	#else
		#define APP_API
	#endif
#else
	#error Only Winodws is currently supported in Stulu
#endif // ST_PLATFORM_WINDOWS