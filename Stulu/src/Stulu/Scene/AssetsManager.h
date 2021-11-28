#pragma once
#include "Stulu/Scene/Material.h"
#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Core/UUID.h"

#include <unordered_map>


namespace Stulu {
	enum class AssetType {
		Unknown,
		Texture2D, Texture, CubeMap,
		Model, Mesh, 
		Material, Shader,
		Scene
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

		static void update(const UUID uuid, Asset data);

		static bool exists(const UUID uuid);
		static bool existsAndType(const UUID uuid, const AssetType type);

		static Asset& get(const UUID uuid);
		const static type_info& getType(const UUID uuid);
		const static AssetType& getAssetType(const UUID uuid);
		template<typename T>
		static T* getAs(const UUID uuid);

		static const AssetType assetTypeFromExtension(const std::string& extension);

		static void loadAllFiles(const std::string& directory);
		static void loadDirectory(const std::string& directory);
		static void loadShaders(const std::string& directory);
		static void loadTextures(const std::string& directory);
		static void loadMaterials(const std::string& directory);

		static UUID getFromPath(const std::string& path);

		template<typename T>
		static T getProperity(Asset& aseet, const std::string& name, const T& nullReturn = T());
		template<typename T>
		static void setProperity(Asset& aseet, const std::string name, const T& value);
	private:
		//uuid ,<data, path>
		static std::unordered_map<UUID, Asset> assets;

		static void createMeta(const UUID uuid, const std::string& path, const AssetType type);
	};
}