#pragma once
#include "Stulu/Panel.h"

namespace Editor {
	class CustomAssetEditor {
	public:
		virtual ~CustomAssetEditor() = default;

		virtual void DrawImGui(Stulu::SharedAssetData* asset) = 0;

		virtual std::type_index GetTypeID() const = 0;
		virtual Stulu::GeneralAsset CreateAsset(const Stulu::UUID& uuid, const std::string& path) { return nullptr; }
	};

	class AssetEditor : public Panel {
	public:
		AssetEditor();
		~AssetEditor() = default;

		void SetAsset(Stulu::UUID uuid) { m_asset = uuid; }

		virtual void DrawImGui() override;

		void AssetEditor::PreWindow();
		void AssetEditor::PostWindow();

	private:
		std::vector<Stulu::Ref<CustomAssetEditor>> m_editors;
		Stulu::UUID m_asset = 0;
		ImVec2 m_indent = ImVec2(0,0);
	};

	template<class T>
	class NativeAssetEditor : public CustomAssetEditor {
		virtual void DrawGui(T* asset) = 0;
		
		virtual void DrawImGui(Stulu::SharedAssetData* asset) override { DrawGui(dynamic_cast<T*>(asset)); }
		virtual std::type_index GetTypeID() const override { return typeid(T); }
	};

	class ModelAssetEditor : public NativeAssetEditor<Stulu::SharedModelAssetData> {
		virtual void DrawGui(Stulu::SharedModelAssetData* asset) override;
	};
	class Texture2DAssetEditor : public NativeAssetEditor<Stulu::SharedTexture2DAssetData> {
		virtual void DrawGui(Stulu::SharedTexture2DAssetData* asset) override;
	};
	class SkyBoxAssetEditor : public NativeAssetEditor<Stulu::SharedSkyBoxAssetData> {
		virtual void DrawGui(Stulu::SharedSkyBoxAssetData* asset) override;
	};
	class MeshAssetEditor : public NativeAssetEditor<Stulu::SharedMeshAssetData> {
		virtual void DrawGui(Stulu::SharedMeshAssetData* asset) override;
	};
	class MaterialAssetEditor : public NativeAssetEditor<Stulu::SharedMaterialAssetData> {
		virtual void DrawGui(Stulu::SharedMaterialAssetData* asset) override;
	};
}