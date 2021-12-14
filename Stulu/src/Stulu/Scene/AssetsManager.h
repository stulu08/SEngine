#pragma once
#include "Stulu/Scene/Material.h"
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Core/UUID.h"

#include <unordered_map>


namespace Stulu {
	bool FileExists(const std::string& name);
	class Model;
	enum class AssetType {
		Unknown = 0,
		Texture2D = 1, Texture = 2, CubeMap = 3,
		Model = 4, Mesh = 5, 
		Material = 6, Shader = 7,
		Scene = 8
	};
	
	struct Asset {
		AssetType type;
		std::any data;
		std::string path;
		UUID uuid;
	};
	class STULU_API AssetsManager {
	public:
		/// read UUID from .meta file
		static UUID add(const std::string& path);
		static void add(const UUID uuid, const std::string& path);
		static void add(const UUID uuid, const std::string& path, const AssetType type);

		static void update(const UUID uuid, const Asset& data);

		static bool exists(const UUID uuid);
		static bool existsAndType(const UUID uuid, const AssetType type);

		static Asset& get(const UUID uuid);
		const static type_info& getType(const UUID uuid);
		const static AssetType& getAssetType(const UUID uuid);
		template<typename T>
		static T* getAs(const UUID uuid);
		static UUID getFromPath(const std::string& path);
		static const AssetType assetTypeFromExtension(const std::string& extension);

		static void loadAllFiles(const std::string& directory);
		static void loadDirectory(const std::string& directory);
		static void loadShaders(const std::string& directory);
		static void loadTextures(const std::string& directory);
		static void loadMaterials(const std::string& directory);

	private:
		//uuid ,<data, path>
		static std::unordered_map<UUID, Asset> assets;

		static void createMeshesFromModel(const Asset uuid);
		static void _createMeshesFromModel(const Asset uuid, Model& model);

		static void createMeta(const UUID uuid, const std::string& path, const AssetType type);
	};
}