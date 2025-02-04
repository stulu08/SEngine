#include "st_pch.h"
#include "Profiler.h"

Profiler Profiler::s_instance;

Profiler& Profiler::GetInstance() {
     return s_instance;
}