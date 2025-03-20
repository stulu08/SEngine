#include "Settings.h"
#include "Stulu/App.h"
#include "Stulu/Controls.h"

using namespace Stulu;

namespace Editor {
	SettingsPanel::SettingsPanel()
		: Panel("Settings") {}

	void SettingsPanel::DrawImGui() {
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
		auto& sceneSettings = App::get().GetLayer().GetActiveScene()->getData();

		Controls::Bool("Enabled", sceneSettings.enablePhsyics3D);

		if (!sceneSettings.enablePhsyics3D)
			ImGui::BeginDisabled();



		if (!sceneSettings.enablePhsyics3D)
			ImGui::EndDisabled();
	}
}

