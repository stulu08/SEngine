#include "st_pch.h"
#include "AssetsManager.h"

#include "Stulu/Serialization/YAML.h"
#include "Assets/TextureAssets.h"
#include "Assets/MeshAsset.h"
#include "Assets/MaterialAsset.h"
#include "Assets/ModelAsset.h"
#include "Assets/ShaderAsset.h"

namespace Stulu {
	static AssetsManager* Instance = nullptr;

	AssetsManager::AssetsManager() {
		RegisterType<SharedMiscFileAssetData>();
		RegisterType<SharedTexture2DAssetData>();
		RegisterType<SharedSkyBoxAssetData>();
		RegisterType<SharedRenderTextureAssetData>();
		RegisterType<SharedMeshAssetData>();
		RegisterType<SharedMaterialAssetData>();
		RegisterType<SharedModelAssetData>();
		RegisterType<SharedShaderAssetData>();
		RegisterType<SharedSceneAssetData>();
		Instance = this;
	}
	AssetsManager::~AssetsManager() {
		m_assets.clear();
	}

	AssetsManager& AssetsManager::GlobalInstance() {
		return *Instance;
	}

	void AssetsManager::LoadDirectoryRecursive(const std::string& path) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
			if (entry.is_directory())
				continue;
			if (entry.path().extension().string() == ".meta")
				continue;

			const std::string& file = entry.path().string();
			LoadFile(file);
		}
	}

	AssetsManager::IDType AssetsManager::LoadFile(const std::string& filePath) {
		const std::filesystem::path path = filePath;
		const std::string extension = path.extension().string();

		if (std::filesystem::is_directory(path)) {
			CORE_WARN("Directory assets are not supported!");
			return IDType::null;
		}
		if (!std::filesystem::exists(path)) {
			CORE_ERROR("File not found: {0}", path);
			return IDType::null;
		}
		if (extension == ".meta") {
			return IDType::null;
		}

		MetaInfo metaInfo;
		if (!ReadFileMeta(&metaInfo, filePath)) {
			metaInfo.uuid = IDType();
			metaInfo.type = FindTypeByExtension(extension);
		}
		if (!WriteFileMeta(metaInfo, filePath)) {
			return IDType::null;
		}

		// check if already loaded
		const auto it = Find(metaInfo.uuid);
		if (it != m_assets.end()) {
			if (it->second->GetTypeID() == metaInfo.type)
				return metaInfo.uuid;

			CORE_ERROR("Asset with UUID {0} already loaded, but found type mismatch: {1}", metaInfo.uuid, path);
		}
		else if (AddAsset(metaInfo.type, filePath, metaInfo.uuid)) {
			return metaInfo.uuid;
		}
		return IDType::null;
	}

	SharedAssetData* AssetsManager::GetFromPath(const std::string& path, bool metaRead) const {
		if (metaRead) {
			MetaInfo metaInfo;
			if (ReadFileMeta(&metaInfo, path)) {
				auto it = Find(metaInfo.uuid);
				if (it != m_assets.end()) {
					SharedAssetData* data = it->second.get();
					if (data) return (data);

					CORE_WARN("Corrupted Asset Registry for Asset: {0}", metaInfo.uuid);
					return nullptr;
				}
			}
		}

		for (const auto& [uuid, asset] : m_assets) {
			if (asset->GetPath() == path) {
				return asset.get();
			}
		}
		return nullptr;
	}

	SharedAssetData* AssetsManager::GetRaw(IDType uuid) const {
		auto it = Find(uuid);
		if (it != m_assets.end()) {
			SharedAssetData* data = it->second.get();
			if (data) return (data);
			else CORE_WARN("Corrupted Asset Registry for Asset: {0}", uuid);
		}
		else {
			CORE_WARN("Asset not found with id: {0}", uuid);
		}
		return nullptr;
	}


	bool AssetsManager::AddAsset(std::type_index type, const std::string& path, IDType uuid, bool forceLoad) {
		const auto createFnIt = m_typeCreateRegistry.find(type);
		if (createFnIt == m_typeCreateRegistry.end()) {
			CORE_WARN("Asset could not be added, type not found in registry for asset: {0}", path);
			return false;
		}

		SharedAssetData* asset = createFnIt->second(uuid, path);
		if (!asset) {
			CORE_ERROR("Error during asset creation for: {0}", path);
			return false;
		}

		auto [it, inserted] = m_assets.emplace(uuid, asset);
		if (!inserted) {
			CORE_WARN("Asset with UUID {0} already exists: {1}", uuid, path);
			delete asset;
			return false;
		}

		if (forceLoad) {
			CORE_TRACE("Loading: {0}", path);
			asset->IncRef();
			asset->Load();
		}

		return true;
	}
	bool AssetsManager::AddAsset(SharedAssetData* asset, IDType uuid, bool forceLoad) {
		if (asset == nullptr) {
			CORE_WARN("Invalid asset provided to AssetsManager::AddAsset!");
			return false;
		}

		if (asset->GetUUID() != uuid) {
			asset->SetUUID(uuid);
		}

		auto [it, inserted] = m_assets.emplace(uuid, asset);
		if (!inserted) {
			CORE_WARN("Asset with UUID {0} already exists", uuid);
			delete asset;
			return false;
		}

		if (forceLoad) {
			CORE_TRACE("Loading: {0}", asset->GetPath());
			asset->IncRef();
			asset->Load();
		}

		return true;
	}

	void AssetsManager::RemoveAsset(IDType uuid) {
		auto it = m_assets.find(uuid);
		if (it == m_assets.end()) {
			CORE_WARN("Asset not found for deletion");
			return;
		}
		m_assets.erase(it);
	}

	bool AssetsManager::UpdatePath(IDType uuid, const std::string& newPath) {
		auto it = m_assets.find(uuid);
		if (it == m_assets.end()) {
			CORE_WARN("Asset not found for path change");
			return false;
		}
		return it->second->ChangePath(newPath);
	}

	bool AssetsManager::ReadFileMeta(MetaInfo* outInfo, const std::string& path) const {
		const std::string metaPath = path + ".meta";

		if (!FileExists(metaPath) || !outInfo) return false;

		outInfo->node = YAML::LoadFile(metaPath);
		YAML::Node& data = outInfo->node;

		IDType uuid = IDType::null;

		if (data["uuid"]) uuid = data["uuid"].as<IDType>();
		if (data["UUID"]) uuid = data["UUID"].as<IDType>();
		if (uuid == IDType::null) return false;

		if (data["TypeName"]) {
			std::string typeName = data["TypeName"].as<std::string>();
			auto it = m_typeNameRegistry.find(typeName);
			if (it != m_typeNameRegistry.end()) {
				outInfo->uuid = uuid;
				outInfo->type = it->second;
				return true;
			}
			else {
				CORE_WARN("Invalid TypeName inside: '{0}'", metaPath);
				CORE_WARN("Using MiscFile type");
				outInfo->uuid = uuid;
				outInfo->type = SharedMiscFileAssetData(nullptr).GetTypeID();
				return true;
			}
		}
		else {
			outInfo->uuid = uuid;
			outInfo->type = FindTypeByExtension(std::filesystem::path(path).extension().string());
			return true;
		}
	}

	bool AssetsManager::WriteFileMeta(MetaInfo& info, const std::string& path) const {
		const std::string metaPath = path + ".meta";
		const auto it = m_typeRegistry.find(info.type);

		if (it != m_typeRegistry.end()) {
			const std::string typeName = it->second->GetTypeName();;

			if (!info.node || !info.node.IsMap()) {
				info.node.reset(YAML::Node(YAML::NodeType::Map));
			}

			info.node["UUID"] = info.uuid;
			info.node["TypeName"] = typeName;

			YAML::Emitter out;
			out << info.node;

			std::ofstream fout(metaPath);
			if (!fout.is_open()) {
				CORE_ERROR("Could not open meta file for writing: '{0}'", metaPath);
				return false;
			}

			fout << out.c_str();
			fout.close();

			return true;
		}
		CORE_WARN("Invalid TypeName inside asset for meta: '{0}'", metaPath);
		return false;
	}


	std::type_index AssetsManager::FindTypeByExtension(const std::string& extension) const {
		if (!extension.empty()) {
			for (const auto& [id, assetInstance] : m_typeRegistry) {
				if (assetInstance->HasExtension(extension))
					return id;
			}
		}

		return SharedMiscFileAssetData(nullptr).GetTypeID();
	}
}