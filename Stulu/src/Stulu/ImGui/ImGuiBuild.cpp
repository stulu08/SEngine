#include "st_pch.h"

#ifdef OPENGL
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include "backends/imgui_impl_opengl4.cpp"
#endif

#ifdef USING_GLFW
	#include "backends/imgui_impl_glfw.cpp"
#endif