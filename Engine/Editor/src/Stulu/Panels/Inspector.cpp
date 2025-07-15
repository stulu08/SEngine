#include "Inspector.h"

#include "Stulu/Panels/Hierarchy.h"
#include "Stulu/App.h"
#include "Stulu/Style.h"
#include "Stulu/Controls.h"
#include "Stulu/MainLayer.h"
using namespace Stulu;

namespace Editor {
	bool TreeNodeInspector(const std::string& name, bool* outBtn, const std::string& btnText) {
		const float buttonWidth = ImGui::GetFontSize();
		const float fullWidth = ImGui::GetContentRegionAvail().x;

		bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap);

		if (outBtn) {
			ImGui::SameLine(fullWidth - ImGui::CalcTextSize(btnText.c_str()).x - ImGui::GetStyle().FramePadding.x * 2.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			*outBtn = ImGui::Button(btnText.c_str());
			ImGui::PopStyleColor(3);
		}

		if (open) {
			::ImGui::Indent(ImGui::GetStyle().FramePadding.x * 3.0f);
			return true;
		}
		return false;
	}

	void TreePopInspector() {
		ImGui::Unindent(ImGui::GetStyle().FramePadding.x * 3.0f);
		ImGui::TreePop();
	}

	Stulu::Mono::Method InspectorPanel::s_renderMethod = nullptr;
	Stulu::Mono::Method InspectorPanel::s_renderGizmosMethod = nullptr;

	InspectorPanel::InspectorPanel()
		: Panel("Inspector") {
		LoadObjects();
	}

	void InspectorPanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Inspector");

		auto& layer = App::get().GetLayer();
		const auto& selectedObjects = layer.GetPanel<Editor::HierarchyPanel>().GetSelected();

		if (selectedObjects.size() == 1) {
			GameObject selected = GameObject(selectedObjects[0], layer.GetActiveScene().get());

			if (!selected.IsValid())
				return;
			
			auto& base = selected.getComponent<GameObjectBaseComponent>();

			ImGui::SameLine(m_windowIndent);

			if (m_nameEditing != selected.GetID()) {
				m_nameEditing = entt::null;
				if (ImGui::Button(ICON_FK_PENCIL)) {
					m_nameEditing = selected.GetID();
				}
				ImGui::SameLine();
				ImGui::Text(base.name.c_str());
			}
			else {
				if (ImGui::Button(ICON_FK_CHECK)) {
					m_nameEditing = entt::null;
				}
				ImGui::SameLine();
				ImGui::InputText("##name", &base.name);
			}

			uint64_t id = (uint64_t)selected.GetID();
			for (const auto& inspector : m_inspectors) {
				if (!inspector->HasComponent(id)) {
					continue;
				}
				const std::string& header = inspector->GetHeader();
				bool isHeaderID = header.rfind("##", 0) == 0;

				if (!isHeaderID) {
					bool removeComp = inspector->GetTypeName() != Component::GetManagedComponentName<TransformComponent>();
					if (TreeNodeInspector(header.c_str(), removeComp ? &removeComp : NULL, ICON_FK_TRASH)) {
						inspector->Invoke(id);
						TreePopInspector();
					}
					if (removeComp) {
						inspector->RemoveComponent(id);
					}
				}
				else {
					ImGui::PushID(header.c_str());
					inspector->Invoke(id);
					ImGui::PopID();
				}
			}
		}
	}

	void InspectorPanel::DrawImGuizmo() {
		auto& layer = App::get().GetLayer();
		const auto& selectedObjects = layer.GetPanel<Editor::HierarchyPanel>().GetSelected();
		for (entt::entity selected : selectedObjects) {
			uint64_t id = (uint64_t)selected;
			for (const auto& inspector : m_inspectors) {
				if (!inspector->HasComponent(id)) {
					continue;
				}
				inspector->InvokeGizmo(id);
			}
		}
	}

	void InspectorPanel::PreWindow() {
		m_windowIndent = ImGui::GetStyle().WindowPadding.x;
		ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, 0.0f);
	}

	void InspectorPanel::PostWindow() {
		ImGui::PopStyleVar();
	}

	bool InspectorPanel::HasInspector(const std::string& typeName) const {
		for (const auto& inspector : m_inspectors) {
			if (inspector->GetTypeName() == typeName)
				return true;
		}
		return false;
	}

	void InspectorPanel::LoadScriptObjects(Ref<ScriptAssembly> assembly) {

		auto& manager = App::get().getAssemblyManager();

		Mono::Class inspectorClass = manager->getScriptCoreAssembly()->CreateClass("Editor", "Inspector");
		Mono::Class inspectorAttribute = manager->getScriptCoreAssembly()->CreateClass("Editor", "InspectorRendererAttribute");

		auto& classes = assembly->LoadAllClasses(inspectorClass);

		s_renderMethod = inspectorClass.GetMethodFromName("Impl_Render", 1);
		s_renderGizmosMethod = inspectorClass.GetMethodFromName("Impl_RenderGizmos", 1);

		Mono::ClassField typeField = inspectorAttribute.GetFieldFromName("type");
		Mono::ClassField nameField = inspectorAttribute.GetFieldFromName("name");
		Mono::ClassField priotityField = inspectorAttribute.GetFieldFromName("priority");

		CORE_ASSERT(typeField, "Field 'type' not found!");
		CORE_ASSERT(nameField, "Field 'name' not found!");
		CORE_ASSERT(priotityField, "Field 'priority' not found!");

		for (Mono::Class& inspector : classes) {
			Mono::CustomAttrInfo atrrInfo = Mono::CustomAttrInfo::FromClass(inspector);
			if (atrrInfo.HasAttribute(inspectorAttribute)) {
				Mono::Object atrributeObject = atrrInfo.GetAttribute(inspectorAttribute);
				if (!atrributeObject)
					continue;

				ManagedInspectorRendererProps props;
				typeField.GetValue(atrributeObject, &props.refType);
				nameField.GetValue(atrributeObject, &props.header);
				priotityField.GetValue(atrributeObject, &props.prio);
				props.inspectorClass = inspector;
				props.assembly = assembly;

				m_inspectors.push_back(createRef<ManagedInspectorRenderer>(props));
			}

			atrrInfo.Free();
		}
	}

	class SpriteRendererInspector : public NativeInspectorRenderer<SpriteRendererComponent> {
	public:
		virtual void DrawGUI(SpriteRendererComponent& comp) const override {
			Controls::Color("Color", comp.color);
			Controls::Default("Tiling", comp.tiling);
			Controls::Default("Texture", comp.texture);
		}
		virtual std::string GetHeader() const override {
			return ICON_FK_PICTURE_O " Sprite Renderer";
		}
	};
	class CircleRendererInspector : public NativeInspectorRenderer<CircleRendererComponent> {
	public:
		virtual void DrawGUI(CircleRendererComponent& comp) const override {
			Controls::Color("Color", comp.color);
			Controls::Slider::Float("Thickness", comp.thickness);
			Controls::Slider::Float("Fade", comp.fade, 0.005f, 5.0f);
		}
		virtual std::string GetHeader() const override {
			return ICON_FK_CIRCLE " Circle Renderer";
		}
	};
	class PostProcessingInspector : public NativeInspectorRenderer<PostProcessingComponent> {
	public:
		virtual void DrawGUI(PostProcessingComponent& comp) const override {
			if (comp.HasEffect<GammaCorrectionEffect>()) {
				GammaCorrectionEffect& gammaEffect = *comp.GetEffect<GammaCorrectionEffect>();
				bool enabled = gammaEffect.IsEnabled();

				if (ImGui::TreeNodeEx("Correction", ImGuiTreeNodeFlags_SpanAvailWidth)) {
					if (Controls::Default("Enabled", enabled)) {
						gammaEffect.SetEnabled(enabled);
					}

					Controls::Default("Enable Gamma", gammaEffect.enableGammaCorrection);
					Controls::Slider::Float("Gamma", gammaEffect.gamma, 1.0f, 5.0f);
					Controls::Combo("Tone Mapping", gammaEffect.toneMappingMode);
					Controls::Slider::Float("Exposure", gammaEffect.exposure, 1.0f, 10.0f);
					if (gammaEffect.toneMappingMode == GammaCorrectionEffect::ReinhardExtended)
						Controls::Slider::Float("Max White", gammaEffect.maxWhite, 0.0f, 10.0f);

					if (ImGui::Button("Remove")) {
						comp.RemoveEffect<GammaCorrectionEffect>();
					}

					ImGui::TreePop();
				}
			}
			if (comp.HasEffect<BloomEffect>()) {
				BloomEffect& bloom = *comp.GetEffect<BloomEffect>();
				bool enabled = bloom.IsEnabled();

				if (ImGui::TreeNodeEx("Bloom", ImGuiTreeNodeFlags_SpanAvailWidth)) {
					if (Controls::Default("Enabled", enabled)) {
						bloom.SetEnabled(enabled);
					}

					Controls::Float("Clamp", bloom.clamp, 0.0f, 1000.0f, 0.1f);
					Controls::Float("Threshold", bloom.threshold, 0.0f, 100.0f, 0.1f);
					Controls::Float("Intensity", bloom.intensity, 0.0f, 100.0f, 0.01f);
					Controls::Slider::Float("Resolution Scale", bloom.resolutionScale, 0.0f, 2.0f);
					Controls::Slider::Float("Knee", bloom.knee, 0.0f, 1.0f);
					Controls::Slider::Float("Diffusion", bloom.diffusion, 0.0f, (float)BLOOM_MAX_SAMPLES);

					if (ImGui::Button("Remove")) {
						comp.RemoveEffect<BloomEffect>();
					}

					ImGui::TreePop();
				}
			}
			if (ImGui::Button("Add Effect")) {
				ImGui::OpenPopup("ToogleAddPFX");
			}
			if (ImGui::BeginPopup("ToogleAddPFX")) {
				if (ImGui::MenuItem("Correction", "", false, comp.HasEffect<GammaCorrectionEffect>())) {
					comp.AddEffect<GammaCorrectionEffect>();
				}
				if (ImGui::MenuItem("Bloom", "", false, comp.HasEffect<BloomEffect>())) {
					comp.AddEffect<BloomEffect>();
				}
				ImGui::EndPopup();
			}
		}
		virtual std::string GetHeader() const override {
			return ICON_FK_MAGIC " Post Processing";
		}
	};

	void InspectorPanel::LoadObjects() {
		m_inspectors.clear();

		auto& manager = App::get().getAssemblyManager();
		LoadScriptObjects(manager->getScriptCoreAssembly());
		LoadScriptObjects(manager->getAppAssembly());

		m_inspectors.push_back(createRef<SpriteRendererInspector>());
		m_inspectors.push_back(createRef<CircleRendererInspector>());
		m_inspectors.push_back(createRef<PostProcessingInspector>());

		std::sort(m_inspectors.begin(), m_inspectors.end(), [](const Ref<InspectorRenderer>& left, const Ref<InspectorRenderer>& right) {
			return left->GetPriority() < right->GetPriority();
			});
	}
}
