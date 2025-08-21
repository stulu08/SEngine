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

		inline WeakAssetHandle() : m_data(nullptr) {}
		inline WeakAssetHandle(std::nullptr_t) : m_data(nullptr) {}
		inline WeakAssetHandle(SharedAssetData* data) : m_data(data) {}

		AssetHandle<T> Lock() const;

		inline void ForceLoad() {
			if (!m_data) return;
			if (IsLoaded()) {
				CORE_INFO("Asset already loaded, force load not possible!");
				return;
			}
			CORE_TRACE("Loading: {0}", m_data->GetPath());
			this->m_data->Load();
		}

		inline auto operator*() const {
			if constexpr (HasNativeAssetType<T>::value)
				return static_cast<T*>(m_data)->GetNative();
			else
				return static_cast<T*>(m_data);
		}
		inline auto operator->() const {
			if constexpr (HasNativeAssetType<T>::value)
				return static_cast<T*>(m_data)->GetNative();
			else
				return static_cast<T*>(m_data);
		}

		inline operator bool() const {
			return IsValid();
		}

		inline T* GetAsset() { return static_cast<T*>(m_data); }
		inline const T* GetAsset() const { return static_cast<const T*>(m_data); }

		inline bool IsValid() const { return m_data; }
		inline bool IsLoaded() const { return m_data ? m_data->Loaded() : false; }
		inline bool IsMemoryLoaded() const { return m_data ? m_data->IsMemoryLoaded() : false; }
		inline UUID GetUUID() const { return m_data ? m_data->GetUUID() : UUID::null; }
		inline const std::string& Path() const { return m_data ? m_data->GetPath() : EmptyString(); }

		static inline std::type_index TypeID() { return typeid(T); }
		// dynamic type check
		static inline bool TypeCheck(SharedAssetData* data) { return dynamic_cast<T*>(data) != nullptr; }

		// used by bindings, only for internal usage
		inline void IncRefCount() {
			// First strong reference: load the asset
			if (this->m_data->UseCount() < 1) {
				CORE_TRACE("Loading: {0}", this->m_data->GetPath());
				this->m_data->Load();
			}
			this->m_data->IncRef();
		}
		// used by bindings, only for internal usage
		inline void DecRefCount() {
			if (this->m_data && this->m_data->DecRef() < 1) {
				this->m_data->Unload();
			}
		}
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
		inline AssetHandle() : WeakAssetHandle<T>(nullptr) {}
		inline AssetHandle(std::nullptr_t) : WeakAssetHandle<T>(nullptr) {}

		// Increments the ref counter and loads the asset if needed
		inline AssetHandle(SharedAssetData* data)
			: WeakAssetHandle<T>(data) {
			Initialize();
		}

		// Copies and increments the ref counter and loads the asset if needed
		inline AssetHandle(const AssetHandle& other)
			: WeakAssetHandle<T>(other.m_data) {
			Initialize();
		}

		// Releases the old refrencence, and copies + increments the ref counter and loads the asset if needed
		inline AssetHandle& operator=(const AssetHandle& other) {
			if (this != &other) {
				Release();
				this->m_data = other.m_data;
				Initialize();
			}
			return *this;
		}

		inline ~AssetHandle() {
			Release();
		}

		// Releases the refrence and unloads the asset if needed
		inline void Release() {
			this->DecRefCount();
			this->m_data = nullptr;
		}

		inline WeakRef AsWeak() const {
			return WeakRef(this->m_data);
		}

		inline size_t UseCount() const { return this->m_data ? this->m_data->UseCount() : 0; }
	protected:
		inline void Initialize() {
			if (!this->m_data) return;

			// Check for the correct type
			if (!WeakAssetHandle<T>::TypeCheck(m_data)) {
				CORE_ERROR("Type mismatch in AssetHandle, expected '{0}', got '{1}'", WeakAssetHandle<T>::TypeID().name(), this->m_data->GetTypeID().name());
				this->m_data = nullptr;
				return;
			}
			this->IncRefCount();
		}

		friend class StuluBindings::AssetHandle;
	};

	template<typename T>
	inline AssetHandle<T> WeakAssetHandle<T>::Lock() const {
		if (IsValid()) return AssetHandle<T>(m_data);
		return AssetHandle<T>(); // empty handle
	}

	using GeneralAsset = AssetHandle<SharedAssetData>;
}