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
