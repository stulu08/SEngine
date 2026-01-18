#include "AssetEditor.h"

#include "AssetBrowser.h"
#include "Stulu/MainLayer.h"
#include "Stulu/Controls.h"

using namespace Stulu;

namespace Editor {
	AssetEditor::AssetEditor() 
		: Panel("Asset Editor") {
		m_editors.push_back(createRef<ModelAssetEditor>());
		m_editors.push_back(createRef<Texture2DAssetEditor>());
		m_editors.push_back(createRef<SkyBoxAssetEditor>());
		m_editors.push_back(createRef<MeshAssetEditor>());
		m_editors.push_back(createRef<MaterialAssetEditor>());
	}

	void AssetEditor::PreWindow() {
		m_indent = ImGui::GetStyle().WindowPadding;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
	}

	void AssetEditor::PostWindow() {
		ImGui::PopStyleVar(1);
	}


	void AssetEditor::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Asset Editor");


		const float availWidth = ImGui::GetContentRegionAvail().x;
		const float xPosStart = ImGui::GetCursorPosX();
		const float YPosStart = ImGui::GetCursorPosY();
		ImGui::SetCursorPos(ImVec2(xPosStart, YPosStart));

		const float availHeigt = ImGui::GetContentRegionAvail().y;
		const float detailsHeight = ImGui::GetTextLineHeight() * 2.0f + ImGui::GetStyle().ItemSpacing.y * 3.0f;
		const float detailsPosY = ImGui::GetCursorPosY() + availHeigt - detailsHeight;


		SharedAssetData* asset = 
			m_asset != Stulu::UUID::null ? 
			AssetsManager::GlobalInstance().GetRaw(m_asset) : 
			nullptr;

		ImGui::SetCursorPos(ImVec2(xPosStart, YPosStart));
		if (ImGui::BeginChild("AssetEditorContent", ImVec2(availWidth, availHeigt - detailsHeight))) {

			if (asset && asset->Loaded()) {
				for (const auto& editor : m_editors) {
					if (editor && editor->GetTypeID() == asset->GetTypeID()) {
						editor->DrawImGui(asset);
					}
				}
			}
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(xPosStart, detailsPosY));
		if (ImGui::BeginChild("AssetEditorDetails", ImVec2(availWidth, detailsHeight))) {

			if (!asset) {
				Controls::TextDisabled("Asset not found - %" IM_PRIu64, m_asset);
			}
			else {
				auto path = std::filesystem::path(asset->GetPath());

				if (std::filesystem::exists(path))
					Controls::Text(std::filesystem::path(asset->GetPath()).stem().string());
				else
					Controls::Text("Loaded from memory");

				Controls::TextDisabled("%s - %" IM_PRIu64, asset->GetTypeName(), asset->GetUUID());
			}
		}
		ImGui::EndChild();
	}

	static void ModelAssetContent(const ModelNode& node) {
		ImGuiTreeNodeFlags treeFlags =
			ImGuiTreeNodeFlags_SpanFullWidth |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_OpenOnArrow;

		const ModelMeshNode& meshNode = node.mesh;

		const bool hasChilds = node.children.size() > 0;
		const bool hasMesh = meshNode.mesh.IsValid();
		const bool hasMaterials = meshNode.materials.size() > 0;
		const bool isLeaf = !hasChilds && !hasMesh && !hasMaterials;

		if (isLeaf)
			treeFlags |= ImGuiTreeNodeFlags_Leaf;

		ImGuiID treeID = ImGui::GetID(node.name.c_str());
		std::string display;
		if (ImGui::TreeNodeGetOpen(treeID)) {
			display = ICON_FK_FOLDER_OPEN + std::string(" ");
		}
		else if (!isLeaf) {
			display = ICON_FK_FOLDER + std::string(" ");
		}
		display += node.name;


		if (ImGui::TreeNodeEx(node.name.c_str(), treeFlags, display.c_str())) {

			if (hasMesh && ImGui::TreeNodeEx("Mesh")) {
				Stulu::UUID mesh = meshNode.mesh.GetUUID();
				Controls::Mesh("Mesh 0", mesh, true);
				ImGui::TreePop();
			}
			if (hasMaterials && ImGui::TreeNodeEx((node.name + "_MATS").c_str(), 0, "Materials (%d)", (int)meshNode.materials.size())) {
				for (size_t i = 0; i < meshNode.materials.size(); i++) {
					Stulu::UUID matUUID = meshNode.materials[i].GetUUID();
					Controls::Material("Material " + std::to_string(i), matUUID, true);
				}
				ImGui::TreePop();
			}
			if (hasChilds) {
				for (const auto& child : node.children) {
					ModelAssetContent(child);
				}
			}

			ImGui::TreePop();
		}
	};

	void ModelAssetEditor::DrawGui(Stulu::SharedModelAssetData* asset) {
		static std::pair<Stulu::UUID, ModelImportSettings> lastSettings;

		if (asset->GetUUID() != lastSettings.first) {
			lastSettings = { asset->GetUUID(), asset->ReadSettings() };
		}
		auto& settings = lastSettings.second;

		Controls::Text("Import Settings");

		Controls::SetControlHelpMark("This step may combine meshes, split large meshes \nand the overall quality of the mesh is improved.");
		Controls::Default("Optimize Meshes", settings.OptimizeMeshes);

		Controls::SetControlHelpMark("This option should be used with caution, it collapses \nunused nodes, resulting in often a single mesh with multiple sub meshes.\nMaterials are also affected by this step and may be remove if not used or duplicated!");
		Controls::Default("Remove Redundancies", settings.RemoveRedundancies);

		Controls::SetControlHelpMark("This option simply makes the model into a more compatible format.\nVertices are populated, data is clamped or missing data will be inserted.");
		Controls::Default("Handle Invalid Data", settings.HandleInvalidData);

		Controls::SetControlHelpMark("This adjust the base scale of the vertex positions, other Software \noften exports models in diffrent units. SEngine uses Meters [m], \nif the model is loaded using Centimeters [cm], use 0.01 here!");
		Controls::Default("Scale Mod", settings.ScaleMod);

		Controls::SetControlHelpMark("Scale the vertices by the nodes scale and set the scale of every Transform to 1.\nThe apllied scale will be: ScaleMod * NodeScale");
		Controls::Default("Scale Vertices", settings.ScaleVertices);
		
		Controls::PopControlHelpMark();

		Controls::Default("Load Meshes", settings.LoadMeshes);
		Controls::Default("Load Materials", settings.LoadMaterials);


		if (ImGui::Button("Save")) {
			asset->SaveSetting(settings);
			asset->Load();
		}

		Controls::Text("Loaded Content");
		ModelAssetContent(asset->GetRootNode());

	}
	void Texture2DAssetEditor::DrawGui(Stulu::SharedTexture2DAssetData* asset) {
		TextureSettings& settings = asset->GetNative()->getSettings();

		Controls::Combo("Format", settings.format);
		Controls::Combo("Filtering", settings.filtering);
		Controls::Combo("Wrap", settings.wrap);
		if(settings.wrap == TextureWrap::ClampToBorder)
			Controls::Default("Border", settings.border);
		Controls::Default("Tiling", settings.tiling);

		if (ImGui::Button("Save")) {
			asset->SaveSetting(settings);
			asset->Load();
		}
	}
	void SkyBoxAssetEditor::DrawGui(Stulu::SharedSkyBoxAssetData* asset) {
		static std::pair<Stulu::UUID, uint32_t> lastSettings;
		if (asset->GetUUID() != lastSettings.first) {
			lastSettings = { asset->GetUUID(), asset->GetNative()->getWidth() };
		}
		uint32_t& resolution = lastSettings.second;

		Controls::ResolutionCombo("Resolution", resolution);

		if (ImGui::Button("Save")) {
			if (!asset->IsMemoryLoaded()) {
				const auto& manager = AssetsManager::GlobalInstance();
				MetaInfo info;
				if (manager.ReadFileMeta(&info, asset->GetPath())) {
					manager.SetMetaValue(info, "Resolution", resolution);
					manager.WriteFileMeta(info, asset->GetPath());
				}
			}

			asset->Unload();
			asset->Load();
		}
	}
	void MeshAssetEditor::DrawGui(Stulu::SharedMeshAssetData* asset) {
		Controls::Text(asset->GetNative()->GetName());
		
		Mesh* mesh = asset->GetNative();

		constexpr auto KB_100 = 1024.0f * 100.0f;
		constexpr auto BYTES_TO_KILO = 1.0f / (1024.0f);
		constexpr auto BYTES_TO_MEGA = 1.0f / (1024.0f * 1024.0);

		float vSize = 0.0f;
		size_t vCount = 0;
		for (auto& buffer : mesh->GetVertexArray()->getVertexBuffers()) {
			vSize += (float)buffer->getSize();
			vCount += (size_t)buffer->getCount();
		}
		if (vSize > KB_100) {
			Controls::Text("Vertices: %" IM_PRIu64 " (%.3f mb)", vCount, vSize * BYTES_TO_MEGA);
		}
		else {
			Controls::Text("Vertices: %" IM_PRIu64 " (%.3f kb)", vCount, vSize * BYTES_TO_KILO);
		}

		float iSize = 0.0f;
		size_t iCount = 0;
		if (Ref<IndexBuffer> ib = mesh->GetVertexArray()->getIndexBuffer()) {
			iSize += (float)ib->getSize();
			iCount += (size_t)ib->getCount();
		}
		if (iSize > KB_100) {
			Controls::Text("Indices:  %" IM_PRIu64 " (%.3f mb)", iCount, iSize * BYTES_TO_MEGA);
		}
		else {
			Controls::Text("Indices: %" IM_PRIu64 " (%.3f kb)", iCount, iSize * BYTES_TO_KILO);
		}

		float total = iSize + vSize;
		if (total > KB_100) {
			Controls::Text("Total size: %.3f mb", total * BYTES_TO_MEGA);
		}
		else {
			Controls::Text("Total size: %.3f kb", total * BYTES_TO_KILO);
		}
	}
	
	template<class T>
	bool MaterialDisplay(const Ref<MaterialProperty>& baseProp, std::function<bool(const std::string&, typename T::NativeType&)> uiFunc = Controls::Default<T::NativeType>) {
		if (baseProp->GetType() == T::PropertyType) {
			Ref<T> prop = std::dynamic_pointer_cast<T>(baseProp);
			T::NativeType value = prop->GetValue();

			if (uiFunc(prop->GetName(), value)) {
				prop->SetValue(value);
				return true;
			}
		}
		return false;
	}
	template<class T>
	bool MaterialDisplayCustom(const Ref<MaterialProperty>& baseProp, std::function<bool(const Ref<T> prop, typename T::NativeType&)> uiFunc) {
		if (baseProp->GetType() == T::PropertyType) {
			Ref<T> prop = std::dynamic_pointer_cast<T>(baseProp);
			T::NativeType value = prop->GetValue();

			if (uiFunc(prop, value)) {
				prop->SetValue(value);
				return true;
			}
		}
		return false;
	}

	void MaterialAssetEditor::DrawGui(Stulu::SharedMaterialAssetData* asset) {
		TestMaterial* material = asset->GetNative();

		auto tMode = material->GetTransparencyMode();
		if (Controls::Combo("Transparency Mode", tMode)) {
			material->SetTransparencyMode(tMode);
		}
		if (tMode == MaterialTransparencyMode::Cutout) {
			float valueCut = material->GetAlphaCutOut();

			if (Controls::Slider::Float("Cut out", valueCut))
				material->SetAlphaCutOut(valueCut);
		}

		for (auto& prop : material->GetProperities()) {
			bool changed = false;

			changed |= MaterialDisplay<MaterialSampler2DProperty>(prop);
			changed |= MaterialDisplay<MaterialSkyBoxProperty>(prop);
			changed |= MaterialDisplay<MaterialFloat2Property>(prop);
			changed |= MaterialDisplay<MaterialFloat3Property>(prop);
			changed |= MaterialDisplay<MaterialFloat4Property>(prop);
			changed |= MaterialDisplay<MaterialIntProperty>(prop);
			changed |= MaterialDisplay<MaterialInt2Property>(prop);
			changed |= MaterialDisplay<MaterialInt3Property>(prop);
			changed |= MaterialDisplay<MaterialInt4Property>(prop);
			changed |= MaterialDisplay<MaterialUIntProperty>(prop);

			changed |= MaterialDisplayCustom<MaterialColorProperty>(prop, [](const auto& colorProp, glm::vec4& color) {
				return Controls::Color(colorProp->GetName(), color, colorProp->IsHDR());
			});
			changed |= MaterialDisplayCustom<MaterialFloatProperty>(prop, [](const auto& floatProp, float& value) {
				if (floatProp->HasLimits())
					return Controls::Slider::Float(floatProp->GetName(), value, floatProp->GetMin(), floatProp->GetMax());
				return Controls::Float(floatProp->GetName(), value, floatProp->GetMin(), floatProp->GetMax());
			});

			if(changed)
				prop->ApplyValue(material);
		}
	}
}
