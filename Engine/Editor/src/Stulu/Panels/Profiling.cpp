#include "Profiling.h"

#include "Stulu/App.h"
#include "Stulu/MainLayer.h"
#include "Stulu/Controls.h"

#include <Stulu/Core/Time.h>
#include <Stulu/Core/Platform.h>
#include <Stulu/Renderer/Renderer.h>
#include <Stulu/Scene/SceneRenderer.h>


#include <magic_enum/magic_enum.hpp>

using namespace Stulu;

namespace Editor {
	ProfilingPanel::ProfilingPanel()
		: Panel("Statistics") {
		m_fps.reserve(50);
		m_virtual.reserve(50);
		m_physical.reserve(50);
	}

	void ProfilingPanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Statistics");

		m_updateTimer += Time::frameTime;
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 1.0f);

		if (ImGui::TreeNodeEx("Timings", ImGuiTreeNodeFlags_Framed)) {
			DrawTimings();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_Framed)) {
			DrawSceneStats();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Physics", ImGuiTreeNodeFlags_Framed)) {
			DrawPhysics();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Pipeline Stats", ImGuiTreeNodeFlags_Framed)) {
			DrawPipelineStats();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Shaders", ImGuiTreeNodeFlags_Framed)) {
			DrawShaderStats();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Resources", ImGuiTreeNodeFlags_Framed)) {
			DrawResources();
			ImGui::TreePop();
		}

		if (RequiresUpdate()) {
			m_updateTimer = 0.0f;
		}

		ImGui::PopStyleVar(1);
	}

	void ProfilingPanel::DrawTimings() {
		if (RequiresUpdate() || m_timings.empty()) {
			m_timings.clear();
			m_timings.insert({ "All", {} });

			for (const auto& [name, time] : Profiler::GetInstance()) {
				const std::string nameStr = name;
				const size_t searchPos = nameStr.find_first_of("-");

				if (searchPos == nameStr.npos) {
					m_timings["Other"].push_back({ name, time });
				}
				else {
					const std::string group = nameStr.substr(0, searchPos);
					const std::string clearName = nameStr.substr(searchPos + 1);
					m_timings[group].push_back({ clearName, time });
				}

				m_timings["All"].push_back({ name, time });
			}
			for (auto& [name, timings] : m_timings) {
				std::sort(timings.begin(), timings.end(), [](const TimingEntry& left, const TimingEntry& right) { return left.time > right.time; });
			}

			// seconds to ms
			m_lastFrameTime = Time::frameTime.getMilliseconds();
			Profiler::GetInstance().Refresh();

			m_fps.push_back(1.0f / Time::frameTime);
			if (m_fps.size() > m_fps.capacity() - 1)
				m_fps.erase(m_fps.begin(), m_fps.begin() + 1);
		}

		{
			std::stringstream fpsDisplay;
			fpsDisplay << std::fixed << std::setprecision(1) << m_fps[m_fps.size() - 1];
			ImGui::PlotLines("Frames/s", m_fps.data(), (int)m_fps.size(), 0, fpsDisplay.str().c_str());
		}


		ImGui::Separator();

		for (auto& [name, timings] : m_timings) {
			if (ImGui::TreeNodeEx(name.c_str())) {
				for (const auto& [name, time] : timings) {
					std::stringstream display;
					display << name << ": " << std::fixed << std::setprecision(3) << time << "ms";
					ImGui::Indent();
					ImGui::Indent();
					ImGui::ProgressBar((float)time / m_lastFrameTime, ImVec2(0.0f, 0.0f), display.str().c_str());
					ImGui::Unindent();
					ImGui::Unindent();
				}
				ImGui::TreePop();
			}
		}
	}

	void ProfilingPanel::DrawShaderStats() {
		const auto& shaders = Renderer::getShaderSystem()->GetShaders();

		ImGui::Spacing();
		ImGui::Text("Loaded %d Shaders", shaders.size());
		ImGui::SameLine();

		const char* refreshText = ICON_FK_REFRESH " Reload Shaders";
		float buttonWidth = ImGui::CalcTextSize(refreshText).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth);
		if (ImGui::Button(refreshText)) {
			Renderer::getShaderSystem()->ReloadShaders();
		}
		ImGui::Spacing();

		ImGui::Separator();

		for (const auto& [name, entry] : shaders) {
			const auto& properies = entry->GetProperities();

			ImGuiTreeNodeFlags flags = 0;
			if (properies.size() <= 0)
				flags |= ImGuiTreeNodeFlags_Leaf;

			if (ImGui::TreeNodeEx(name.c_str(), flags)) {
				for (auto& prop : properies) {
					ImGui::TreeNodeEx(prop->getName().c_str(), ImGuiTreeNodeFlags_Leaf, "%s: %s", prop->getName().c_str(), magic_enum::enum_name(prop->getType()).data());
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
	}

	void ProfilingPanel::DrawPipelineStats() {

		if (ImGui::TreeNodeEx("Render Stats")) {

			ImGui::TreeNodeEx("DrawCalls", ImGuiTreeNodeFlags_Leaf, "Drawcalls: % d", ST_PROFILING_RENDERDATA_GETDRAWCALLS());
			ImGui::TreePop();

			ImGui::TreeNodeEx("Vertices", ImGuiTreeNodeFlags_Leaf, "Vertices: %d", ST_PROFILING_RENDERDATA_GETVERTICES());
			ImGui::TreePop();

			ImGui::TreeNodeEx("Indices", ImGuiTreeNodeFlags_Leaf, "Indices: %d", ST_PROFILING_RENDERDATA_GETINDICES());
			ImGui::TreePop();

			ImGui::TreeNodeEx("Triangles", ImGuiTreeNodeFlags_Leaf, "Triangles: %d", (int)(ST_PROFILING_RENDERDATA_GETINDICES() / 3));
			ImGui::TreePop();

			ImGui::TreePop();
		}
	}

	void ProfilingPanel::DrawSceneStats() {
		Ref<Scene> scene = App::get().GetLayer().GetActiveScene();
		if (!scene) {
			return;
		}

		const auto& renderer = scene->getRenderer();
		auto& sceneData = scene->getData();
		
		if (ImGui::TreeNodeEx("Shadows")) {
			ImGui::Image(renderer->GetShadowMap()->getDepthAttachment(), glm::vec2(256.0f), { 0,1 }, { 1, 0 });
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Display")) {
			Controls::BitFlag("Wireframe", ST_ShaderViewFlags_DisplayVertices, sceneData.shaderFlags);
			Controls::BitFlag("Depth", ST_ShaderViewFlags_DisplayDepth, sceneData.shaderFlags);
			Controls::BitFlag("Lighting", ST_ShaderViewFlags_DisplayLighting, sceneData.shaderFlags);
			Controls::BitFlag("Normals", ST_ShaderViewFlags_DisplayNormal, sceneData.shaderFlags);
			Controls::BitFlag("Tex Coords", ST_ShaderViewFlags_DisplayTexCoords, sceneData.shaderFlags);
			Controls::BitFlag("Diffuse", ST_ShaderViewFlags_DisplayDiffuse, sceneData.shaderFlags);
			Controls::BitFlag("Specular", ST_ShaderViewFlags_DisplaySpecular, sceneData.shaderFlags);
			Controls::BitFlag("Ambient", ST_ShaderViewFlags_DisplayAmbient, sceneData.shaderFlags);
			Controls::BitFlag("Roughness", ST_ShaderViewFlags_DisplayRoughness, sceneData.shaderFlags);
			Controls::BitFlag("Metallic", ST_ShaderViewFlags_DisplayMetallic, sceneData.shaderFlags);
			Controls::BitFlag("Ambient Occlusion", ST_ShaderViewFlags_DisplayOcclusion, sceneData.shaderFlags);
			Controls::BitFlag("Emission", ST_ShaderViewFlags_DisplayEmission, sceneData.shaderFlags);
			ImGui::TreePop();
		}
	}

	void ProfilingPanel::DrawPhysics() {
		Ref<Scene> scene = App::get().GetLayer().GetActiveScene();
		if (scene->PhysicsEnable()) {
			const auto& physics = scene->getPhysics();
			bool value = false;

			for (auto enumValue : magic_enum::enum_values<PhsicsDebugViuals>()) {
				bool value = physics->GetDebugVisual(enumValue) > 0.0f;
				std::string enumName = std::string(magic_enum::enum_name(enumValue));
				if (Controls::Bool(enumName, value)) {
					physics->SetDebugVisual(enumValue, value ? 1.0f : 0.0f);
				}
			}
		}
		else {
			ImGui::Text("Only availabe during runtime with physics enabled");
		}

		
	}

	void ProfilingPanel::DrawResources() {
		if (RequiresUpdate()) {
			MemoryUsageInfo info = Platform::getMemoryUsage();

			float virt = static_cast<float>(info.virtualUsedByProcess) / 1048576;

			m_virtual.push_back(virt);
			if (m_virtual.size() > m_virtual.capacity() - 1)
				m_virtual.erase(m_virtual.begin(), m_virtual.begin() + 1);

			float phys = static_cast<float>(info.physicalUsedByProcess) / 1048576;
			m_physical.push_back(phys);
			if (m_physical.size() > m_physical.capacity() - 1)
				m_physical.erase(m_physical.begin(), m_physical.begin() + 1);

		}
		std::stringstream virtDisplay;
		virtDisplay << std::fixed << std::setprecision(1) << m_virtual[m_virtual.size() - 1] << "MB";
		ImGui::PlotLines("Virtual Memory", m_virtual.data(), (int)m_virtual.size(), 0, virtDisplay.str().c_str());

		std::stringstream physDisplay;
		physDisplay << std::fixed << std::setprecision(1) << m_physical[m_physical.size() - 1] << "MB";
		ImGui::PlotLines("Physical Memory", m_physical.data(), (int)m_physical.size(), 0, physDisplay.str().c_str());

		
	}
}