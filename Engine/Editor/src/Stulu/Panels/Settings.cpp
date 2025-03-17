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
		auto& settings = sceneSettings.physicsData;

		Controls::Bool("Enabled", sceneSettings.enablePhsyics3D);

		if (!sceneSettings.enablePhsyics3D)
			ImGui::BeginDisabled();

		Controls::Vector3("Global Gravity", settings.gravity, 0.5f);
		Controls::Float("Speed", settings.speed);


		Controls::SetControlHelpMark(R"(For simulating roughly human-sized in metric units, 1 is a good choice.
If simulation is done in centimetres, use 100 instead.This is used to
estimate certain length - related tolerances.)");
		Controls::Float("Length Unit", settings.length, 0.0f, 1E12F);


		Controls::SetControlHelpMark(R"(How man CPU Threads should be used for simulating, this limited to the number of threads 
of the executing machine.
It is suggested to set this value to something low, 
since threads may run out of sync and physics will be miscalculated (phasing, ...)!)");
		Controls::UInt("Worker Threads", settings.workerThreads, 0, std::thread::hardware_concurrency());
		
		Controls::PopControlHelpMark();

		if (!sceneSettings.enablePhsyics3D)
			ImGui::EndDisabled();
	}
}

