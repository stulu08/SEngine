#pragma once
#include "Stulu/Panel.h"

#include <Stulu/Types/Stack.h>
#include <Stulu/Debug/Profiler.h>
#include <Stulu/Debug/RenderDataProfiler.h>
#include <map>

namespace Editor {
	struct TimingEntry {
		TimingEntry(const std::string& Name, double Time)
			: name(Name), time(Time) {}

		std::string name;
		double time;
	};
}

namespace std {
	template <>
	struct tuple_size<::Editor::TimingEntry> : std::integral_constant<size_t, 2> {};

	template <size_t Index>
	struct tuple_element<Index, ::Editor::TimingEntry> {
		using type = std::conditional_t<Index == 0, std::string, double>;
	};

	template <size_t Index>
	auto& get(::Editor::TimingEntry& entry) {
		if constexpr (Index == 0) return entry.name;
		else return entry.time;
	}

	template <size_t Index>
	auto& get(const ::Editor::TimingEntry& entry) {
		if constexpr (Index == 0) return entry.name;
		else return entry.time;
	}
}

namespace Editor{ 
	using TimingGroup = std::vector<TimingEntry>;
	using TimingGroupList = std::map<std::string, TimingGroup>;

	class ProfilingPanel : public Panel {
	public:
		ProfilingPanel();
		virtual ~ProfilingPanel() = default;

		virtual void DrawImGui() override;
		virtual void DrawImGuizmo() override {}


	private:
		TimingGroupList m_timings;
		float m_lastFrameTime = 0.0f;
		float m_updateTimer = 0.0f;

		std::vector<float> m_fps;
		std::vector<float> m_virtual;
		std::vector<float> m_physical;

		void DrawTimings();
		void DrawShaderStats();
		void DrawPipelineStats();
		void DrawSceneStats();
		void DrawPhysics();
		void DrawResources();

		inline bool RequiresUpdate() const { return m_updateTimer >= 1.0f; }
	};
}