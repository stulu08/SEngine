#pragma once
#include "Stulu/Core/Core.h"

namespace Stulu::Profiling {
	class RenderDataProfiler {
	public:
		static void begin() { 
			drawCalls = 0;
			VerticesCount = 0;
			IndicesCount = 0;
		}


		static void addDrawCall() { drawCalls++; }
		static int getDrawCalls() { return drawCalls; }

		static void addVertices(int count) { VerticesCount = count; }
		static int getVerticesCount() { return VerticesCount; }

		static void addIndices(int count) { IndicesCount = count; }
		static int getIndicesCount() { return IndicesCount; }
	private:
		static inline int drawCalls = 0;
		static inline int VerticesCount = 0;
		static inline int IndicesCount = 0;
	};
}
#ifdef ST_PROFILING_RENDERDATA
#define ST_PROFILING_RENDERDATA_BEGIN()			Stulu::Profiling::RenderDataProfiler::begin()
#define ST_PROFILING_RENDERDATA_ADDDRAWCALL()	Stulu::Profiling::RenderDataProfiler::addDrawCall()
#define ST_PROFILING_RENDERDATA_ADDVERTICES(x)	Stulu::Profiling::RenderDataProfiler::addVertices(x)
#define ST_PROFILING_RENDERDATA_ADDINDICES(x)	Stulu::Profiling::RenderDataProfiler::addIndices(x)

#define ST_PROFILING_RENDERDATA_GETDRAWCALLS()	Stulu::Profiling::RenderDataProfiler::getDrawCalls()
#define ST_PROFILING_RENDERDATA_GETVERTICES()	Stulu::Profiling::RenderDataProfiler::getVerticesCount()
#define ST_PROFILING_RENDERDATA_GETINDICES()	Stulu::Profiling::RenderDataProfiler::getIndicesCount()

#else
#define ST_PROFILING_RENDERDATA_BEGIN()
#define ST_PROFILING_RENDERDATA_ADDDRAWCALL()
#define ST_PROFILING_RENDERDATA_ADDVERTICES(x)
#define ST_PROFILING_RENDERDATA_ADDINDICES(x)

#define ST_PROFILING_RENDERDATA_GETDRAWCALLS()
#define ST_PROFILING_RENDERDATA_GETVERTICES()
#define ST_PROFILING_RENDERDATA_GETINDICES()
#endif // ST_PROFILING_RENDERDATA
