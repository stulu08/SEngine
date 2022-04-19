#include "st_pch.h"
#include "AssetsManager.h"
#include "Stulu/Scene/YAML.h"
#include "Stulu/Scene/Model.h"


namespace Stulu {
	std::unordered_map<UUID, Asset> AssetsManager::assets;

	bool FileExists(const std::string& name) {
		ST_PROFILING_FUNCTION();
		if (FILE* file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}

	UUID AssetsManager::add(const std::string& path) {
		ST_PROFILING_FUNCTION();
		if (std::filesystem::is_directory(path))
			return addDirectory(path);
		if (!FileExists(path + ".meta")) {
			createMeta(UUID(), path, assetTypeFromExtension(path.substr(path.find_last_of('.'), path.npos)));
		}
		YAML::Node data = YAML::LoadFile(path + ".meta");
		UUID uuid(data["uuid"].as<uint64_t>());
		if (!exists(uuid))
			add(uuid,path, (AssetType)data["type"].as<int>());

		return uuid;
	}
	void AssetsManager::add(const UUID& uuid, const std::string& path) {
		ST_PROFILING_FUNCTION();
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
			update(uuid, { type,Texture2D::create(path,getProperity<TextureSettings>(path,"format")),path,uuid});
			break;
		case Stulu::AssetType::Texture:
			update(uuid, { type,Texture2D::create(path),path,uuid });
			break;
		case Stulu::AssetType::CubeMap:
			update(uuid, { type,CubeMap::create(path),path,uuid });
			break;
		case Stulu::AssetType::Model:
			createMeshesFromModel({ type,0,path,uuid });
			break;
		case Stulu::AssetType::Script:
			update(uuid, { type,path,path,uuid });
			break;
		case Stulu::AssetType::Material:
			update(uuid, { type,Material::fromDataStringPath(path,uuid),path,uuid });
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
		ST_PROFILING_FUNCTION();
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
		if (!FileExists(path + ".meta")) {
			createMeta(UUID(), path, AssetType::Directory);
		}
		YAML::Node data = YAML::LoadFile(path + ".meta");
		UUID uuid(data["uuid"].as<uint64_t>());

		if (!exists(uuid))
			assets[uuid] = Asset{ AssetType::Directory ,path,path,uuid};
		return uuid;
	}

	void AssetsManager::update(const UUID& uuid, const Asset& data) {
		bool update = true;
		switch (data.type)
		{
		case Stulu::AssetType::Texture2D:
			std::any_cast<Ref<Texture2D>>(data.data)->uuid = uuid;
			break;
		case Stulu::AssetType::Texture:
			std::any_cast<Ref<Texture2D>>(data.data)->uuid = uuid;
			break;
		case Stulu::AssetType::CubeMap:
			std::any_cast<Ref<CubeMap>>(data.data)->uuid = uuid;
			break;
		case Stulu::AssetType::Mesh:
			std::any_cast<MeshAsset>(data.data).uuid = uuid;
			break;
		case Stulu::AssetType::Material:
			std::any_cast<Material>(data.data).m_uuid = uuid;
			break;
		case Stulu::AssetType::Shader:
			//check if shader name is present
			for (auto& i : assets) {
				if (i.second.type != AssetType::Shader)
					continue;
				if (std::any_cast<Ref<Shader>>(i.second.data)->getName() == std::any_cast<Ref<Shader>>(data.data)->getName()) {
					CORE_ERROR("Shader {0} is allready present, choose another shader name", std::any_cast<Ref<Shader>>(data.data)->getName());
					update = false;
					break;
				}
			}
			//change the material which uses the shader
			if(update)
			for (auto& i : assets) {
				if (i.second.type != AssetType::Material)
					continue;
				if (std::any_cast<Material&>(i.second.data).m_shader->getName() == std::any_cast<Ref<Shader>>(data.data)->getName()) {
					std::any_cast<Material&>(i.second.data).m_shader = std::any_cast<Ref<Shader>>(data.data);
					std::any_cast<Material&>(i.second.data).m_shaderUuid = uuid;
					std::any_cast<Material&>(i.second.data).toDataStringFile(i.second.path);
					std::any_cast<Material&>(i.second.data).bind();
				}
			}
			break;
		}
		//update meta
		if (FileExists(data.path)) {
			createMeta(uuid, data.path, data.type);
		}
		if(update)
			assets[uuid] = data;
	}

	void AssetsManager::remove(const UUID& uuid, bool deleteFile, bool deleteMetaFile) {
		if (exists(uuid)) {
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
			assets.erase(uuid);
		}
	}

	bool Stulu::AssetsManager::exists(const UUID& uuid) {
		ST_PROFILING_FUNCTION();
		return assets.find(uuid) != assets.end();
	}
	bool AssetsManager::existsAndType(const UUID& uuid, const AssetType type) {
		ST_PROFILING_FUNCTION();
		if (uuid == UUID::null)
			return false;
		return (assets.find(uuid) != assets.end() && assets[uuid].type == type);
	}

	Asset& Stulu::AssetsManager::get(const UUID& uuid) {
		ST_PROFILING_FUNCTION();
		if(exists(uuid))
			return assets[uuid];
		CORE_ERROR("UUID not present in assets");
		return NullAsset;
	}
	const type_info& Stulu::AssetsManager::getType(const UUID& uuid) {
		ST_PROFILING_FUNCTION();
		if (exists(uuid))
			return assets[uuid].data.type();
		CORE_ERROR("UUID not present in assets");
		return NullAsset.data.type();
	}
	const AssetType& AssetsManager::getAssetType(const UUID& uuid) {
		ST_PROFILING_FUNCTION();
		return assets[uuid].type;
	}
	const AssetType AssetsManager::assetTypeFromExtension(const std::string& extension) {
		ST_PROFILING_FUNCTION();
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga")
			return AssetType::Texture;
		if (extension == ".skybox" || extension == ".hdr")
			return AssetType::CubeMap;
		else if (extension == ".glb" || extension == ".obj" || extension == ".fbx")
			return AssetType::Model;
		else if (extension == ".mat")
			return AssetType::Material;
		else if (extension == ".glsl" || extension == ".vert" || extension == ".frag" || extension == ".sshader")
			return AssetType::Shader;
		else if (extension == ".scene" || extension == ".stulu")
			return AssetType::Scene;
		else if (extension == ".cs")
			return AssetType::Script;
		else if (extension == "")
			return AssetType::Directory;
		return AssetType::Unknown;
	}
	UUID AssetsManager::getFromPath(const std::string& path) {
		ST_PROFILING_FUNCTION();
		for (auto& i : assets) {
			if(path == i.second.path)
				return i.first;
		}
		if (std::filesystem::is_directory(path))
			return addDirectory(path);
		return add(path);
	}

	void AssetsManager::loadAllFiles(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading all files from directory: {0}", directory);
		//at first shaders and textures need to be loaded for the materials 
		loadShaders(directory);
		loadTextures(directory);
		loadMaterials(directory);
		loadDirectory(directory);
	}
	void AssetsManager::loadDirectory(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Files: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				addDirectory(path.string());
				loadDirectory(path.string());
				continue;
			}
			if (path.extension() != ".meta")
				add(path.string());
		}
	}
	void AssetsManager::loadShaders(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Shaders: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadShaders(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::Shader)
				add(path.string());
		}
	}
	void AssetsManager::loadTextures(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Textures: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadTextures(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::Texture)
				add(path.string());
		}
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadTextures(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::CubeMap)
				add(path.string());
		}
	}
	void AssetsManager::loadMaterials(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Loading Material: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadMaterials(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::Material)
				add(path.string());
		};
	}
	void AssetsManager::reloadShaders(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_TRACE("Reloading Shaders: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				reloadShaders(path.string());
				continue;
			}
			if (assetTypeFromExtension(path.extension().string()) == AssetType::Shader)
				addOrReload(path.string());
		}
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
		if (data["props"]["meshes"]) {
			for (auto n : data["props"]["meshes"]) {
				meshesUuids[UUID(n["uuid"].as<uint64_t>())] = n["index"].as<size_t>();
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
			out << YAML::EndMap;
			out << YAML::EndSeq;
			out << YAML::EndMap;


			FILE* file = fopen((asset.path + ".meta").c_str(), "w");
			fprintf(file, out.c_str());
			fclose(file);

			return _createMeshesFromModel(asset, model);
		}
		CORE_ASSERT(model.getMeshes().size() == meshesUuids.size(), "Mesh count in model does not equal mesh count in meta file");
		for (auto i : meshesUuids) {
			MeshAsset& m = model.getMeshes()[i.second];
			for (auto& mA : model.getMeshes()) {
				if (mA.parentMeshAsset == m.uuid)
					mA.parentMeshAsset = i.first;
			}
			m.uuid = i.first;
			update(i.first, { AssetType::Mesh, m,"",i.first });
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