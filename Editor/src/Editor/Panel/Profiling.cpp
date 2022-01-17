#include "imgui/imgui.h"
#include "Profiling.h"
namespace Stulu {
	void ProfilingPanel::draw(Timestep& timestep, bool* open) {
		ST_PROFILING_FUNCTION();
		if (ImGui::Begin("Profiling", open)) {

			bool update = false;
			if (m_updateTimer >= .25f) {
				m_updateTimer = 0.0f;
				update = true;
			}
			m_updateTimer += timestep;

			if(ImGui::TreeNodeEx("Frametime", ImGuiTreeNodeFlags_DefaultOpen)){
				if (update) {
					ST_PROFILING_SCOPE("Editor Profiling FPS update");
					m_fps.push_back(1.0f / timestep);
					if (m_fps.size() > 99)
						m_fps.erase(m_fps.begin(), m_fps.begin() + 1);
					//m_fAvg = Math::avg(m_fps);
					m_fAvg = m_fps[m_fps.size()-1];

					m_delta.push_back(timestep.getMilliseconds());
					if (m_delta.size() > 99)
						m_delta.erase(m_delta.begin(), m_delta.begin() + 1);
					//m_dAvg = Math::avg(m_delta);
					m_dAvg = m_delta[m_delta.size() - 1];
				}
				ImGui::PlotLines("FPS", m_fps.data(), (int)m_fps.size(), 0, (std::to_string(m_fAvg)).c_str(), 0.0f, m_fAvg + m_fAvg / 2.0f);
				ImGui::PlotLines("Deltatime", m_delta.data(), (int)m_delta.size(), 0, (std::to_string(m_dAvg) + "ms").c_str(), 0.0f, m_dAvg + m_dAvg / 2.0f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Memory Usage", ImGuiTreeNodeFlags_DefaultOpen)) {
				MemoryUsageInfo info = Platform::getMemoryUsage();

				if (update) {
					ST_PROFILING_SCOPE("Editor Memory FPS update");
					float virt = static_cast<float>(info.virtualUsedByProcess) / 1048576;
					m_virtualM.push_back(virt);
					if (m_virtualM.size() > 99)
						m_virtualM.erase(m_virtualM.begin(), m_virtualM.begin() + 1);
					//m_vAvg = Math::avg(m_virtualM);
					m_vAvg = m_virtualM[m_virtualM.size() - 1];


					float phys = static_cast<float>(info.physicalUsedByProcess) / 1048576;
					m_physical.push_back(phys);
					if (m_physical.size() > 99)
						m_physical.erase(m_physical.begin(), m_physical.begin() + 1);
					//m_pAvg = Math::avg(m_physical);
					m_pAvg = m_physical[m_physical.size() - 1];

				}
				ImGui::PlotLines("Virtual ", m_virtualM.data(), (int)m_virtualM.size(), 0, (std::to_string(m_vAvg) + "mb").c_str(), m_vAvg - m_vAvg / 2.0f, m_vAvg + m_vAvg / 2.0f);
				ImGui::PlotLines("Physical", m_physical.data(), (int)m_physical.size(), 0, (std::to_string(m_pAvg) + "mb").c_str(), m_pAvg - m_pAvg / 2.0f, m_pAvg + m_pAvg / 2.0f);
				ImGui::TreePop();
			}
#if ST_PROFILING_RENDERDATA
			if (ImGui::TreeNodeEx("Runtime Render Data", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Runtime time: %.1f", Time::time.getSeconds());
				if (ImGui::TreeNodeEx("DrawingforCams", ImGuiTreeNodeFlags_DefaultOpen, "Drawing for %d Camera(s):", ST_PROFILING_RENDERDATA_GETCAMERAS())) {
					ImGui::Text("Drawcalls: %d", ST_PROFILING_RENDERDATA_GETDRAWCALLS());
					ImGui::Text("Vertices: %d", ST_PROFILING_RENDERDATA_GETVERTICES());
					ImGui::Text("Indices: %d", ST_PROFILING_RENDERDATA_GETINDICES());
					ImGui::Text("Triangles: %d", (int)(ST_PROFILING_RENDERDATA_GETINDICES() / 3));
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
#endif
		}
		ImGui::End();
	}
}