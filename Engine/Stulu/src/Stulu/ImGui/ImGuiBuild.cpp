#include "st_pch.h"

#ifdef OPENGL
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#if IMGUI_OPENGL_3
		#include "backends/imgui_impl_opengl3.cpp"
	#endif
	#if IMGUI_OPENGL_4
		#include "backends/imgui_impl_opengl4.cpp"
	#endif
#endif

#ifdef USING_GLFW
	#include "backends/imgui_impl_glfw.cpp"
#endif