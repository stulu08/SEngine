#pragma once
#include "Stulu/Types/UUID.h"
#include "Stulu/Core/Log.h"

#include <typeindex>

namespace Stulu {
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
	/// </summary>
	class SharedAssetData {
	public:
		SharedAssetData(UUID uuid, const std::string& path, size_t refCount = 0)
			: m_id(uuid), m_path(path), m_refCount(refCount) {}

		inline const std::string& GetPath() const { return m_path; }
		inline UUID GetUUID() const { return m_id; }

	protected:
		virtual const std::type_info& GetTypeID() const = 0;
		inline size_t UseCount() const { return m_refCount; }
		inline size_t IncRef() {
			return ++m_refCount;
		}
		inline size_t DecRef() {
			return --m_refCount;
		}

		virtual void Unload() = 0;
		virtual void Load() = 0;
		virtual bool Loaded() const = 0;
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
	class SharedTexture2DAssetData : public SharedAssetData {
	public:
		SharedTexture2DAssetData(UUID uuid, const std::string& path)
			: SharedAssetData(uuid, path) {}

	};
}