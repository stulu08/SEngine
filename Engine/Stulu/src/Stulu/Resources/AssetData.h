#pragma once
#include "Stulu/Types/UUID.h"
#include "Stulu/Core/Log.h"

#include <typeindex>

namespace Stulu {
	// Prefix for memory assets
	inline const char* MemoryAssetPath = ":";

	template<class T>
	class WeakAssetHandle;
	template<class T>
	class AssetHandle;

	/// <summary>
	/// <para> This class is a wrapper for the actual asset, it handles loading and unloading. </para>
	/// <para> It stores the UUID as a refrence for the asset inside the AssetsManager. </para>
	/// <para> Also the refrence count is stored here, which will increase if an AssetHandle is created or decrease if deleted </para>
	/// <para> The refrence count will always start at 0. </para>
	/// <para> Instances should only be created inside the AssetsManager! </para>
	/// <para> Be sure to always have an nullptr constructor for empty instances, this is used internaly for identifying the type based on its name/id and file extension! </para>
	/// </summary>
	class STULU_API SharedAssetData {
	public:
		// empty constructor
		SharedAssetData(std::nullptr_t)
			: m_id(UUID::null), m_path(), m_refCount(0) {}

		SharedAssetData(UUID uuid, const std::string& path, size_t refCount = 0)
			: m_id(uuid), m_path(path), m_refCount(refCount) {}

		inline const std::string& GetPath() const { return m_path; }
		inline virtual bool ChangePath(const std::string& newPath) { 
			m_path = newPath;
			return true;
		}

		inline UUID GetUUID() const { return m_id; }
		inline virtual void SetUUID(UUID newId) { m_id = newId; }
		virtual std::type_index GetTypeID() const = 0;
		virtual const char* GetTypeName() const = 0;
		virtual bool Loaded() const = 0;

		virtual inline bool IsMemoryLoaded() const {
			return m_path.empty() || (m_path.rfind(MemoryAssetPath, 0) == 0);
		}
		virtual void Unload() = 0;
		virtual bool Load() = 0;
		virtual bool Save() const { return true; };

		virtual bool HasExtension(const std::string& extension) const { return false; };

	protected:
		inline size_t UseCount() const { return m_refCount; }
		inline size_t IncRef() {
			return ++m_refCount;
		}
		inline size_t DecRef() {
			return --m_refCount;
		}
	private:
		UUID m_id;
		std::string m_path;
		size_t m_refCount;

		template<class T>
		friend class WeakAssetHandle;
		template<class T>
		friend class AssetHandle;
		friend class AssetsManager;
	};

	class STULU_API SharedMiscFileAssetData : public SharedAssetData {
	public:
		SharedMiscFileAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}
		SharedMiscFileAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}

		virtual std::type_index GetTypeID() const override {
			return typeid(SharedMiscFileAssetData);
		}
		virtual const char* GetTypeName() const override {
			return "MiscFileAsset";
		}
		virtual bool HasExtension(const std::string& extension) const override {
			return false;
		};

		virtual void Unload() override { }
		virtual bool Load() override { return true; }
		virtual bool Save() const override { return true; }
		virtual bool Loaded() const override { return true; }
	};
	class STULU_API SharedSceneAssetData : public SharedAssetData {
	public:
		SharedSceneAssetData(std::nullptr_t)
			: SharedAssetData(nullptr) {}
		SharedSceneAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}

		virtual std::type_index GetTypeID() const override {
			return typeid(SharedSceneAssetData);
		}
		virtual const char* GetTypeName() const override {
			return "SceneAsset";
		}
		virtual bool HasExtension(const std::string& extension) const override {
			return extension == ".scene" || extension == ".sscene";
		};

		virtual void Unload() override { }
		virtual bool Load() override { return true; }
		virtual bool Save() const override { return true; }
		virtual bool Loaded() const override { return true; }
	};
	using SceneAsset = AssetHandle<SharedSceneAssetData>;
}