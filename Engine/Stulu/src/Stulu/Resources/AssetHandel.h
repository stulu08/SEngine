#pragma once
#include "AssetData.h"

namespace Stulu {
	template<class T>
	class AssetHandle;

	// non-owning handle
	template<class T = SharedAssetData>
	class WeakAssetHandle {
	public:
		using SharedAssetDataType = T;

		WeakAssetHandle() : m_data(nullptr) {}
		WeakAssetHandle(SharedAssetData* data) : m_data(data) {}

		AssetHandle<T> Lock() const;

		void ForceLoad() {
			if (!m_data) return;

			if (IsLoaded()) {
				CORE_INFO("Asset already loaded, force load not possible!");
				return;
			}
			this->m_data->Load();
		}

		// Should only be used with valid AssetHandles
		T* operator->() const { return static_cast<T*>(m_data); }
		T& operator*() const { return *(static_cast<T*>(m_data)); }

		bool IsValid() const { return m_data; }
		bool IsLoaded() const { return m_data ? m_data->Loaded() : false; }
		UUID GetUUID() const { return m_data ? m_data->GetUUID() : UUID::null; }
		const std::string& Path() const { return m_data ? m_data->GetPath() : ""; }
		const bool HasPath() const { return !Path().empty(); }
		static const std::type_info& TypeID() { return typeid(T); }
	protected:
		SharedAssetData* m_data;
	};

	/// <summary>
	/// This is the actual refrence to an asset inside the AssetsManager.
	/// Loading and unloading will be triggred from here.
	/// </summary>
	/// <typeparam name="T"> This should be the asset type (SharedTexture2DAssetData, ...) </typeparam>
	template<class T = SharedAssetData>
	class AssetHandle : public WeakAssetHandle<T> {
	public:
		using SharedAssetDataType = T;
		using WeakRef = WeakAssetHandle<T>;

		// Empty asset
		AssetHandle() : WeakAssetHandle<T>(nullptr) {}

		// Increments the ref counter and loads the asset if needed
		AssetHandle(SharedAssetData* data) 
			: WeakAssetHandle<T>(data) {
			Initialize();
		}

		// Copies and increments the ref counter and loads the asset if needed
		AssetHandle(const AssetHandle& other)
			: WeakAssetHandle<T>(other.m_data) {
			Initialize();
		}

		// Releases the old refrencence, and copies + increments the ref counter and loads the asset if needed
		AssetHandle& operator=(const AssetHandle& other) {
			if (this != &other) {
				Release();
				this->m_data = other.m_data;
				Initialize();
			}
			return *this;
		}

		~AssetHandle() {
			Release();
		}

		// Releases the refrence and unloads the asset if needed
		void Release() {
			if (this->m_data && this->m_data->DecRef() < 1) {
				this->m_data->Unload();
			}
			this->m_data = nullptr;
		}

		WeakRef AsWeak() const {
			return WeakRef(this->m_data);
		}

		size_t UseCount() const { return this->m_data ? this->m_data->UseCount() : 0; }
	protected:
		void Initialize() {
			if (!this->m_data) return;

			// Check for the correct type
			if (this->TypeID() != this->m_data->GetTypeID()) {
				CORE_ERROR("Type mismatch in AssetHandle, expected '{0}', got '{1}'", this->TypeID().name(), this->m_data->GetTypeID().name());
				return;
			}

			// First strong reference: load the asset
			if (this->m_data->UseCount() < 1) {
				this->m_data->Load();
			}
			this->m_data->IncRef();
		}
	};

	template<typename T>
	AssetHandle<T> WeakAssetHandle<T>::Lock() const {
		if (IsValid()) return AssetHandle<T>(m_data);
		return AssetHandle<T>(); // empty handle
	}


	using Texture2DAsset = AssetHandle<SharedTexture2DAssetData>;

	/*
	class AssetManager {
	public:
		template<class T = AssetHandle<T>>
		void Get(UUID id) {
			auto it = m_assets.find(id);
			if (it != m_assets.end()) {
				SharedAssetData* data = it->second;
				if (data) return AssetHandle<T::SharedAssetDataType>(data); // promote to strong handle
			}

			// Load asset manually
			T* asset = LoadAssetFromDisk<T>(id);
			auto* data = new SharedAssetData(id, asset);
			m_assets[id] = data;

			return AssetHandle<T>(data);
		}
		void Test() {
			Texture2DAsset asset = nullptr;
			
		}
	private:
		std::unordered_map<UUID, SharedAssetData*> m_assets;
	};
	*/
}