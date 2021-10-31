#include "st_pch.h"
#include "AssetsManager.h"
#include "Stulu/Scene/YAML.h"
#include "Stulu/Renderer/Model.h"

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
		if (!FileExists(path + ".meta")) {
			createMeta(UUID(), path, assetTypeFromExtension(path.substr(path.find_last_of('.'), path.npos)));
		}
		YAML::Node data = YAML::LoadFile(path + ".meta");
		UUID uuid(data["uuid"].as<uint64_t>());
		if (!exists(uuid))
			add(uuid,path, (AssetType)data["type"].as<int>());

		return uuid;
	}
	void AssetsManager::add(const UUID uuid, const std::string& path) {
		ST_PROFILING_FUNCTION();
		if(!exists(uuid))
			add(uuid, path, assetTypeFromExtension(path.substr(path.find_last_of('.'), path.npos)));
	}
	void AssetsManager::add(const UUID uuid, const std::string& path, const AssetType type) {
		ST_PROFILING_FUNCTION();
		if (!FileExists(path)) {
			createMeta(uuid, path, type);
		}
		switch (type)
		{
		case Stulu::AssetType::Unknown:
			assets[uuid] = { type,path,path,uuid };
			break;
		case Stulu::AssetType::Texture2D:
			assets[uuid] = { type, Texture2D::create(path),path,uuid };
			break;
		case Stulu::AssetType::Texture:
			assets[uuid] = { type,(Ref<Texture>)Texture2D::create(path),path,uuid };
			break;
		case Stulu::AssetType::CubeMap:
			assets[uuid] = { type,CubeMap::create(path),path,uuid };
			break;
		case Stulu::AssetType::Model:
			assets[uuid] = { type,Model(path),path,uuid };
			break;
		case Stulu::AssetType::Mesh:
			//assets[uuid] = { Mesh(path),path };
			break;
		case Stulu::AssetType::Material:
			assets[uuid] = { type,Material::fromDataStringPath(path,uuid),path,uuid };
			break;
		case Stulu::AssetType::Shader:
			assets[uuid] = { type,Shader::create(path),path,uuid };
			break;
		case Stulu::AssetType::Scene:
			assets[uuid] = { type,path,path,uuid };
			break;
		default:
			break;
		}

	}

	void AssetsManager::update(const UUID uuid, Asset data) {
		assets[uuid] = data;
	}

	bool Stulu::AssetsManager::exists(const UUID uuid) {
		ST_PROFILING_FUNCTION();
		return assets.find(uuid) != assets.end();
	}

	bool AssetsManager::existsAndType(const UUID uuid, const AssetType type) {
		ST_PROFILING_FUNCTION();
		return assets.find(uuid) != assets.end() && assets[uuid].type == type;
	}

	Asset& Stulu::AssetsManager::get(const UUID uuid) {
		ST_PROFILING_FUNCTION();
		CORE_ASSERT(exists(uuid),"UUID not present in assets");
		return assets[uuid];
	}
	const type_info& Stulu::AssetsManager::getType(const UUID uuid) {
		ST_PROFILING_FUNCTION();
		CORE_ASSERT(exists(uuid),"UUID not present in assets");
		return assets[uuid].data.type();
	}
	const AssetType& AssetsManager::getAssetType(const UUID uuid) {
		ST_PROFILING_FUNCTION();
		return assets[uuid].type;
	}
	template<class T>
	inline T* AssetsManager::getAs(const UUID uuid) {
		ST_PROFILING_FUNCTION();
		if(exists)
			return assets[uuid].first._Cast<T>();

		CORE_ASSERT(exists, "UUID not present in assets");
		return nullptr;
	}

	const AssetType AssetsManager::assetTypeFromExtension(const std::string& extension) {
		ST_PROFILING_FUNCTION();
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga" || extension == ".hdr")
			return AssetType::Texture;
		if (extension == ".skybox")
			return AssetType::CubeMap;
		else if (extension == ".glb" || extension == ".obj")
			return AssetType::Model;
		else if (extension == ".mesh" || extension == ".smesh")
			return AssetType::Mesh;
		else if (extension == ".mat")
			return AssetType::Material;
		else if (extension == ".glsl" || extension == ".vert" || extension == ".frag" || extension == ".sshader")
			return AssetType::Shader;
		else if (extension == ".scene" || extension == ".stulu")
			return AssetType::Scene;
		return AssetType::Unknown;
	}

	UUID AssetsManager::getFromPath(const std::string& path) {
		ST_PROFILING_FUNCTION();
		for (auto& i : assets) {
			if(path == i.second.path)
				return i.first;
		}
		
		return add(path);
	}

	void AssetsManager::loadAllFiles(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_INFO("Loading all files from directory: {0}", directory);
		loadShaders(directory);
		loadTextures(directory);
		loadMaterials(directory);
		loadDirectory(directory);
	}
	void AssetsManager::loadDirectory(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_INFO("Loading Files: {0}", directory);
		for (auto& dir : std::filesystem::directory_iterator(directory)) {
			const auto& path = dir.path();
			if (dir.is_directory()) {
				loadDirectory(path.string());
				continue;
			}
			if(path.extension() != ".meta")
				add(path.string());
		}
	}
	void AssetsManager::loadShaders(const std::string& directory) {
		ST_PROFILING_FUNCTION();
		CORE_INFO("Loading Shaders: {0}", directory);
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
		CORE_INFO("Loading Textures: {0}", directory);
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
		CORE_INFO("Loading Material: {0}", directory);
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

	void AssetsManager::createMeta(const UUID uuid, const std::string& path, const AssetType type) {
		ST_PROFILING_FUNCTION();
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "uuid" << YAML::Value << (uint64_t)uuid;
		out << YAML::Key << "type" << YAML::Value << (int)type;
		out << YAML::EndMap;

		FILE* file = fopen((path + ".meta").c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);
	}
}