#pragma once
#include "Stulu/Core/Core.h"

namespace Stulu::Profiling {
	class STULU_API RenderDataProfiler {
	public:
		static inline void enable() {
			enabled = true;
		}
		static inline bool isEnabled() {
			return enabled;
		}
		static inline void disable() {
			enabled = false;
		}
		static inline void begin() { 
			VerticesCount = 0;
			IndicesCount = 0;
			CameraCount = 0;
			drawCalls = 0;
		}

		static inline void addDrawCall() { if(enabled)drawCalls++; }
		static inline int& getDrawCalls() { return drawCalls; }

		static inline void addVertices(int count) { if (enabled)VerticesCount += count; }
		static inline int& getVerticesCount() { return VerticesCount; }

		static inline void addIndices(int count) { if (enabled)IndicesCount += count; }
		static inline int& getIndicesCount() { return IndicesCount; }

		static inline void addCameras(int count) { if (enabled)CameraCount += count; }
		static inline int& getCameraCount() { return CameraCount; }
	private:
		static inline bool enabled = false;
		static inline int drawCalls = 0;
		static inline int VerticesCount = 0;
		static inline int IndicesCount = 0;
		static inline int CameraCount = 0;
	};
}
#if ST_PROFILING_RENDERDATA
#define ST_PROFILING_RENDERDATA_ENABLE()			Stulu::Profiling::RenderDataProfiler::enable()
#define ST_PROFILING_RENDERDATA_DISABLE()			Stulu::Profiling::RenderDataProfiler::disable()
#define ST_PROFILING_RENDERDATA_BEGIN()				Stulu::Profiling::RenderDataProfiler::begin()
#define ST_PROFILING_RENDERDATA_ADDDRAWCALL()		Stulu::Profiling::RenderDataProfiler::addDrawCall()
#define ST_PROFILING_RENDERDATA_ADDVERTICES(x)		Stulu::Profiling::RenderDataProfiler::addVertices((int)x)
#define ST_PROFILING_RENDERDATA_ADDINDICES(x)		Stulu::Profiling::RenderDataProfiler::addIndices((int)x)
#define ST_PROFILING_RENDERDATA_ADDCAMERAS(x)		Stulu::Profiling::RenderDataProfiler::addCameras((int)x)

#define ST_PROFILING_RENDERDATA_GETENABLE()			Stulu::Profiling::RenderDataProfiler::isEnabled()
#define ST_PROFILING_RENDERDATA_GETDRAWCALLS()		Stulu::Profiling::RenderDataProfiler::getDrawCalls()
#define ST_PROFILING_RENDERDATA_GETVERTICES()		Stulu::Profiling::RenderDataProfiler::getVerticesCount()
#define ST_PROFILING_RENDERDATA_GETINDICES()		Stulu::Profiling::RenderDataProfiler::getIndicesCount()
#define ST_PROFILING_RENDERDATA_GETCAMERAS()		Stulu::Profiling::RenderDataProfiler::getCameraCount()

#else
#define ST_PROFILING_RENDERDATA_ENABLE()
#define ST_PROFILING_RENDERDATA_DISABLE()
#define ST_PROFILING_RENDERDATA_BEGIN()
#define ST_PROFILING_RENDERDATA_ADDDRAWCALL()
#define ST_PROFILING_RENDERDATA_ADDVERTICES(x)
#define ST_PROFILING_RENDERDATA_ADDINDICES(x)
#define ST_PROFILING_RENDERDATA_ADDCAMERAS(x)

#define ST_PROFILING_RENDERDATA_GETENABLE()		false
#define ST_PROFILING_RENDERDATA_GETDRAWCALLS()	-1
#define ST_PROFILING_RENDERDATA_GETVERTICES()	-1
#define ST_PROFILING_RENDERDATA_GETINDICES()	-1
#define ST_PROFILING_RENDERDATA_GETCAMERAS()	-1
#endif // ST_PROFILING_RENDERDATA
