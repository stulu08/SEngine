#pragma once
#include "AssetHandel.h"

#include "Stulu/Resources/AssetData.h"
#include "Stulu/Serialization/YAML.h"

#include <map>
#include <magic_enum/magic_enum.hpp>

namespace Stulu {
	struct MetaInfo {
		UUID uuid = UUID();
		// placeholder for default constructor
		std::type_index type = typeid(void);

		YAML::Node node;
	};

	class STULU_API AssetsManager {
	public:
		using IDType = Stulu::UUID;
		// i need to use Ref here since Scope will not work in an map (missing construct operators), but since i only use one refrence and there is not getter it will work just fine
		using AssetMap = std::map<IDType, Ref<SharedAssetData>>;
		
		AssetsManager();
		virtual ~AssetsManager();
		
		void LoadDirectoryRecursive(const std::string& path);

		IDType LoadFile(const std::string& path);
		SharedAssetData* GetFromPath(const std::string& path, bool metaRead = true) const;
		SharedAssetData* GetRaw(IDType uuid) const;

		// Will not consider meta files, force loads will increase the ref counter by one, the object will always be loaded
		virtual bool AddAsset(std::type_index type, const std::string& path, IDType uuid, bool persisten = false);
		virtual bool AddAsset(SharedAssetData* asset, IDType uuid, bool persisten = false);

		virtual void RemoveAsset(IDType uuid);

		// Will trigger the change path event on SharedAssetData
		bool UpdatePath(IDType uuid, const std::string& newPath);

		// Expects an asset file as path, not an .meta file
		bool ReadFileMeta(MetaInfo* outInfo, const std::string& path) const;
		// Expects an asset file as path, not an .meta file
		bool WriteFileMeta(MetaInfo& info, const std::string& path) const;
		

		template<class T /*= AssetHandle<SharedAssetData> */>
		inline bool TypeCheck(IDType id) const {
			const auto it = Find(id);
			if (it != m_assets.end()) {
				SharedAssetData* data = it->second.get();
				if (data) return data->GetTypeID() == T::TypeID();
			}
			return false;
		}

		// Returns an invalid asset if not found
		template<class T /*= AssetHandle<SharedAssetData> */>
		inline T GetAsset(IDType id) {
			auto it = Find(id);
			if (it != m_assets.end()) {
				SharedAssetData* data = it->second.get();

				if (data) return T(data);
				else CORE_WARN("Corrupted Asset Registry for Asset: {0}", id);
			}
			else {
				CORE_WARN("Asset not found with id: {0}", id);
			}
			return InvalidAsset<T>();
		}

		inline bool Contains(IDType id) const { return Find(id) != m_assets.end(); }
		inline AssetMap::iterator Find(IDType id) { return m_assets.find(id); }
		inline const AssetMap::const_iterator Find(IDType id) const { return m_assets.find(id); }

		std::type_index FindTypeByExtension(const std::string& extension) const;
		const char* GetTypeName(std::type_index type) const {
			return m_typeRegistry.at(type)->GetTypeName();
		}
		template<class T /*= AssetHandle<SharedAssetData> */>
		const char* GetTypeNameT() const {
			const auto it = m_typeRegistry.find(typeid(T::SharedAssetDataType));
			if (it != m_typeRegistry.end()) {
				return it->second->GetTypeName();
			}
			return SharedMiscFileAssetData(nullptr).GetTypeName();
		}

		template<class T /*= AssetHandle<SharedAssetData> */>
		static const T InvalidAsset() { return T(); }
		static AssetsManager& GlobalInstance();
	protected:
		AssetMap m_assets;
		// used for finding the type from it's name
		std::unordered_map<std::string, std::type_index> m_typeNameRegistry;
		// used for finding the type by it's index
		std::unordered_map<std::type_index, Ref<SharedAssetData>> m_typeRegistry;
		// used for creating instances of the asset
		std::unordered_map<std::type_index, std::function<SharedAssetData*(IDType, const std::string&)>> m_typeCreateRegistry;

	public:
		template<class T>
		inline void RegisterType() {
			// The instance will be deleted at destruction of the AssetsManager
			SharedAssetData* emptyAsset = new T(nullptr);

			m_typeNameRegistry.insert({ emptyAsset->GetTypeName(), emptyAsset->GetTypeID() });

			m_typeRegistry.emplace(emptyAsset->GetTypeID(), emptyAsset);

			m_typeCreateRegistry.emplace(emptyAsset->GetTypeID(), [](IDType id, const std::string& path) {
				return new T(id, path);
				});
		}

	public:
		bool HasMetaValue(const MetaInfo& meta, const std::string& key) const {
			return (bool)meta.node[key];
		}

		// Function for default structures
		template<class T>
		typename std::enable_if<!std::is_enum<T>::value, T>::type 
		GetMetaValue(const MetaInfo& meta, const std::string& key, const T& defaultValue) const {
			return meta.node[key].as<T>(defaultValue);
		}
		
		template<typename T>
		void SetMetaValue(MetaInfo& meta, const std::string& key, T value) const {
			if constexpr (std::is_enum<T>::value) {
				meta.node[key] = std::string(magic_enum::enum_name(value));
			}
			else {
				meta.node[key] = value;
			}
		}

		// Utility function for enums
		template<class T>
		typename std::enable_if<std::is_enum<T>::value, T>::type
		GetMetaValue(const MetaInfo& meta, const std::string& key, const T& defaultValue) const {
			if (!meta.node[key])
				return defaultValue;

			const std::string str = meta.node[key].as<std::string>();
			auto value = magic_enum::enum_cast<T>(str);
			if (value.has_value())
				return value.value();
			else
				return defaultValue;
		}
	};
}