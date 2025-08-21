#pragma once
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Scripting/Managed/Bindings/Bindings.h"

#include "Asset.h"

namespace StuluBindings {
	template<class T = Stulu::GeneralAsset>
	class AssetWrapper {
	public:
		static inline Stulu::AssetsManager& GetManager() {
			return Stulu::AssetsManager::GlobalInstance();
		}

		static inline auto GetAsset(Stulu::UUID assetID) {
			return GetManager().GetAsset<T>(assetID);
		}
		static inline T SaveGetAsset(Stulu::UUID assetID, bool loadCheck = true) {
			T asset = GetManager().GetAsset<T>(assetID);
			if (asset.IsValid() && (asset.IsLoaded() || !loadCheck)) {
				return asset;
			}
			return nullptr;
		}
	};
}