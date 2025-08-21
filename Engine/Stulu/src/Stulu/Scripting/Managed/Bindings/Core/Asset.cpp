#include "st_pch.h"

#include "Stulu/Scripting/Managed/Bindings/Bindings.h"
#include "Stulu/Scripting/Managed/Mono.h"
#include "Asset.h"


using namespace Stulu;

namespace StuluBindings {
	std::vector<AssetHandle::TrackedManagedAsset> AssetHandle::g_TrackedAssets;
	std::mutex AssetHandle::g_AssetsMutex;


	inline bool operator==(const AssetHandle::TrackedManagedAsset& left, const AssetHandle::TrackedManagedAsset& right) {
		return ((uint32_t)left.gcHandle == (uint32_t)right.gcHandle) && left.assetID == right.assetID;
	}

	uint32_t AssetHandle::InitHandle(Stulu::Mono::Object assetHandle, size_t uuid) {
		CleanUpAssets();

		if (!assetHandle) {
			CORE_WARN("AssetHandle::InitHandle, assetHandle was null!");
			return 0;
		}
		std::scoped_lock lock(g_AssetsMutex);

		if (uuid != Stulu::UUID::null) {
			Stulu::GeneralAsset handle = Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::GeneralAsset>(uuid);
			if (handle.IsValid()) {
				handle.IncRefCount();

				Mono::GCHandle gcHandle = Mono::GCHandle::NewWeakRef(assetHandle, false);
				if (gcHandle != 0)
					g_TrackedAssets.push_back({ gcHandle, uuid });
				else
					CORE_ERROR("Internal mono Error!");
				return gcHandle;
			}
		}
		CORE_WARN("AssetHandle::InitHandle, asset not found with UUID: {0}", uuid);
		return 0;
	}
	void AssetHandle::ReleaseHandle(uint32_t gcHandle, size_t uuid) {
		AssetHandle::TrackedManagedAsset target = { gcHandle, uuid };
		
		if (gcHandle == 0) {
			CORE_WARN("AssetHandle::ReleaseHandle invalid handle!");
			return;
		}
		std::scoped_lock lock(g_AssetsMutex);

		auto it = std::find(g_TrackedAssets.begin(), g_TrackedAssets.end(), target);
		if (it != g_TrackedAssets.end()) {
			if (InternalReleaseHandle(*it)) {
				g_TrackedAssets.erase(it);
			}
		}
		
	}

	static constexpr size_t kMaxCleanupsPerCall = 10;
	void AssetHandle::CleanUpAssets(bool checkAll) {
		if (Mono::Thread::GetCurrent())
			Mono::Thread::Attach(getManager()->getRootDomain());

		if (checkAll) {
			CORE_INFO("Running managed GC...");
			Mono::GC::Collect(Mono::GC::MaxGenerations());
		}

		std::scoped_lock lock(g_AssetsMutex);

		size_t cleaned = 0;
		for (auto it = g_TrackedAssets.begin(); it != g_TrackedAssets.end();) {
			if (cleaned < kMaxCleanupsPerCall && !checkAll)
				break;

			if (InternalReleaseHandle(*it)) {
				CORE_INFO("Released unrefrenced managed asset: {0}", it->assetID);
				it = g_TrackedAssets.erase(it);
				cleaned++;
			}
			else {
				++it;
			}
		}
	}
	bool AssetHandle::InternalReleaseHandle(AssetHandle::TrackedManagedAsset& target) {
		Mono::Object targetObject = target.gcHandle.GetTarget();
		if (!targetObject) {
			if (target.assetID != Stulu::UUID::null) {
				Stulu::GeneralAsset asset = Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::GeneralAsset>(target.assetID);
				if (asset.IsValid()) {
					asset.DecRefCount();
				}
			}
			target.gcHandle.Free();
			return true;
		}
		return false;
	}
}