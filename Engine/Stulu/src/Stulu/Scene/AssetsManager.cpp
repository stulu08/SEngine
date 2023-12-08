#include "st_pch.h"
#include "AssetsManager.h"
#include "Stulu/Scene/YAML.h"
#include "Stulu/Scene/Model.h"
#include "Stulu/Scene/Components/Camera.h"
#include <Stulu/Core/Resources.h>

namespace Stulu {
	std::unordered_map<UUID, Asset> AssetsManager::assets;
	std::unordered_map<AssetType, std::vector<UUID>> AssetsManager::assetsTypeList;

	bool FileExists(const std::string& name) {
		if (FILE* file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}

	UUID AssetsManager::add(const std::string& path) {
		if (std::filesystem::is_directory(path))
			return addDirectory(path);
		UUID uuid;
		AssetType type;
		if (!FileExists(path + ".meta")) {
			if (path.find_last_of('.') == path.npos)
				return UUID::null;
			uuid = UUID();
			type = assetTypeFromExtension(path.substr(path.find_last_of('.'), path.npos));
			createMeta(uuid, path, type);
		}
		else {
			try {
				YAML::Node data = YAML::LoadFile(path + ".meta");
				uuid = UUID(data["uuid"].as<uint64_t>());
				type = (AssetType)data["type"].as<int>();
			}
			catch (YAML::Exception ex) {
				std::filesystem::remove(path + ".meta");
				return add(path);
			}
			
		}

		if (!exists(uuid))
			add(uuid, path, type);

		return uuid;
	}
	void AssetsManager::add(const UUID& uuid, const std::string& path) {
		if(!exists(uuid))
			add(uuid, path, assetTypeFromExtension(path.substr(path.find_last_of('.'), path.npos)));
	}
	void AssetsManager::add(const UUID& uuid, const std::string& path, const AssetType type) {
		ST_PROFILING_FUNCTION();
		if (!FileExists(path)) {
			createMeta(uuid, path, type);
		}
		switch (type)
		{
		case Stulu::AssetType::Unknown:
			update(uuid, { type,path,path,uuid });
			break;
		case Stulu::AssetType::Texture2D:
			update(uuid, { type,static_cast<Ref<Texture>>(Texture2D::create(path,getProperity<TextureSettings>(path,"format"))),path,uuid});
			break;
		case Stulu::AssetType::SkyBox:
			update(uuid, { type,static_cast<Ref<Texture>>(SkyBox::create(path)),path,uuid });
			break;
		case Stulu::AssetType::RenderTexture:
			update(uuid, { type, Resources::getBlackTexture(), path, uuid });
			break;
		case Stulu::AssetType::Model:
			createMeshesFromModel({ type,0,path,uuid });
			break;
		case Stulu::AssetType::Script:
			update(uuid, { type,path,path,uuid });
			break;
		case Stulu::AssetType::Material:
			update(uuid, { type,Material::create(path,uuid),path,uuid });
			break;
		case Stulu::AssetType::Shader:
			update(uuid, { type,Shader::create(path),path,uuid });
			break;
		case Stulu::AssetType::Scene:
			update(uuid,{ type,path,path,uuid });
			break;
		default:
			break;
		}

	}

	void AssetsManager::addOrReload(const std::string& path) {
		if (!FileExists(path + ".meta")) {
			createMeta(UUID(), path, assetTypeFromExtension(path.substr(path.find_last_of('.'), path.npos)));
		}
		YAML::Node data = YAML::LoadFile(path + ".meta");
		UUID uuid(data["uuid"].as<uint64_t>());
		if (exists(uuid))
			remove(uuid);
		add(uuid, path, (AssetType)data["type"].as<int>());

	}

	UUID AssetsManager::addDirectory(const std::string& path) {
		UUID uuid;
		if (!FileExists(path + ".meta")) {
			uuid = UUID();
			createMeta(uuid, path, AssetType::Directory);
		}
		else {
			try {
				YAML::Node data = YAML::LoadFile(path + ".meta");
				uuid = UUID(data["uuid"].as<uint64_t>());
			}
			catch (YAML::Exception ex) {
				std::filesystem::remove(path + ".meta");
				return addDirectory(path);
			}

		}
		if (!exists(uuid))
			update(uuid, Asset{ AssetType::Directory ,path,path,uuid });

		return uuid;
	}

	MeshAsset& AssetsManager::createMeshAsset(const Ref<Mesh>& mesh, const std::string& name, UUID uuid) {
		MeshAsset asset;
		asset.mesh = mesh;
		asset.name = name;
		asset.uuid = uuid;
		if (mesh)
			asset.hasMesh = true;
		AssetsManager::update(uuid, { AssetType::Mesh, asset, "",uuid });
		return getAs<MeshAsset>(uuid);
	}

	void AssetsManager::update(const UUID& uuid, const Asset& data) {
		ST_PROFILING_FUNCTION();
		if ((uuid == 301|| uuid == 302) && assets.find(uuid) != assets.end()) {
			CORE_ASSERT(false, "White or Black Texture is being overwritten");
		}

		switch (data.type)
		{
		case Stulu::AssetType::Texture2D: 
			{
				Ref<Texture> texture = std::any_cast<Ref<Texture>>(data.data);
				texture->uuid = uuid;
				if(FileExists(data.path + ".meta"))
					AssetsManager::setProperity<TextureSettings>(data.path, std::pair<std::string, TextureSettings> {"format", texture->getSettings()});
				break;
			}
		case Stulu::AssetType::RenderTexture:
			std::any_cast<Ref<Texture>>(data.data)->uuid = uuid;
			break;
		case Stulu::AssetType::SkyBox:
			std::any_cast<Ref<Texture>>(data.data)->uuid = uuid;
			break;
		case Stulu::AssetType::Mesh:
			std::any_cast<const MeshAsset&>(data.data).uuid = uuid;
			break;
		case Stulu::AssetType::Material:
			std::any_cast<Ref<Material>>(data.data)->m_uuid = uuid;
			break;
		}
		//update meta
		if (data.type != AssetType::Directory && FileExists(data.path)) {
			createMeta(uuid, data.path, data.type);
		}

		assetsTypeList[data.type].push_back(uuid);
		assets[uuid] = data;
	}

	void AssetsManager::remove(const UUID& uuid, bool deleteFile, bool deleteMetaFile) {
		ST_PROFILING_FUNCTION();
		if (exists(uuid)) {
			Asset& asset = assets[uuid];
			if (asset.type == AssetType::Directory) {
				for (auto& file : std::filesystem::directory_iterator(asset.path)) {
					AssetsManager::remove(getFromPath(file.path().string()), true, true);
				}
			}
			try {
				std::string path = get(uuid).path;

				std::filesystem::remove_all(path.c_str());

				if (deleteMetaFile && FileExists(path + ".meta")) {
					std::remove((path + ".meta").c_str());
				}
			}
			catch (std::exception ex) {
				CORE_ERROR("Couldn't delete Files/Directory with UUID: {0}\n	:{1}", uuid, ex.what())
			}
			if (asset.type == AssetType::Material) {
				std::any_cast<Ref<Material>>(asset.data).reset();
			}
			std::vector<UUID>& typeList = assetsTypeList[asset.type];
			typeList.erase(std::find(typeList.begin(), typeList.end(), uuid));
			asset.data.reset();
			assets.erase(uuid);
			
		}
	}

	bool Stulu::AssetsManager::exists(const UUID& uuid) {
		if (uuid == UUID::null)
			return false;
		return assets.find(uuid) != assets.end();
	}
	bool AssetsManager::existsAndType(const UUID& uuid, const AssetType type) {
		if (uuid == UUID::null)
			return false;
		return (assets.find(uuid) != assets.end() && assets[uuid].type == type);
	}

	Asset& Stulu::AssetsManager::get(const UUID& uuid) {
		if(exists(uuid))
			return assets[uuid];
		CORE_ERROR("UUID not present in assets");
		return NullAsset;
	}
	const AssetType AssetsManager::getAssetTypeByPath(const std::string& path) {
		YAML::Node data = YAML::LoadFile(path + ".meta");
		return (AssetType)data["type"].as<int>();

	}
	const type_info& Stulu::AssetsManager::getType(const UUID& uuid) {
		if (exists(uuid))
			return assets[uuid].data.type();
		CORE_ERROR("UUID not present in assets");
		return NullAsset.data.type();
	}
	const AssetType& AssetsManager::getAssetType(const UUID& uuid) {
		return assets[uuid].type;
	}
	const AssetType AssetsManager::assetTypeFromExtension(const std::string& extension) {
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga")
			return AssetType::Texture2D;
		if (extension == ".skybox" || extension == ".hdr")
			return AssetType::SkyBox;
		if (extension == ".srt")
			return AssetType::RenderTexture;
		else if (extension == ".glb" || extension == ".gltf" || extension == ".obj" || extension == ".fbx")
			return AssetType::Model;
		else if (extension == ".mat")
			return AssetType::Material;
		else if (extension == ".glsl" || extension == ".vert" || extension == ".frag" || extension == ".shader")
			return AssetType::Shader;
		else if (extension == ".scene" || extension == ".ssc")
			return AssetType::Scene;
		else if (extension == ".cs")
			return AssetType::Script;
		else if (extension == "")
			return AssetType::Directory;
		return AssetType::Unknown;
	}
	UUID AssetsManager::getFromPath(const std::filesystem::path& path) {
		if (path.empty() || !std::filesystem::exists(path))
			return false;


		if (FileExists(path.string() + ".meta")) {
			YAML::Node data = YAML::LoadFile(path.string() + ".meta");
			return UUID(data["uuid"].as<uint64_t>());
		}
		
		for (auto& i : assets) {
			if (path.compare(i.second.path) == 0)
				return i.first;
		}
		return UUID::null;
	}
	UUID AssetsManager::getFromPath(const std::filesystem::path& path, AssetType type) {
		if (path.empty() || !std::filesystem::exists(path))
			return false;
		for (UUID id : assetsTypeList[type]) {
			Asset& i = get(id);
			if (path.compare(i.path) == 0)
				return id;
		}

		return UUID::null;
	}

	void AssetsManager::loadAllFiles(const std::string& directory, bool loadNewFiles) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading all files from directory: {0}", directory);
		//at first shaders and textures need to be loaded for the materials and other
		loadShaders(directory, loadNewFiles);
		loadTextures(directory, loadNewFiles);
		loadMaterials(directory, loadNewFiles);
		loadDirectory(directory, loadNewFiles);
	}
	void AssetsManager::loadDirectory(const std::string& directory, bool loadNewFiles) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Files: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				if (FileExists(path.string() + ".meta"))
					addDirectory(path.string());
				else if (loadNewFiles)
					addDirectory(path.string());
				loadDirectory(path.string());
				continue;
			}
			if (path.extension() != ".meta") {
				if (FileExists(path.string() + ".meta"))
					add(path.string());
				else if (loadNewFiles)
					add(path.string());
			}
		}
	}
	void AssetsManager::loadShaders(const std::string& directory, bool loadNewFiles) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Shaders: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadShaders(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::Shader) {
				if (FileExists(path.string() + ".meta"))
					add(path.string());
				else if(loadNewFiles)
					add(path.string());
			}
				
		}
	}
	void AssetsManager::loadTextures(const std::string& directory, bool loadNewFiles) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Textures: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadTextures(path.string());
				continue;
			}
			if ((assetTypeFromExtension(path.extension().string()) == AssetType::Texture2D)) {
				if (FileExists(path.string() + ".meta"))
					add(path.string());
				else if (loadNewFiles)
					add(path.string());
			}
		}
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadTextures(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::SkyBox) {
				if (FileExists(path.string() + ".meta"))
					add(path.string());
				else if (loadNewFiles)
					add(path.string());
			}
				
		}
	}
	void AssetsManager::loadMaterials(const std::string& directory, bool loadNewFiles) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Material: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadMaterials(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::Material) {
				if (FileExists(path.string() + ".meta"))
					add(path.string());
				else if (loadNewFiles)
					add(path.string());
			}
		};
	}
	void AssetsManager::reloadShaders(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Reloading Shaders: {0}", directory);
		for (auto& [uuid, asset] : assets) {
			if (asset.type != AssetType::Shader) {
				continue;
			}
			if (FileExists(asset.path)) {
				CORE_TRACE("Reloading Shader: {0}", asset.path);

				Ref<Shader>& shader = std::any_cast<Ref<Shader>&>(asset.data);
				shader->reload(asset.path);
			}
		}
	}
	UUID AssetsManager::getModelFromMesh(UUID mesh) {
		if (mesh == UUID::null)
			return UUID::null;

		for (auto& [uuid, asset] : assets) {
			if (asset.type == AssetType::Model) {
				Model& model = std::any_cast<Model&>(asset.data);
				for (auto& meshAsset : model.getMeshes()) {
					if (meshAsset.uuid == mesh)
						return uuid;
				}
			}
		}
		return UUID::null;
	}
	UUID AssetsManager::getModelFromMaterial(UUID material) {
		if (material == UUID::null)
			return UUID::null;

		for (auto& [uuid, asset] : assets) {
			if (asset.type == AssetType::Model) {
				Model& model = std::any_cast<Model&>(asset.data);
				for (auto& mats : model.getMaterials()) {
					if (mats.second->m_uuid == material)
						return uuid;
				}
			}
		}
		return UUID::null;
	}
	void AssetsManager::createMeshesFromModel(const Asset asset) {
		Model model = Model(asset.path);
		_createMeshesFromModel(asset, model);
	}
	void AssetsManager::_createMeshesFromModel(const Asset asset, Model& model) {
		ST_PROFILING_FUNCTION();
		YAML::Node data = YAML::LoadFile(asset.path + ".meta");
		//uuid, index in model
		std::unordered_map<UUID, size_t> meshesUuids;
		std::unordered_map<UUID, uint32_t> materialUuids;
		if (data["props"]["meshes"] && data["props"]["materials"]) {
			for (auto n : data["props"]["meshes"]) {
				meshesUuids[UUID(n["uuid"].as<uint64_t>())] = n["index"].as<size_t>();
			}
			for (auto n : data["props"]["materials"]) {
				materialUuids[UUID(n["uuid"].as<uint64_t>())] = n["index"].as<uint32_t>();
			}
		}
		else {

			YAML::Emitter out, c;

			out << YAML::BeginMap;
			out << YAML::Key << "uuid" << YAML::Value << (uint64_t)asset.uuid;
			out << YAML::Key << "type" << YAML::Value << (int)asset.type;
			out << YAML::Key << "props";
			out << YAML::BeginMap;

			out << YAML::Key << "meshes" << YAML::Value << YAML::BeginSeq;
			for (uint32_t i = 0; i < model.getMeshes().size(); i++) {
				out << YAML::BeginMap;
				out << YAML::Key << "uuid" << YAML::Value << (uint64_t)UUID();
				out << YAML::Key << "index" << YAML::Value << i;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::Key << "materials" << YAML::Value << YAML::BeginSeq;
			for (uint32_t i = 0; i < model.getMaterials().size(); i++) {
				out << YAML::BeginMap;
				out << YAML::Key << "uuid" << YAML::Value << (uint64_t)UUID();
				out << YAML::Key << "index" << YAML::Value << i;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;

			out << YAML::EndMap;

			out << YAML::EndMap;


			FILE* file = fopen((asset.path + ".meta").c_str(), "w");
			fprintf(file, out.c_str());
			fclose(file);

			return _createMeshesFromModel(asset, model);
		}
		for (auto& [uuid, index] : materialUuids) {
			Ref<Material>& m = model.getMaterials()[index];
			m->m_uuid = uuid;
			m->isReadonly = true;
			update(uuid, { AssetType::Material, m,"",uuid });
		}

		for (auto i : meshesUuids) {
			MeshAsset& m = model.getMeshes()[i.second];
			for (auto& mA : model.getMeshes()) {
				if (mA.parentMeshAsset == m.uuid)
					mA.parentMeshAsset = i.first;
			}
			for (int32_t mId : m.materialIDs) {
				m.materials.push_back(model.getMaterials()[mId]->m_uuid);
			}
			m.uuid = i.first;
			update(i.first, { AssetType::Mesh, m, "",i.first});
		}
		update(asset.uuid, { asset.type,model,asset.path,asset.uuid });
	}

	void AssetsManager::createMeta(const UUID& uuid, const std::string& path, const AssetType type) {
		ST_PROFILING_FUNCTION();
		YAML::Node props;
		bool hasProps = false;
		if (FileExists(path + ".meta")) {
			YAML::Node data = YAML::LoadFile(path + ".meta");
			if (data["props"]) {
				props = data["props"];
				hasProps = true;
			}
		}
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "uuid" << YAML::Value << (uint64_t)uuid;
		out << YAML::Key << "type" << YAML::Value << (int)type;
		if (hasProps) {
			out << YAML::Key << "props" << YAML::Value;
			out << props;
		}
		out << YAML::EndMap;

		FILE* file = fopen((path + ".meta").c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);
	}
}