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

		if (ImGui::CollapsingHeader("Physics")) {
			DrawPhysics();
		}
		if (ImGui::CollapsingHeader("Graphics")) {
			DrawGraphics();
		}
	}
	void SettingsPanel::DrawGraphics() {
		auto& sceneSettings = App::get().GetLayer().GetActiveScene()->getData();
		if (ImGui::TreeNode("Scene")) {
			auto& globalGraphics = sceneSettings.graphicsData;
			Controls::Float("Environment LOD", globalGraphics.env_lod, 0.0f, 6.0f);
			
			ImGui::Text("Shadows");
			ImGui::PushID("##ShadowSettings");
			Controls::Default("Distance", globalGraphics.shadowDistance);
			Controls::Default("Rach(Far)", globalGraphics.shadowFar);
			Controls::Default("Map Size", globalGraphics.shadowMapSize);
			ImGui::PopID();

			ImGui::Text("Fog");
			ImGui::PushID("##FogSettings");
			auto& fog = globalGraphics.fog;
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
			ImGui::PopID();

			ImGui::TreePop();
		}
	}
	void SettingsPanel::DrawPhysics() {
		auto& scene = App::get().GetLayer().GetActiveScene();
		auto& sceneSettings = scene->getData();

		Controls::Bool("Enabled", sceneSettings.enablePhsyics3D);

		if (!sceneSettings.enablePhsyics3D || !scene->getCaller()->HasLayer<PhysicsScene>())
			return;
		
		PhysicsScene& physicsLayer = scene->getCaller()->GetLayer<PhysicsScene>();

		glm::vec3 gravity = physicsLayer.GetGravity();
		if (Controls::Vector3("Scene Gravity", gravity)) {
			physicsLayer.SetGravity(gravity);
		}
		
	}
}

