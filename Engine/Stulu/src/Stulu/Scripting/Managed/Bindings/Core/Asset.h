#pragma once
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Scripting/Managed/Bindings/Bindings.h"
namespace StuluBindings {
	class AssetHandle {
	public:
		static inline MonoString* GetPath(size_t uuid) {
			if (uuid != Stulu::UUID::null) {
				Stulu::SharedAssetData* data = Stulu::AssetsManager::GlobalInstance().GetRaw(uuid);
				if (data) {
					return Stulu::Mono::String::New(getCoreDomain(), data->GetPath());
				}
			}
			return Stulu::Mono::String::New(getCoreDomain(), "");
		}
		static inline bool GetLoaded(size_t uuid) {
			if (uuid != Stulu::UUID::null) {
				Stulu::SharedAssetData* data = Stulu::AssetsManager::GlobalInstance().GetRaw(uuid);
				if (data) {
					return data->Loaded();
				}
			}
			return false;
		}

		static uint32_t InitHandle(Stulu::Mono::Object assetHandle, size_t uuid);
		static void ReleaseHandle(uint32_t gcHandle, size_t uuid);
		static void CleanUpAssets(bool checkAll = false);

		struct TrackedManagedAsset {
			// Mono weak GC handle
			Stulu::Mono::GCHandle gcHandle;
			Stulu::UUID assetID;
		};
	private:
		static bool InternalReleaseHandle(AssetHandle::TrackedManagedAsset& target);
		static std::vector<TrackedManagedAsset> g_TrackedAssets;
		static std::mutex g_AssetsMutex;
	};
}