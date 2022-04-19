#pragma once
#include "Stulu/Scene/Material.h"
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Core/UUID.h"
#include "YAML.h"
#include <unordered_map>


namespace Stulu {
	bool FileExists(const std::string& name);
	class Model;
	enum class AssetType {
		Unknown = 0,
		Texture2D = 1, Texture = 2, CubeMap = 3,
		Model = 4, Mesh = 5, 
		Material = 6, Shader = 7,
		Scene = 8,
		Directory = 9,
		Script = 10
	};
	
	struct Asset {
		AssetType type;
		std::any data;
		std::string path;
		UUID uuid;

	};
	static inline Asset NullAsset{ AssetType ::Unknown,nullptr,"",UUID::null};
	class STULU_API AssetsManager {
	public:
		/// read UUID from .meta file
		static UUID add(const std::string& path);
		static void add(const UUID& uuid, const std::string& path);
		static void add(const UUID& uuid, const std::string& path, const AssetType type);
		static void addOrReload(const std::string& path);
		static UUID addDirectory(const std::string& path);

		static void update(const UUID& uuid, const Asset& data);

		static void remove(const UUID& uuid, bool deleteFile = false, bool deleteMetaFile = false);

		static bool exists(const UUID& uuid);
		static bool existsAndType(const UUID& uuid, const AssetType type);

		static Asset& get(const UUID& uuid);
		const static type_info& getType(const UUID& uuid);
		const static AssetType& getAssetType(const UUID& uuid);
		template<typename T>
		static inline T* getAs(const UUID& uuid) {
			ST_PROFILING_FUNCTION();
			if (exists)
				return assets[uuid].first._Cast<T>();

			CORE_EROR(exists, "UUID not present in assets");
			return nullptr;
		}
		static UUID getFromPath(const std::string& path);
		static const AssetType assetTypeFromExtension(const std::string& extension);

		static void loadAllFiles(const std::string& directory);
		static void loadDirectory(const std::string& directory);
		static void loadShaders(const std::string& directory);
		static void loadTextures(const std::string& directory);
		static void loadMaterials(const std::string& directory);

		static void reloadShaders(const std::string& directory);

		static inline bool hasProperity(const std::string path, const std::string& properity) {
			ST_PROFILING_FUNCTION();
			if (FileExists(path + ".meta")) {
				YAML::Node data = YAML::LoadFile(path + ".meta");
				if (data["props"]) {
					return true;
				}
			}
			return false;
		}
		template <typename T>
		static inline T getProperity(const std::string path, const std::string& properity) {
			ST_PROFILING_FUNCTION();
			if (FileExists(path + ".meta")) {
				YAML::Node data = YAML::LoadFile(path + ".meta");
				if (data["props"]) {
					YAML::Node props = data["props"];
					if (props[properity.c_str()]) {
						return props[properity.c_str()].as<T>();
					}
				}
			}
			return T();
		}
		template <typename T>
		static inline void setProperitys(const std::string path, const std::vector<std::pair<std::string, T>>& vs) {
			ST_PROFILING_FUNCTION();
			if (!FileExists(path + ".meta")) {
				createMeta(UUID(), path, assetTypeFromExtension(path.substr(path.find_last_of('.'), path.npos)));
			}
			YAML::Node data = YAML::LoadFile(path + ".meta");
			UUID uuid(data["uuid"].as<uint64_t>());
			const AssetType type = (AssetType)data["type"].as<int>();

			YAML::Node props;
			bool hasProps = false;
			if (FileExists(path + ".meta")) {
				YAML::Node data = YAML::LoadFile(path + ".meta");
				if (data["props"]) {
					props = data["props"];
					for (auto& v : vs) {
						props[v.first.c_str()] = v.second;
					}
					hasProps = true;
				}
			}


			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "uuid" << YAML::Value << (uint64_t)uuid;
			out << YAML::Key << "type" << YAML::Value << (int)type;
			out << YAML::Key << "props" << YAML::Value;
			if (hasProps) {
				out << props;
			}
			else {
				out << YAML::BeginMap;
				for (auto& v : vs) {
					out << YAML::Key << v.first.c_str() << YAML::Value << v.second;
				}
				out << YAML::EndMap;
			}
			out << YAML::EndMap;

			FILE* file = fopen((path + ".meta").c_str(), "w");
			fprintf(file, out.c_str());
			fclose(file);
		}
		template <typename T>
		static inline void setProperity(const std::string path, const std::pair<std::string, T>& s) { setProperitys<T>(path, std::vector<std::pair<std::string, T>>{s}); }
	private:
		//uuid ,<data, path>
		static std::unordered_map<UUID, Asset> assets;

		static void createMeshesFromModel(const Asset uuid);
		static void _createMeshesFromModel(const Asset uuid, Model& model);

		static void createMeta(const UUID& uuid, const std::string& path, const AssetType type);
	};
}