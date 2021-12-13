#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <any>
#include <set>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <future>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <iomanip>
#include <xhash>
#include <numeric>
#include <filesystem>

#include "Stulu/Core/Log.h"
#include "Stulu/Debug/Instrumentor.h"
#include "Stulu/Debug/RenderDataProfiler.h"


#ifdef ST_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <commdlg.h>
	#include <psapi.h>
	#include <shlobj_core.h>
#endif // ST_PLATFORM_WINDOWS

