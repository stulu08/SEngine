#pragma once
#include "Stulu/Scene/Material.h"
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Core/UUID.h"
#include "YAML.h"
#include <unordered_map>


namespace Stulu {
	bool FileExists(const std::string& name);
	class STULU_API Model;
	struct MeshAsset;
	enum class AssetType {
		Unknown = 0,
		Texture2D = 1, RenderTexture = 2, SkyBox = 3,
		Model = 4, Mesh = 5, 
		Material = 6, Shader = 7,
		Scene = 8,
		Directory = 9,
		Script = 10
	};
	
	struct Asset {
		AssetType type = AssetType::Unknown;
		std::any data = nullptr;
		std::string path = "";
		UUID uuid = UUID::null;

	};
	static inline Asset NullAsset{ AssetType::Unknown,nullptr,"",UUID::null};
	class STULU_API AssetsManager {
	public:
		/// read UUID from .meta file
		static UUID add(const std::string& path);
		static void add(const UUID& uuid, const std::string& path);
		static void add(const UUID& uuid, const std::string& path, const AssetType type);
		static void addOrReload(const std::string& path);
		static UUID addDirectory(const std::string& path);

		static MeshAsset& createMeshAsset(const Ref<Mesh>& mesh, const std::string & = "New Mesh", UUID = UUID());

		static void update(const UUID& uuid, const Asset& data);

		static void remove(const UUID& uuid, bool deleteFile = false, bool deleteMetaFile = false);

		static bool exists(const UUID& uuid);
		static bool existsAndType(const UUID& uuid, const AssetType type);

		static inline std::vector<UUID>& getAllByType(AssetType type) { return assetsTypeList[type]; }

		static Asset& get(const UUID& uuid);
		static inline bool saveGet(const UUID& uuid, Asset& outAsset) {
			if (exists(uuid)) {
				outAsset = assets[uuid];
				return true;
			}
			return false;
		}
		const static AssetType getAssetTypeByPath(const std::string& path);
		const static type_info& getType(const UUID& uuid);
		const static AssetType& getAssetType(const UUID& uuid);
		template<typename T>
		static inline T* getAsPtr(const UUID& uuid) {
			if (exists(uuid))
				return assets[uuid].data._Cast<T>();
			else
				CORE_ERROR("UUID not present in assets");
			return nullptr;
		}
		template<typename T>
		static inline T& getAs(const UUID& uuid) {
			if (!exists(uuid)) {
				CORE_ERROR("UUID not present in assets");

			}

			return std::any_cast<T&>(assets[uuid].data);
		}
		static UUID getFromPath(const std::filesystem::path& path);
		static UUID getFromPath(const std::filesystem::path& path, AssetType type);
		static const AssetType assetTypeFromExtension(const std::string& extension);

		static void loadAllFiles(const std::string& directory, bool loadNewFiles = true);
		static void loadDirectory(const std::string& directory, bool loadNewFiles = true);
		static void loadShaders(const std::string& directory, bool loadNewFiles = true);
		static void loadTextures(const std::string& directory, bool loadNewFiles = true);
		static void loadMaterials(const std::string& directory, bool loadNewFiles = true);

		static void reloadShaders(const std::string& directory);

		static UUID getModelFromMesh(UUID mesh);
		static UUID getModelFromMaterial(UUID material);

		static inline bool hasProperity(const std::string path, const std::string& properity) {
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
		static void createMeta(const UUID& uuid, const std::string& path, const AssetType type);
	private:
		//uuid ,<data, path>
		static std::unordered_map<UUID, Asset> assets;
		static std::unordered_map<AssetType, std::vector<UUID>> assetsTypeList;

		static void createMeshesFromModel(const Asset uuid);
		static void _createMeshesFromModel(const Asset uuid, Model& model);

		friend class SceneSerializer;
	};
}