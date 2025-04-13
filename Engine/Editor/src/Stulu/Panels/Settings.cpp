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
			Controls::Float("Distance", globalGraphics.shadowDistance);
			Controls::Float("Rach(Far)", globalGraphics.shadowFar);
			Controls::UInt("Map Size", globalGraphics.shadowMapSize);
			ImGui::TreePop();
		}
	}
	void SettingsPanel::DrawPhysics() {
		auto& scene = App::get().GetLayer().GetActiveScene();
		auto& sceneSettings = scene->getData();

		Controls::Bool("Enabled", sceneSettings.enablePhsyics3D);

		if (!sceneSettings.enablePhsyics3D)
			ImGui::BeginDisabled();

		PhysicsScene& physicsLayer = scene->getCaller()->GetLayer<PhysicsScene>();

		glm::vec3 gravity = physicsLayer.GetGravity();
		if (Controls::Vector3("Scene Gravity", gravity)) {
			physicsLayer.SetGravity(gravity);
		}
		
		
		if (!sceneSettings.enablePhsyics3D)
			ImGui::EndDisabled();
	}
}

