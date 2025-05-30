#pragma once
#include "AssetData.h"
namespace StuluBindings {
	class AssetHandle;
}

namespace Stulu {
	template<class T>
	class AssetHandle;

	template<typename T, typename = void>
	struct HasNativeAssetType : std::false_type {};
	template<typename T>
	struct HasNativeAssetType<T, std::void_t<typename T::NativeType>> : std::true_type {};


	// non-owning handle
	template<class T = SharedAssetData>
	class WeakAssetHandle {
	public:
		using SharedAssetDataType = T;

		WeakAssetHandle() : m_data(nullptr) {}
		WeakAssetHandle(std::nullptr_t) : m_data(nullptr) {}
		WeakAssetHandle(SharedAssetData* data) : m_data(data) {}

		AssetHandle<T> Lock() const;

		void ForceLoad() {
			if (!m_data) return;
			if (IsLoaded()) {
				CORE_INFO("Asset already loaded, force load not possible!");
				return;
			}
			CORE_TRACE("Loading: {0}", m_data->GetPath());
			this->m_data->Load();
		}

		auto operator*() const {
			if constexpr (HasNativeAssetType<T>::value)
				return static_cast<T*>(m_data)->GetNative();
			else
				return static_cast<T*>(m_data);
		}
		auto operator->() const {
			if constexpr (HasNativeAssetType<T>::value)
				return static_cast<T*>(m_data)->GetNative();
			else
				return static_cast<T*>(m_data);
		}

		T* GetAsset() { return static_cast<T*>(m_data); }
		const T* GetAsset() const { return static_cast<const T*>(m_data); }

		bool IsValid() const { return m_data; }
		bool IsLoaded() const { return m_data ? m_data->Loaded() : false; }
		bool IsMemoryLoaded() const { return m_data ? m_data->IsMemoryLoaded() : false; }
		UUID GetUUID() const { return m_data ? m_data->GetUUID() : UUID::null; }
		const std::string& Path() const { return m_data ? m_data->GetPath() : EmptyString(); }

		static std::type_index TypeID() { return typeid(T); }
	protected:
		SharedAssetData* m_data = nullptr;
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
		AssetHandle(std::nullptr_t) : WeakAssetHandle<T>(nullptr) {}

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
			DecRefCount();
			this->m_data = nullptr;
		}

		WeakRef AsWeak() const {
			return WeakRef(this->m_data);
		}

		size_t UseCount() const { return this->m_data ? this->m_data->UseCount() : 0; }
	protected:
		void Initialize() {
			if (!this->m_data) return;

			// check for correct type only if a specific type is provided
			if constexpr (!std::is_same<T, SharedAssetData>::value) {
				// Check for the correct type
				if (WeakAssetHandle<T>::TypeID() != this->m_data->GetTypeID()) {
					CORE_ERROR("Type mismatch in AssetHandle, expected '{0}', got '{1}'", WeakAssetHandle<T>::TypeID().name(), this->m_data->GetTypeID().name());
					this->m_data = nullptr;
					return;
				}
			}
			IncRefCount();
		}

		// used by bindings
		void IncRefCount() {
			// First strong reference: load the asset
			if (this->m_data->UseCount() < 1) {
				CORE_TRACE("Loading: {0}", this->m_data->GetPath());
				this->m_data->Load();
			}
			this->m_data->IncRef();
		}
		void DecRefCount() {
			if (this->m_data && this->m_data->DecRef() < 1) {
				this->m_data->Unload();
			}
		}

		friend class StuluBindings::AssetHandle;
	};

	template<typename T>
	AssetHandle<T> WeakAssetHandle<T>::Lock() const {
		if (IsValid()) return AssetHandle<T>(m_data);
		return AssetHandle<T>(); // empty handle
	}

	using GeneralAsset = AssetHandle<SharedAssetData>;
}