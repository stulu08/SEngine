#include "Settings.h"

#include "Stulu/App.h"
#include "Stulu/Controls.h"
#include "Stulu/MainLayer.h"
#include <Stulu/Physics/PhysicsScene.h>

using namespace Stulu;

namespace Editor {
	SettingsPanel::SettingsPanel()
		: Panel("Settings") {}

	void SettingsPanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Settings");
		m_scene = App::get().GetLayer().GetActiveScene().get();

		if (ImGui::CollapsingHeader("Physics")) {
			DrawPhysics();
		}
		if (ImGui::CollapsingHeader("Graphics")) {
			DrawGraphics();
		}
	}
	void SettingsPanel::DrawGraphics() {

		auto& sceneSettings = m_scene->getData();
		if (ImGui::TreeNode("Scene")) {
			auto& globalGraphics = sceneSettings.graphicsData;
			DrawShadowSettings(globalGraphics.shadows);
			DrawFogSettings(globalGraphics.fog);

			ImGui::TreePop();
		}
	}
	void SettingsPanel::DrawShadowSettings(ShadowSettings& settings) {
		if (ImGui::TreeNodeEx("Shadows", ImGuiTreeNodeFlags_Framed)) {
			bool shadowUpdate = false;
			shadowUpdate |= Controls::ResolutionCombo("Map Size", settings.MapSize);

			if (ImGui::TreeNodeEx("Distance", ImGuiTreeNodeFlags_DefaultOpen)) {
				Controls::Default("Max Distance", settings.FarPlane);
				Controls::Float("Blending Distance", settings.BlendingDistance, 0.0f, 50.0f, 0.25f);
				Controls::Float("View Distance Multiplier", settings.ZMult, 0.0f, 50.0f, 0.25f);
				Controls::Float("Near Plane", settings.NearPlane, 0.00001f, 50.0f, 0.01f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Quality", ImGuiTreeNodeFlags_DefaultOpen)) {
				Controls::Combo("Sample Quality", settings.SampleQuality, "8@16@32@64");
				Controls::Combo("Soft Quality", settings.PCSSQuality, "8@16@32@64");
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Cascades")) {
				size_t cascadeCount = settings.CascadeSplits.size();
				if (Controls::Slider::GenericInt("Cascade Count", cascadeCount, 1, ST_MAX_SHADOW_CASCADES)) {
					settings.CascadeSplits.resize(cascadeCount);
					shadowUpdate = true;
				}

				ImGui::Indent();
				for (size_t i = 0; i < cascadeCount; i++) {
					// last split should be shadow distance
					if (i == cascadeCount - 1) {
						settings.CascadeSplits[i] = settings.FarPlane;
						break;
					}

					float prev = i <= 0 ? settings.NearPlane : settings.CascadeSplits[i - 1];
					float next = settings.CascadeSplits[i + 1];
					settings.CascadeSplits[i] = glm::clamp(settings.CascadeSplits[i], prev, next);

					Controls::Slider::Float("Split " + std::to_string(i + 1), settings.CascadeSplits[i], prev, next, "%.1f");
				}
				ImGui::Unindent();
				std::sort(settings.CascadeSplits.begin(), settings.CascadeSplits.end());;

				// draw display
				ImGui::Text("Cascades");
				ImGui::SameLine();
				if (Controls::AlignedButton(ICON_FK_REFRESH, 1.0f)) {
					settings.CascadeSplits = Math::CalculateCascadeSplits(cascadeCount, settings.FarPlane);
				}
				{
					static ImU32 colors[] = {
						IM_COL32(200, 80, 80, 255),
						IM_COL32(80, 200, 80, 255),
						IM_COL32(80, 80, 200, 255),
						IM_COL32(200, 200, 80, 255),
						IM_COL32(80, 200, 200, 255),
						IM_COL32(200, 80, 200, 255)
					};

					ImDrawList* drawList = ImGui::GetWindowDrawList();

					const ImVec2 start = ImGui::GetCursorScreenPos();
					const float barHeight = ImGui::GetTextLineHeight() + ImGui::GetFrameHeight();
					const float barWidth = ImGui::GetContentRegionAvail().x;
					const float meterPerPixel = settings.FarPlane / barWidth;

					for (size_t i = 0; i < cascadeCount; i++) {
						const float prev = (i <= 0 ? 0.0f : settings.CascadeSplits[i - 1]) / meterPerPixel;
						const float current = settings.CascadeSplits[i] / meterPerPixel;
						drawList->AddRectFilled(ImVec2(start.x + prev, start.y), ImVec2(start.x + current, start.y + barHeight), colors[i]);

					}

					ImGui::Dummy(ImVec2(barWidth, barHeight));
				}
				ImGui::TreePop();
			}





			if (shadowUpdate) {
				m_scene->getRenderer()->resizeShadowMap();
			}
			ImGui::TreePop();
		}
	}

	void SettingsPanel::DrawFogSettings(FogSettings& fog) {
		if (ImGui::TreeNodeEx("Fog", ImGuiTreeNodeFlags_Framed)) {
			FogSettings::Mode fogMode = (FogSettings::Mode)fog.fogMode;
			if (Controls::Combo("Mode", fogMode)) {
				fog.fogMode = (float)(fogMode);
			}
			if (fogMode != FogSettings::Mode::Disabled) {
				Controls::Color("Color", fog.fogColor);

				if (fogMode == FogSettings::Mode::Linear) {
					Controls::Default("Linear Start", fog.linearFogStart);
					Controls::Default("Linear End", fog.linearFogEnd);
				}
				else {
					Controls::Slider::Float("Density", fog.fogDensity, 0.0f, 0.04f, "%.5f");
				}
				bool useHorizontalFog = fog.EnableFogHorizon != 0.0f;
				if (Controls::Bool("Horizont Fog", useHorizontalFog)) {
					fog.EnableFogHorizon = useHorizontalFog ? 1.0f : 0.0f;
				}
				if (useHorizontalFog) {
					Controls::Slider::Float("Horizon Strength", fog.FogHorizonStrength, 0.0f, 1.0f);
					Controls::Float("Horizon Falloff", fog.FogHorizonFalloff, 0.0f, 1000.0f);
				}

				bool useGroundFog = fog.EnableGroundFog != 0.0f;
				if (Controls::Bool("Ground Fog", useGroundFog)) {
					fog.EnableGroundFog = useGroundFog ? 1.0f : 0.0f;
				}
				if (useGroundFog) {
					Controls::Slider::Float("Ground Fog Strength", fog.FogGroundStrength, 0.0f, 1.0f);
				}

				bool useHorizonHeightFog = fog.EnableHorizonHeightFog != 0.0f;
				if (Controls::Bool("Height Fog", useHorizonHeightFog)) {
					fog.EnableHorizonHeightFog = useHorizonHeightFog ? 1.0f : 0.0f;
				}
				if (useHorizonHeightFog) {
					Controls::Default("Horizon Offset", fog.FogHorizonOffset);
					Controls::Default("Horizon Height Falloff", fog.FogHorizonHeightFalloff);
				}

			}
			ImGui::TreePop();
		}
	}

	void SettingsPanel::DrawPhysics() {
		auto& sceneSettings = m_scene->getData();

		Controls::Bool("Enabled", sceneSettings.enablePhsyics3D);

		if (!sceneSettings.enablePhsyics3D || !m_scene->getCaller() || !m_scene->getCaller()->HasLayer<PhysicsScene>())
			return;
		
		PhysicsScene& physicsLayer = m_scene->getCaller()->GetLayer<PhysicsScene>();

		glm::vec3 gravity = physicsLayer.GetGravity();
		if (Controls::Vector3("Scene Gravity", gravity)) {
			physicsLayer.SetGravity(gravity);
		}
		
	}
}

